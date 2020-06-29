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
/// @brief  Definition of encapsulated string manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/21
///   Changed:  Created by adding an encapsulated string class
///


#ifndef _CST_TOOLS_CMN_STRING_H_
#define _CST_TOOLS_CMN_STRING_H_

#include <string>

///
/// @brief  The base namespace for Crystal Speech Toolkit
///
namespace cst
{
    ///
    /// @brief  The namespace for common useful utilities, such as: string, vector, etc.
    ///
    namespace cmn
    {
        ///
        /// @brief  Encapsulated string class
        ///
        typedef std::string string;

        ///
        /// @brief  Encapsulated wstring class
        ///
        typedef std::wstring wstring;

    }//namespace cmn

    ///
    /// @brief  The namespace for string utilities, such as: encoding conversion, replace, trim, tokenize etc.
    ///
    namespace str
    {
        ///
        /// @brief  Write formatted data to a string
        ///
        int snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);

        ///
        /// @brief  Compare characters of two strings without regard to case
        ///
        int wcsnicmp(const wchar_t *string1, const wchar_t *string2, size_t count);

    }//namespace str
}

#endif//_CST_TOOLS_CMN_STRING_H_
