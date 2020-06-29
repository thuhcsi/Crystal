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
/// @brief  Head file for language conversion interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Re-implemented with new interface
///

#ifndef _CST_TTS_BASE_LANG_LANGCONVERT_H_
#define _CST_TTS_BASE_LANG_LANGCONVERT_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "utils/utl_chineseconv.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  Perform the language conversion
            ///
            /// The functionality of language conversion module is to convert the language
            /// of the content to the language which current TTS engine supports.
            /// For example, to perform the conversion between Traditional Chinese
            /// and Simplified Chinese characters.
            ///
            /// Whether language conversion will be performed depends on the capability of the TTS engine.
            /// If the language can not be supported (or translated) by the TTS engine, it will be left unchanged.
            ///
            /// For example, if the input SSML document contains Simplified Chinese, Traditional Chinese,
            /// English and Japanese.
            ///
            /// If a TTS engine can support all languages, no language conversion will be performed.
            ///
            /// If a TTS engine can only support Simplified Chinese and can read out the English letters,
            /// all Traditional Chinese characters will be converted to Simplified Chinese characters,
            /// the English characters will be converted to "say-as" element,
            /// while the Japanese characters will be left unchanged (hence will not be read out by the TTS engine).
            ///
            /// The document for "Tags for Identifying Languages and Matching of Language Tags" 
            /// is available at http://www.ietf.org/rfc/bcp/bcp47.txt.
            ///
            class CLangConvert : public xml::CSSMLTraversal
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CLangConvert(const wchar_t *strConvTableFile);

            public:
                ///
                /// @brief  The main processing procedure of text segmentation module
                ///
                /// @param  [in]  pSSMLDocument  Input SSML document for text segmentation
                /// @param  [out] pSSMLDocument  Return SSML document with text segmentation information
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument);

            protected:
                ///
                /// @brief  Process a specific SSML document node for SSML document traversing
                ///
                virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for overriding
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Detect the source written language (transcription language) from the speaking language
                ///
                /// @param  [in]  speakLang Target speaking language of the synthetic speech
                /// @param  [out] srcLang   Source written language of the SSML document (as defined in SSML document, "" for any language)
                ///
                virtual int detectSourceLanguage(const std::wstring &speakLang, std::wstring &srcLang);

                ///
                /// @brief  Detect the target written language (transcription language) from the speaking language
                ///
                /// @param  [in]  speakLang Target speaking language of the synthetic speech
                /// @param  [out] tgtLang   Target written language of the SSML document
                ///
                virtual int detectTargetLanguage(const std::wstring &speakLang, std::wstring &tgtLang);

                ///
                /// @brief  Perform the language conversion
                ///
                /// @param  [in]  srcLang       Source written language (as defined in SSML document, "" for any language)
                /// @param  [in]  tgtLang       Target written language (as defined in SSML document, "" for any language)
                /// @param  [in]  textString    Text content to be converted
                /// @param  [out] textString    Return the converted text content
                ///
                virtual int doConversion(const std::wstring &srcLang, const std::wstring &tgtLang, std::wstring &textString);

            protected:
                cmn::CChineseConvert m_convertChinese;  ///< Language conversion processor
                std::wstring         m_wstrSourceLang;  ///< Source written language
                std::wstring         m_wstrTargetLang;  ///< Target written language
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_LANG_LANGCONVERT_H_
