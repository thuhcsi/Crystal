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
/// @brief  Head file for preprocessing functions
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_PREP_PREPROCESS_H_
#define _CST_TTS_BASE_PREP_PREPROCESS_H_

#include "cmn/cmn_string.h"
#include "utils/utl_string.h"
#include "../datamanage/base_module.h"
#include "../datamanage/data_datamanager.h"

namespace cst
{
    namespace tts
    {
        // forward class reference
        enum EInputTextType;

        namespace base
        {
            ///
            /// @brief  The class which performs text piece segmentation processing
            ///
            /// The functionalities of preprocessing module are as follows:
            /// (1) To parse the SSML tag contained in the original input text;
            /// (2) To convert all the input text into UTF-16 little-endian string;
            /// (3) To generate a complete well-formatted SSML document from the input text.
            ///
            class CPreProcess : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                /// @param  [in] pDataManager   Handle to the data manager of current TTS engine
                /// @param  [in] pTTSEngineType String indicating the type of the TTS engine. 
                ///                             "zh-cmn" for Putonghua TTS, "zh-yue" for Cantonese TTS.
                ///
                CPreProcess(const CDataManager *pDataManager, const wchar_t * pTTSEngineType);

            public:
                ///
                /// @brief  The main processing procedure of the preprocessing module
                ///
                /// Step 1: ASCII -> UTF-16
                ///
                /// This procedure will first convert the input text into UTF-16 little-endian string,
                /// and then call the other process() with wchar_t string as input.
                ///
                /// @param  [out] pSSMLDocument Return the complete well-formatted SSML document
                /// @param  [in]  strIn         Input multi-byte string to be parsed. The encoding of the string should be specified
                /// @param  [in]  encodingIn    Encoding of the input multi-byte string
                /// @param  [in]  ssmlTagIn     Indicate whether the input is 
                ///                             1) a well-formated SSML document with full SSML "speak" root element, or 
                ///                             2) a text string with only partial SSML tags (without SSML "speak" root element), or 
                ///                             3) a raw text string without any SSML tag
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument, const cmn::string &strIn, str::EEncoding encodingIn, EInputTextType textTagIn);

                ///
                /// @brief  The main processing procedure of the preprocessing module
                ///
                /// Step 2: UTF16 -> SSML Text -> SSML Binary
                ///
                /// @param  [out] pSSMLDocument Return the complete well-formatted SSML document
                /// @param  [in]  wstrIn        Input wide-character string to be parsed
                /// @param  [in]  ssmlTagIn     Indicate whether the input is 
                ///                             1) a well-formated SSML document with full SSML "speak" root element, or 
                ///                             2) a text string with only partial SSML tags (without SSML "speak" root element), or 
                ///                             3) a raw text string without any SSML tag
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument, const cmn::wstring &wstrIn, EInputTextType textTagIn);

            protected:
                ///
                /// @brief  Override procedure from CModule, NOT used.
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument)
                {
                    return 0;
                }

            protected:
                cmn::wstring m_wstrTTSEngineType;

            public:
                const static cmn::wstring m_strNewLine;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_PREP_PREPROCESS_H_
