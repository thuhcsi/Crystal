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
/// @brief  Head file of encapsulated HTS synthesizer with wave synthesizer interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Xujun (xjun.cn@gmail.com), John (john.zywu@gmail.com)
///   Date:     2007/06/15
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/30
///   Changed:  Modified by using the new definitions
/// - Version:  0.2.5
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/07/30
///   Changed:  Modified by using HTS synthesizer
///


#ifndef _CST_TTS_CHINESE_HTS_SYNTHESIZER_H_
#define _CST_TTS_CHINESE_HTS_SYNTHESIZER_H_

#include "ttsbase/datavoice/wav_synthesizer.h"
#include "htslib/hts_synthesizer.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  Encapsulate the HTS synthesizer with wave synthesizer interface
            ///
            class CHtsSynthesizer : public base::CWavSynthesizer, hts::CHtsSynthesizer
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CHtsSynthesizer() : base::CWavSynthesizer(), hts::CHtsSynthesizer() {}

                ///
                /// @brief  Destructor
                ///
                virtual ~CHtsSynthesizer() {}

                ///
                /// @brief  Initialize the HTS synthesizer
                ///
                virtual bool initialize(const std::wstring &wstrPath, const std::wstring &wstrCfgFile);

                ///
                /// @brief  Terminate and free the HTS synthesizer
                ///
                virtual bool terminate();

            public:
                ///
                /// @brief  Get the wave sampling rate (samples per second)
                ///
                virtual int getSamplesPerSec() const {return hts::CHtsSynthesizer::getSampleRate();}

                ///
                /// @brief  Get the wave sampling precision (bits per sample)
                ///
                virtual int getBitsPerSample() const {return 16;}

                ///
                /// @brief  Get the wave channel number
                ///
                virtual int getChannels() const {return 1;}

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Hide some of the overrides for TTSBase
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Initialize the speech library module
                ///
                virtual bool initialize(const std::wstring &wstrPath) {return false;}

                ///
                /// @brief  Get the wave data for the specific speech unit of the input phoneme
                ///
                virtual bool getWave(const std::wstring &wstrPhoneme, uint8 *waveData, uint32 &waveLen) const {return false;}

                ///
                /// @brief  Get the wave length for the specific speech unit of the input phoneme
                ///
                virtual uint32 getWaveLength(const std::wstring &wstrPhoneme) const {return 0;}

            };
        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_HTS_SYNTHESIZER_H_
