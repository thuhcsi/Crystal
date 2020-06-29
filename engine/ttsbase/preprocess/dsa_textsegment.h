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
/// @brief    Head file for segmenting text into pieces for later document structure analysis module
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/02/01
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSA_TEXTSEGMENT_H_
#define _CST_TTS_BASE_DSA_TEXTSEGMENT_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class which performs text piece segmentation processing
            ///
            /// The functionality of text segmentation is to segment input text into several text pieces.
            /// Each piece will only contain the text with the same text category TYPE.
            /// Here, the TYPE is related to special constructs in the written form of text.
            /// Different TYPE of texts are often related to different "say-as" element in SSML document.
            /// 
            class CTextSegment : public CModule, public xml::CSSMLTraversal
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CTextSegment(const CDataManager *pDataManager) : CModule(pDataManager) {}

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
                virtual int process(xml::CSSMLDocument * pSSMLDocument);


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for SSML document traversing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Process a specific SSML document node for SSML document traversing,
                ///         to extract text into text items for further segmentation
                ///
                virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

            protected:
                ///
                /// @brief  Text items to be segmented (Values are stored while performing SSML document traversing)
                ///
                std::vector<xml::CXMLNode*> m_textItems;


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for further text piece segmentation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Text piece information returned by text segmentation processing function
                ///
                /// Each text piece is often related to a "say-as" element in SSML document, and
                /// attribute "interpret", "format", and "detail" values are used to generate the "say-as" element.
                ///
                /// There are at least two kinds of special values for "interpret" attribute:
                /// (1) "" (empty)
                ///     If text pieces is body text (e.g. Chinese for Putonghua TTS, or English for English TTS),
                ///     the returned "interpret" will be "" (empty),
                ///     the "format" will be "" (empty) too.
                /// (2) "punctuation"
                ///     If text piece is terminator for paragraph or sentence,
                ///     the returned "interpret" will be "punctuation", and
                ///     the "format" will be "p" for paragraph terminator and "s" for sentence terminator.
                ///
                class CTextPiece
                {
                public:
                    std::wstring wstrText;      ///< Content of the text piece
                    std::wstring wstrInterpret; ///< Content type of the text (valid for special constructs)
                    std::wstring wstrFormat;    ///< Precise format of the text for content type
                    std::wstring wstrDetail;    ///< Level of detail to be read aloud or rendered
                };

            protected:
                ///
                /// @brief  Segment the input text into text pieces
                ///
                /// This is the main function for text segmentation.
                /// Implementation tips:
                /// (1) should detect the special constructs (that affect the pronunciation);
                /// (2) should mark up the terminator for paragraph or sentence.
                ///
                /// @see CTextPiece for detailed description of the returned text pieces
                ///
                /// @param  [in]  wstrText      Input text to be segmented
                /// @param  [out] textPieces    Return the segmented text pieces of the input text
                ///
                virtual void segmentText(const std::wstring &wstrText, std::vector<CTextPiece> &textPieces);

            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_DSA_TEXTSEGMENT_H_
