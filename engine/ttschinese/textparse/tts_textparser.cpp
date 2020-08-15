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
/// @brief  Implementation file initializing the text parser for Chinese Text-to-Speech (TTS) engine
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

#include "tts_textparser.h"
#include "ttsbase/datamanage/base_moduleapi.h"
#include "ttschinese/textparse/data_textdata.h"

namespace cst
{
    namespace tts
    {
        extern "C"
        {
            base::CDataManager *CreateDataManager(const base::DataConfig &dataConfig)
            {
                Chinese::CTextData *pDataManager = new Chinese::CTextData();
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

            CST_EXPORT void GetModuleInfo(std::map<std::string, ModuleInfo> &modules)
            {
                modules["CTextData"] = ModuleInfo("cst::tts::Chinese::CTextData", CreateDataManager, DeleteDataManager);
            }
        }
    }
}
