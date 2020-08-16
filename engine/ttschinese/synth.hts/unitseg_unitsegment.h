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
/// @brief  Head file for unit segmentation module of Chinese TTS engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///

#ifndef _CST_TTS_CHINESE_UNITSEG_UNITSEGMENT_H_
#define _CST_TTS_CHINESE_UNITSEG_UNITSEGMENT_H_

#include "ttsbase/synthesize/unitseg_unitsegment.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class to perform unit segmentation for Chinese TTS engine
            ///
            class CUnitSegment : public base::CUnitSegment
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CUnitSegment(const base::CDataManager *pDataManager) : base::CUnitSegment(pDataManager) {}

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Inherited from the base class of base::CUnitSegment
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Segment the input text pieces and phonemes into basic units
                ///
                virtual bool segmentToUnits(const std::wstring &wstrText, const std::wstring &wstrPhoneme,
                    std::vector<std::wstring> &unitTexts, std::vector<std::wstring> &unitPhonemes);

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Internal facilitate procedures
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Tokenize the phoneme string into a vector of phonemes
                ///
                /// @param  [in]  instr     Input phoneme string to be segmented
                /// @param  [out] phonemes  Return the vector of phonemes
                ///
                /// @return Return the reference to the vector of phonemes
                ///
                std::vector<std::wstring> &segmentPhoneme(const std::wstring &instr, std::vector<std::wstring> &phonemes) const;

                ///
                /// @brief  Judge whether it is retroflex (Erhua) according to Pinyin and Text
                ///
                /// @param  [in] wcsPinyin  Pinyin string to be judged
                /// @param  [in] wcsText    Text string of Chinese characters
                ///
                /// @return Whether it is retroflex (Erhua)
                ///
                bool isRetroflex(const wchar_t *wcsPinyin, const wchar_t *wcsText) const;

            };

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_UNITSEG_UNITSEGMENT_H_
