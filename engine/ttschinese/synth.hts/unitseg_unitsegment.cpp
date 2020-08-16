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
/// @brief  Implementation file for unit segmentation module of Chinese TTS engine
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
/// - Version:  0.2.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/01/20
///   Changed:  Added processing of UTF-16 non-BMP characters in segmentToUnits
///

#include "utils/utl_regexp.h"
#include "unitseg_unitsegment.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            bool CUnitSegment::segmentToUnits(const std::wstring &wstrText, const std::wstring &wstrPhoneme,
                std::vector<std::wstring> &unitTexts, std::vector<std::wstring> &unitPhonemes)
            {
                unitTexts.clear();
                unitPhonemes.clear();

                // tokenize phoneme string
                segmentPhoneme(wstrPhoneme, unitPhonemes);

                // fetch text characters
                size_t numPhonemes = unitPhonemes.size();
                if (numPhonemes <= 1)
                {
                    // only one phoneme string or no phoneme
                    // return the entire text characters
                    unitTexts.push_back(wstrText);
                }
                else
                {
                    // it should be Chinese characters
                    wchar_t sylText[500];
                    const wchar_t *pNextText = wstrText.c_str();
                    for (std::vector<std::wstring>::const_iterator it = unitPhonemes.begin(); it != unitPhonemes.end(); ++it)
                    {
                        if (*pNextText == 0)
                        {
                            // insert dummy characters, until all phonemes are fetched
                            unitTexts.push_back(L"");
                            continue;
                        }
                        // fetch Chinese character
                        pNextText = str::fetchCharacter(pNextText, sylText);
                        if (isRetroflex(it->c_str(), pNextText))
                        {
                            // is retroflex (Erhua), fetch one more Chinese character
                            pNextText = str::fetchCharacter(pNextText, sylText+wcslen(sylText));
                        }
                        unitTexts.push_back(sylText);
                    }
                    if (*pNextText != 0)
                    {
                        // still characters left,
                        // append to the last text string
                        unitTexts.back() += pNextText;
                    }
                }
                return true;
            }

            bool CUnitSegment::isRetroflex(const wchar_t *wcsPinyin, const wchar_t *wcsText) const
            {
                // judge input text
                if (wcsText[0] != L'\u513F') //¶ù
                {
                    // not retroflex if next character is not Chinese "er2/¶ù"
                    return false;
                }
                // judge Pinyin
                size_t len = wcslen(wcsPinyin);
                if (len == 0)
                {
                    // not valid Pinyin
                    return false;
                }
                if (wcsPinyin[len-1] >= L'0' && wcsPinyin[len-1] <= L'9')
                {
                    // tone
                    len--;
                }
                if (len > 0 && wcsPinyin[len-1] == L'r' && (len != 2 || wcsPinyin[0] != L'e'))
                {
                    // Pinyin ends with 'r' and
                    // entire Pinyin is not 'er'
                    return true;
                }
                // not Erhua
                return false;
            }

            std::vector<std::wstring> &CUnitSegment::segmentPhoneme(const std::wstring &instr, std::vector<std::wstring> &phonemes) const
            {
                // A two pass tokenization method is used:
                //  Pass 1: segment the phonemes according to delimiters;
                //  Pass 2: 1) phoneme string of symbols (non-Chinese) is regarded as whole;
                //  Pass 2: 2) phoneme string of Chinese Pinyin will be segmented further by tones.

                static std::wstring delimiters = L"- \t\u3000";         // for segmenting phonemes in pass 1
                static cmn::CRegexp regPinyin  = L"^[a-zA-Z]+[0-9]?";   // for match Pinyin in pass 2

                // Pass 1: segment phonemes according to delimiters
                std::vector<std::wstring> vecPhonemes;
                std::wstring tmpstr = instr;
                str::replace(tmpstr, L"_", L" _"); // insert "space" before "_", so that multiple symbols can be segmented
                str::tokenize(tmpstr, delimiters, vecPhonemes);

                // Pass 2: retrieve detailed phonemes
                for (std::vector<std::wstring>::iterator it = vecPhonemes.begin(); it != vecPhonemes.end(); ++it)
                {
                    std::wstring &ph = *it;
                    if (ph.length() == 0)
                        continue;;
                    if (ph[0] == L'_')
                    {
                        // Pass 2: 
                        // 1) phoneme string of symbols: start with "_", and treat as a whole
                        phonemes.push_back(ph);
                    }
                    else if (ph[0] != 0)
                    {
                        // Pass 2:
                        // 2) phoneme string of Chinese Pinyin: segment it into syllables
                        const wchar_t* pstr = ph.c_str();
                        while (*pstr)
                        {
                            // match phoneme string
                            int len = 1;
                            if (regPinyin.match(pstr))
                                len = regPinyin.sublength(0);

                            // add phoneme string
                            phonemes.push_back(std::wstring(pstr, len));
                            pstr += len;
                        }
                    }
                }
                return phonemes;
            }

        }//namespace Chinese
    }
}
