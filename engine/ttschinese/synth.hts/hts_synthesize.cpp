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
/// @brief  Implementation file for HMM based speech synthesis of Chinese TTS engine using HTS synthesizer
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///


#include "data_voicedata.h"
#include "hts_synthesize.h"
#include <cmath>

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            int CHtsSynthesize::synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData)
            {
                // convert to segment information
                std::vector<CSegInfo> segInfo;
                CSSML2Lab::buildLabInfo(sentenceInfo, segInfo);

                // get LAB string for HTS engine
                std::string labString;
                for (std::vector<CSegInfo>::const_iterator it = segInfo.begin(); it != segInfo.end(); ++it)
                {
                    labString += it->asLabel();
                }

                // synthesize speech with HTS engine
                const CVoiceData *dataManager = base::CSynthesize::getDataManager<CVoiceData>();
                hts::CHtsSynthesizer *htsEngine = (hts::CHtsSynthesizer*)&(dataManager->getWavSynthesizer());
                hts::SynthCfg htsOut;
                htsOut.outWavData = true;
                htsOut.speedRate  = dataManager->getGlobalSetting().rate;
                htsOut.volumeRate = dataManager->getGlobalSetting().volume;
                htsOut.halfTone   = 12.0f*log(dataManager->getGlobalSetting().pitch)/log(2.0f); // change pitch (f0*=2 if half_tone=12; f0/=2 if half_tone=-12)
                bool succ = htsEngine->synthesize(labString.c_str(), htsOut, false);
                if (!succ)
                    return ERROR_DATA_READ_FAULT;

                // return waveform
                if (htsOut.wavData.size() > 0)
                {
                    waveData.appendData((const byte*)&htsOut.wavData[0], htsOut.wavData.size()*sizeof(short));
                }

                // save begin, end time for each segment
                std::vector<int>::const_iterator bit = htsOut.segBeg.begin();
                std::vector<int>::const_iterator eit = htsOut.segEnd.begin();
                for (std::vector<CSegInfo>::iterator it = segInfo.begin(); it != segInfo.end(); ++it, ++bit, ++eit)
                {
                    it->begTime = *bit;
                    it->endTime = *eit;
                }

                // save duration for each unit
                for (std::vector<CSegInfo>::iterator it = segInfo.begin(); it != segInfo.end(); )
                {
                    CUnitItem *unit = it->pUnit;
                    float dur  = 0;
                    while (it != segInfo.end() && it->pUnit == unit)
                    {
                        dur += (it->endTime - it->begTime) / (float)1e4;
                        it++;
                    }
                    unit->duration = dur;
                }

                return ERROR_SUCCESS;
            }

        }//namespace Chinese
    }
}
