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
/// @brief  Definition of common types for the project
///
/// @version    0.1.0
/// @date       2007/06/15
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/15
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_CMN_TYPE_H_
#define _CST_TTS_BASE_CMN_TYPE_H_

//namespace cst
//{

///@todo    keep only: byte, int, size_t, icode_t, handle
///@todo    there are definitions in CrystalTTS.h

    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64;
    typedef unsigned int        icode_t;

    typedef unsigned long   ulong;      // used in CWaveData (needed?)
    typedef unsigned short  ushort;
    typedef unsigned int    uint;
    typedef unsigned char   byte;
    //typedef unsigned short  icode_t;
    typedef void*           handle;
#ifndef NULL
#   define  NULL            0
#endif

    static const icode_t INVALID_ICODE  = (icode_t)-1;
    static const uint32  INVALID_UNITID = (uint32)-1;
//}

    template<class _T, class _R>
    _T cst_dynamic_cast(_R value)
    {
#if defined(CST_NO_RTTI_SUPPORT)
        // RTTI (Run-time type information) is not supported
        // Force type conversion
        return (_T)(value);
#else
        // RTTI (Run-time type information) is supported
        return dynamic_cast<_T>(value);
#endif
    };

#endif//_CST_TTS_BASE_CMN_TYPE_H_
