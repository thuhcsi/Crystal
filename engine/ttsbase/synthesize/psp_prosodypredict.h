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
/// @brief  Head file for prosodic prediction interface
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

#ifndef _CST_TTS_BASE_PSP_PROSODYPREDICT_H_
#define _CST_TTS_BASE_PSP_PROSODYPREDICT_H_

#include <stack>
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
            /// @brief  The class which performs the prosody prediction processing
            ///
            /// Producing human-like prosody is important for making speech sound natural and
            /// for correctly conveying the meaning of spoken language.
            ///
            /// Prosody is a set of acoustic features that include the pitch, the timing (or rhythm),
            /// the pausing, the speaking rate (or duration), the emphasis and many other features.
            ///
            /// The functionality of prosody prediction module is
            /// (1) to separate the word into basic units for speech synthesis using the "unit" element;
            /// (2) to predict the target prosodic information for a certain word or unit,
            ///     using the "prosody" or "emphasis" element.
            ///
            class CProsodyPredict : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CProsodyPredict(const CDataManager *pDataManager) : CModule(pDataManager) {}

            public:
                ///
                /// @brief  The main processing procedure of the prosody prediction module
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document with word and pronunciation information
                /// @param  [out] pSSMLDocument  Return the SSML document with basic unit and prosody information
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
                /// @brief  Relative prosody information specified by SSML "prosody" and "emphasis" elements
                ///
                struct SSMLProsody
                {
                public:
                    float   pitch;          ///< Relative pitch change (The final pitch mean will be multiplied by this value)
                    float   rate;           ///< Relative duration change (The final duration will be multiplied by this value)
                    float   volume;         ///< Relative volume change (The final amplitude will be multiplied by this value)
                public:
                    SSMLProsody() {pitch=rate=volume=1;}    ///< Set default value
                };

                ///
                /// @brief  The unit information for prosody prediction
                ///
                class CUnitItem
                {
                public:
                    // content
                    xml::CXMLElement *pXMLNode; ///< "unit"/"break" element in SSML document
                    std::wstring  wstrText;     ///< Text piece content of the unit
                    std::wstring  wstrPhoneme;  ///< Phoneme (pronunciation) of the unit
                    // boundary
                    bool    isBreak;            ///< Whether current item is "break" element
                    int     boundaryType;       ///< Boundary type of current "break" OR after current unit (defined in xml::CSSMLDocument::EProsodyBoudaryType)
                    bool    isStressed;         ///< Whether this unit is stressed (emphasized)
                    // unit prosody
                    float   duration;           ///< Target duration of the unit (Unit: ms)
                    float   amplitude;          ///< Target amplitude of the unit (Range: -32767, 32768)
                    float   pitchmean;          ///< Target mean (average) pitch of the unit (Unit: Hz)
                    float   contour[10];        ///< Target pitch contour of the unit, sampled at 10 equal distance points (Unit: Hz)
                    // SSML prosody
                    SSMLProsody ssmlProsody;    ///< User specified relative prosody change by SSML "prosody" and "emphasis" elements
                public:
                    CUnitItem() {clear();}      ///< Constructor
                    void clear();               ///< Clear the content
                };

                ///
                /// @brief  The internal document information assisting prosody prediction processing
                ///
                class CPSPDocument : public CTTSDocument<CUnitItem>, public xml::CSSMLTraversal
                {
                protected:
                    ///
                    /// @brief  Process a specific SSML document node, to build the internal document
                    ///
                    virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

                    ///
                    /// @brief  Perform the post-processing of a specific SSML document node
                    ///
                    virtual int postProcessNode(xml::CXMLNode *pNode);

                protected:
                    ///
                    /// @brief  Get the unit information from the SSML element
                    ///
                    /// @note   Validation assurance: \n
                    ///         The "unit" element can only contain "phoneme" element, and
                    ///         the "phoneme" element can only contain text
                    ///
                    int parseUnitInfo(xml::CXMLElement *pWordNode, CUnitItem &unitInfo) const;

                    ///
                    /// @brief  Get the prosody information from the SSML element
                    ///
                    int parseProsodyInfo(xml::CXMLElement *pProsodyNode, SSMLProsody &prosInfo) const;

                    ///
                    /// @brief  Get the emphasis information from the SSML element
                    ///
                    int parseEmphasisInfo(xml::CXMLElement *pEmphasisNode, SSMLProsody &prosInfo) const;

                    ///
                    /// @brief  Stacked SSML prosody (as only the latest "prosody", "emphasis" will take effect)
                    ///
                    std::stack<SSMLProsody> m_stackProsody;

                };//CPSPDocument


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Operations for performing prosody prediction
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  prosodyDocument   The internal results to be written
                /// @param  [out] pSSMLDocument     Return the SSML document with prosody prediction result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CPSPDocument &prosodyDocument);

                ///
                /// @brief  Perform the prosody prediction for the whole internal document
                ///
                /// @param  [in]  prosodyDocument   The internal document containing unit information
                /// @param  [out] prosodyDocument   Return the result with prosody prediction result
                ///
                virtual int predictProsody(CPSPDocument &prosodyDocument);

                ///
                /// @brief  Perform the prosody prediction for each basic unit in one sentence
                ///
                /// The input unit items have already been converted to basic units of speech synthesis.
                /// This default implementation of prosody prediction just sets the default duration
                ///
                /// @param  [in]  sentenceInfo  The basic unit information for a sentence
                /// @param  [out] sentenceInfo  Return the result with prosody prediction result
                ///
                virtual int predictProsody(std::vector<CUnitItem> &sentenceInfo);

            };//CProsodyPredict

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_PSP_PROSODYPREDICT_H_
