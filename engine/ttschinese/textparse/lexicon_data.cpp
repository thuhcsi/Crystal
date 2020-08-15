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
/// @brief    Implementation file for lexicon management module which loads data into memory
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/06/12
///   Changed:  Created
///


#include "lexicon_data.h"
#include "cmn/cmn_textfile.h"
#include <sstream>

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            CLexiconData::CLexiconData(bool ignorePOS)
            {
                m_ignorePOS    = ignorePOS;
                m_nMaxWordLen  = 0;
            }

            CLexiconData::~CLexiconData()
            {
                clear();
            }

            bool CLexiconData::clear()
            {
                m_vecLexemes.clear();
                m_mapLexemes.clear();
                m_mapPOSFrequency.clear();
                m_nMaxWordLen  = 0;
                return true;
            }

            bool CLexiconData::load(const wchar_t *fileName)
            {
                // open the text file
                std::wstring txtBuf;
                cmn::CTextFile txtFile;
                if (!txtFile.open(fileName, L"rb"))
                {
                    return false;
                }
                txtFile.readString(txtBuf);

                // load with istream
                std::wistringstream wstristream(txtBuf);
                std::wstring oneLine;
                std::vector<std::wstring> valLine;

                // loop for each line
                CLexeme lexItem;
                while (std::getline(wstristream, oneLine))
                {
                    str::tokenize(oneLine, L",", valLine);

                    // check for language and alphabet
                    if (valLine.size() == 2)
                    {
                        m_descriptor.wstrLanguage = valLine[0];
                        m_descriptor.wstrAlphabet = str::trim(valLine[1], L"\r\n");
                        continue;
                    }

                    // deal with grapheme ","
                    if (valLine.size() == 5 && valLine[0] == L"" && valLine[1] == L"")
                    {
                        valLine[1] = L",";
                        valLine.erase(valLine.begin());
                    }

                    // check validation
                    if (valLine.size() != 4)
                        return false;

                    // grapheme
                    lexItem.wstrGrapheme = valLine[0];

                    // phoneme
                    lexItem.wstrPhoneme  = valLine[1];

                    // pos
                    lexItem.wstrPOS = valLine[2];

                    // frequency
                    lexItem.nFrequency = 1; // default
                    std::swscanf(valLine[3].c_str(), L"%d", &lexItem.nFrequency);

                    // insert item
                    addWord(lexItem);
                }

                // initialize POS frequency table
                buildPOSFrequencyTable();

                return true;
            }

            bool CLexiconData::addWord(const CLexeme &lexemeInfo)
            {
                std::map<std::wstring, size_t>::iterator it = m_mapLexemes.find(lexemeInfo.wstrGrapheme);

                RawLexeme newLexeme;
                newLexeme.wstrGrapheme= lexemeInfo.wstrGrapheme;
                newLexeme.wstrPhoneme = lexemeInfo.wstrPhoneme;
                newLexeme.wstrPOS     = lexemeInfo.wstrPOS;
                newLexeme.nFrequency  = lexemeInfo.nFrequency;

                if (it == m_mapLexemes.end())
                {
                    // no related word entry yet, insert new one
                    m_vecLexemes.push_back(std::vector<RawLexeme>(1, newLexeme));
                    m_mapLexemes.insert(std::make_pair(lexemeInfo.wstrGrapheme, m_vecLexemes.size()-1));

                    // update the maximum word length
                    size_t len = lexemeInfo.wstrGrapheme.length();
                    if (len > m_nMaxWordLen)
                        m_nMaxWordLen = len;

                    return true;
                }
                else
                {
                    // word entry exists, insert new lexeme
                    std::vector<RawLexeme> &tmpLexemes = m_vecLexemes[it->second];
                    std::vector<RawLexeme>::iterator itLex = tmpLexemes.begin();
                    if (!m_ignorePOS)
                    {
                        // search POS only when multiple POS attributes are kept
                        for (; itLex != tmpLexemes.end(); itLex ++)
                        {
                            if (itLex->wstrPOS == newLexeme.wstrPOS)
                                break;
                            // replace original lexeme whose POS is "empty"
                            if (itLex->wstrPOS.empty() && !newLexeme.wstrPOS.empty())
                                break;
                        }
                    }
                    if (itLex != tmpLexemes.end())
                    {
                        // update existing lexeme
                        itLex->wstrPhoneme = newLexeme.wstrPhoneme;
                        itLex->nFrequency  = newLexeme.nFrequency;
                        if (!newLexeme.wstrPOS.empty())
                            itLex->wstrPOS = newLexeme.wstrPOS;
                    }
                    else
                    {
                        // insert new lexeme
                        tmpLexemes.push_back(newLexeme);
                    }
                    return true;
                }
            }

            size_t CLexiconData::lookupWord(const std::wstring &wstrWord, std::vector<CLexeme> &vecLexeme) const
            {
                std::map<std::wstring, size_t>::const_iterator itFind = m_mapLexemes.find(wstrWord);
                if (itFind == m_mapLexemes.end())
                {
                    // not found
                    return 0;
                }

                // retrieve lexemes
                CLexeme tmpLexeme;
                const std::vector<RawLexeme> &tmpLexemes = m_vecLexemes[itFind->second];
                vecLexeme.reserve(vecLexeme.size() + tmpLexemes.size());

                for (std::vector<RawLexeme>::const_iterator it = tmpLexemes.begin(); it != tmpLexemes.end(); it++)
                {
                    tmpLexeme.wstrGrapheme = wstrWord;
                    tmpLexeme.wstrPhoneme  = it->wstrPhoneme;
                    tmpLexeme.wstrPOS      = it->wstrPOS;
                    tmpLexeme.nFrequency   = it->nFrequency;
                    vecLexeme.push_back(tmpLexeme);
                }
                return tmpLexemes.size();
            }

            size_t CLexiconData::lookupWord(const std::wstring &wstrWord, const std::wstring &wstrPOS, CLexeme &lexeme) const
            {
                std::map<std::wstring, size_t>::const_iterator itFind = m_mapLexemes.find(wstrWord);
                if (itFind == m_mapLexemes.end())
                {
                    // not found
                    return 0;
                }

                // retrieve lexeme
                const std::vector<RawLexeme> &tmpLexemes = m_vecLexemes[itFind->second];
                for (std::vector<RawLexeme>::const_iterator it = tmpLexemes.begin(); it != tmpLexemes.end(); it++)
                {
                    if (it->wstrPOS == wstrPOS)
                    {
                        // found
                        lexeme.wstrGrapheme = wstrWord;
                        lexeme.wstrPhoneme  = it->wstrPhoneme;
                        lexeme.wstrPOS      = it->wstrPOS;
                        lexeme.nFrequency   = it->nFrequency;
                        return 1;
                    }
                }

                // not found
                return 0;
            }

            size_t CLexiconData::getPOSFrequency(const std::wstring &wstrPOS) const
            {
                std::map<std::wstring, size_t>::const_iterator it = m_mapPOSFrequency.find(wstrPOS);
                if (it != m_mapPOSFrequency.end())
                {
                    return it->second;
                }
                return 0;
            }

            void CLexiconData::buildPOSFrequencyTable()
            {
                std::map<std::wstring, size_t>::iterator itPOS;
                std::vector<std::vector<RawLexeme> >::const_iterator itLex;
                std::vector<RawLexeme>::const_iterator itItem;
                for (itLex = m_vecLexemes.begin(); itLex != m_vecLexemes.end(); itLex++)
                {
                    /// @todo non Chinese characters will not be calculated
                    for (itItem = itLex->begin(); itItem != itLex->end(); itItem++)
                    {
                        if (itItem->wstrEquivalent.length() > 0)
                        {
                            continue;
                        }
                        // update POS frequency
                        itPOS = m_mapPOSFrequency.find(itItem->wstrPOS);
                        if (itPOS == m_mapPOSFrequency.end())
                        {
                            m_mapPOSFrequency.insert(std::make_pair(itItem->wstrPOS, itItem->nFrequency));
                        }
                        else
                        {
                            itPOS->second += itItem->nFrequency;
                        }
                    }
                }
            }

        }//namespace Chinese
    }
}
