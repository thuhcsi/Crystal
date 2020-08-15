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
/// @brief  Head file for the interface of wave synthesizer which returns wave data for specific phoneme
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/15
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/30
///   Changed:  Modified by using the new definitions
///


#ifndef _CST_TTS_BASE_WAV_SYNTHESIZER_H_
#define _CST_TTS_BASE_WAV_SYNTHESIZER_H_

#include "cmn/cmn_type.h"
#include <string>
#include <vector>

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The base implementation interface class for wave synthesizer which returns wave data for specific phoneme
            ///
            class CWavSynthesizer
            {
            public:
                ///
                /// @brief  The detailed description of the wave synthesizer
                ///
                /// The languages and the accents must appear in pairs, which means each language must have one accent.
                /// If there is no (obvious) accent for one language, the accent must be set to "" (empty string).
                /// For example, "zh-cmn:zh-HK" means the speaker can speak Chinese Putonghua but with Hong Kong accent.
                /// "zh-yue" means the speaker can speak Chinese Cantonese (does not care about the accent).
                ///
                class CDescriptor
                {
                public:
                    wchar_t         chGender;       ///< The gender of the speaker for the wave synthesizer
                    int             nAge;           ///< The age in years (since birth) of the speaker
                    int             nVariant;       ///< The variant of the other characters of the speaker (e.g. the second male child voice)
                    std::wstring    wstrName;       ///< The name of the speaker
                    std::vector<std::wstring> vecLanguage;  ///< The languages which the speaker can speak
                    std::vector<std::wstring> vecAccent;    ///< The accents of the speaker, corresponding to languages one by one
                };

            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Initialization and finalization
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Constructor
                ///
                CWavSynthesizer() : m_bInitialized(false) {}

                ///
                /// @brief  Destructor
                ///
                virtual ~CWavSynthesizer() {m_bInitialized = false;}

                ///
                /// @brief  Initialize the wave synthesizer
                ///
                /// @param  [in] wstrPath   The path name where wave synthesizer data are stored
                ///
                /// @return Whether wave synthesizer is initialized successfully
                ///
                virtual bool initialize(const std::wstring &wstrPath) = 0;

                ///
                /// @brief  Terminate and free the wave synthesizer, close all the data
                ///
                /// @return Whether wave synthesizer is terminated successfully
                ///
                virtual bool terminate() = 0;

                ///
                /// @brief  Indicating whether wave synthesizer is initialized
                ///
                bool isReady() const {return m_bInitialized;}

            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Common data manipulation (overrides)
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Get wave data for the input phoneme
                ///
                /// @param  [in]  wstrPhoneme   The input phoneme whose wave data is to be returned
                /// @param  [in]  waveLen       The maximum buffer length to retrieve the wave data (Unit: uint8/byte)
                /// @param  [out] waveLen       Return the actual wave length of the retrieved wave data (unit: uint8/byte)
                /// @param  [out] waveData      Return the retrieved wave data
                ///
                /// @return Whether operation is successful
                ///
                virtual bool getWave(const std::wstring &wstrPhoneme, uint8 *waveData, uint32 &waveLen) const = 0;

                ///
                /// @brief  Get wave length for the input phoneme
                ///
                /// @param  [in]  wstrPhoneme   The input phoneme whose wave data is to be returned
                ///
                /// @return The wave length of the specified speech unit (Unit: uint8/byte)
                ///
                virtual uint32 getWaveLength(const std::wstring &wstrPhoneme) const = 0;

            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Wave synthesizer general information manipulation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Get the detailed description of the wave synthesizer
                ///
                virtual const CDescriptor &getDescriptor() const {return m_descriptor;}

                ///
                /// @brief  Get the wave sampling rate (samples per second)
                ///
                virtual int getSamplesPerSec() const = 0;

                ///
                /// @brief  Get the wave sampling precision (bits per sample)
                ///
                virtual int getBitsPerSample() const = 0;

                ///
                /// @brief  Get the wave channel number
                ///
                virtual int getChannels() const = 0;

            protected:
                bool m_bInitialized;        ///< Whether the wave synthesizer is initialized or not
                CDescriptor m_descriptor;   ///< The detailed description of the wave synthesizer

            private:
                ///
                /// @brief  Declared to prevent calling and implementation
                ///
                CWavSynthesizer &operator = (const CWavSynthesizer &);
                ///
                /// @brief  Declared to prevent calling and implementation
                ///
                CWavSynthesizer(const CWavSynthesizer &);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_WAV_SYNTHESIZER_H_
