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
/// @brief      Head file for the basic module class
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/25
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_BASE_MODULE_H_
#define _CST_TTS_BASE_BASE_MODULE_H_

namespace cst
{
    // Forward declaration of SSML document class
    namespace xml {class CSSMLDocument;}

    namespace tts
    {
        namespace base
        {
            // Forward declaration of the data manager class
            class CDataManager;

            ///
            /// @brief  The base class for the TTS engine modules
            ///
            class CModule
            {
            public:
                /// Constructor
                CModule(const CDataManager *pDataManager) : m_pDataManager(pDataManager) {}

                /// Destructor
                virtual ~CModule() {}

            public:
                ///
                /// @brief  The main processing procedure for the module operation
                ///
                /// @param  [in]  pSSMLDocument  The input SSML document to be processed
                /// @param  [out] pSSMLDocument  Return the SSML document which has been processed
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument *pSSMLDocument) = 0;

            public:
                ///
                /// @brief  Get the handle to the data manager, the returned type is specified by the caller
                ///
                template <class _Ty>
                const _Ty *getDataManager() const
                {
                    return cst_dynamic_cast<const _Ty*>(m_pDataManager);
                }

            private:
                /// Handle to the data manager which maintains the supporting data needed by TTS engine
                const CDataManager *m_pDataManager;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_BASE_MODULE_H_
