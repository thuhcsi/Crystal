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
/// @brief  Implementation file initializing the speech synthesis modules for Chinese Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/11/16
///   Changed:  Implemented the framework
///

#include "tts_synthesizer.h"
#include "ttsbase/datamanage/base_moduleapi.h"
#include "ttschinese/synth.hts/data_voicedata.h"
#include "ttschinese/synth.hts/unitseg_unitsegment.h"
#include "ttschinese/synth.hts/psp_prosodypredict.h"
#include "ttschinese/synth.hts/hts_synthesize.h"

namespace cst
{
    namespace tts
    {
        extern "C"
        {
            base::CDataManager *CreateDataManager(const base::DataConfig &dataConfig)
            {
                Chinese::CVoiceData *pDataManager = new Chinese::CVoiceData();
                if (pDataManager == NULL || !pDataManager->initialize(dataConfig))
                {
                    delete pDataManager;
                    pDataManager = NULL;
                }
                return pDataManager;
            }

            void DeleteDataManager(base::CDataManager *pDataManager)
            {
                delete pDataManager;
            }

            base::CModule *CreateUnitSegment(const base::CDataManager *pDataManager)
            {
                return new Chinese::CUnitSegment(pDataManager);
            }

            base::CModule *CreateProsodyPredict(const base::CDataManager *pDataManager)
            {
                return new Chinese::CProsodyPredict(pDataManager);
            }

            base::CModule *CreateSynthesize(const base::CDataManager *pDataManager)
            {
                Chinese::CHtsSynthesize* pSynthesizer = new Chinese::CHtsSynthesize(pDataManager);
                return (base::CSynthesize*)pSynthesizer;
            }

            void DeleteModule(base::CModule *pModule)
            {
                delete pModule;
            }

            CST_EXPORT void GetModuleInfo(std::map<std::string, ModuleInfo> &modules)
            {
                modules["CVoiceData"] = ModuleInfo("cst::tts::Chinese::CVoiceData", CreateDataManager, DeleteDataManager);
                modules["CUnitSegment"] = ModuleInfo("cst::tts::Chinese::CUnitSegment", CreateUnitSegment, DeleteModule);
                modules["CProsodyPredict"] = ModuleInfo("cst::tts::Chinese::CProsodyPredict", CreateProsodyPredict, DeleteModule);
                modules["CSynthesize"] = ModuleInfo("cst::tts::Chinese::CHtsSynthesize", CreateSynthesize, DeleteModule);
            }
        }
    }
}
