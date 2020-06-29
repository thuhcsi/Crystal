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
/// @brief  Head file for functions of wave data manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSP_WAVEDATA_H_
#define _CST_TTS_BASE_DSP_WAVEDATA_H_

#include "dsp_databuffer.h"

namespace cst
{
    ///
    /// @brief  The namespace for dsp (digital signal processing) related manipulation
    ///
    /// Currently, the audio (speech) signal processing modules are included.
    ///
    namespace dsp
    {
        ///
        /// @brief  The class which manipulates the waveform-audio data automatically
        ///
        class CWaveData : public CDataBuffer
        {
        public:
            ///
            /// @brief  Default constructor
            ///
            CWaveData () {setFormat(16000, 16, 1);}

            ///
            /// @brief  Copy constructor
            ///
            CWaveData(const CWaveData &right);

            ///
            /// @brief  Destructor, clear the data
            ///
            virtual ~CWaveData() {clear();}

            ///
            /// @brief  Assignment operator =, assign right operand to left operand.
            ///
            CWaveData &operator = (const CWaveData &right);

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  Data related procedures
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Call this function to make sure the data and the length is correctly aligned.
            ///
            /// If bits per sample of the audio data is 16, and the data size (length in byte) is not even,
            /// then the data is mis-aligned. User can call this function to correct the alignment.
            ///
            /// A <b>zero byte will be appended</b> if the data is mis-aligned.
            ///
            /// @return Whether operation is successful
            ///
            /// @see    assignData, appendData, resize
            ///
            bool alignData();

            ///
            /// @brief  Get the sample value at current data position
            ///
            /// @param  [in] sampleIndex  The index position of current sample to be retrieved (unit in samples)
            ///
            /// @return The sample value at current data position
            ///
            int getSampleValue( unsigned long sampleIndex ) const;

            ///
            /// @brief  Set the sample value to the data pointer
            ///
            /// @param  [in]  sampleIndex  The index position of current sample to be set (unit in samples)
            /// @param  [in]  sampleValue  The value to be set to the wave sample
            ///
            void setSampleValue( unsigned long sampleIndex, int sampleValue );

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  Format related procedures
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Set the format of the audio data
            ///
            /// Call this function before adding (setting) any data.
            /// All the stored data will be cleared after calling this function.
            ///
            /// @param  [in] nSamplesPerSec Sample rat, in samples per second (hertz, Hz)
            /// @param  [in] wBitsPerSample Bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
            /// @param  [in] nChannels      Number of channels in the waveform-audio data
            /// @param  [in] wFormat        Waveform-audio format type. Currently, only WAVE_FORMAT_PCM(=1) is supported
            ///
            /// @return Whether operation is successful
            ///
            bool setFormat(unsigned long nSamplesPerSec, unsigned short wBitsPerSample, unsigned short nChannels, unsigned short wFormat=1/*WAVE_FORMAT_PCM*/);

            ///
            /// @brief  Get the waveform-audio format type, currently only WAVE_FORMAT_PCM is supported
            ///
            unsigned short getFormatTag() const {return m_wFormatTag;}

            ///
            /// @brief  Get the number of channels in the waveform-audio data
            ///
            unsigned short getChannels() const {return m_nChannels;}

            ///
            /// @brief  Get the bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
            ///
            unsigned short getBitsPerSample() const {return m_wBitsPerSample;}

            ///
            /// @brief  Get the sample rat, in samples per second (hertz, Hz)
            ///
            unsigned long  getSamplesPerSec() const {return m_nSamplesPerSec;}

        protected:
            unsigned short  m_wFormatTag;       ///< Waveform-audio format type, currently only WAVE_FORMAT_PCM is supported
            unsigned short  m_nChannels;        ///< Number of channels in the waveform-audio data
            unsigned long   m_nSamplesPerSec;   ///< Sampling rate, in samples per second (hertz, Hz)
            unsigned short  m_wBitsPerSample;   ///< Bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
        };
    }
}

#endif//_CST_TTS_BASE_DSP_WAVEDATA_H_
