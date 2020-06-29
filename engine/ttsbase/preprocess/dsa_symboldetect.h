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
/// @brief    Head file for detecting the symbols
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/06/22
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSA_SYMBOLDETECT_H_
#define _CST_TTS_BASE_DSA_SYMBOLDETECT_H_

#include "xml/ssml_document.h"
#include "utils/utl_regexp.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            class CSymbolDetect
            {
            public:
                static bool detectPunctuation(const std::wstring &wstrText, int &nSymStartPos, int &nSymLength);

                static bool detectFormat(const std::wstring &wstrSymbol, std::wstring &wstrInterpretAs, std::wstring &wstrFormat, std::wstring &wstrDetail);

                static bool isCertainFormat(const cmn::wstring wstrText, const cmn::CRegexp *regexp);

            protected:
                enum ECharacterType
                {
                    //    CHR_NONE,        ///

                    CHR_BLANK,          ///< blank and tab
                    //    CHR_RETURN,        /// line feed and carrige return
                    CHR_TEXT,           ///< Chinese characters
                    CHR_EXCLAMATION,    ///< exclamation symbol
                    CHR_QUOTE,          ///< '"' , ''' , used in normalization
                    CHR_SINGLEQUOTE,
                    CHR_DOLLAR,         ///< '$'
                    CHR_PERCENT,        ///< '%' , per thousand mark , used in normalization,
                    CHR_LBRACKET,       ///< '(' '[', full-width '{'
                    CHR_RBRACKET,       ///< ')' ']', full-width '}'
                    CHR_MULTIPLY,       ///< '*' , used in normalization
                    CHR_PLUS,           ///< '+' , used in normalization
                    CHR_COMMA,          ///< comma
                    CHR_COMMA_CHINESE,   ///< chinese comma
                    CHR_MINUS,          ///< minus '-' and tidle '~', used in normalization,
                    CHR_PERIOD,         ///< full-width period
                    CHR_DIVIDE,         ///< '/' , used in normalization
                    CHR_NUMBER,         ///< numbers
                    CHR_COLON,          ///< colons
                    CHR_COLON_CHINESE,  ///< Chinese colons;
                    CHR_SEMICOLON,      ///< semicolons
                    CHR_LESS,           ///< less than '<' , used in normalization,
                    CHR_EQUAL,          ///< equal '=' , used in normalization,
                    CHR_GREATER,        ///< greater than '>'
                    CHR_QUESTION,       ///< question mark
                    CHR_LETTER,         ///< letters
                    CHR_PAUSE,          ///< Chinese dot used to seprate words
                    CHR_DOT,            ///< dot, half-with period
                    CHR_DASH,           ///< underscore
                    CHR_DASH_CHINESE,
                    CHR_ELLIPSIS,       ///< full width sign of "..."
                    CHR_LEFTQUOTE,      ///< full-width left sigle and doule quote, and book
                    ///< name symbol , used in normalization,
                    CHR_LEFTQUOTE_CHINESE,
                    CHR_RIGHTQUOTE,     ///< full-width right sigle and doule quote, and book
                    ///< name symbol , used in normalization,
                    CHR_RIGHTQUOTE_CHINESE,
                    CHR_YUAN,           ///< Renminbi Yuan sign
                    CHR_LBWORD,         ///< half-width '{'
                    CHR_RBWORD,         ///< half-width '}'
                    //chrDian
                    CHR_UNKNOWN         ///< Type unknown. The character is not assigned a type
                };

                static ECharacterType getCharType(wchar_t wchChar);

                static bool judgePunctuation(const std::wstring &wstrText, int &nSymStartPos, int &nSymLength);

            protected:
                const static cmn::wstring m_str_terminator_paragraph;
                const static cmn::wstring m_str_terminator_sentence;

                const static cmn::CRegexp regTerminator[];
                const static cmn::CRegexp *regExpArrayStrict[];

                const static cmn::wstring wstrInterpretArray[];
                const static cmn::wstring wstrFormatArray[][10];
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_DSA_SYMBOLDETECT_H_
