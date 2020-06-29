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
/// @brief    Head file for the interface of lexicon accessing
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/03
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_LEXICON_LEXICON_H_
#define _CST_TTS_BASE_LEXICON_LEXICON_H_

#include <string>
#include <vector>

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The base class for lexeme item information.
            ///         It can be inherited for different engines and modules.
            ///
            class CLexeme
            {
            public:
                std::wstring wstrGrapheme;  ///< Grapheme (word) information
                std::wstring wstrPhoneme;   ///< Phoneme (pronunciation) of the word
                std::wstring wstrPOS;       ///< Part-of-speech of the word
                size_t       nFrequency;    ///< Statistical appearance frequency of the word
                virtual ~CLexeme() {}       ///< Only the object with virtual tables can use dynamic_cast
            };

            ///
            /// @brief  The base implementation interface class for lexicon accessing
            ///
            class CLexicon
            {
            public:
                ///
                /// @brief  Detailed description of the lexicon
                ///
                class Descriptor
                {
                public:
                    std::wstring wstrAlphabet;  ///< Pronunciation alphabet of the lexicon
                    std::wstring wstrLanguage;  ///< Language for which pronunciation lexicon is relevant
                public:
                    /// Whether two descriptors are the same
                    bool operator==(const Descriptor &right) const
                    {
                        return (right.wstrAlphabet == wstrAlphabet) && (right.wstrLanguage == wstrLanguage);
                    }
                };

            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Initialization
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Constructor
                ///
                CLexicon() {}

                ///
                /// @brief  Destructor
                ///
                virtual ~CLexicon() {}

                ///
                /// @brief  Clear lexicon data
                ///
                /// @return Whether data are cleared successfully
                ///
                virtual bool clear() = 0;

                ///
                /// @brief  Get the detailed description of the lexicon
                ///
                virtual const Descriptor &getDescriptor() const {return m_descriptor;}

                ///
                /// @brief  Set the detailed description of the lexicon
                ///
                virtual void setDescriptor(const Descriptor &descriptor) {m_descriptor = descriptor;}


            public:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Lexicon manipulation
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Get the maximum word length of the entries in the current lexicon
                ///
                /// @return The maximum length (number of characters) of the word in current lexicon
                ///
                virtual size_t getWordMaxLen() const = 0;

                ///
                /// @brief  Lookup word in the lexicon, return all the entries which <b>whole match</b> the input word string
                ///
                /// @param  [in]  wstrWord  Word text to be searched in the lexicon
                /// @param  [out] vecLexeme Return the retrieved lexeme items (in vector). The new result is <b>APPEND</b> to the vector
                ///
                /// @return The number of lexeme items found and appended
                ///
                virtual size_t lookupWord(const std::wstring &wstrWord, std::vector<CLexeme> &vecLexeme) const = 0;

                ///
                /// @brief  Lookup word in the lexicon, return the entry which <b>whole match</b> the input word and part-of-speech
                ///
                /// This default implementation uses the above lookupWord() directly and compares the POS of the returned entries.
                /// Overriding class can re-implement this function to achieve better performance if possible.
                ///
                /// @param  [in]  wstrWord  Word text to be searched in the lexicon
                /// @param  [in]  wstrPOS   Part-of-speech to be searched in the lexicon
                /// @param  [out] lexeme    Return the retrieved lexeme item
                ///
                /// @return The number of lexeme found (1 if item is found, 0 otherwise)
                ///
                virtual size_t lookupWord(const std::wstring &wstrWord, const std::wstring &wstrPOS, CLexeme &lexeme) const;

                ///
                /// @brief  Get the phoneme (pronunciation) information for the word with part-of-speech
                ///
                /// The procedure will lookup the word in the lexicon by matching both text and POS.
                /// The procedure will:
                /// (1) return the phoneme directly if both text and POS are matched;
                /// (2) return the most appropriate one (e.g. partial match of POS) if POS is not matched
                ///     (currently, return the first default phoneme of the word);
                /// (3) match the surname and given name respectively if POS is "nr" for name entities; and
                /// (4) match the text characters one by one if text is not found in the lexicon.
                ///
                /// This default implementation uses the above lookupWord() directly and compares the POS of the returned entries.
                /// Overriding class can re-implement this function to achieve better performance if possible.
                ///
                /// @param  [in]  wstrWord   The word text to lookup in the lexicon
                /// @param  [in]  wstrPOS    The part-of-speech information of the word
                /// @param  [out] retPhoneme Return the phoneme of the queries word
                ///
                /// @return Reference to the returned phoneme string
                ///
                virtual const std::wstring &getPhoneme(const std::wstring &wstrWord, const std::wstring &wstrPOS, std::wstring &retPhoneme) const;

                ///
                /// @brief  Get the alphabet information of the lexicon
                ///
                virtual const std::wstring &getAlphabet() const {return m_descriptor.wstrAlphabet;}


            protected:
                Descriptor m_descriptor;    ///< Detailed description of the lexicon


            private:
                ///
                /// @brief  Declared to prevent calling and implementation
                ///
                CLexicon &operator = (const CLexicon &);
                ///
                /// @brief  Declared to prevent calling and implementation
                ///
                CLexicon(const CLexicon &);
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_LEXICON_LEXICON_H_
