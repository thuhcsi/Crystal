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
/// @brief    Head file for document structure analysis interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_DSA_DOCSTRUCT_H_
#define _CST_TTS_BASE_DSA_DOCSTRUCT_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"

#include "dsa_symboldetect.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            class CDocStruct : public CModule, public xml::CSSMLTraversal
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CDocStruct(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure of the prosody prediction module
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with word and pronunciation information
                /// @param  [out] pSSMLDocument  Return the SSML document with prosody information
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument * pSSMLDocument);

            protected:
                ///
                /// @brief  Process a specific SSML document node, to get text information for document structure analysis
                ///
                virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                ///
                /// @brief  Perform the post-processing of SSML document node
                ///
                virtual int postProcessNode(xml::CXMLNode *pNode);

            protected:
                std::wstring m_wstrParsingFragmentTag;
                std::vector< std::pair<xml::CXMLNode*, int> > m_numFragments;

            protected:
                bool isTerminator(const xml::CXMLNode *pNode, const std::wstring &fragmentTag);

            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_DSA_DOCSTRUCT_H_
