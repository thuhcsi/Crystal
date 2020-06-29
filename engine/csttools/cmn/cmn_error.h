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
/// @brief
///
/// @version    0.1.0
/// @date       2007/06/05
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/06/05
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_CMN_ERROR_H_
#define _CST_TTS_BASE_CMN_ERROR_H_

#include <assert.h>

namespace cst
{

// Part from WinError.h

//
// MessageId: ERROR_SUCCESS
//
// MessageText:
//
//  The operation completed successfully.
//
#define ERROR_SUCCESS                       0L


//
// MessageId: ERROR_BAD_FORMAT
//
// MessageText:
//
//  An attempt was made to load a program with an incorrect format.
//
#define ERROR_BAD_FORMAT                    11L


// MessageId: ERROR_INVALID_PARAMETER
//
// MessageText:
//
//  The parameter is incorrect.
//
#define ERROR_INVALID_PARAMETER             87L

//
// MessageId: ERROR_OPEN_FAILED
//
// MessageText:
//
//  The system cannot open the device or file specified.
//
#define ERROR_OPEN_FAILED                   110L

    //
    // MessageId: ERROR_OUTOFMEMORY
    //
    // MessageText:
    //
    //  Not enough storage is available to complete this operation.
    //
#define ERROR_OUTOFMEMORY                14L

#define ERROR_MAX_EXIST_ERROR_NO            13884L

// User defined error code
#define ERROR_ENGINE_TYPE_INCORRECT         ( ERROR_MAX_EXIST_ERROR_NO + 1 )
#define ERROR_INVALID_SSML_DOCUMENT         ( ERROR_MAX_EXIST_ERROR_NO + 2 )
#define ERROR_NOT_INITIALIZED               ( ERROR_MAX_EXIST_ERROR_NO + 3 )
#define ERROR_UNSUPPORTED_LANGUAGE          ( ERROR_MAX_EXIST_ERROR_NO + 4 )
#define ERROR_DATA_READ_FAULT               ( ERROR_MAX_EXIST_ERROR_NO + 5 )
#define ERROR_NOT_IMPLEMENTED               ( ERROR_MAX_EXIST_ERROR_NO + 6 )


} // End of namespace cst

#endif // End of _CST_TTS_BASE_CMN_ERROR_H_