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
/// @brief  Implementation file for concatenation synthesis interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/13
///   Changed:  Modified the interface to use CWaveData directly
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///


#include "../datavoice/data_voicedata.h"
#include "splib_speechlib.h"
#include "synth_concatenate.h"
#include "xml/ssml_helper.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            //////////////////////////////////////////////////////////////////////////
            //
            //  Operations for performing the concatenation synthesis
            //
            //////////////////////////////////////////////////////////////////////////

            int CSynthConcatenate::synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData)
            {
                // get phoneme internal code
                const CSpeechLib *splib = (CSpeechLib*)&getDataManager<CVoiceData>()->getWavSynthesizer();
                for (std::vector<CUnitItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                    it->phonemeID = splib->getICodeFromPhoneme(it->wstrPhoneme);

                // perform unit selection first
                int flag = selectUnit(sentenceInfo);
                if (flag != ERROR_SUCCESS)
                    return flag;

                // perform concatenate synthesis
                int samplesPerSec = splib->getSamplesPerSec();
                int bitsPerSample = splib->getBitsPerSample();
                int channels      = splib->getChannels();

                // navigate all unit or break items
                dsp::CWaveData waveBuffer;
                waveBuffer.setFormat(samplesPerSec, bitsPerSample, channels);
                for (std::vector<CUnitItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    CUnitItem &tgtUnit = *it;

                    if (tgtUnit.isBreak)
                    {
                        // synthesize break
                        uint32 silenceLength = (uint32)(tgtUnit.duration / 1000 * samplesPerSec * bitsPerSample * channels / 16) * 2; // align to even
                        if (!waveData.appendData(NULL, silenceLength))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                    else if (tgtUnit.unitID == INVALID_UNITID)
                    {
                        // unit not found in speech library
                        // fill silence with specified length directly
                        uint32 silenceLength = (uint32)(tgtUnit.duration / 1000 * samplesPerSec * bitsPerSample * channels / 16) * 2; // align to even
                        if (!waveData.appendData(NULL, silenceLength))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                    else
                    {
                        // synthesize basic unit
                        uint32 waveLength = splib->getWaveLength(tgtUnit.phonemeID, tgtUnit.unitID);
                        if (!waveBuffer.resize(waveLength))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                        // get data from speech library
                        if (!splib->getWave(tgtUnit.phonemeID, tgtUnit.unitID, waveBuffer.getData(), waveLength))
                        {
                            return ERROR_DATA_READ_FAULT;
                        }
                        waveBuffer.resize(waveLength);
                        // append data
                        if (!waveData.appendData(waveBuffer.getData(), waveBuffer.getLength()))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                }
                return ERROR_SUCCESS;
            }

            int CSynthConcatenate::selectUnit(std::vector<CUnitItem> &sentenceInfo)
            {
                // the default implementation of unit selection
                // just select the first unit as result

                const CSpeechLib *splib = (CSpeechLib*)&getDataManager<CVoiceData>()->getWavSynthesizer();

                for (std::vector<CUnitItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    CUnitItem &unitInfo = *it;

                    // search the speech unit with wave data
                    uint idx, candNum = splib->getUnitNumber(unitInfo.phonemeID);
                    for (idx = 0; idx < candNum; idx ++)
                        if (splib->getWaveLength(unitInfo.phonemeID, idx) > 0)
                            break;

                    // set INVALID_UNITID(-1) if no unit with wave found
                    if (idx == candNum)
                        unitInfo.unitID = INVALID_UNITID;
                    else
                        unitInfo.unitID = idx;
                }

                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
