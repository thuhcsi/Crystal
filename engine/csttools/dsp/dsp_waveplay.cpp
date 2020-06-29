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
/// @brief  Implementation file for wave play and device manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#if defined(WIN32)
#   include <windows.h>
#   include <mmsystem.h>
#   pragma comment(lib, "winmm.lib")
#   pragma message("WIN32: Under Microsoft Windows Operation System")
#   pragma message("WIN32: Linking with winmm.lib")
#elif defined(WINCE)
#   include <windows.h>
//#   include <mmsystem.h>
//#   pragma comment(lib, "winmm.lib")
#elif defined(__GNUC__)
#include <stdio.h>
#include <cstdlib>      // wcstombs()
#include <sys/ioctl.h>  // ioctl()
#   include "utils/utl_string.h"
#   include "dsp/dsp_wavefile.h"
#define AUDIO_DEVICE    "/dev/dsp"
//#include <alsa/asoundlib.h>
#else
#   error "Sorry for not support OS:>"
#endif//WIN32

#include "dsp_waveplay.h"
#include "dsp_wavedevice.h"
#include "dsp_wavedata.h"


namespace cst
{
    namespace dsp
    {
        bool CWavePlay::playWaveFile(const wchar_t *fileName, int playMode)
        {
#if defined(WIN32)
            unsigned long mode = (playMode==ModeSync) ? (SND_FILENAME|SND_SYNC|SND_NODEFAULT) : (SND_FILENAME|SND_ASYNC|SND_NODEFAULT);
            if (PlaySoundW(fileName, NULL, mode))
                return true;
            else
                return false;
#elif defined(WINCE)
            unsigned long mode = (playMode==ModeSync) ? SND_SYNC : SND_ASYNC;
            if (sndPlaySound(fileName, mode))
                return true;
            else
                return false;
#elif defined(__GNUC__)
            /*
            // Handle for the PCM device
            snd_pcm_t *pcm_handle;
            // Playback stream
            snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
            // This structure contains information about the hardware and can be used to specify the configuration to be used for the PCM stream.
            snd_pcm_hw_params_t *hwparams;
            */

            // @todo binbinsh: better linux wav player
            char cCommand[1000];
            char cFileName[1000];
            wcstombs(cFileName, fileName, 1000);
            sprintf(cCommand, "mplayer %s 2>&1 > /dev/null", cFileName);
            if(system(cCommand))
                return true;
            else
                return false;
#else
            return false;
#endif
        }

#if defined(WIN32)
        CWaveOut  g_waveOutDev;
        CWaveData g_waveData;
#endif

        bool CWavePlay::playWaveData(const CWaveData &waveData, int playMode)
        {
#if defined(WIN32)
            if (playMode & ModeWait)
            {
                // wait until previous waveform play finishes
                g_waveOutDev.wait();
            }
            // wave data should be saved here
            // as the input waveData might be cleared in the caller function
            g_waveData = waveData;
            // open the new wave out device
            if (!g_waveOutDev.open(WAVE_MAPPER, g_waveData.getSamplesPerSec(), g_waveData.getBitsPerSample(), g_waveData.getChannels()))
            {
                return false;
            }
            // output waveform data
            if (!g_waveOutDev.write(g_waveData.getData(), g_waveData.getLength()))
            {
                g_waveOutDev.close();
                return false;
            }
            // play synchronously or not
            if (playMode & ModeSync)
            {
                // wait until current waveform play finishes
                g_waveOutDev.wait();
            }
            return true;
#elif defined(__GNUC__)
            char tmp[20] = "wav.tmp.XXXXXX.wav";
            mkstemps(tmp, 4);
            std::wstring wavFile = str::mbstowcs(tmp);
            dsp::CWaveFile::save(wavFile.c_str(), waveData, dsp::CWaveFile::modeWrite);
            playWaveFile(wavFile.c_str(), ModeSync);
            remove(tmp);
            return true;
#else
            return false;
#endif
        }
    }
}
