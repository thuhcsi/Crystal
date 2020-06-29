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
/// @brief    Head file for supporting data management interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_DATA_DATAMANAGER_H_
#define _CST_TTS_BASE_DATA_DATAMANAGER_H_

#include "cmn/cmn_error.h"
#include "cmn/cmn_string.h"
#include "cmn/cmn_vector.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  Data configuration for initializing TTS engine and driven data
            ///
            class DataConfig
            {
            public:
                std::wstring    wstrLangTag;        ///< Language tag of data and engine (e.g. "zh-cmn"/"zh-yue" for Chinese Putonghua/Cantonese)
                std::wstring    wstrLexiconPath;    ///< Path where lexicon data are stored
                std::wstring    wstrVoicePath;      ///< Path where voice (speech) library data are stored
                bool            bInitText;          ///< Initialize text parsing module or not (DEFAULT: true)
                bool            bInitProsody;       ///< Initialize prosody prediction module or not (DEFAULT: true)
                bool            bInitSynth;         ///< Initialize speech synthesis module or not (DEFAULT: true)
                bool            bDetectDataFormat;  ///< Automatically detect version of voice library and lexicon data (DEFAULT: true)
                bool            bUseInnerSplib;     ///< Use internal voice library (debug version) or not (DEFAULT: false)
                bool            bLoadLexiconData;   ///< Load all lexicon data into memory or not (DEFAULT: false)

            public:
                ///
                /// @brief  Default constructor
                ///
                DataConfig();

                ///
                /// @brief  Judge whether two configurations are the same
                ///
                bool operator==(const DataConfig &right) const;
            };


            ///
            /// @brief  Global variables and settings for TTS engine
            ///
            class TTSSetting
            {
            public:
                float   rate;               ///< Duration modification ratio            [0.0--10.0], DEF [1.0]
                float   volume;             ///< Amplitude (volume) modification ratio      [>=0.0], DEF [1.0]
                float   pitch;              ///< Mean (average) pitch modification ratio [0.5--2.0], DEF [1.0]

            public:
                ///
                /// @brief  Constructor
                ///
                TTSSetting() : rate(1), volume(1), pitch(1) {}

                ///
                /// @brief  Destructor
                ///
                virtual ~TTSSetting() {}
            };

            ///
            /// @brief  The class for managing the supporting data (e.g. lexicon, speech library, etc)
            ///
            class CDataManager
            {
            public:
                ///
                /// @brief  Default constructor
                ///
                CDataManager() {}

                ///
                /// @brief  Default destructor
                ///
                virtual ~CDataManager() {}

                ///
                /// @brief  Initialize the data manager from the configuration information
                ///
                /// @param  [in] dataConfig     The configuration information
                ///
                /// @return Whether operation is successful
                ///
                virtual bool initialize(const DataConfig &dataConfig)
                {
                    m_config = dataConfig;
                    return true;
                }

                ///
                /// @brief  Terminate (free) the initialized data
                ///
                virtual bool terminate()
                {
                    return true;
                }

                ///
                /// @brief  Return data configuration
                ///
                const DataConfig &getDataConfig() const {return m_config;}

                ///
                /// @brief  Get global variables and settings for TTS engine
                ///
                const TTSSetting &getGlobalSetting() const {return m_ttsSetting;}
                TTSSetting &getGlobalSetting() {return m_ttsSetting;}

            protected:
                ///
                /// @brief  The configuration information for the supporting data management
                ///
                DataConfig m_config;

                ///
                /// @brief  Global variables and settings for the TTS engine
                ///
                TTSSetting m_ttsSetting;

            private:
                ///
                /// @brief  Declared to prevent calling, will not be implemented
                ///
                CDataManager &operator = (const CDataManager &);
                ///
                /// @brief  Declared to prevent calling, will not be implemented
                ///
                CDataManager(const CDataManager &);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_DATA_DATAMANAGER_H_
