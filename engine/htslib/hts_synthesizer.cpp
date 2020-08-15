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
/// @brief  Implementation file for HTS synthesizer (HMM based speech synthesis)
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///


#include "hts_synthesizer.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace cst
{
    namespace tts
    {
        namespace hts
        {
            bool CHtsSynthesizer::loadConfig(const char *modelPath, const char *configFile, HtsCfg &htsCfg)
            {
                std::string strPath = modelPath; strPath += "/";

                // load arguments from configuration file
                std::vector<std::string> args;
                {
                    std::ifstream fin((strPath+configFile).c_str());
                    if (!fin)
                        return false;
                    std::string tmpstr;
                    while (std::getline(fin, tmpstr))
                    {
                        const char* pstr = tmpstr.c_str();
                        size_t i = 0, j = 0, len = tmpstr.length();
                        while (i<len)
                        {
                            for (; i<len && isspace(pstr[i]); i++);     // skip leading spaces
                            for (j=i; i<len && !isspace(pstr[i]); i++); // get argument
                            args.push_back(tmpstr.substr(j, i-j));
                        }
                    }
                }

                // parse arguments
                size_t num_interp = 0;
                for (size_t i = 0; i < args.size(); ++i)
                {
                    const char* argv = args[i].c_str();
                    if (*argv != '-')
                    {
                        // not option, process to next one
                        continue;
                    }
                    switch (*(argv+1))
                    {
                    case 't':
                        switch (*(argv+2))
                        {
                        case 'd': htsCfg.fnTreeDur.push_back(strPath+args[i+1]); break;
                        case 'm': htsCfg.fnTreeMgc.push_back(strPath+args[i+1]); break;
                        case 'f':
                        case 'p': htsCfg.fnTreeLf0.push_back(strPath+args[i+1]); break;
                        case 'l': htsCfg.fnTreeLpf.push_back(strPath+args[i+1]); break;
                        }
                        ++i;
                        break;
                    case 'm':
                        switch (*(argv+2))
                        {
                        case 'd': htsCfg.fnModelDur.push_back(strPath+args[i+1]); break;
                        case 'm': htsCfg.fnModelMgc.push_back(strPath+args[i+1]); break;
                        case 'f':
                        case 'p': htsCfg.fnModelLf0.push_back(strPath+args[i+1]); break;
                        case 'l': htsCfg.fnModelLpf.push_back(strPath+args[i+1]); break;
                        case 'u': htsCfg.fnModelUVProp = strPath+args[i+1];       break;
                        }
                        ++i;
                        break;
                    case 'd':
                        switch (*(argv+2))
                        {
                        case 'm': htsCfg.fnWinMgc.push_back(strPath+args[i+1]); break;
                        case 'f':
                        case 'p': htsCfg.fnWinLf0.push_back(strPath+args[i+1]); break;
                        case 'l': htsCfg.fnWinLpf.push_back(strPath+args[i+1]); break;
                        }
                        ++i;
                        break;
                    case 's':
                        htsCfg.samplingRate = atoi(args[i+1].c_str());
                        ++i;
                        break;
                    case 'p':
                        htsCfg.framePeriod = atoi(args[i+1].c_str());
                        ++i;
                        break;
                    case 'a':
                        htsCfg.valAlpha = (float)atof(args[i+1].c_str());
                        ++i;
                        break;
                    case 'g':
                        htsCfg.valGamma = atoi(args[i+1].c_str());
                        ++i;
                        break;
                    case 'l':
                        htsCfg.useLogGain = true;
                        break;
                    case 'b':
                        htsCfg.valBeta = (float)atof(args[i+1].c_str());
                        ++i;
                        break;
                    case 'r':
                        ++i;
                        break;
                    case 'f':
                        ++i;
                        break;
                    case 'u':
                        htsCfg.uvThreshold = (float)atof(args[i+1].c_str());
                        ++i;
                        break;
                    case 'i':
                        num_interp = atoi(args[i+1].c_str());
                        ++i;
                        for (size_t j=0; j<num_interp; ++j, ++i)
                            htsCfg.rateInterp.push_back((float)atof(args[i+1].c_str()));
                        break;
                    case 'e':
                        switch (*(argv+2))
                        {
                        case 'm': htsCfg.fnTreeGVMgc.push_back(strPath+args[i+1]); break;
                        case 'f':
                        case 'p': htsCfg.fnTreeGVLf0.push_back(strPath+args[i+1]); break;
                        case 'l': htsCfg.fnTreeGVLpf.push_back(strPath+args[i+1]); break;
                        }
                        ++i;
                        break;
                    case 'c':
                        switch (*(argv+2))
                        {
                        case 'm': htsCfg.fnModelGVMgc.push_back(strPath+args[i+1]); break;
                        case 'f':
                        case 'p': htsCfg.fnModelGVLf0.push_back(strPath+args[i+1]); break;
                        case 'l': htsCfg.fnModelGVLpf.push_back(strPath+args[i+1]); break;
                        }
                        ++i;
                        break;
                    case 'j':
                        switch (*(argv+2))
                        {
                        case 'm': htsCfg.gvWeightMgc = (float)atof(args[i+1].c_str()); break;
                        case 'f':
                        case 'p': htsCfg.gvWeightLf0 = (float)atof(args[i+1].c_str()); break;
                        case 'l': htsCfg.gvWeightLpf = (float)atof(args[i+1].c_str()); break;
                        }
                        ++i;
                        break;
                    case 'k':
                        htsCfg.fnGVSwitch = strPath+args[i+1];
                        ++i;
                        break;
                    default: // ignore this unknown option
                        ++i;
                        break;
                    }
                }

                // number of models, trees check
                num_interp = htsCfg.rateInterp.size();
                if (num_interp == 0)
                {
                    htsCfg.rateInterp.push_back(1.0f);
                    num_interp = 1;
                }
                if (num_interp != htsCfg.fnTreeDur.size() || num_interp != htsCfg.fnTreeMgc.size() || num_interp != htsCfg.fnTreeLf0.size() ||
                    num_interp != htsCfg.fnModelDur.size()|| num_interp != htsCfg.fnModelMgc.size()|| num_interp != htsCfg.fnModelLf0.size())
                    return false;
                if (htsCfg.fnTreeLpf.size() > 0 || htsCfg.fnModelLpf.size() > 0)
                    if (num_interp != htsCfg.fnTreeLpf.size() || num_interp != htsCfg.fnModelLpf.size())
                        return false;

                return true;
            }

            char** CHtsSynthesizer::newStrBuf(const std::vector<std::string> &inStr)
            {
                size_t num_str = inStr.size();
                char** ret_buf = new char*[num_str];
                for (size_t i=0; i<num_str; ++i)
                {
                    ret_buf[i] = (char*)inStr[i].c_str();
                }
                return ret_buf;
            }

            bool CHtsSynthesizer::open(const char *modelPath, const char *configFile)
            {
                HtsCfg cfg;

                // load configurations
                if (!loadConfig(modelPath, configFile, cfg))
                    return false;

                // open HTS engine
                return open(cfg);
            }

            bool CHtsSynthesizer::open(const HtsCfg &cfg)
            {
                // synthesis parameters
                int    sampling_rate     = cfg.samplingRate;    // 16000
                int    fperiod           = cfg.framePeriod;     // 80
                double alpha             = cfg.valAlpha;        // 0.42
                double stage             = cfg.valGamma;        // 0.0 gamma = -1.0/stage
                double beta              = cfg.valBeta;         // 0.0
                double uv_threshold      = cfg.uvThreshold;     // 0.5
                double gv_weight_mgc     = cfg.gvWeightMgc;     // 1.0
                double gv_weight_lf0     = cfg.gvWeightLf0;     // 1.0
                double gv_weight_lpf     = cfg.gvWeightLpf;     // 1.0
                HTS_Boolean use_log_gain = cfg.useLogGain;      // FALSE

                // delta window handler for mel-cepstrum
                int num_ws_mgc = (int)cfg.fnWinMgc.size();
                char** fn_ws_mgc = newStrBuf(cfg.fnWinMgc);
                // delta window handler for log f0
                int num_ws_lf0 = (int)cfg.fnWinLf0.size();
                char **fn_ws_lf0 = newStrBuf(cfg.fnWinLf0);
                // delta window handler for log f0
                int num_ws_lpf = (int)cfg.fnWinLpf.size();
                char **fn_ws_lpf = newStrBuf(cfg.fnWinLpf);

                // prepare for interpolation
                int num_interp = (int)cfg.rateInterp.size();        // number of speakers for interpolation
                int num_ms_lpf = (int)cfg.fnModelLpf.size();
                char **fn_ms_dur    = newStrBuf(cfg.fnModelDur);    // file names of models
                char **fn_ms_mgc    = newStrBuf(cfg.fnModelMgc);
                char **fn_ms_lf0    = newStrBuf(cfg.fnModelLf0);
                char **fn_ms_lpf    = newStrBuf(cfg.fnModelLpf);
                char **fn_ts_dur    = newStrBuf(cfg.fnTreeDur);    // file names of trees
                char **fn_ts_mgc    = newStrBuf(cfg.fnTreeMgc);
                char **fn_ts_lf0    = newStrBuf(cfg.fnTreeLf0);
                char **fn_ts_lpf    = newStrBuf(cfg.fnTreeLpf);
                char **fn_ms_gvmgc  = cfg.fnModelGVMgc.size() == num_interp ? newStrBuf(cfg.fnModelGVMgc) : NULL; // file names of global variances
                char **fn_ms_gvlf0  = cfg.fnModelGVLf0.size() == num_interp ? newStrBuf(cfg.fnModelGVLf0) : NULL;
                char **fn_ms_gvlpf  = cfg.fnModelGVLpf.size() == num_interp ? newStrBuf(cfg.fnModelGVLpf) : NULL;
                char **fn_ts_gvmgc  = cfg.fnTreeGVMgc.size()  == num_interp ? newStrBuf(cfg.fnTreeGVMgc)  : NULL; // file names of global variance trees
                char **fn_ts_gvlf0  = cfg.fnTreeGVLf0.size()  == num_interp ? newStrBuf(cfg.fnTreeGVLf0)  : NULL;
                char **fn_ts_gvlpf  = cfg.fnTreeGVLpf.size()  == num_interp ? newStrBuf(cfg.fnTreeGVLpf)  : NULL;
                double *rate_interp = new double[num_interp];       // interpolation rate of different speakers
                for (int i=0; i<num_interp; ++i)
                    rate_interp[i] = cfg.rateInterp[i];

                // initialize HTS engine
                //  (stream[0] = spectrum , stream[1] = lf0, stream[2] = low-pass filter)
                if (num_ms_lpf > 0)
                    HTS_Engine_initialize(&engine, 3);
                else
                    HTS_Engine_initialize(&engine, 2);

                // load duration model
                HTS_Engine_load_duration_from_fn(&engine, fn_ms_dur, fn_ts_dur, num_interp);
                // load stream[0] (spectrum model)
                HTS_Engine_load_parameter_from_fn(&engine, fn_ms_mgc, fn_ts_mgc, fn_ws_mgc, 0, FALSE, num_ws_mgc, num_interp);
                // load stream[1] (lf0 model)
                HTS_Engine_load_parameter_from_fn(&engine, fn_ms_lf0, fn_ts_lf0, fn_ws_lf0, 1, TRUE, num_ws_lf0, num_interp);
                // load stream[2] (low-pass filter model)
                if (num_ms_lpf > 0)
                    HTS_Engine_load_parameter_from_fn(&engine, fn_ms_lpf, fn_ts_lpf, fn_ws_lpf, 2, FALSE, num_ws_lpf, num_interp);
                // load gv[0] (GV for spectrum)
                if (fn_ms_gvmgc != NULL)
                    HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvmgc, fn_ts_gvmgc, 0, num_interp);
                // load gv[1] (GV for lf0)
                if (fn_ms_gvlf0 != NULL)
                    HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvlf0, fn_ts_gvlf0, 1, num_interp);
                // load gv[2] (GV for low-pass filter)
                if (fn_ms_gvlpf != NULL)
                    HTS_Engine_load_gv_from_fn(&engine, fn_ms_gvlpf, fn_ts_gvlpf, 2, num_interp);
                // load GV switch
                if (cfg.fnGVSwitch.size() > 0)
                    HTS_Engine_load_gv_switch_from_fn(&engine, (char*)cfg.fnGVSwitch.c_str());

                // set parameters
                HTS_Engine_set_sampling_rate(&engine, sampling_rate);
                HTS_Engine_set_fperiod(&engine, fperiod);
                HTS_Engine_set_alpha(&engine, alpha);
                HTS_Engine_set_gamma(&engine, (int)stage);
                HTS_Engine_set_log_gain(&engine, use_log_gain);
                HTS_Engine_set_beta(&engine, beta);
                HTS_Engine_set_msd_threshold(&engine, 1, uv_threshold); // set voiced/unvoiced threshold for stream[1]
                HTS_Engine_set_gv_weight(&engine, 0, gv_weight_mgc);
                HTS_Engine_set_gv_weight(&engine, 1, gv_weight_lf0);
                if (num_ms_lpf > 0)
                    HTS_Engine_set_gv_weight(&engine, 2, gv_weight_lpf);
                for (int i=0; i<num_interp; ++i)
                {
                    HTS_Engine_set_duration_interpolation_weight(&engine, i, rate_interp[i]);
                    HTS_Engine_set_parameter_interpolation_weight(&engine, 0, i, rate_interp[i]);
                    HTS_Engine_set_parameter_interpolation_weight(&engine, 1, i, rate_interp[i]);
                    if (num_ms_lpf > 0)
                        HTS_Engine_set_parameter_interpolation_weight(&engine, 2, i, rate_interp[i]);
                }
                if (fn_ms_gvmgc != NULL)
                {
                    for (int i=0; i<num_interp; ++i)
                        HTS_Engine_set_gv_interpolation_weight(&engine, 0, i, rate_interp[i]);
                }
                if (fn_ms_gvlf0 != NULL)
                {
                    for (int i=0; i<num_interp; ++i)
                        HTS_Engine_set_gv_interpolation_weight(&engine, 1, i, rate_interp[i]);
                }
                if (fn_ms_gvlpf != NULL)
                {
                    for (int i=0; i<num_interp; ++i)
                        HTS_Engine_set_gv_interpolation_weight(&engine, 2, i, rate_interp[i]);
                }

                // load unvoiced proportion for each phoneme
                FILE *uvFp = fopen(cfg.fnModelUVProp.c_str(), "rt");
                if (uvFp)
                {
                    int uvPropNum = 0;
                    fscanf(uvFp, "%d", &uvPropNum);
                    char  phone[256];
                    float prop;
                    for (int i=0; i<uvPropNum; ++i)
                    {
                        fscanf(uvFp, "%s%f", phone, &prop);
                        uvProps.insert(std::make_pair(phone, prop));
                    }
                    fclose(uvFp);
                }

                delete []rate_interp;
                delete []fn_ms_dur;
                delete []fn_ms_mgc;
                delete []fn_ms_lf0;
                delete []fn_ms_lpf;
                delete []fn_ts_dur;
                delete []fn_ts_mgc;
                delete []fn_ts_lf0;
                delete []fn_ts_lpf;
                delete []fn_ms_gvmgc;
                delete []fn_ms_gvlf0;
                delete []fn_ms_gvlpf;
                delete []fn_ts_gvmgc;
                delete []fn_ts_gvlf0;
                delete []fn_ts_gvlpf;
                delete []fn_ws_mgc;
                delete []fn_ws_lf0;
                delete []fn_ws_lpf;

                initialized = true;
                return true;
            }

            bool CHtsSynthesizer::close()
            {
                if (initialized)
                    HTS_Engine_clear(&engine);

                initialized = false;
                return true;
            }

            bool CHtsSynthesizer::synthesize(const char *inLabel, SynthCfg &synCfg, bool isFile)
            {
                HTS_Boolean phoneme_alignment = synCfg.usePhoneAlignment; // FALSE
                double volume       = synCfg.volumeRate;// 1.0
                double speech_speed = synCfg.speedRate; // 1.0
                double half_tone    = synCfg.halfTone;  // 0.0

                // load label information
                if (isFile)
                    HTS_Engine_load_label_from_fn(&engine, (char*)inLabel);
                else
                    HTS_Engine_load_label_from_string(&engine, (char*)inLabel);
                // modify label
                if (phoneme_alignment)
                {
                    HTS_Label_set_frame_specified_flag(&engine.label, TRUE);
                }
                // modify label
                if (speech_speed != 1.0)
                {
                    HTS_Label_set_speech_speed(&engine.label, speech_speed);
                }

                // parse label and determine state duration
                HTS_Engine_create_sstream(&engine);

                // modify f0
                if (half_tone != 0.0)
                {
                    for (int i=0; i<HTS_SStreamSet_get_total_state(&engine.sss); ++i)
                    {
                        double f = HTS_SStreamSet_get_mean(&engine.sss, 1, i, 0);
                        f += half_tone * log(2.0) / 12;
                        if (f < log(10.0))
                            f = log(10.0);
                        HTS_SStreamSet_set_mean(&engine.sss, 1, i, 0, f);
                    }
                }

                // modify unvoiced proportion
                if (uvProps.size() != 0)
                {
                    modifyUnvoicedProportion();
                }

                // generate speech parameter vector sequence
                HTS_Engine_create_pstream(&engine);

                // set volume
                HTS_Engine_set_volume(&engine, volume);

                // synthesize speech
                HTS_Engine_create_gstream(&engine);

                // output trace information
                if (synCfg.fnOutTrace.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutTrace.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_information(&engine, fp);
                        fclose(fp);
                    }
                }

                // output label file with duration
                if (synCfg.fnOutDur.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutDur.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_label(&engine, fp);
                        fclose(fp);
                    }
                }

                // output raw audio
                if (synCfg.fnOutRaw.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutRaw.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_generated_speech(&engine, fp);
                        fclose(fp);
                    }
                }

                // output wave data to file
                if (synCfg.fnOutWav.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutWav.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_riff(&engine, fp);
                        fclose(fp);
                    }
                }

                // output spectrum
                if (synCfg.fnOutMgc.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutMgc.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_generated_parameter(&engine, fp, 0);
                        fclose(fp);
                    }
                }

                // output log f0
                if (synCfg.fnOutLf0.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutLf0.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_generated_parameter(&engine, fp, 1);
                        fclose(fp);
                    }
                }

                // output low-pass filter
                if (synCfg.fnOutLpf.length() > 0)
                {
                    FILE* fp = fopen(synCfg.fnOutLpf.c_str(), "wb");
                    if (fp)
                    {
                        HTS_Engine_save_generated_parameter(&engine, fp, 2);
                        fclose(fp);
                    }
                }

                // output wave data in buffer
                if (synCfg.outWavData)
                {
                    synCfg.wavData.assign(engine.gss.gspeech, engine.gss.gspeech+engine.gss.total_nsample);
                    // retrieve segment durations (in samples)
                    synCfg.segBeg.clear();
                    synCfg.segEnd.clear();
                    const int nstate = HTS_ModelSet_get_nstate(&engine.ms);
                    const double rate = engine.global.fperiod * 1e+7 / engine.global.sampling_rate;
                    for (int i = 0, state = 0, frame = 0; i < HTS_Label_get_size(&engine.label); i++)
                    {
                        int j, duration;
                        for (j = 0, duration = 0; j < nstate; j++)
                            duration += HTS_SStreamSet_get_duration(&engine.sss, state++);
                        synCfg.segBeg.push_back(frame * rate);
                        frame += duration;
                        synCfg.segEnd.push_back(frame * rate);
                    }
                }

                // free
                HTS_Engine_refresh(&engine);

                return true;
            }

            void CHtsSynthesizer::modifyUnvoicedProportion()
            {
                int labFrameLength;
                int labVoicedStartList[20][2];
                int labVoicedStartNum;
                HTS_Boolean lastMsdFlag;
                int bestVoicedStart;
                int minErr;
                int minErrId;
                int lab,state;//,id;

                for (lab=0,state=0;lab<HTS_Label_get_size(&engine.label);lab++)
                {
                    labFrameLength=0;
                    labVoicedStartNum=0;
                    lastMsdFlag=FALSE;

                    // find every unvoiced to voiced change in the state sequence of the label
                    for (int i=0; i<HTS_ModelSet_get_nstate(&engine.ms); i++, state++)
                    {
                        if (HTS_SStreamSet_get_msd(&engine.sss,1,state) > engine.global.msd_threshold[1])
                        {
                            if (lastMsdFlag==FALSE)
                            {
                                labVoicedStartList[labVoicedStartNum][0] = labFrameLength;
                                labVoicedStartList[labVoicedStartNum][1] = state;
                                labVoicedStartNum++;
                            }
                            lastMsdFlag = TRUE;
                        }
                        else
                        {
                            lastMsdFlag = FALSE;
                        }
                        labFrameLength += HTS_SStreamSet_get_duration(&engine.sss,state);
                    }

                    // load the best voiced start point of the label
                    char *pLabString = HTS_Label_get_string(&engine.label,lab);
                    std::map<std::string, float>::const_iterator cit = uvProps.begin();
                    for (; cit != uvProps.end(); ++cit)
                    {
                        if (strstr(pLabString, cit->first.c_str()))
                            break;
                    }
                    if (cit != uvProps.end())
                    {
                        // force to be unvoiced from the 2nd state, and be voiced for the 1st state
                        if (cit->second == 1.0f)
                        {
                            for (int i=lab*HTS_ModelSet_get_nstate(&engine.ms); i<lab*HTS_ModelSet_get_nstate(&engine.ms)+1; i++)
                            {
                                engine.sss.sstream[1].msd[i] += 1;
                            }
                            for (int i=lab*HTS_ModelSet_get_nstate(&engine.ms)+1; i<(lab+1)*HTS_ModelSet_get_nstate(&engine.ms); i++)
                            {
                                engine.sss.sstream[1].msd[i] *= -1;
                            }
                            continue;
                        }

                        // force to be voiced from the 1st state
                        if (cit->second == 0.0f)
                        {
                            for (int i=lab*HTS_ModelSet_get_nstate(&engine.ms); i<(lab+1)*HTS_ModelSet_get_nstate(&engine.ms); i++)
                            {
                                engine.sss.sstream[1].msd[i] += 1;
                            }
                            continue;
                        }

                        // find which change is closest to the best voiced start point
                        bestVoicedStart = (int)(cit->second*labFrameLength);
                        minErr   = 100000000;
                        minErrId = -1;
                        for (int i=0; i<labVoicedStartNum; i++)
                        {
                            int labLen = abs(labVoicedStartList[i][0]-bestVoicedStart);
                            if (minErr > labLen)
                            {
                                minErr   = labLen;
                                minErrId = i;
                            }
                        }

                        //regular the unvoiced and voiced state sequence
                        if (minErrId >= 0)
                        {
                            for (int i=lab*HTS_ModelSet_get_nstate(&engine.ms); i<labVoicedStartList[minErrId][1]; i++)
                            {
                                engine.sss.sstream[1].msd[i] *= -1;
                            }
                            for (int i=(lab+1)*HTS_ModelSet_get_nstate(&engine.ms)-1; i>=labVoicedStartList[minErrId][1]; i--)
                            {
                                engine.sss.sstream[1].msd[i] += 1.0;
                            }
                        }
                    }
                }
            }

            int CHtsSynthesizer::getSampleRate() const
            {
                if (initialized)
                    return HTS_Engine_get_sampling_rate((HTS_Engine*)&engine);
                else
                    return 16000;
            }

        }//namespace Putonghua
    }
}
