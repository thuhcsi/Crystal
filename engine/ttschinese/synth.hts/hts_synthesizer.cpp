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
/// @brief  Implementation file of encapsulated HTS synthesizer with wave synthesizer interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Xujun (xjun.cn@gmail.com), John (john.zywu@gmail.com)
///   Date:     2007/06/15
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/30
///   Changed:  Modified by using the new definitions
/// - Version:  0.2.5
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/07/30
///   Changed:  Modified by using HTS synthesizer
///


#include "hts_synthesizer.h"
#include "utils/utl_string.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            bool CHtsSynthesizer::initialize(const std::wstring &wstrPath, const std::wstring &wstrCfgFile)
            {
                m_bInitialized = hts::CHtsSynthesizer::open(str::wcstombs(wstrPath).c_str(), str::wcstombs(wstrCfgFile).c_str());
                return m_bInitialized;
            }

            bool CHtsSynthesizer::terminate()
            {
                if (hts::CHtsSynthesizer::close())
                {
                    m_bInitialized = false;
                    return true;
                }
                return false;
            }

        }//namespace Chinese
    }
}
