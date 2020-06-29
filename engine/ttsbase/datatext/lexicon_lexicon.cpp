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
/// @brief    Implementation file for the interface of lexicon accessing
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#include "lexicon_lexicon.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            size_t CLexicon::lookupWord(const std::wstring &wstrWord, const std::wstring &wstrPOS, CLexeme &lexeme) const
            {
                // get all items of the word
                std::vector<CLexeme> vecLexeme;
                lookupWord(wstrWord, vecLexeme);

                // find POS
                for (std::vector<CLexeme>::const_iterator it = vecLexeme.begin(); it != vecLexeme.end(); it++)
                {
                    if (it->wstrPOS == wstrPOS)
                    {
                        // found
                        lexeme = *it;
                        return 1;
                    }
                }

                // not found
                return 0;
            }

            const std::wstring &CLexicon::getPhoneme(const std::wstring &wstrWord, const std::wstring &wstrPOS, std::wstring &retPhoneme) const
            {
                // The procedure will lookup the word in the lexicon by matching both text and POS.
                // The procedure will:
                // (1) return the phoneme directly if both text and POS are matched;
                // (2) return the most appropriate one (e.g. partial match of POS) if POS is not matched
                //     (currently, return the first default phoneme of the word);
                // (3) match the surname and given name respectively if POS is "nr" for name entities; and
                // (4) match the text characters one by one if text is not found in the lexicon.

                std::vector<CLexeme> wordEntries;
                size_t nEntries = lookupWord(wstrWord, wordEntries);
                for (size_t i = 0; i< nEntries; i++)
                {
                    if (wstrPOS == wordEntries[i].wstrPOS)
                    {
                        // word and POS matched
                        retPhoneme = wordEntries[i].wstrPhoneme;
                        return retPhoneme;
                    }
                }
                if (nEntries > 0)
                {
                    // POS not matched
                    // use the phoneme in the first entry
                    retPhoneme = wordEntries[0].wstrPhoneme;
                    return retPhoneme;
                }
                else
                {
                    // word not found
                    retPhoneme = L"";
                    std::wstring cpyWord = wstrWord;
                    size_t nWordLen = wstrWord.length();
                    size_t i = 0;

                    // match name entity first
                    if (wstrPOS == L"nr")
                    {
                        // surname (POS = "ng") is matched here
                        CLexeme wordEntry;
                        while (cpyWord.length()  >= 1)
                        {
                            if (lookupWord(cpyWord, L"ng", wordEntry) > 0)
                            {
                                retPhoneme = retPhoneme + L" " + wordEntry.wstrPhoneme;
                                i += cpyWord.length();
                                break;
                            }
                            cpyWord.erase(cpyWord.length()-1, 1);
                        }
                        // given name will be matched as follows
                    }

                    // get phoneme by forward maximum match
                    while (i < nWordLen)
                    {
                        cpyWord = wstrWord.substr(i);

                        // query words from maximum length till 1 or word matched
                        wordEntries.clear();
                        while (cpyWord.length()  >= 1)
                        {
                            if (lookupWord(cpyWord, wordEntries) > 0)
                            {
                                retPhoneme = retPhoneme + L" " + wordEntries[0].wstrPhoneme;
                                break;
                            }
                            cpyWord.erase(cpyWord.length()-1, 1);
                        }

                        // advance to next word
                        i += (cpyWord.length() > 0) ? cpyWord.length() : 1;
                    }

                    // remove the first "-" or " "
                    if (retPhoneme.length() > 0)
                        retPhoneme = retPhoneme.substr(1);
                    return retPhoneme;
                }
            }

        }//namespace base
    }
}
