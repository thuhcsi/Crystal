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
/// @brief  Head file for speech related digital signal processing algorithm functions
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Cong Honglei (conghonglei@gmail.com)
///   Date:     2007/06/13
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Refined to static function for all signal processing algorithms
///


#ifndef _CST_TTS_BASE_DSP_DSPALGORITHM_H_
#define _CST_TTS_BASE_DSP_DSPALGORITHM_H_

namespace cst
{
    namespace dsp
    {
        // forward class reference
        class CWaveData;

        ///
        /// @brief  The class which manipulates the signal processing algorithms
        ///
        class CDSPAlgorithm
        {
        public:
            ///
            /// @brief  Amplify the wave data from start to end in samples
            ///
            /// @param  [in]  sampleStart   The start position of wave data to amplify, in samples
            /// @param  [in]  sampleEnd     The end position of wave data to amplify, in samples
            /// @param  [in]  ampScale      The amplify scale value
            /// @param  [in]  waveData      The input wave data
            /// @param  [out] waveData      Return the wave data with window multiplied
            ///
            static bool amplify(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd, float ampScale);

            ///
            /// @brief  Multiply a normal (whole) Hanning window on the wave data from start to end in samples
            ///
            /// @param  [in]  sampleStart   The start position of wave data to multiply window, in samples
            /// @param  [in]  sampleEnd     The end position of wave data to multiply window, in samples
            /// @param  [in]  waveData      The input wave data
            /// @param  [out] waveData      Return the wave data with window multiplied
            ///
            static bool hanningNormal(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd);

            ///
            /// @brief  Multiply a up hill Hanning window on the wave data from start to end in samples
            ///
            /// @param  [in]  sampleStart   The start position of wave data to multiply window, in samples
            /// @param  [in]  sampleEnd     The end position of wave data to multiply window, in samples
            /// @param  [in]  waveData      The input wave data
            /// @param  [out] waveData      Return the wave data with window multiplied
            ///
            static bool hanningUp(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd);

            ///
            /// @brief  Multiply a down hill Hanning window on the wave data from start to end in samples
            ///
            /// @param  [in]  sampleStart   The start position of wave data to multiply window, in samples
            /// @param  [in]  sampleEnd     The end position of wave data to multiply window, in samples
            /// @param  [in]  waveData      The input wave data
            /// @param  [out] waveData      Return the wave data with window multiplied
            ///
            static bool hanningDown(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd);

            ///
            /// @brief  Multiply the window to the wave data, starting from offset with the length (both in samples)
            ///
            /// @param  [in]  pWindow       The window to be added to the wave data
            /// @param  [in]  sampleOffset  The offset position from start of the wave data, in samples
            /// @param  [in]  sampleLength  The length of the window, in samples
            /// @param  [in]  waveData      The input wave data
            /// @param  [out] waveData      Return the wave data with window multiplied
            ///
            /// @return Whether operation is successful
            ///
            static bool multiplyWindow(CWaveData &waveData, float *pWindow, unsigned long sampleOffset, unsigned long sampleLength);

            ///
            /// @brief  Apply the Hanning overlap add of two wave data
            ///
            /// The left wave data will be applied "hanningUp" window first,
            /// the right wave data will be applied "hanningDown" window first,
            /// then the two wave data are overlapping added.
            ///
            /// @param  [in]  waveLeft  The left wave data to be overlap added
            /// @param  [in]  waveRight The right wave data to be overlap added
            /// @param  [out] waveOut   Return the wave data out
            ///
            static bool hanningOverlap(const CWaveData &waveLeft, const CWaveData &waveRight, CWaveData &waveOut);

            ///
            /// @brief  Overlap add the wave data "waveFrom" to wave data "waveTo"
            ///
            /// After adding, the returned data "waveTo" is the sample sum of the two wave data.<br>
            /// The procedure means: waveTo = waveTo + waveFrom.
            ///
            /// @note   Before adding, please make sure the length of "waveTo" is not smaller than "waveFrom".
            ///         Otherwise, procedure will return error.
            ///
            /// @param  [in]  waveTo    The to wave data, left operand
            /// @param  [in]  waveFrom  The from wave data, right operand
            /// @param  [out] waveTo    Return the resulting sum wave data
            ///
            /// @return Whether operation is successful
            ///
            static bool overlapAdd(CWaveData &waveTo, const CWaveData &waveFrom);

        protected:
            ///
            /// @brief  Get and calculate the Hanning window given the window width
            ///
            /// @param  [in]  nLength   The window length
            /// @param  [out] pHanning  Return the window values
            ///
            static void getHanningWindow(float *pHanning, unsigned long nLength);

            ///
            /// @brief  Get and calculate the Hanning up window (the up side hill)
            ///
            static void getHanningUpHill(float *pHanning, unsigned long nLength);

            ///
            /// @brief  Get and calculate the Hanning down window (the down side hill)
            ///
            static void getHanningDownHill(float *pHanning, unsigned long nLength);

        protected:
            ///
            /// @brief  The PI definition for mathematical calculating
            ///
            static const double PI;
        };
    }
}

#endif//_CST_TTS_BASE_DSP_DSPALGORITHM_H_
