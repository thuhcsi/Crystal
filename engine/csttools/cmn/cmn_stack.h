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
/// @brief  Definition of encapsulated stack manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/21
///   Changed:  Created by adding an encapsulated stack class
///

#ifndef _CST_TOOLS_CMN_STACK_H_
#define _CST_TOOLS_CMN_STACK_H_

#include <stack>

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  Encapsulated stack class
        ///
        /// This encapsulated stack class is provided to avoid direct use of "std" related class
        /// for later easy migration.
        ///
        template<class Type>
        class stack : public std::stack<Type>
        {
        };

    }
}

#endif//_CST_TOOLS_CMN_STACK_H_
