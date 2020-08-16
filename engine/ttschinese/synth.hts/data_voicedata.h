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
/// @brief    Head file for voice data management of Chinese Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///

#ifndef _CST_TTS_CHINESE_DATA_VOICEDATA_H_
#define _CST_TTS_CHINESE_DATA_VOICEDATA_H_

#include "ttsbase/datavoice/data_voicedata.h"
#include "hts_synthesizer.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class for managing the voice data (e.g. speech library, etc)
            ///
            class CVoiceData : public base::CVoiceData
            {
            public:
                ///
                /// @brief  Get the HTS synthesizer for HMM based speech synthesis
                ///
                virtual const CHtsSynthesizer &getWavSynthesizer() const
                {
                    return *(CHtsSynthesizer*)m_pWavSynthesizer;
                }

            public:
                ///
                /// @brief  Initialize the data manager from the configuration information
                ///
                /// @param  [in] dataConfig     The configuration information
                ///
                /// @return Whether operation is successful
                ///
                virtual bool initialize(const base::DataConfig &dataConfig);
            };

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_DATA_VOICEDATA_H_
