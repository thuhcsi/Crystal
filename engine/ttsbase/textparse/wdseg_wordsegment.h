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
/// @brief  Head file for word segmentation interface
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

#ifndef _CST_TTS_BASE_WDSEG_WORDSEGMENT_H_
#define _CST_TTS_BASE_WDSEG_WORDSEGMENT_H_

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
            /// @brief  The class which performs the word segmentation processing
            ///
            /// The functionality of word segmentation module is to segment the input sentence 
            /// into lexicon words using "w" element according to pronunciation lexicon.
            ///
            /// Sometimes, the part-of-speech (POS) or other information is very useful to select
            /// the appropriate pronunciation for polyphone or homograph.
            /// The POS information can be generated and set the "role" attribute in "w" element,
            /// for example, "w role=adj".
            ///
            /// The generated word sequence can result in improved cues for prosodic control (e.g. pause) and
            /// may assist the synthesizer in selection of the correct pronunciation for homographs.
            ///
            class CWordSegment : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CWordSegment(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure of the word segmentation module
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with sentence information
                /// @param  [out] pSSMLDocument  Return the SSML document with word segmentation information
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
                /// @brief  The text item information to be used in the processing of word segmentation module
                ///
                /// The pXMLNode is the handle to the xml node in SSML document, which can be: \n
                /// "text" node for pure text piece (bPhoneme=false and bWord=false), \n
                /// "phoneme" node for the text piece under phoneme element (bPhoneme=true), \n
                /// "w" node for the text piece under word element (bWord=true).
                ///
                /// It should also be noted that "w" node has higher priority than "phoneme" node.
                /// "bPhoneme" can take the value "true" only when "phoneme" element is not in "w" element.
                /// For example, in "It is <phoneme ph=''>New York</phoneme>", bPhoneme is true (i.e. bWord=false, bPhoneme=true);
                /// while in "It is <w><phoneme ph=''>New York</phoneme></w>", bPhoneme is false (i.e. bWord=true, bPhoneme=false).
                ///
                class CTextItem
                {
                public:
                    CTextItem() : pXMLNode(NULL), bWord(false), bPhoneme(false) {}  ///< Constructor
                    xml::CXMLNode   *pXMLNode;      ///< Handle to the xml node in SSML document, see CTextItem for details
                    bool            bWord;          ///< Whether text piece is under "w" element (i.e. pXMLNode is "w" element)
                    bool            bPhoneme;       ///< Whether text piece is under "phoneme" element (i.e. pXMLNode is "phoneme" element)
                    std::wstring    wstrText;       ///< The text piece
                    std::wstring    wstrPOS;        ///< Part-of-speech (POS) of text piece (i.e. word) after word segmentation
                };

                ///
                /// @brief  The internal document information assisting the word segmentation processing
                ///
                class CWdSegDocument : public CTTSDocument<CTextItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process SSML document node, for building internal document by SSML traversing
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                    ///
                    /// @brief  Perform the post-processing of SSML document node, for SSML traversing
                    ///
                    virtual int postProcessNode(xml::CXMLNode *pNode);

                protected:
                    CTextItem m_tmpItem;    ///< Temp item information for sharing data between processNode while traversing
                };

            protected:
                ///
                /// @brief  Perform the word segmentation for each text piece in the entire document
                ///
                /// @param  [in]  wdsegDocument   The internal document data
                /// @param  [out] wdsegDocument   Return the result with word segmentation information
                ///
                virtual int segmentToWord(CWdSegDocument &wdsegDocument);

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  wdsegDocument   The internal results to be written
                /// @param  [out] pSSMLDocument   Return the SSML document with word segmentation result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CWdSegDocument &wdsegDocument);

                ///
                /// @brief  Perform the forward match for one text item, return the result in CTextItem vector
                ///
                /// @param  [in]  lexicon       The lexicon data from which word entries are matched
                /// @param  [in]  srcText       The source text piece item to be segmented
                /// @param  [out] resTextList   Return the segmentation result
                ///
                virtual int forwardMatch(const CLexicon &lexicon, const CTextItem &srcText, std::vector<CTextItem> &resTextList);

                ///
                /// @brief  Perform the word segmentation for each text piece in one sentence
                ///
                /// In this base class implementation, the basic maximum forward matching algorithm is used for Chinese;
                ///
                /// @param  [in]  sentenceInfo  The text piece information for a sentence
                /// @param  [out] sentenceInfo  Return the result with word segmentation result
                ///
                virtual int segmentSentence(std::vector<CTextItem> &sentenceInfo);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_WDSEG_WORDSEGMENT_H_
