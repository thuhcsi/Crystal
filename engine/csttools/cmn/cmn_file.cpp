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
/// @brief  Implementation file of encapsulated file manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/14
///   Changed:  Created
///

#include "cmn_file.h"
#include <utils/utl_string.h>

namespace cst
{
    namespace cmn
    {
        FILE* wfopen(const wchar_t *filename, const wchar_t *mode)
        {
#if defined(WIN32) || defined(WINCE)
            // call _wfopen directly
            FILE* fp = NULL;
            _wfopen_s(&fp, filename, mode);
            return fp;
#elif defined(__GNUC__)
            // convert from UTF-16 to UTF-8, and
            // call fopen
            std::string mbsName = str::wcstombs(filename);
            std::string mbsMode = str::wcstombs(mode);
            return fopen(mbsName.c_str(), mbsMode.c_str());
#else
            return NULL;
#endif
        }

        bool CFile::open(const wchar_t *filename, const wchar_t *mode)
        {
            if (m_pFile!=NULL)
                close();
            m_pFile = wfopen(filename, mode);
            return (m_pFile!=NULL);
        }

        bool CFile::close()
        {
            if (m_pFile==NULL || fclose(m_pFile)==0)
            {
                m_pFile = NULL;
                return true;
            }
            return false;
        }

        size_t CFile::read(void *buffer, size_t size, size_t count) const
        {
            if (m_pFile==NULL)
                return 0;
            return fread(buffer, size, count, m_pFile);
        }

        size_t CFile::write(const void *buffer, size_t size, size_t count) const
        {
            if (m_pFile==NULL || buffer==NULL)
                return 0;
            return fwrite(buffer, size, count, m_pFile);
        }

        bool CFile::seek(long long offset, int origin) const
        {
            if (m_pFile==NULL)
                return false;
#if defined(WIN32) || defined(WINCE)
            if (_fseeki64(m_pFile, offset, origin)!=0)
                return false;
#else
            if (fseek(m_pFile, offset, origin)!=0)
                return false;
#endif
            return true;
        }

        long long CFile::tell() const
        {
            if (m_pFile==NULL)
                return -1;
#if defined(WIN32) || defined(WINCE)
            return _ftelli64(m_pFile);
#else
            return ftell(m_pFile);
#endif
        }
    }
}
