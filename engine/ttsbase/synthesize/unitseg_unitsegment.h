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
/// @brief  Head file for segment the input text pieces and phonemes into basic units
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///

#ifndef _CST_TTS_BASE_UNITSEG_UNITSEGMENT_H_
#define _CST_TTS_BASE_UNITSEG_UNITSEGMENT_H_

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
            /// @brief  The class which segments the input text pieces and phonemes into basic units
            ///
            /// The functionality of this module is 
            /// to separate the word into basic units for speech synthesis using the "unit" element
            ///
            class CUnitSegment : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CUnitSegment(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure of the unit segmentation module
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with word and pronunciation information
                /// @param  [out] pSSMLDocument  Return the SSML document with basic unit information
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
                /// @brief  The text piece information for unit segmentation
                ///
                class CTextItem
                {
                public:
                    // content
                    xml::CXMLElement *pWordNode;///< "w" element in SSML document
                    std::wstring  wstrText;     ///< Text piece content
                    std::wstring  wstrPhoneme;  ///< Phoneme (pronunciation) of the text
                    std::wstring  wstrAlphabet; ///< Phoneme alphabet
                public:
                    CTextItem() {clear();}      ///< Constructor
                    void clear();               ///< Clear the content
                };

                ///
                /// @brief  The internal document information assisting unit segmentation processing
                ///
                class CUnitSegDocument : public CTTSDocument<CTextItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process a specific SSML document node, to build the internal document
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                protected:
                    ///
                    /// @brief  Get the word text information from the SSML element
                    ///
                    /// @note   Validation assurance: \n
                    ///         The "w" element can only contain "phoneme" element, and
                    ///         the "phoneme" element can only contain text
                    ///
                    int parseWordInfo(xml::CXMLElement *pWordNode, CTextItem &wordInfo) const;

                };//CUnitSegDocument


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Operations for performing unit segmentation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  unitDocument      The internal results to be written
                /// @param  [out] pSSMLDocument     Return the SSML document with basic unit result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CUnitSegDocument &unitDocument);

                ///
                /// @brief  Convert the input text items to basic units for the whole internal document
                ///
                /// @param  [in]  unitDocument      The internal document containing text information
                /// @param  [out] unitDocument      Return the result where text items are converted to basic units
                ///
                virtual int convertToUnits(CUnitSegDocument &unitDocument);

                ///
                /// @brief  Convert the input text items to basic units for speech synthesis
                ///
                /// @param  [in]  sentenceInfo  The text items for a sentence
                /// @param  [out] sentenceInfo  Return the result where text items are converted to basic units
                ///
                virtual int convertToUnits(std::vector<CTextItem> &sentenceInfo);

                ///
                /// @brief  Segment the input text pieces and phonemes into basic units
                ///
                /// @param  [in]  wstrText      Input text pieces with several basic units
                /// @param  [in]  wstrPhoneme   Phoneme of the input text with several basic units
                /// @param  [out] unitTexts     Return the text piece list of the basic units
                /// @param  [out] unitPhonemes  Return the phoneme list of the basic units
                ///
                virtual bool segmentToUnits(const std::wstring &wstrText, const std::wstring &wstrPhoneme,
                    std::vector<std::wstring> &unitTexts, std::vector<std::wstring> &unitPhonemes);

            };//CUnitSegment

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_UNITSEG_UNITSEGMENT_H_
