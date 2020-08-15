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
/// @brief  Head file for speech synthesis interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/13
///   Changed:  Modified the interface to use CWaveData directly
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///

#ifndef _CST_TTS_BASE_SYNTH_SYNTHESIZE_H_
#define _CST_TTS_BASE_SYNTH_SYNTHESIZE_H_

#include "xml/ssml_document.h"
#include "xml/ssml_traversal.h"
#include "../datamanage/base_module.h"
#include "../datamanage/base_ttsdocument.h"
#include "dsp/dsp_wavedata.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class to perform the speech synthesis operation
            ///
            /// The functionality of speech synthesis module is
            /// (1) to retrieve the raw wave data of a specific unit from wave synthesizer for each basic unit,
            /// (2) to concatenate the above wave data to generate the whole wave data.
            ///
            class CSynthesize : public CModule
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CSynthesize(const CDataManager *pDataManager) : CModule(pDataManager) {}

            protected:
                ///
                /// @brief  The empty procedure for implementing the pure virtual process in base class
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument) {return ERROR_NOT_IMPLEMENTED;}

            public:
                ///
                /// @brief  The main processing procedure of the speech synthesis module
                ///
                /// @param  [in]  pSSMLDocument The input SSML document with unit selection and target prosodic information
                /// @param  [out] waveData      Return the synthetic wave data with FORMAT set. All original data (if any) are to be CLEARed.
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument, dsp::CWaveData &waveData);


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for internal document processing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  The basic unit information to be synthesized
                ///
                class CUnitItem
                {
                public:
                    // context
                    xml::CXMLElement *pXMLNode; ///< Handle to the "unit"/"break" element in SSML document
                    std::wstring  wstrText;     ///< Text content of the basic unit
                    std::wstring  wstrPhoneme;  ///< Phoneme (pronunciation) of the unit
                    icode_t phonemeID;          ///< Internal code of the phoneme
                    uint32  unitID;             ///< The index of the selected unit
                    // boundary type
                    int     boundaryType;       ///< Boundary type after current unit (defined in xml::CSSMLDocument::EProsodyBoudaryType)
                    bool    isBreak;            ///< Whether current unit is "break" element in SSML document
                    // prosody information
                    bool    isStressed;         ///< Whether current unit is stressed (emphasized)
                    float   duration;           ///< Target duration of the unit (Unit: ms)
                    float   amplitude;          ///< Target amplitude of the unit (Range: -32767, 32768)
                    float   pitchmean;          ///< Target mean (average) pitch of the unit (Unit: Hz)
                    float   contour[10];        ///< Target pitch contour of the unit, sampled at 10 equal distance points (Unit: Hz)
                    float   rate;               ///< Duration modification ratio
                    float   volume;             ///< Amplitude (volume) modification ratio
                    float   pitch;              ///< Mean (average) pitch modification ratio
                public:
                    CUnitItem() {clear();}      ///< Constructor
                    void clear();               ///< Clear the content
                };

                ///
                /// @brief  The internal document information assisting unit selection processing
                ///
                class CSynthDocument : public CTTSDocument<CUnitItem>, public xml::CSSMLTraversal
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
                    /// @brief  Get the basic unit information from the SSML document
                    ///
                    /// @note   Validation assurance: \n
                    ///         The "unit" element can only contain "prosody" element, and
                    ///         the "prosody" element can only contain "phoneme" element, and
                    ///         the "phoneme" element can only contain text
                    ///
                    /// @see    CUnitSelect::getBasicUnitInfo
                    ///
                    int getBasicUnitInfo(xml::CXMLElement *pUnitNode, CUnitItem &unitInfo) const;

                    ///
                    /// @brief  Get the break information from the SSML document
                    ///
                    int getBreakInfo(xml::CXMLElement *pBreakNode, CUnitItem &breakInfo) const;

                    ///
                    /// @brief  Set the boundary type for all units until one non-break unit
                    ///
                    /// This procedure will navigate the sentence from end() to begin(), and
                    /// set all the boundary type until the non-break unit is encountered
                    ///
                    int setBoundaryTypeTillNonbreak(std::vector<CUnitItem> &sentence, int boundaryType) const;
                };


            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Operations for performing the speech synthesis
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Write the result from internal result to the SSML document
                ///
                /// @param  [in]  synthDocument     The internal results to be written
                /// @param  [out] pSSMLDocument     Return the SSML document with speech synthesis result
                ///
                virtual int writeResult(xml::CSSMLDocument *pSSMLDocument, CSynthDocument &synthDocument);

                ///
                /// @brief  Perform speech synthesis for the whole internal document
                ///
                /// It should be noted that the new synthetic wave data is APPENDed to the original data.
                /// The original data should be KEPT UNCHANGED.
                ///
                /// @param  [in]  synthDocument Internal document containing basic unit information to be synthesized
                /// @param  [out] waveData      Return the synthetic wave data by APPENDing the new data
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int synthesize(CSynthDocument &synthDocument, dsp::CWaveData &waveData);

                ///
                /// @brief  Perform speech synthesis for one sentence
                ///
                /// It should be noted that the new synthetic wave data is APPENDed to the original data.
                /// The original data should be KEPT UNCHANGED.
                ///
                /// The procedure should retrieve the wave data from wave synthesizer, and
                /// then append the wave data to generate the whole wave data.
                ///
                /// @param  [in]  sentenceInfo  Target basic unit information for a sentence to be synthesized
                /// @param  [out] waveData      Return the synthetic wave data by APPENDing the new data
                ///
                virtual int synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_SYNTH_SYNTHESIZE_H_
