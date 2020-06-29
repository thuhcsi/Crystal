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
/// @brief  Head file for wave device manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2006/02/20
///   Changed:  Created
///


#ifndef _CST_TOOLS_DSP_WAVEDEVICE_H_
#define _CST_TOOLS_DSP_WAVEDEVICE_H_

#ifndef WAVE_MAPPER
#define WAVE_MAPPER ((unsigned int)-1)  // device ID for wave device mapper
#endif

namespace cst
{
    namespace dsp
    {
        ///
        /// @brief  The class for wave out device manipulation (output waveform to device)
        ///
        class CWaveOut
        {
        public:
            ///
            /// @brief  Constructor
            ///
            CWaveOut();

            ///
            /// @brief  Destructor
            ///
            virtual ~CWaveOut();

        public:
            ///
            /// @brief  Open the wave out device
            ///
            /// @param  [in] devID          Wave out device ID to be opened. WAVE_MAPPER is used to automatically select the device.
            /// @param  [in] nSamplesPerSec Sampling rate of the waveform data to be output
            /// @param  [in] nBitsPerSample Bits per sample of the waveform data to be output
            /// @param  [in] nChannel       Channel number of the waveform data to be output
            ///
            /// @return Whether operation is successful
            ///
            bool open(unsigned int devID=WAVE_MAPPER, int nSamplesPerSec=16000, int nBitsPerSample=16, int nChannels=1);

            ///
            /// @brief  Close the wave out device
            ///
            bool close();

            ///
            /// @brief  Output (play) the waveform data to the wave out device
            ///
            /// @param  [in] pData      The waveform data to be output to device
            /// @param  [in] nByteLen   Byte length of the waveform data
            ///
            bool write(const void* pData, unsigned int nByteLen);

            ///
            /// @brief  Stop playing the wave data (stop the wave out device)
            ///
            bool stop();

            ///
            /// @brief  Pause playing the wave data (pause the wave out device) 
            ///
            bool pause();

            ///
            /// @brief  Resume playing the wave data (resume the paused wave out device)
            ///
            bool resume();

            ///
            /// @brief  Wait until wave data playing is finished (wait until the wave out device is idle and ready for next wave data output)
            ///
            bool wait();

            ///
            /// @brief  Indicate whether wave out device is opened successfully
            ///
            bool isOpened();

            ///
            /// @brief  Indicate whether it is still outputting the waveform data
            ///
            bool isWorking();

        protected:
            unsigned int m_nDevID;  ///< ID of the wave device, usually WAVE_MAPPER is used
            void* m_hDevice;        ///< Handle to the opened wave device
            void* m_pWaveHdr;       ///< Wave header
        public:
            bool  m_bWorking;       ///< Indicating whether is still outputting wave data

        protected:
            ///
            /// @brief  Callback function to access the working status of the wave out device
            ///
            //static void wavOutProc(void* hwo, unsigned int uMsg, unsigned long* dwInstance, unsigned long dwParam1, unsigned long dwParam2);
        };

    }
}

#endif//_CST_TOOLS_DSP_WAVEDEVICE_H_
