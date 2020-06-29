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
/// @brief    Implementation file for supporting data management interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#include "data_datamanager.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            //////////////////////////////////////////////////////////////////////////
            //
            //  Data configuration management
            //
            //////////////////////////////////////////////////////////////////////////

            DataConfig::DataConfig()
            {
                bInitText      = true;
                bInitProsody   = true;
                bInitSynth     = true;
                bDetectDataFormat= true;
                bUseInnerSplib   = false;
                bLoadLexiconData = false;
            }

            bool DataConfig::operator==(const DataConfig &right) const
            {
                bool equal = true
                    && (right.wstrLangTag == wstrLangTag)
                    && (right.wstrLexiconPath == wstrLexiconPath)
                    && (right.wstrVoicePath == wstrVoicePath)
                    && (right.bInitText == bInitText)
                    && (right.bInitProsody == bInitProsody)
                    && (right.bInitSynth == bInitSynth)
                    && (right.bUseInnerSplib == bUseInnerSplib)
                    && (right.bLoadLexiconData == bLoadLexiconData);
                return equal;
            }

        }//namespace base
    }
}
