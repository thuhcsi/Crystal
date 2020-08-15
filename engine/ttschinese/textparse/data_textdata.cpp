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
/// @brief    Implementation file for text analysis data management of Chinese Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#include "data_textdata.h"
#include "lexicon_data.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            bool CTextData::initialize(const base::DataConfig &dataConfig)
            {
                m_config = dataConfig;
                if (!m_config.bInitText)
                    return true;

                CLexiconData *pLexicon = new CLexiconData();
                if (pLexicon == NULL)
                    return false;

                // initialize text data
                bool bsucc = true;
                {
                    std::wstring wstrPath = m_config.wstrLexiconPath;

                    // load default lexicon (for GBK characters)
                    bsucc = bsucc && pLexicon->load((wstrPath+L"/gbk.bin").c_str());

                    // load char lexicon (for English/Greek letters and key words)
                    bsucc = bsucc && pLexicon->load((wstrPath+L"/symbol.lex").c_str());
                }
                if (!bsucc)
                {
                    pLexicon->clear();
                    delete pLexicon;
                    pLexicon = NULL;
                }

                // save text data
                m_pLexicon = pLexicon;
                return bsucc;
            }

        }//namespace Chinese
    }
}
