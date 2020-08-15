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
/// @brief    Head file for lexicon management module which loads data into memory
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/05/12
///   Changed:  Created
///


#ifndef _CST_TTS_CHINESE_LEXICON_DATA_H_
#define _CST_TTS_CHINESE_LEXICON_DATA_H_

#include "ttsbase/datatext/lexicon_lexicon.h"
#include <map>

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  Use CLexeme in TTSBase directly
            ///
            using base::CLexeme;

            ///
            /// @brief  Lexicon management which loads word entries into memory
            ///
            ///
            /// This implementation extends basic Lexicon by incorporating "word appearance frequency" information.
            /// 
            class CLexiconData : public base::CLexicon
            {
            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Input/Output manipulation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Constructor
                ///
                /// @param  [in] ignorePOS  Whether POS attribute of the lexeme will be ignored.
                ///                         If true, only the last occurred non-empty POS will be kept.
                ///
                CLexiconData(bool ignorePOS = false);

                ///
                /// @brief  Destructor
                ///
                virtual ~CLexiconData();

                ///
                /// @brief  Load lexicon data from CSV (comma-separated values) file
                ///
                /// @param  [in] fileName   Name of the CSV file where lexicon data are stored
                ///
                /// @param  Whether lexicon data are loaded successfully
                ///
                virtual bool load(const wchar_t *fileName);

                ///
                /// @brief  Clear lexicon data
                ///
                /// @return Whether data are cleared successfully
                ///
                virtual bool clear();

            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Lexicon manipulation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Get the maximum word length of lexeme entries
                ///
                /// @return The maximum length (number of characters) of words in current lexicon
                ///
                virtual size_t getWordMaxLen() const {return m_nMaxWordLen;}

                ///
                /// @brief  Lookup the word in the lexicon, return all the entries which <b>whole match</b> the input word string
                ///
                /// @param  [in]  wstrWord      The string of the word to be searched in the lexicon
                /// @param  [out] vecLexeme     Return the retrieved lexeme items (in vector). The new result is <b>APPEND</b> to the vector
                ///
                /// @return The number of lexeme items found and appended
                ///
                virtual size_t lookupWord(const std::wstring &wstrWord, std::vector<CLexeme> &vecLexeme) const;

                ///
                /// @brief  Lookup word in the lexicon, return the entry which <b>whole match</b> the input word and part-of-speech
                ///
                /// @param  [in]  wstrWord  Word text to be searched in the lexicon
                /// @param  [in]  wstrPOS   Part-of-speech to be searched in the lexicon
                /// @param  [out] lexeme    Return the retrieved lexeme item
                ///
                /// @return The number of lexeme found (1 if item is found, 0 otherwise)
                ///
                virtual size_t lookupWord(const std::wstring &wstrWord, const std::wstring &wstrPOS, CLexeme &lexeme) const;

                ///
                /// @brief  Add one word entry into the lexicon
                ///
                /// @param  [in] lexemeInfo Lexeme word entry information to be added
                ///
                /// @return Whether word lexeme is added successfully
                ///
                virtual bool addWord(const CLexeme &lexemeInfo);

                ///
                /// @brief  Get appearance frequency of the given POS (part-of-speech), 0 if not found
                ///
                /// @param  [in] wstrPOS    POS to be queried
                ///
                /// @return Appearance frequency of the queried POS
                ///
                virtual size_t getPOSFrequency(const std::wstring &wstrPOS) const;


            protected:
                ///
                /// @brief  Initialize the POS appearance frequency table
                ///
                /// The POS appearance frequency is built according to the appearance frequency of the word.
                /// Each word has the POS tag and the appearance frequency.
                /// The appearance frequencies of all words with the same POS are added to get the POS frequency.
                ///
                void buildPOSFrequencyTable();

                ///
                /// @brief  Internal raw lexeme item, used for each lexeme in lexicon.
                ///         Each word in lexicon has a vector of such raw items.
                ///
                struct RawLexeme
                {
                    std::wstring wstrGrapheme;      ///< Grapheme (word text)
                    std::wstring wstrEquivalent;    ///< Equivalent, for Traditional-Simplified Chinese conversion
                    std::wstring wstrAlias;         ///< Alias, for text normalization of symbols
                    std::wstring wstrPhoneme;       ///< Phoneme (pronunciation) of the word
                    std::wstring wstrPOS;           ///< Part-of-speech of the word
                    size_t       nFrequency;        ///< Appearance frequency of the word
                };

            protected:
                /// Vector storing lexicon data
                std::vector<std::vector<RawLexeme> > m_vecLexemes;
                /// Mapping between word and lexeme index
                std::map<std::wstring, size_t> m_mapLexemes;
                /// POS appearance frequency table
                std::map<std::wstring, size_t> m_mapPOSFrequency;
                /// Maximum word length (number of characters) in current lexicon
                size_t m_nMaxWordLen;
                /// Ignore POS (Part-of-Speech) when loading lexemes (only the last occurred non-empty POS will be kept)
                bool m_ignorePOS;
            };

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_LEXICON_DATA_H_
