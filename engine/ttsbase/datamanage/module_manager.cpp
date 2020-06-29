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
/// @brief  Implementation file for managing modules loaded from dynamic libraries for Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/1/20
///   Changed:  Implemented the framework and basic interfaces
///

#include "module_manager.h"
#include "xml/xml_dom.h"
#include "data_datamanager.h"
#include <iostream>

#if defined(WIN32)
#   include <Windows.h>
#elif defined(__GNUC__)
#   include <dlfcn.h>
#else
#   error "Operating system or Complier not supported!"
#endif

namespace cst
{
    namespace tts
    {
        namespace base
        {
            bool CModuleManager::loadLibrary(const wchar_t* filename)
            {
                // load library dynamically
#if defined(WIN32)
                // load DLL dynamically
                HINSTANCE dlib = LoadLibraryW(filename);
                if (!dlib)
                    return false;
                // find the GetModuleInfo function
                GetModuleInfoFunc* GetModuleInfo = (GetModuleInfoFunc*)(GetProcAddress(dlib, "GetModuleInfo"));
                if (!GetModuleInfo)
                {
                    FreeLibrary(dlib);
                    return false;
                }
#elif defined(__GNUC__)
                // load dynamic library dynamically
                std::string dlfile = str::wcstombs(filename);
                void *dlib = dlopen(dlfile.c_str(), RTLD_NOW);
                if (!dlib)
                    return false;
                // find the GetModuleInfo function
                GetModuleInfoFunc* GetModuleInfo = (GetModuleInfoFunc*)(dlsym(dlib, "GetModuleInfo"));
                if (!GetModuleInfo)
                {
                    dlclose(dlib);
                    return false;
                }
#endif
                // fill in module information
                (*GetModuleInfo)(modules);
                dlibs.push_back(dlib);
                return true;
            }

            int CModuleManager::loadModules(const wchar_t *configFile, DataConfig &dataConfig)
            {
                // load configuration file
                xml::CXMLDocument cfg;
                if (!cfg.load(configFile))
                {
                    return ERROR_OPEN_FAILED;
                }

                // parse configuration file
                const xml::CXMLElement *pModule = (xml::CXMLElement*)cfg.firstChild(L"modules", xml::CXMLNode::XML_ELEMENT);
                if (pModule == NULL || pModule->nextSibling(L"modules", xml::CXMLNode::XML_ELEMENT) != NULL)
                {
                    return ERROR_OPEN_FAILED;
                }
                dataConfig.wstrLangTag = pModule->getAttribute(L"xml:lang");
                for (const xml::CXMLElement *pElement = (xml::CXMLElement*)pModule->firstChild(xml::CXMLNode::XML_ELEMENT); pElement != NULL; pElement=(xml::CXMLElement*)pElement->nextSibling(xml::CXMLNode::XML_ELEMENT))
                {
                    if (pElement->value() == L"dlib")
                    {
                        // load modules from a dynamic library
                        if (!loadLibrary(pElement->getAttribute(L"file").c_str()))
                        {
                            ///@todo should raise the waring message: telling that dynamic library is not loaded
                            std::wcout << L"Dynamic library: '" << pElement->getAttribute(L"file") << L"' load failed!" << std::endl;
                            //return ERROR_OPEN_FAILED;
                        }
                    }
                    else if (pElement->value() == L"textdata")
                    {
                        // set textdata info
                        dataConfig.wstrLexiconPath = pElement->getAttribute(L"path");
                        dataConfig.bLoadLexiconData= pElement->getAttribute(L"loaddata") == L"true" ? true : false;
                    }
                    else if (pElement->value() == L"voicedata")
                    {
                        // set voicedata info
                        dataConfig.wstrVoicePath = pElement->getAttribute(L"path");
                    }
                }
                return ERROR_SUCCESS;
            }

            int CModuleManager::freeModules()
            {
                // close all the dynamic libraries
                for (std::vector<void*>::iterator it = dlibs.begin(); it != dlibs.end(); ++it)
                {
#if defined(WIN32)
                    FreeLibrary(HINSTANCE(*it));
#elif defined(__GNUC__)
                    dlclose(*it);
#endif
                }
                // clear data
                dlibs.clear();
                modules.clear();
                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
