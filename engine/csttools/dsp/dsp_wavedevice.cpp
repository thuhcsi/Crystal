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
/// @brief  Implementation file for wave device manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2006/02/20
///   Changed:  Created
///


#ifdef WIN32
#   include <windows.h>
#   include <mmsystem.h>
#   pragma comment(lib, "winmm.lib")
#   pragma comment(lib, "user32.lib")
#   pragma message("WIN32: Under Microsoft Windows Operation System")
#   pragma message("WIN32: Linking with winmm.lib")
#else
#   error "Sorry for not support OS except Windows Now:>"
#endif

#include "dsp_wavedevice.h"

namespace cst
{
    namespace dsp
    {
        //////////////////////////////////////////////////////////////////////////
        //
        //  WaveOut manipulation
        //
        //////////////////////////////////////////////////////////////////////////

        //void CWaveOut::wavOutProc(void* hwo, unsigned int uMsg, unsigned long* dwInstance, unsigned long dwParam1, unsigned long dwParam2)
        void CALLBACK wavOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2)
        {
            CWaveOut* pWaveOut = (CWaveOut*)dwInstance;
            if (uMsg == WOM_DONE)
                pWaveOut->m_bWorking = false;
            // written as this to avoid include <windows.h> and <mmsystem.h> in head file
            // the declaration SHOULD actually be:
            // void CALLBACK wavOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2)
        }

        CWaveOut::CWaveOut() : m_hDevice(NULL), m_pWaveHdr(NULL), m_nDevID(0), m_bWorking(false)
        {
        }

        CWaveOut::~CWaveOut()
        {
            stop();
            close();
        }

        bool CWaveOut::isOpened()
        {
            return (m_hDevice != NULL);
        }

        bool CWaveOut::isWorking()
        {
            return (m_hDevice != NULL) && (m_bWorking);
        }

        bool CWaveOut::open(unsigned int devID, int nSamplesPerSec, int nBitsPerSample, int nChannels)
        {
            if (m_hDevice != NULL)
            {
                if (m_nDevID == devID)
                    return true;    // already opened, just return
                else
                    return false;   // already opened, can not open another different device
            }
            WAVEFORMATEX waveFormat;
            MMRESULT     waveResult;
            WAVEOUTCAPS  waveOutDevCaps;
            HWAVEOUT     waveOut;
            // Wave header
            m_pWaveHdr = (WAVEHDR*)malloc(sizeof(WAVEHDR));
            if (m_pWaveHdr == NULL)
            {
                m_pWaveHdr = NULL;
                m_hDevice  = NULL;
                m_nDevID   = 0;
                m_bWorking = false;
                return false;
            }
            // Prepare the wave format
            waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
            waveFormat.nChannels       = nChannels;
            waveFormat.nSamplesPerSec  = nSamplesPerSec;
            waveFormat.wBitsPerSample  = nBitsPerSample;
            waveFormat.nAvgBytesPerSec = nSamplesPerSec*nChannels*nBitsPerSample/8;
            waveFormat.nBlockAlign     = nChannels*nBitsPerSample/8;
            waveFormat.cbSize          = 0;
            // Check the capability
            waveResult = waveOutGetDevCaps(devID, &waveOutDevCaps, sizeof(WAVEOUTCAPS));
            if (waveResult != MMSYSERR_NOERROR)
            {
                free(m_pWaveHdr);
                m_pWaveHdr = NULL;
                m_hDevice  = NULL;
                m_nDevID   = 0;
                m_bWorking = false;
                return false;
            }
            // Open waveOut device
            waveResult = waveOutOpen(&waveOut, devID, (WAVEFORMATEX*)&waveFormat, (DWORD_PTR)wavOutProc, (DWORD_PTR)this ,CALLBACK_FUNCTION);
            if (waveResult != MMSYSERR_NOERROR)
            {
                free(m_pWaveHdr);
                m_pWaveHdr = NULL;
                m_hDevice  = NULL;
                m_nDevID   = 0;
                m_bWorking = false;
                return false;
            }
            m_hDevice  = waveOut;
            m_nDevID   = devID;
            m_bWorking = false;
            return true;
        }

        bool CWaveOut::close()
        {
            if (m_hDevice == NULL)
                return true;
            // stop first
            if (!stop())
                return false;
            // close device
            waveOutUnprepareHeader((HWAVEOUT)m_hDevice, (WAVEHDR*)m_pWaveHdr, sizeof(WAVEHDR));
            waveOutClose((HWAVEOUT)m_hDevice);
            free(m_pWaveHdr);
            m_pWaveHdr = NULL;
            m_hDevice  = NULL;
            m_nDevID   = 0;
            return true;
        }

        bool CWaveOut::write(const void* pData, unsigned int nByteLen)
        {
            if (m_hDevice == NULL)
                return false;
            MMRESULT waveResult;
            // Prepare wave header
            WAVEHDR* pWaveHdr = (WAVEHDR*)m_pWaveHdr;
            pWaveHdr->dwBufferLength = nByteLen;
            pWaveHdr->lpData         = (LPSTR)pData;
            pWaveHdr->dwFlags        = 0;
            waveResult = waveOutPrepareHeader((HWAVEOUT)m_hDevice, pWaveHdr, sizeof(WAVEHDR)); 
            if ( (waveResult != MMSYSERR_NOERROR) || (pWaveHdr->dwFlags != WHDR_PREPARED) )
                return false;
            // WaveOut play
            waveResult = waveOutWrite((HWAVEOUT)m_hDevice, pWaveHdr, sizeof(WAVEHDR));
            if (waveResult!=MMSYSERR_NOERROR) 
            {
                waveOutUnprepareHeader((HWAVEOUT)m_hDevice, (WAVEHDR*)m_pWaveHdr, sizeof(WAVEHDR));
                return false;
            }
            m_bWorking = true;
            return true;
        }

        bool CWaveOut::stop()
        {
            if (m_hDevice == NULL)
                return false;
            waveOutReset((HWAVEOUT)m_hDevice);
            m_bWorking= false;
            return true;
        }

        bool CWaveOut::pause()
        {
            if (m_hDevice == NULL)
                return false;
            waveOutPause((HWAVEOUT)m_hDevice);
            return true;
        }

        bool CWaveOut::resume()
        {
            if (m_hDevice == NULL)
                return false;
            waveOutRestart((HWAVEOUT)m_hDevice);
            return true;
        }

        bool CWaveOut::wait()
        {
            if (m_hDevice == NULL)
                return false;
            while (m_bWorking)
            {
                Sleep(10);
            }
            return true;
        }

    }
}
