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
/// @brief  Implementation of encapsulated string manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/21
///   Changed:  Created by adding an encapsulated string class
///

#include <cstdarg>
#include "cmn_string.h"


namespace cst
{
    namespace str
    {
        int snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...)
        {
#pragma warning( push )
#pragma warning( disable : 4996 )
            va_list arglist;
            va_start(arglist, format);
#if defined(WIN32) || defined(WINCE)
            int ret = _vsnwprintf(buffer, count, format, arglist);
#else
            int ret = vswprintf(buffer, count, format, arglist);
#endif
            va_end(arglist);
            return ret;
#pragma warning( pop )
        }

        int wcsnicmp(const wchar_t *string1, const wchar_t *string2, size_t count)
        {
#if defined(WIN32) || defined(WINCE)
            return _wcsnicmp(string1, string2, count);
#else
            return wcsncasecmp(string1, string2, count);
#endif

        }

    }//namespace str
}
