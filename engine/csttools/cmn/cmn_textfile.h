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
/// @brief  Definition of text file with different encodings manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/05/10
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_CMN_TEXTFILE_H_
#define _CST_TTS_BASE_CMN_TEXTFILE_H_

#include "cmn_file.h"
#include "utils/utl_string.h"

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  Standard input/output file manipulation with encoding
        ///
        class CTextFile : public CFile
        {
        public:
            ///
            /// @brief  Default constructor
            ///
            CTextFile() : m_encoding(str::ENC_ANSI) {}

            ///
            /// @brief  Open the file with specified mode and encoding
            ///
            /// @param  [in] filename   Name of the file to be opened
            /// @param  [in] mode       Type of access permitted, takes the values as defined in "<stdio.h>"
            /// @param  [in] encoding   Encoding of the file text
            ///
            /// @return Whether filename is opened successfully or not
            ///
            virtual bool open(const wchar_t *filename, const wchar_t *mode, str::EEncoding encoding=str::ENC_AUTO);

            ///
            /// @brief  Reads all text data into the string from the file
            ///
            /// @return  false if end-of-file was reached without reading any data
            ///
            virtual bool readString(std::wstring &retstr);

            ///
            /// @brief  Writes string text from a buffer to the file
            ///
            virtual bool writeString(const wchar_t *retstr);

            ///
            /// @brief  Get the encoding of the text file
            ///
            const str::EEncoding &getEncoding() const {return m_encoding;}

        protected:
            /// Text encoding of the text file (default is ENC_ANSI)
            str::EEncoding m_encoding;
            /// Length of the Byte Order Mark (e.g. FF FE for UTF16LE, EF BB BF for UTF8)
            size_t m_BOMLen;
        };

    }
}

#endif//_CST_TTS_BASE_CMN_TEXTFILE_H_
