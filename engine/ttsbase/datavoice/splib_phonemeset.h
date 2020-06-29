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
/// @brief  Head file for the interface of phoneme set (pronunciation dictionary) manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/10/01
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_PHONEME_SET_H_
#define _CST_TTS_BASE_PHONEME_SET_H_

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The base implementation interface class for phoneme set manipulation
            ///
            class CPhonemeSet
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CPhonemeSet() {}

                ///
                /// @brief  Destructor
                ///
                virtual ~CPhonemeSet() {}

                ///
                /// @brief  Initialize the phoneme set
                ///
                /// @param  [in] fileName   Name of the file where phoneme set is stored
                ///
                /// @return Whether phoneme set is initialized successfully
                ///
                virtual bool initialize(const std::wstring &fileName) = 0;

                ///
                /// @brief  Terminate and free the phoneme set, close all the data
                ///
                /// @return Whether phoneme set is terminated successfully
                ///
                virtual bool terminate() = 0;

                ///
                /// @brief  Save the phoneme set to the file
                ///
                /// @param  [in] fileName   Name of the file to which phoneme set is saved
                ///
                /// @param  Whether phoneme set is saved successfully
                ///
                virtual bool save(const std::wstring &fileName) = 0;

            public:
                ///
                /// @brief  Get internal code of the input phoneme.
                ///
                /// The internal code will be used for all the data manipulation interfaces.
                ///
                /// @param  [in] wstrPhoneme    The input phoneme whose internal code is to be returned
                ///
                /// @return The internal code of the input phoneme
                /// @retval INVALID_ICODE(-1) The input phoneme is not valid or supported
                ///
                virtual icode_t getICode(const std::wstring &phoeneme) const = 0;

                ///
                /// @brief  Get the corresponding phoneme of the input internal code.
                ///
                /// @param  [in] iCode  The internal code whose corresponding phoneme is to be returned
                ///
                /// @return The phoneme of the input internal code
                /// @retval ""(empty string)  The input internal code is not valid or supported
                ///
                virtual std::wstring getPhoneme(icode_t iCode) const = 0;

                ///
                /// @brief  Get value of the upper bound (maximum) internal code
                ///
                /// The returned value indicates the dimension of the icode (phoneme) array 
                /// in the phoneme set. It can be used to initialize the vector (array) to
                /// store phoneme related information in the phoneme set.
                ///
                /// @return The value of the upper bound (maximum) internal code
                ///
                virtual icode_t getMaximumICode() const = 0;

                ///
                /// @brief  Extract one phoneme from the input text string
                ///
                /// @param  [in]  wstrInput     Input text string containing a sequence of phonemes
                /// @param  [out] wstrPhoneme   Return the extracted phoneme string
                ///
                /// @return The start position of remaining string
                ///
                virtual const wchar_t *extractOnePhoneme(const wchar_t *input, std::wstring &phoneme) const = 0;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_PHONEME_SET_H_
