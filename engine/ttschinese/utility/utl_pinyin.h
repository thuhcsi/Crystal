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
/// @brief  Head file for Pinyin utility routines
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/12/20
///   Changed:  Created
///

#ifndef _CST_TTS_CHINESE_UTL_PINYIN_H_
#define _CST_TTS_CHINESE_UTL_PINYIN_H_

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class containing the definition of Pinyin as well as the utilities for processing Pinyin strings.
            ///
            class CPinyin
            {
            public:
                ///
                /// @brief  Split the Pinyin into initial, final, retroflex (Erhua, if any), and tone
                ///
                /// All the output buffer array MUST be allocated before calling the function.
                /// The buffer should contain enough space including the terminating NULL L'\0'.
                ///
                /// @param  [in]  pinyin    Input Pinyin to be separated
                /// @param  [out] initial   String array for storing the returned initial
                /// @param  [out] final     String array for storing the returned final
                /// @param  [out] retroflex String array for storing the returned retroflex (Erhua: "rr") if any
                /// @param  [out] tone      Return the tone number
                ///
                /// @return Whether operation is successful
                ///
                static bool split(const wchar_t *pinyin, wchar_t *initial, wchar_t *final, wchar_t *retroflex, int &tone);

            };//CPinyin

        }//namespace Chinese
    }
}

#endif //_CST_TTS_CHINESE_UTL_PINYIN_H_
