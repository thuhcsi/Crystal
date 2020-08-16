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
/// @brief  Head file for prosodic prediction module of Chinese TTS engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   Yongxin Wang (fefe.wyx@gmail.com)
///   Data:     2007/06/13
///   Changed:  implemented neutral prosody prediction
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
/// - Version:  0.2.5
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/07/30
///   Changed:  No need of prosody prediction for HTS synthesizer
///

#ifndef _CST_TTS_CHINESE_PSP_PROSODYPREDICT_H_
#define _CST_TTS_CHINESE_PSP_PROSODYPREDICT_H_

#include "ttsbase/synthesize/psp_prosodypredict.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class to perform prosody prediction for Chinese TTS engine
            ///
            class CProsodyPredict : public base::CProsodyPredict
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CProsodyPredict(const base::CDataManager *pDataManager) : base::CProsodyPredict(pDataManager) {}

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Inherited from the base class of base::CProsodyPredict
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Perform the prosody prediction for each basic unit in one sentence
                ///
                /// Current implementation does nothing for HTS based speech synthesis
                ///
                virtual int predictProsody(std::vector<CUnitItem> &sentenceInfo);

            };

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_PSP_PROSODYPREDICT_H_
