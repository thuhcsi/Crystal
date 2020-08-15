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
/// @brief  Head file for concatenation synthesis interface
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

#ifndef _CST_TTS_BASE_SYNTH_CONCATENATE_H_
#define _CST_TTS_BASE_SYNTH_CONCATENATE_H_

#include "ttsbase/synthesize/synth_synthesize.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class to perform the concatenation synthesis operation
            ///
            /// The functionality of concatenation synthesis module is
            /// (1) to perform unit selection for all units in the sentence,
            /// (2) to retrieve the raw wave data of a specific unit from speech library for each basic unit,
            /// (3) to call the prosody modification module to modify the wave data to match the target prosody,
            /// (4) to concatenate the above wave data to generate the whole wave data.
            ///
            class CSynthConcatenate : public CSynthesize
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CSynthConcatenate(const CDataManager *pDataManager) : CSynthesize(pDataManager) {}

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Operations for performing the concatenation synthesis
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Perform concatenation synthesis for basic units in one sentence
                ///
                /// It should be noted that the new synthetic wave data is APPENDed to the original data.
                /// The original data should be KEPT UNCHANGED.
                ///
                /// The procedure will:
                /// (1) first perform unit selection for all units in the sentence,
                /// (2) retrieve the wave data from speech library,
                /// (3) perform the prosody modification, and 
                /// (4) finally append the wave data to generate the whole wave data.
                ///
                /// This default implementation will return the input wave data directly WITHOUT prosody modification.
                ///
                /// @param  [in]  sentenceInfo  Target basic unit information for a sentence to be synthesized
                /// @param  [out] waveData      Return the synthetic wave data by APPENDing the new data
                ///
                virtual int synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData);

                ///
                /// @brief  Selects the appropriate speech units from library (unit selection) for each basic unit in one sentence
                ///
                /// This function is called as the first step in the synthesize() procedure.
                /// Every derived concatenation synthesis method should implement this function.
                ///
                /// The functionality of unit selection module is to select a speech unit from the speech library
                /// which is mostly appropriate to match the target basic unit to be synthesized.
                /// Here the "appropriate" means both the contextual information and the prosodic information of
                /// the selected speech unit are closest to the target information.
                ///
                /// The index of the selected speech unit is stored in the "uid" attribute of "unit" element.
                /// For example, "unit uid=zhong1:2" means the 2nd unit for phoneme (Pinyin) "zhong1".
                ///
                /// This default implementation of unit selection just selects the first unit as result
                ///
                /// @param  [in]  sentenceInfo  The target basic unit information for a sentence
                /// @param  [out] sentenceInfo  Return the result with unit selection result
                ///
                virtual int selectUnit(std::vector<CUnitItem> &sentenceInfo);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_SYNTH_CONCATENATE_H_
