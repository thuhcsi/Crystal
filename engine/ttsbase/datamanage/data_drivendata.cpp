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
/// @brief    Implementation file for the global driven data management
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/25
///   Changed:  Created
///


#include "cmn/cmn_type.h"
#include "data_drivendata.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            // The static vector maintaining the data mangers for all the TTS engines
            std::vector<CDrivenData::CDataManagerRef> CDrivenData::m_vecDataManager;

        }//namespace base
    }
}
