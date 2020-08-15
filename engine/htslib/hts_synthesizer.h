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
/// @brief  Head file for HTS synthesizer (HMM based speech synthesis)
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///

#ifndef _CST_TTS_HTS_SYNTHESIZER_H_
#define _CST_TTS_HTS_SYNTHESIZER_H_

#include "HTS_engine.h"
#include <vector>
#include <string>
#include <map>

namespace cst
{
    namespace tts
    {
        namespace hts
        {
            ///
            ///	@brief  Configuration for the HTS engine
            ///
            struct HtsCfg
            {
                HtsCfg() : samplingRate(16000),framePeriod(80),valAlpha(0.42f),valGamma(0),valBeta(0.0f),uvThreshold(0.5f),gvWeightMgc(1.0f),gvWeightLf0(1.0f),gvWeightLpf(1.0f),useLogGain(false) {}
                std::vector<std::string> fnTreeDur;     ///< -td tree: Decision tree files for state duration
                std::vector<std::string> fnTreeMgc;     ///< -tm tree: Decision tree files for spectrum
                std::vector<std::string> fnTreeLf0;     ///< -tf tree: Decision tree files for Log F0
                std::vector<std::string> fnTreeLpf;     ///< -tl tree: Decision tree files for low-pass filter
                std::vector<std::string> fnModelDur;    ///< -md pdf:  Model files for state duration
                std::vector<std::string> fnModelMgc;    ///< -mm pdf:  Model files for spectrum
                std::vector<std::string> fnModelLf0;    ///< -mf pdf:  Model files for Log F0
                std::vector<std::string> fnModelLpf;    ///< -ml pdf:  Model files for low-pass filter
                std::vector<std::string> fnTreeGVMgc;   ///< -em tree: Decision tree files for GV of spectrum
                std::vector<std::string> fnTreeGVLf0;   ///< -ef tree: Decision tree files for GV of Log F0
                std::vector<std::string> fnTreeGVLpf;   ///< -el tree: Decision tree files for GV of low-pass filter
                std::vector<std::string> fnModelGVMgc;  ///< -cm pdf:  Filenames of GV for spectrum
                std::vector<std::string> fnModelGVLf0;  ///< -cf pdf:  Filenames of GV for Log F0
                std::vector<std::string> fnModelGVLpf;  ///< -cl pdf:  Filenames of GV for low-pass filter
                std::vector<std::string> fnWinMgc;      ///< -dm win:  Window files for calculation delta of spectrum
                std::vector<std::string> fnWinLf0;      ///< -df win:  Window files for calculation delta of Log F0
                std::vector<std::string> fnWinLpf;      ///< -dl win:  Window files for calculation delta of low-pass filter
                std::vector<float>       rateInterp;    ///< -i  i f1 ... fi: Enable interpolation & specify number(i), coefficient(f), [1--], DEF [1]
                int   samplingRate;                     ///< -s  i:    Sampling frequency,                                         [1--48000], DEF [16000]
                int   framePeriod;                      ///< -p  i:    Frame period (point),                                            [1--], DEF [80]
                float valAlpha;                         ///< -a  f:    All-pass constant,                                          [0.0--1.0], DEF [0.42]
                int   valGamma;                         ///< -g  i:    Gamma = -1/i (if i=0 then gamma=0),                              [0--], DEF [0]
                float valBeta;                          ///< -b  f:    Post-filtering coefficient,                                [-0.8--0.8], DEF [0.0]
                float uvThreshold;                      ///< -u  f:    Voice/unvoiced threshold,                                   [0.0--1.0], DEF [0.5]
                float gvWeightMgc;                      ///< -jm f:    Weight of GV for spectrum,                                  [0.0--2.0], DEF [1.0]
                float gvWeightLf0;                      ///< -jf f:    Weight of GV for Log F0,                                    [0.0--2.0], DEF [1.0]
                float gvWeightLpf;                      ///< -jl f:    Weight of GV for log-pass filter,                           [0.0--2.0], DEF [1.0]
                bool  useLogGain;                       ///< -l:       Regard input as log gain and output linear one (LSP)
                std::string fnGVSwitch;                 ///< -k  tree: GV switch
                std::string fnModelUVProp;              ///< -mu pdf:  Model file for unvoiced proportion
            };

            struct SynthCfg
            {
                SynthCfg() : usePhoneAlignment(false),volumeRate(1),speedRate(1),halfTone(0),outWavData(false) {}
                bool  usePhoneAlignment;    ///< -vp:   Whether use phoneme alignment for duration
                float volumeRate;           ///< -v  f: Volume rate,                                   [>=0.0], DEF [1.0]
                float speedRate;            ///< -r  f: Speech speech rate,                        [0.0--10.0], DEF [1.0]
                float halfTone;             ///< -fm f: Add half-tone (change pitch/f0),          [-24.0-24.0], DEF [0.0]
                std::string fnOutDur;       ///< -od s: Filename of output label with duration
                std::string fnOutMgc;       ///< -om s: Filename of output spectrum
                std::string fnOutLf0;       ///< -of s: Filename of output Log F0
                std::string fnOutLpf;       ///< -ol s: Filename of output low-pass filter
                std::string fnOutRaw;       ///< -or s: Filename of output raw audio (generated speech)
                std::string fnOutWav;       ///< -ow s: Filename of output wav audio (generated speech)
                std::string fnOutTrace;     ///< -ot s: Filename of output trace information
                bool outWavData;            ///< Whether will output raw wave data in wavData
                std::vector<short> wavData; ///< To retrieve raw wave data if needed
                std::vector<int> segBeg;    ///< To retrieve segment begin time (in HTS format)
                std::vector<int> segEnd;    ///< To retrieve segment end time (in HTS format)
            };

            ///
            ///	@brief  Class for HTS synthesizer (HMM based speech synthesis)
            ///
            class CHtsSynthesizer
            {
            public:
                /// Constructor
                CHtsSynthesizer() : initialized(false) {}

                /// Destructor
                virtual ~CHtsSynthesizer() {close();}

            public:
                ///
                ///	@brief  Open the HTS synthesizer
                ///
                /// @param  [in] modelPath  Path name of the voice models
                /// @param  [in] configFile File name of the configuration of the voices
                ///
                bool open(const char *modelPath, const char *configFile);

                ///
                ///	@brief  Open the HTS synthesizer
                ///
                /// @param  [in] cfg    Configurations of the HTS synthesizer
                ///
                bool open(const HtsCfg &cfg);

                ///
                ///	@brief  Close the HTS synthesizer
                ///
                bool close();

                ///
                ///	@brief  Synthesize the speech from the input label file or label string
                ///
                /// @param  [in]  inLabel   The input label information (label file or label string)
                /// @param  [in]  synCfg    Configuration for speech generation
                /// @param  [in]  isFile    true if the input inLabel is label file, or false for label string
                ///
                bool synthesize(const char *inLabel, SynthCfg &synCfg, bool isFile=true);

                ///
                /// @brief  Return the sampling rate of the synthetic speech
                ///
                int getSampleRate() const;

            protected:

                ///
                ///	@brief  Modify the unvoiced proportion according to the input probability
                ///
                void modifyUnvoicedProportion();

                ///
                ///	@brief  Load the HTS engine configurations from file
                ///
                /// @param  [in]  modelPath     Path name of the voice models
                /// @param  [in]  configFile    File name of the configuration of the voices
                /// @param  [out] htsCfg        Output the configurations of the HTS engine
                ///
                bool loadConfig(const char *modelPath, const char *configFile, HtsCfg &htsCfg);

                ///
                /// @brief  Create buffer for storing char*
                ///
                char** newStrBuf(const std::vector<std::string> &inStr);

            protected:
                bool initialized;                       ///< Whether HTS engine is initialized
                HTS_Engine engine;                      ///< HTS engine
                std::map<std::string, float> uvProps;   ///< Unvoiced proportions for each phoneme
            };

        }//namespace base
    }
}

#endif//_CST_TTS_HTS_SYNTHESIZER_H_
