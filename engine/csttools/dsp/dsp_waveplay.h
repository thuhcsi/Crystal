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
/// @brief  Head file for wave play and device manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSP_WAVEPLAY_H_
#define _CST_TTS_BASE_DSP_WAVEPLAY_H_

namespace cst
{
    namespace dsp
    {
        // forward class reference
        class CWaveData;

        ///
        /// @brief  The class which manipulates the signal processing algorithms
        ///
        class CWavePlay
        {
        public:
            ///
            /// @brief  The wave data play mode
            ///
            enum EPlayMode
            {
                ModeAsync = 0x00,       ///< Play asynchronously and function returns immediately after beginning sound
                ModeSync  = 0x01,       ///< Play synchronously and function does not return until sound ends
                ModeWait  = 0x02,       ///< Start play current data ONLY when the previous wave data has been played
            };

        public:
            ///
            /// @brief  Play the waveform specified by the file name
            ///
            /// @param  [in] fileName   The name of the file containing the waveform. NULL to stop current playing waveform.
            /// @param  [in] playMode   The waveform play mode: Synchronously or asynchronously
            ///
            static bool playWaveFile(const wchar_t *fileName, int playMode);

            ///
            /// @brief  Play the waveform data in the data buffer
            ///
            /// @param  [in] waveData   The waveform data to be played
            /// @param  [in] playMode   The waveform play mode: Synchronously or asynchronously and if wait until previous wave play stops
            ///
            static bool playWaveData(const CWaveData &waveData, int playMode);
        };
    }
}

#endif//_CST_TTS_BASE_DSP_WAVEPLAY_H_
