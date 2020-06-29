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
/// @brief
///
/// @version    0.1.1
/// @date       2007/08/03
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/08/03
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/08/03
///   Changed:  Modified the initialization module
///

#ifndef _CST_TTS_BASE_CMN_CHN_CONV_H_
#define _CST_TTS_BASE_CMN_CHN_CONV_H_

#include "cmn/cmn_string.h"

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  The class performs the conversion between Simplified Chinese and Traditional Chinese
        ///
        class CChineseConvert
        {
        public:
            ///
            /// @brief  Constructor
            ///
            CChineseConvert() : m_bInitialized(false) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CChineseConvert() {terminate();}

            ///
            /// @brief  Initialize the Chinese conversion module
            ///
            /// @param  [in] wstrFileName   The file name where conversion table is stored
            ///
            /// @return Whether conversion module is initialized successfully
            ///
            virtual bool initialize(const wchar_t *wstrFileName);

            ///
            /// @brief  Terminate the conversion module, free all data
            ///
            virtual void terminate();

            ///
            /// @brief  Convert the input string to Traditional Chinese
            ///
            /// @param  [in]  str   The input string to be converted
            /// @param  [out] str   Return the converted string in Traditional Chinese
            ///
            virtual void toTraditional(wstring &str) const;

            ///
            /// @brief  Convert the input string to Simplified Chinese
            ///
            /// @param  [in]  str   The input string to be converted
            /// @param  [out] str   Return the converted string in Simplified Chinese
            ///
            virtual void toSimplified(wstring &str) const;

        protected:
            ///
            /// @brief  Find the specific char in the conversion table, return the index of the char
            ///
            /// @param  [in] table  The conversion table where the char is to be located
            /// @param  [in] chr    The char to be found in the table
            ///
            /// @return The index of the char in the conversion table, or -1 if not found.
            ///
            int findInTable(const wstring &table, wchar_t chr) const;

        protected:
            bool   m_bInitialized;  ///< Whether conversion module is initialized
            wstring m_S2T_STable;  ///< The Simplified  characters table for Simplified to Traditional conversion (sorted by Unicode)
            wstring m_S2T_TTable;  ///< The Traditional characters table for Simplified to Traditional conversion
            wstring m_T2S_TTable;  ///< The Traditional characters table for Traditional to Simplified conversion (sorted by Unicode)
            wstring m_T2S_STable;  ///< The Simplified  characters table for Traditional to Simplified conversion
        };
        
    } // End of namespace cmn
} // End of namespace cst

#endif // End of _CST_TTS_BASE_CMN_CHN_CONV_H_