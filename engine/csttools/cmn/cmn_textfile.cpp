//
//  Crystal Text-to-Speech Engine
//
//  Copyright (c) 2007 THU-CUHK Joint Research Center for
//  Media Sciences, Technologies and Systems. All rights reserved.
//
//  http://mjrc.sz.tsinghua.edu.cn
//
//  Redistribution and use in source and binary forms, with or without
//  modification, is not allowed, unless a valid written license is
//  granted by THU-CUHK Joint Research Center.
//
//  THU-CUHK Joint Research Center has the rights to create, modify,
//  copy, compile, remove, rename, explain and deliver the source codes.
//


///
/// @file
///
/// @brief  Implementation file of text file with different encodings manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/05/10
///   Changed:  Created
///

#include "cmn_textfile.h"

namespace cst
{
    namespace cmn
    {
        bool CTextFile::open(const wchar_t *filename, const wchar_t *mode, str::EEncoding encoding)
        {
            // open file
            if (!CFile::open(filename, mode))
                return false;

            bool isRead = true;
            while (*mode)
            {
                if (*mode == L'w')
                    isRead = false;
                mode++;
            }

            if (isRead)
            {
                // detect encoding automatically if not specified
                // skip BOM
                unsigned char BOM[3] = {0};
                CFile::read(BOM, sizeof(unsigned char), 3); // if file size is less than 3, BOM will contain the actual data
                if (BOM[0] == 0xFF && BOM[1] == 0xFE)
                {
                    // FF FE -> UTF-16 little endian
                    m_encoding = (encoding == str::ENC_AUTO) ? str::ENC_UTF16 : encoding;
                    m_BOMLen   = 2;
                }
                else if (BOM[0] == 0xFE && BOM[1] == 0xFF)
                {
                    // FE FF -> UTF-16 big endian
                    m_encoding = (encoding == str::ENC_AUTO) ? str::ENC_UTF16BE : encoding;
                    m_BOMLen = 2;
                }
                else if (BOM[0] == 0xEF && BOM[1] == 0xBB && BOM[2] == 0xBF)
                {
                    // EF BB BF -> UTF-8
                    m_encoding = (encoding == str::ENC_AUTO) ? str::ENC_UTF8 : encoding;
                    m_BOMLen   = 3;
                }
                else
                {
                    // default or specified encoding
                    m_encoding = (encoding == str::ENC_AUTO) ? str::ENC_ANSI : encoding;
                    m_BOMLen   = 0;
                }
                // set file cursor just after BOM
                CFile::seek((long long)m_BOMLen, SEEK_SET);
            }
            else
            {
                // write BOM
                unsigned char BOM[3] = {0};
                m_encoding = encoding;
                if (m_encoding == str::ENC_UTF16)
                {
                    BOM[0] = 0xFF;
                    BOM[1] = 0xFE;
                    m_BOMLen = 2;
                }
                else if (m_encoding == str::ENC_UTF16BE)
                {
                    BOM[0] = 0xFE;
                    BOM[1] = 0xFF;
                    m_BOMLen = 2;
                }
                else if (m_encoding == str::ENC_UTF8)
                {
                    BOM[0] = 0xEF;
                    BOM[1] = 0xBB;
                    BOM[2] = 0xBF;
                    m_BOMLen = 3;
                }
                else
                {
                    m_BOMLen = 0;
                }
                CFile::write(BOM, sizeof(unsigned char), m_BOMLen);
            }

            return true;
        }

        bool CTextFile::readString(std::wstring &retstr)
        {
            // get string buffer size
            long long posBeg = CFile::tell();
            CFile::seek(0, SEEK_END);
            long long posEnd = CFile::tell();
            CFile::seek(posBeg, SEEK_SET);

            // read data into buffer
            size_t mbscnt = size_t(posEnd - posBeg);
            char *mbsbuf = new char[mbscnt+1];
            if (CFile::read(mbsbuf, sizeof(char), mbscnt) != mbscnt)
            {
                delete []mbsbuf;
                return false;
            }
            mbsbuf[mbscnt] = 0;

            // convert to wstring
            size_t wcscnt = mbstowcs(mbsbuf, mbscnt, NULL, 0, m_encoding);
            wchar_t *wcsbuf = new wchar_t[wcscnt+1];
            wcscnt = mbstowcs(mbsbuf, mbscnt, wcsbuf, wcscnt, m_encoding);
            wcsbuf[wcscnt] = 0;

            // set return value
            retstr = wcsbuf;

            delete []wcsbuf;
            delete []mbsbuf;
            return true;
        }

        bool CTextFile::writeString(const wchar_t *retstr)
        {
            if (m_encoding == str::ENC_UTF16)
            {
                // no conversion is needed
                size_t wcscnt = wcslen(retstr);
                return (CFile::write(retstr, sizeof(wchar_t), wcscnt) == wcscnt);
            }

            // get required length for multi-byte-character string
            size_t wcscnt = wcslen(retstr);
            size_t mbscnt = wcstombs(retstr, wcscnt, NULL, 0, m_encoding);

            // perform conversion
            char *mbsbuf = new char[mbscnt+1];
            mbscnt = wcstombs(retstr, wcscnt, mbsbuf, mbscnt, m_encoding);
            mbsbuf[mbscnt] = 0;

            // write to file
            size_t mbswrite = CFile::write(mbsbuf, sizeof(char), mbscnt);

            delete []mbsbuf;
            return (mbswrite == mbscnt);
        }
    }
}
