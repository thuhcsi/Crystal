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
/// @brief      Head file for the module API related definitions
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/25
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_BASE_MODULEAPI_H_
#define _CST_TTS_BASE_BASE_MODULEAPI_H_

#include <map>
#include <string>

#if defined(WIN32)
#if defined(CSTDLL_EXPORTS)
#   define CST_EXPORT __declspec(dllexport)
#else
#   define CST_EXPORT __declspec(dllimport)
#endif
#else //WIN32
#   define CST_EXPORT
#endif//WIN32

namespace cst
{
    namespace tts
    {
        namespace base
        {
            // Forward declaration of classes
            class CDataManager;
            class DataConfig;
            class CModule;
        }
    }
}

///
/// @brief  Module information that are to be returned by the dynamic libraries
///
class ModuleInfo
{
    /// Type for the function to create module
    typedef cst::tts::base::CModule *CreateModuleFunc(const cst::tts::base::CDataManager *);
    /// Type for the function to delete module
    typedef void DeleteModuleFunc(cst::tts::base::CModule *);
    /// Type for the function to create data manager
    typedef cst::tts::base::CDataManager *CreateDataManagerFunc(const cst::tts::base::DataConfig &);
    /// Type for the function to delete data manager
    typedef void DeleteDataManagerFunc(cst::tts::base::CDataManager *);
public:
    /// Constructors
    ModuleInfo() : name(""), createModule(NULL), deleteModule(NULL) {};
    ModuleInfo(std::string mname, CreateModuleFunc* cfunc, DeleteModuleFunc* dfunc) : name(mname), createModule(cfunc), deleteModule(dfunc) {}
    ModuleInfo(std::string mname, CreateDataManagerFunc* cfunc, DeleteDataManagerFunc* dfunc) : name(mname), createDataMan(cfunc), deleteDataMan(dfunc) {}
public:
    std::string name;                           ///< Name of the module or data manager
    union
    {
        CreateModuleFunc* createModule;         ///< Function handle to create module
        CreateDataManagerFunc* createDataMan;   ///< Function handle to create data manager
    };
    union
    {
        DeleteModuleFunc* deleteModule;         ///< Function handle to delete module
        DeleteDataManagerFunc* deleteDataMan;   ///< Function handle to delete data manager
    };
};

///
/// @brief  Type for the function used to retrieve module information from dynamic library
///
/// All dynamic libraries should implement the "GetModuleInfo" function with the this function type.
///
/// An example is as follows.
/// @verbatim
///     extern "C"
///     {
///         base::CModule *CreateGrapheme2Phoneme(const base::CDataManager *pDataManager)
///         {
///             return new Putonghua::CGrapheme2Phoneme(pDataManager);
///         }
///         
///         void DeleteModule(base::CModule *pModule)
///         {
///             delete pModule;
///         }
///         
///         CST_EXPORT void GetModuleInfo(std::map<std::string, ModuleInfo> &modules)
///         {
///             modules["CGrapheme2Phoneme"] = ModuleInfo("cst::tts::Putonghua::CGrapheme2Phoneme", CreateGrapheme2Phoneme, DeleteModule);
///         }
///     }
/// @endverbatim
///
typedef void GetModuleInfoFunc(std::map<std::string, ModuleInfo> &);

#endif//_CST_TTS_BASE_BASE_MODULEAPI_H_
