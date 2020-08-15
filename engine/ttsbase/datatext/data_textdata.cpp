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
/// @brief    Implementation file for text analysis data management interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#include "data_textdata.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ////////////////////////////////////////////////////////////////////////////
            ////
            ////  Dictionary related management
            ////
            ////////////////////////////////////////////////////////////////////////////

            bool CTextData::initialize(const DataConfig &dataConfig)
            {
                return CDataManager::initialize(dataConfig);
            }

            bool CTextData::terminate()
            {
                if (m_pLexicon)
                {
                    m_pLexicon->clear();
                    delete m_pLexicon;
                    m_pLexicon = NULL;
                }
                return CDataManager::terminate();
            }

            const CLexicon &CTextData::getLexicon() const
            {
                // assert lexicon is not NULL
                assert(m_pLexicon != NULL);
                return *m_pLexicon;
            }

        }//namespace base
    }
}
