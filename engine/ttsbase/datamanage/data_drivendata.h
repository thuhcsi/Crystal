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
/// @brief    Head file for the global driven data management
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/25
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_DATA_DRIVENDATA_H_
#define _CST_TTS_BASE_DATA_DRIVENDATA_H_

#include <string>
#include <vector>
#include "data_datamanager.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The global driven data manager which maintains a list of data managers for all the initialized TTS engines.
            ///
            /// During Text-to-Speech synthesizing, the supporting data or driven data are read only and can be only initialized once.
            /// Once the data are initialized, they can be shared between different synthesizing threads even difference processes.
            ///
            /// A list of initialized driven data for different TTS engines are maintained in this global driven data manager, with
            /// reference count for each initialized driven data.
            ///
            class CDrivenData
            {
            public:
                ///
                /// @brief  Initialize a specific TTS engine related data manager module
                /// @param  [in] strPath    The path where dictionary related data are stored.
                ///
                /// @return The code indicating whether dictionary is initialized successfully
                /// @retval ERROR_SUCCESS   The dictionary is initialized successfully
                ///
                template<class _Ty>
                static const _Ty* initDataManager(const DataConfig &dataConfig)
                {
                    // search in the initialized data managers
                    std::vector<CDataManagerRef>::iterator it;
                    for (it = m_vecDataManager.begin(); it != m_vecDataManager.end(); it++)
                    {
                        // judge whether two data manager modules are the same by configuration information
                        if (it->pDataManager && (dataConfig == it->pDataManager->getDataConfig()))
                        {
                            it->nReferenceCount ++;
                            return cst_dynamic_cast<const _Ty*>(it->pDataManager);
                        }
                    }

                    // not initialized yet
                    // create new data manager
                    CDataManager* pDataMan = new _Ty();
                    if (pDataMan == NULL)
                    {
                        return NULL;
                    }
                    if (!pDataMan->initialize(dataConfig))
                    {
                        delete pDataMan;
                        return NULL;
                    }

                    // add new data manager to the data manager list
                    m_vecDataManager.push_back(CDataManagerRef(pDataMan));
                    return cst_dynamic_cast<const _Ty*>(pDataMan);
                }

                ///
                /// @brief  Free (terminate) a specific TTS engine related data manager module
                ///
                /// @param  [in] pDataManager   Handle to the data manager to be freed
                ///
                /// @return Whether the operation is successful
                ///
                static bool freeDataManager(const CDataManager* pDataManager)
                {
                    std::vector<CDataManagerRef>::iterator it;
                    for (it = m_vecDataManager.begin(); it != m_vecDataManager.end(); it++)
                    {
                        if (it->pDataManager == pDataManager)
                        {
                            it->nReferenceCount --;
                            if (it->nReferenceCount == 0)
                            {                            
                                // now the reference count is zero, the data manager could be released
                                delete it->pDataManager;
                                m_vecDataManager.erase(it);
                            }
                            return true;
                        }
                    }
                    return false;
                }

            protected:
                ///
                /// @brief  The data manager with reference count
                ///
                class CDataManagerRef
                {
                public:
                    CDataManagerRef(CDataManager *pData) : pDataManager(pData), nReferenceCount(1) {}   ///< Constructor
                    CDataManager *pDataManager;     ///< Pointer to the TTS engine data manager
                    int           nReferenceCount;  ///< Reference count of the data manager
                };

                ///
                /// @brief  The vector which maintains all the initialized data managers for each TTS engine
                ///
                static std::vector<CDataManagerRef> m_vecDataManager;

            protected:
                ///
                /// @brief  Empty constructor. Made protected to keep singleton.
                ///
                CDrivenData() {}

                ///
                /// @brief  Empty destructor.
                ///
                virtual ~CDrivenData() {}

            private:
                ///
                /// @brief  Declared to prevent calling, will not be implemented
                ///
                CDrivenData &operator = (const CDrivenData &);

                ///
                /// @brief  Declared to prevent calling, will not be implemented
                ///
                CDrivenData(const CDrivenData &);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_DATA_DRIVENDATA_H_
