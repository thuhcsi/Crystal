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
/// @brief  Implementation file defining the speech synthesis interfaces for Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/11/16
///   Changed:  Implemented the framework and basic interfaces
///

#include "tts_synthesizer.h"
#include "../datamanage/data_drivendata.h"
#include "../datavoice/data_voicedata.h"
#include "../synthesize/unitseg_unitsegment.h"
#include "../synthesize/psp_prosodypredict.h"
#include "../synthesize/synth_synthesize.h"
#include <iostream>

namespace cst
{
    namespace tts
    {
        namespace base
        {
            CSynthesizer::CSynthesizer()
            {
                // initialize modules
                dataManager         = NULL;
                unitSegmentor       = NULL;
                prosodyPredictor    = NULL;
                waveSynthesizer     = NULL;

                initialized   = false;
            }

            CSynthesizer::~CSynthesizer()
            {
                terminate();
            }

            int CSynthesizer::initialize(const wchar_t *configFile)
            {
                // close engine first if initialized
                terminate();

                // set default modules in case the dynamic library only defines part of them
                setDefaultModules(modules);

                // load modules from dynamic library
                DataConfig dataConfig;
                int flag = loadModules(configFile, dataConfig);
                if (flag != ERROR_SUCCESS)
                {
                    return flag;
                }

                ///@todo log used modules
                {
                    std::wcout << str::mbstowcs(modules["CVoiceData"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CUnitSegment"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CProsodyPredict"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CSynthesize"].name) << std::endl;
                }

                // initialize modules
                dataManager = dynamic_cast<CVoiceData*>(modules["CVoiceData"].createDataMan(dataConfig));
                if (dataManager == NULL)
                {
                    initialized = false;
                    return ERROR_OPEN_FAILED;
                }
                unitSegmentor    = dynamic_cast<CUnitSegment*>(modules["CUnitSegment"].createModule(dataManager));
                prosodyPredictor = dynamic_cast<CProsodyPredict*>(modules["CProsodyPredict"].createModule(dataManager));
                waveSynthesizer  = dynamic_cast<CSynthesize*>(modules["CSynthesize"].createModule(dataManager));

                initialized = true;
                return ERROR_SUCCESS;
            }

            int CSynthesizer::terminate()
            {
                if (!initialized)
                    return ERROR_SUCCESS;

                // delete modules
                modules["CUnitSegment"].deleteModule(unitSegmentor);
                modules["CProsodyPredict"].deleteModule(prosodyPredictor);
                modules["CSynthesize"].deleteModule(waveSynthesizer);

                // free the data manager
                modules["CVoiceData"].deleteDataMan(dataManager);

                // reset modules
                dataManager         = NULL;
                unitSegmentor       = NULL;
                prosodyPredictor    = NULL;
                waveSynthesizer     = NULL;

                initialized = false;
                return ERROR_SUCCESS;
            }

            int CSynthesizer::getSetting(TTSSetting& setting)
            {
                if (!initialized)
                    return ERROR_NOT_INITIALIZED;

                setting = dataManager->getGlobalSetting();
                return ERROR_SUCCESS;
            }

            int CSynthesizer::setSetting(const TTSSetting& setting)
            {
                if (!initialized)
                    return ERROR_NOT_INITIALIZED;

                // cast to non-const
                CDataManager* pDataManager = (CDataManager*)dataManager;
                pDataManager->getGlobalSetting() = setting;
                return ERROR_SUCCESS;
            }

            int CSynthesizer::getWaveFormat(ushort& nChannels, ushort& nBitsPerSample, ulong& nSamplesPerSec)
            {
                if (!initialized)
                    return ERROR_NOT_INITIALIZED;

                const CWavSynthesizer &wavsyn = dataManager->getWavSynthesizer();

                nChannels      = wavsyn.getChannels();
                nBitsPerSample = wavsyn.getBitsPerSample();
                nSamplesPerSec = wavsyn.getSamplesPerSec();

                return ERROR_SUCCESS;
            }

            int CSynthesizer::process(xml::CSSMLDocument &ssmlDocument, dsp::CWaveData &waveData)
            {
                if (!initialized)
                    return ERROR_NOT_INITIALIZED;

                // clear data first
                waveData.clear();

                // the SSML document
                xml::CSSMLDocument *pDocument = &ssmlDocument;

                // unit segment
                int retCode = unitSegment(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // prosodic prediction
                retCode = prosodyPredict(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // speech synthesize
                retCode = waveSynthesize(pDocument, waveData);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                return ERROR_SUCCESS;
            }

            int CSynthesizer::unitSegment(xml::CSSMLDocument *pSSMLDocument)
            {
                return unitSegmentor->process(pSSMLDocument);
            }

            int CSynthesizer::prosodyPredict(xml::CSSMLDocument *pSSMLDocument)
            {
                return prosodyPredictor->process(pSSMLDocument);
            }

            int CSynthesizer::waveSynthesize(xml::CSSMLDocument *pSSMLDocument, dsp::CWaveData &waveData)
            {
                return waveSynthesizer->process(pSSMLDocument, waveData);
            }


            base::CDataManager *CreateVoiceData(const base::DataConfig &dataConfig)
            {
                base::CVoiceData *pDataManager = new base::CVoiceData();
                if (pDataManager == NULL || !pDataManager->initialize(dataConfig))
                {
                    delete pDataManager;
                    pDataManager = NULL;
                }
                return pDataManager;
            }

            void DeleteVoiceData(base::CDataManager *pDataManager)
            {
                delete pDataManager;
            }

            base::CModule *CreateUnitSegment(const base::CDataManager *pDataManager)
            {
                return new base::CUnitSegment(pDataManager);
            }

            base::CModule *CreateProsodyPredict(const base::CDataManager *pDataManager)
            {
                return new base::CProsodyPredict(pDataManager);
            }

            base::CModule *CreateSynthesize(const base::CDataManager *pDataManager)
            {
                return new base::CSynthesize(pDataManager);
            }

            void DeleteSynthModule(base::CModule *pModule)
            {
                delete pModule;
            }

            void CSynthesizer::setDefaultModules(std::map<std::string, ModuleInfo> &modules)
            {
                modules["CVoiceData"] = ModuleInfo("cst::tts::base::CVoiceData", CreateVoiceData, DeleteVoiceData);
                modules["CUnitSegment"] = ModuleInfo("cst::tts::base::CUnitSegment", CreateUnitSegment, DeleteSynthModule);
                modules["CProsodyPredict"] = ModuleInfo("cst::tts::base::CProsodyPredict", CreateProsodyPredict, DeleteSynthModule);
                modules["CSynthesize"] = ModuleInfo("cst::tts::base::CSynthesize", CreateSynthesize, DeleteSynthModule);
            }

        }//namespace base
    }
}
