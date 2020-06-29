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
/// @brief  Head file for grapheme-to-phoneme conversion interface
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
///

#ifndef _CST_TTS_BASE_GTP_GRAPHEME2PHONEME_H_
#define _CST_TTS_BASE_GTP_GRAPHEME2PHONEME_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"
#include "../datamanage/base_ttsdocument.h"
#include "../datatext/data_textdata.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class which performs the grapheme (text) to phoneme conversion processing
            ///
            /// The synthesizer only knows how to "speak" a word when the pronunciation of the word is known.
            /// The grapheme to phoneme module derives the pronunciations for each word.
            ///
            /// The functionality of grapheme to phoneme conversion module is
            /// (1) to derive the pronunciation for each word;
            /// (2) to process the "tone Sandhi" for special characters.
            ///
            /// The results will be stored in the "phoneme" element.
            ///
            class CGrapheme2Phoneme : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CGrapheme2Phoneme(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure for grapheme to phoneme conversion
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with word segmentation information
                /// @param  [out] pSSMLDocument  Return the SSML document with phoneme information
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument);


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for internal document processing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  The word item information to be used in the processing of grapheme-to-phoneme module
                ///
                class CWordItem
                {
                public:
                    CWordItem() : pWordNode(NULL), bFixed(false) {} ///< Constructor
                    xml::CXMLElement *pWordNode;    ///< Handle to the "w" element in SSML document
                    std::wstring    wstrWord;       ///< Text content of the word
                    std::wstring    wstrPOS;        ///< POS (part-of-speech) of the word
                    std::wstring    wstrPhoneme;    ///< Phoneme (pronunciation) of the word
                    std::wstring    wstrAlphabet;   ///< Phoneme alphabet
                    bool            bFixed;         ///< Whether phoneme is fixed (should NOT be changed). True when "phoneme" is given in the input
                };

                ///
                /// @brief  The internal document information assisting the grapheme-to-phoneme processing
                ///
                class CG2PDocument : public CTTSDocument<CWordItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process a specific SSML document node, 
                    ///         required by SSML document traversing for building the internal document
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                protected:
                    ///
                    /// @brief  Retrieve the word information from the SSML element
                    ///
                    /// All the children of the "w" element are processed to retrieved the word information.
                    ///
                    /// @note   Currently in this implementation, 
                    ///         the "w" element can only contain text and "phoneme" element;
                    ///         the "phoneme" element can only contain text.
                    ///
                    /// @param  [in]  pWord     The word element node in the SSML document
                    /// @param  [out] wordItem  Return the word information
                    ///
                    virtual int processWordNode(xml::CXMLElement *pWord, CWordItem &wordItem);
                };

            protected:
                ///
                /// @brief  Get the phoneme (pronunciation) information for each word (grapheme) in the entire document
                ///
                /// In this base class implementation, the phoneme for each word is retrieved directly from
                /// the lexicon by matching word text and POS information, without any post-processing.
                ///
                /// @param  [in]  g2pDocument   The internal document data
                /// @param  [out] g2pDocument   Return the result with phoneme information
                ///
                virtual int getPhoneme(CG2PDocument &g2pDocument);

                ///
                /// @brief  Get the phoneme information for one sentence
                ///
                /// All the words in one sentence are considered together for phoneme generation,
                /// even though they could be separated by several SSML elements.
                ///
                /// @param  [in]  sentenceInfo  The word (grapheme) information for a sentence
                /// @param  [out] sentenceInfo  Return the result with phoneme information
                ///
                virtual int getPhoneme(std::vector<CWordItem> &sentenceInfo);

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  g2pDocument   The internal results to be written
                /// @param  [out] pSSMLDocument Return the SSML document with phoneme result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CG2PDocument &g2pDocument);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_GTP_GRAPHEME2PHONEME_H_
