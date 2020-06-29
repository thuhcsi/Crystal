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
/// @brief    Implementation file for voice data management interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#include "data_voicedata.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ////////////////////////////////////////////////////////////////////////////
            //
            //  Speech library related management
            //
            ////////////////////////////////////////////////////////////////////////////

            const CSpeechLib &CVoiceData::getSpeechLib() const
            {
                // This implementation is meaningless!
                // Just provided to prevent getSpeechLib() from being abstract.
                CSpeechLib *pSpeechLib = NULL;
                assert(pSpeechLib != NULL);
                return *pSpeechLib;
            }

        }//namespace base
    }
}
