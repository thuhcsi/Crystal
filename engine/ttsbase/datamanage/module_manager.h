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
/// @brief  Head file for managing modules loaded from dynamic libraries for Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/1/20
///   Changed:  Implemented the framework and basic interfaces
///

#ifndef _CST_TTS_BASE_MODULE_MANAGER_H_
#define _CST_TTS_BASE_MODULE_MANAGER_H_

#include <vector>
#include "base_moduleapi.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The class for managing modules dynamically loaded from dynamic libraries
            ///
            class CModuleManager
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CModuleManager() {}

                ///
                /// @brief  Destructor
                ///
                virtual ~CModuleManager() {freeModules();}

                ///
                /// @brief  Load modules from the dynamic libraries specified by the configuration file
                ///
                /// @param  [in]  configFile    Configuration file for loading modules
                /// @param  [out] dataConfig    Return the configuration data for later module initialization
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int loadModules(const wchar_t *configFile, DataConfig &dataConfig);

                ///
                /// @brief  Free the modules
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int freeModules();

            protected:
                ///
                /// @brief  Load modules from a dynamic library
                ///
                /// @param  [in] filename   Filename of the dynamic library to be loaded
                ///
                /// @return Whether operation is successful
                ///
                bool loadLibrary(const wchar_t* filename);

            protected:
                /// Loaded modules
                std::map<std::string, ModuleInfo> modules;
                /// Loaded dynamic libraries (e.g. "DLL" on Windows, "SO" on Linux)
                std::vector<void*> dlibs;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_MODULE_MANAGER_H_
