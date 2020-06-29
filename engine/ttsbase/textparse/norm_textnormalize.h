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
/// @brief  Head file for text normalization interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Added the default implementation by internal document processing
///

#ifndef _CST_TTS_BASE_NORM_TEXTNORMALIZE_H_
#define _CST_TTS_BASE_NORM_TEXTNORMALIZE_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"
#include "../datamanage/base_ttsdocument.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class which performs the text normalization processing
            ///
            /// All written languages have special constructs that require a conversion of the written form
            /// (orthographic form) (e.g. symbols, punctuations, digital numbers, etc.) into the spoken form.
            /// Text normalization is an automated process that performs this conversion.
            ///
            /// In this module, the suggested processing procedure is
            /// (1) to markup all special constructs with "say-as" or "sub" element;
            /// (2) to convert the content of "say-as" or "sub" element into spoken form characters or pronunciation.
            ///
            class CTextNormalize : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CTextNormalize(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure of the text normalization module
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with special constructs to be processed
                /// @param  [out] pSSMLDocument  Return the SSML document with special constructs normalized to text
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument * pSSMLDocument);

            protected:
                //virtual int detectConstructs();


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for internal document processing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  The text item information to be used in the processing of text normalization
                ///
                /// The text information could be from "say-as" element, or just from "text" node of SSML document.
                /// If text is from "say-as" element, the "pSayasNode" will be the handle to the "say-as" element
                /// in SSML document; otherwise, the "pSayasNode" will be NULL.
                ///
                /// The text information are stored in the "wstrText" member variable.
                /// The normalized text context will be returned in the "wstrNormWord" and "wstrPOS".
                ///
                class CTextItem
                {
                public:
                    CTextItem(): pSayasNode(NULL){} ///< Constructor
                    xml::CXMLElement *pSayasNode;   ///< Handle to "say-as" element in SSML document (or NULL if it is just "text" node)
                    std::wstring    wstrText;       ///< Original text content (under "say-as" element or just "text" node)
                    std::wstring    wstrNormWord;   ///< Normalized text content as the word ("w")
                    std::wstring    wstrPOS;        ///< POS (part-of-speech) of the word
                };

                ///
                /// @brief  The internal document information assisting the text normalization processing
                ///
                class CNormDocument : public CTTSDocument<CTextItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process a specific SSML document node, 
                    ///         required by SSML document traversing for building the internal document
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);
                };

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Operations for performing text normalization
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  normDocument  The internal results to be written
                /// @param  [out] pSSMLDocument Return the SSML document with text normalization result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CNormDocument &normDocument);

                ///
                /// @brief  Perform the text normalization for "say-as" text content in the whole internal document
                ///
                /// @param  [in]  normDocument  The internal document containing text content to be normalized
                /// @param  [out] normDocument  Return the result with text normalization result
                ///
                virtual int doNormalize(CNormDocument &normDocument);

                ///
                /// @brief  Perform the text normalization for "say-as" text content in one sentence
                ///
                /// This default implementation just copy the original text to the normalized result, and set POS to L"x" (Unknown)
                ///
                /// @param  [in]  sentenceInfo  Text content information of a sentence to be normalized
                /// @param  [out] sentenceInfo  Return the result with text normalization result
                ///
                virtual int doNormalize(std::vector<CTextItem> &sentenceInfo);

            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_NORM_TEXTNORMALIZE_H_
