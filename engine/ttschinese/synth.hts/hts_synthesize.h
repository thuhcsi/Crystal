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
/// @brief  Head file for HMM based speech synthesis of Chinese TTS engine using HTS synthesizer
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///


#ifndef _CST_TTS_CHINESE_HTS_SYNTHESIZE_H_
#define _CST_TTS_CHINESE_HTS_SYNTHESIZE_H_

#include "hts_ssml2lab.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class to perform HTS synthesis for Chinese TTS engine
            ///
            class CHtsSynthesize : public CSSML2Lab
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CHtsSynthesize(const base::CDataManager *pDataManager) : CSSML2Lab(pDataManager) {}

            protected:
                ///
                /// @brief  Perform speech synthesis for one sentence
                ///
                /// It should be noted that the new synthetic wave data is APPENDed to the original data.
                /// The original data should be KEPT UNCHANGED.
                ///
                /// @param  [in]  sentenceInfo  Target basic unit information for a sentence to be synthesized
                /// @param  [out] waveData      Return the synthetic wave data by APPENDing the new data
                ///
                virtual int synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData);
            };

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_HTS_SYNTHESIZE_H_
