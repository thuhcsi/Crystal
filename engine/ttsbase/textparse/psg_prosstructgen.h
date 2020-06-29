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
/// @brief  Head file for prosodic structure generation interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Converted SSML document to internal representation for processing
/// - Version:  0.2.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/01/10
///   Changed:  For boundary type, consider the boundary after current word
/// - Version:  0.2.2
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/07/20
///   Changed:  Add the "bFixed" flag to indicate the original user specific boundary type.
///

#ifndef _CST_TTS_BASE_PSG_PROSSTRUCTGEN_H_
#define _CST_TTS_BASE_PSG_PROSSTRUCTGEN_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"
#include "../datamanage/base_ttsdocument.h"
#include "../datamanage/data_datamanager.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class which performs the prosodic structure generation processing
            ///
            /// During speaking, the speaker always tends to insert appropriate breaks between different words.
            /// This is necessary for speaker to take the breath. There are two levels of breaks, the first one
            /// is related to prosodic words, the second one is related to bigger break indicating prosodic phrases.
            ///
            /// The prosodic structure (including prosodic words and prosodic phrases) is important for
            /// generating synthetic speech with natural prosody and high intelligence.
            ///
            /// This module is to generate both prosodic word boundary and prosodic phrase boundary by 
            /// inserting appropriate "<break>" elements.
            ///
            class CProsodicStructGenerate : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CProsodicStructGenerate(const CDataManager *pDataManager) : CModule(pDataManager) {}

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
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for internal document processing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Definition for the prosody boundary type
                ///
                enum EProsodyBoudaryType
                {
                    PROSBOUNDTYPE_SYLLABLE  = 0,    ///< Syllable boundary
                    PROSBOUNDTYPE_PWORD     = 1,    ///< Prosody word boundary
                    PROSBOUNDTYPE_PPHRASE   = 2,    ///< Prosody phrase boundary
                    PROSBOUNDTYPE_SENTENCE  = 3,    ///< Sentence boundary
                };

                ///
                /// @brief  Text item information to be used in prosodic structure generation
                ///
                class CWordItem
                {
                public:
                    CWordItem() : pWordNode(NULL), nBoundaryType(PROSBOUNDTYPE_SYLLABLE), bFixed(false), bBOS(false) {}  ///< Constructor
                    xml::CXMLNode   *pWordNode;     ///< Handle to the "w" element in SSML document
                    std::wstring    wstrText;       ///< Text piece
                    std::wstring    wstrPOS;        ///< Part-of-speech after word segmentation
                    int             nBoundaryType;  ///< Boundary type after this word, 0: none/syllable, 1: prosodic word, 2: prosodic phrase, 3: sentence
                    bool            bFixed;         ///< Whether boundary type is fixed (should NOT be changed). "true" when boundary is given in the input
                    bool            bBOS;           ///< Whether current word is the BOS (begin-of-sentence)
                };

                ///
                /// @brief  The internal document information assisting prosodic structure generation processing
                ///
                /// @todo   Add processing for the "break" element in the original document
                ///
                class CPSGDocument : public CTTSDocument<CWordItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process SSML document node, for building internal document by SSML traversing
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                protected:
                    CWordItem m_tmpItem;    ///< Temp item information for sharing data between processNode while traversing
                };

            protected:
                ///
                /// @brief  Generate the prosodic structure for the entire document
                ///
                /// The original user specific boundary type (i.e. bFixed is true) should be kept unchanged.
                ///
                /// @param  [in]  psgDocument   The internal document data
                /// @param  [out] psgDocument   Return the result with prosodic structure information
                ///
                virtual int generateProsodicStructure(CPSGDocument &psgDocument);

                ///
                /// @brief  Generate the prosodic structure for one sentence
                ///
                /// The original user specific boundary type (i.e. bFixed is true) should be kept unchanged.
                ///
                /// @param  [in]  sentenceInfo  The word information for a sentence
                /// @param  [out] sentenceInfo  Return the result with prosodic structure
                ///
                virtual int generateProsodicStructure(std::vector<CWordItem> &sentenceInfo);

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  psgDocument   The internal results to be written
                /// @param  [out] pSSMLDocument Return the SSML document with prosodic structure information
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CPSGDocument &psgDocument);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_PSG_PROSSTRUCTGEN_H_
