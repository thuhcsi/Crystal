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
/// @brief    Implementation file for detecting the symbols
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/06/22
///   Changed:  Created
///

#include "cmn/cmn_error.h"
#include "dsa_symboldetect.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            const cmn::wstring CSymbolDetect::m_str_terminator_paragraph = L"[\x19]";
            const cmn::wstring CSymbolDetect::m_str_terminator_sentence = L"[。|．|.|，|,|：|:|？|?|！|!]";

            const cmn::CRegexp CSymbolDetect::regTerminator[] =
            {
                cmn::CRegexp((L"^"+m_str_terminator_sentence).c_str(), false),
                cmn::CRegexp((L"^"+m_str_terminator_paragraph).c_str(), false),
                cmn::CRegexp(NULL, false)
            };
            const cmn::CRegexp* CSymbolDetect::regExpArrayStrict[] =
            {
                regTerminator,
                NULL
            };
            const cmn::wstring CSymbolDetect::wstrInterpretArray[]=
            {
                L"punctuation"
            };
            const cmn::wstring CSymbolDetect::wstrFormatArray[][10]=
            {
                {L"s", L"p"}
            };

            bool CSymbolDetect::isCertainFormat(const cmn::wstring wstrText, const cmn::CRegexp* regexp)
            {
                if ( regexp->compiled() && regexp->match(wstrText.c_str()))
                {
                    return true;
                }
                return false;
            }

            CSymbolDetect::ECharacterType CSymbolDetect::getCharType(wchar_t wchChar)
            {
                switch(wchChar)
                {
                case L' ':case L'\u3000':case L'\t':
                    return CHR_BLANK;       // blank and tab
                case L'!':case L'\uff01':
                    return CHR_EXCLAMATION; // exclamation symbol
                case L'\'':case L'\"':case L'\uff02':case L'\uff07':
                    return CHR_QUOTE;       // '"' '''
                case L'$':case L'\uff04':
                    return CHR_DOLLAR;      // '$'
                case L'%':case L'\uff05':case L'\u2030':
                    return CHR_PERCENT;     // '%' , per thousand mark
                case L'(':case L'[':case L'\uff08':case L'\uff3b':case L'\uff5b':
                    return CHR_LBRACKET;
                case L')':case L']':case L'\uff09':case L'\uff3d':case L'\uff5d':
                    return CHR_RBRACKET;
                case L'*':case L'\uff0a':
                    return CHR_MULTIPLY;    // '*'
                case L'+':case L'\uff0b':
                    return CHR_PLUS;        // '+'
                    //case L',':case L'\uff0c':
                case L',':
                    return CHR_COMMA;
                case L'\uff0c':
                    return CHR_COMMA_CHINESE;       // comma
                case L'-':case L'~':case L'\uff0d':case L'\uff5e':
                    return CHR_MINUS;       // '-', '~'
                case L'\u3002':
                    return CHR_PERIOD;      // full-width period
                case L'/':case L'\uff0f':
                    return CHR_DIVIDE;
                case L'0': case L'1': case L'2': case L'3': case L'4':
                case L'5': case L'6': case L'7': case L'8': case L'9':
                case L'\uff10':case L'\uff11':case L'\uff12':case L'\uff13':case L'\uff14':
                case L'\uff15':case L'\uff16':case L'\uff17':case L'\uff18':case L'\uff19':
                    return CHR_NUMBER;      // numbers
                case L':': 
                    return CHR_COLON;       // colons
                case L'\uff1a':
                    return CHR_COLON_CHINESE;
                case L';': case L'\uff1b':
                    return CHR_SEMICOLON;   // semicolons
                case L'<':case L'\uff1c':
                    return CHR_LESS;        // less than, '<'
                case L'=':case L'\uff1d':
                    return CHR_EQUAL;       // equal '='
                case L'>':case L'\uff1e':
                    return CHR_GREATER;     // greater than, '>'
                case L'?':case L'\uff1f':
                    return CHR_QUESTION;    // question mark
                case L'A': case L'B': case L'C': case L'D': case L'E': case L'F': case L'G':
                case L'H': case L'I': case L'J': case L'K': case L'L': case L'M': case L'N':
                case L'O': case L'P': case L'Q': case L'R': case L'S': case L'T':
                case L'U': case L'V': case L'W': case L'X': case L'Y': case L'Z':
                case L'a': case L'b': case L'c': case L'd': case L'e': case L'f': case L'g':
                case L'h': case L'i': case L'j': case L'k': case L'l': case L'm': case L'n':
                case L'o': case L'p': case L'q': case L'r': case L's': case L't':
                case L'u': case L'v': case L'w': case L'x': case L'y': case L'z':
                case L'\uff21': case L'\uff22': case L'\uff23': case L'\uff24': case L'\uff25': case L'\uff26': case L'\uff27':
                case L'\uff28': case L'\uff29': case L'\uff2a': case L'\uff2b': case L'\uff2c': case L'\uff2d': case L'\uff2e':
                case L'\uff2f': case L'\uff30': case L'\uff31': case L'\uff32': case L'\uff33': case L'\uff34':
                case L'\uff35': case L'\uff36': case L'\uff37': case L'\uff38': case L'\uff39': case L'\uff3a':
                case L'\uff41': case L'\uff42': case L'\uff43': case L'\uff44': case L'\uff45': case L'\uff46': case L'\uff47':
                case L'\uff48': case L'\uff49': case L'\uff4a': case L'\uff4b': case L'\uff4c': case L'\uff4d': case L'\uff4e':
                case L'\uff4f': case L'\uff50': case L'\uff51': case L'\uff52': case L'\uff53': case L'\uff54':
                case L'\uff55': case L'\uff56': case L'\uff57': case L'\uff58': case L'\uff59': case L'\uff5a':
                    return CHR_LETTER;      // letters
                case L'\u3001':
                    return CHR_PAUSE;       // Chinese puctuation, used to seprate words
                    // in western countries, ',' is used.
                case L'.':case L'\uff0e':
                    return CHR_DOT;         // dot, half-with period
                case L'_': 
                    return CHR_DASH;        // underscore
                case L'\u2014':
                    return CHR_DASH_CHINESE;
                case L'\u2026':
                    return CHR_ELLIPSIS;    // full width "..."
                case L'\u2018':case L'\u300a':
                    return CHR_LEFTQUOTE;   // left sigle quote, double quote, tile mark
                case L'\u201c':
                    return CHR_LEFTQUOTE_CHINESE;
                case L'\u2019':case L'\u300b':
                    return CHR_RIGHTQUOTE;  // right sigle quote, double quote, tile mark
                case L'\u201d':
                    return CHR_RIGHTQUOTE_CHINESE;
                    //    chrRQuote            ,
                case L'\uffe5':
                    return CHR_YUAN;        // RMB yuan
                case L'{':
                    return CHR_LBWORD;      // '{'
                case L'}':
                    return CHR_RBWORD;      // '}'
                    //chrDian
                default:
                    if (
                        (wchChar >= L'\u4e00' && wchChar <= L'\u9fbb') || // unified area
                        (wchChar >= L'\u3400' && wchChar <= L'\u4db5') || // Ext A
                        (wchChar >= L'\uf900' && wchChar <= L'\ufad9')    // Compability
                        )
                    {
                        return CHR_TEXT;
                    }
                    else
                    {
                        return CHR_UNKNOWN;
                    }
                }
            }

            bool CSymbolDetect::judgePunctuation(const std::wstring &wstrText, int &nSymStartPos, int &nSymLength)
            {
                if (nSymLength == 0)
                    return false;
                if (nSymLength > 1) // e.g. "\n\r"
                    return true;

                // judge whether it really is punctuation
                ECharacterType typeCurt = getCharType(wstrText[nSymStartPos]);
                ECharacterType typePrev = nSymStartPos>0 ? getCharType(wstrText[nSymStartPos-1]) : CHR_UNKNOWN;
                ECharacterType typeNext = nSymStartPos+1<wstrText.length() ? getCharType(wstrText[nSymStartPos+1]) : CHR_UNKNOWN;
                switch (typeCurt)
                {
                case CHR_COLON:
                case CHR_COMMA:
                    //if the comma or colon is not between two numbers
                    if (typePrev !=CHR_NUMBER ||typeNext !=CHR_NUMBER)
                        return true;
                    else
                        return false;
                case CHR_DOT:
                    //if the dot is not between two numbers and not between two English letters
                    if ((typePrev == CHR_NUMBER && typeNext == CHR_NUMBER) ||
                        (typePrev == CHR_LETTER && typeNext == CHR_LETTER) )
                        return false;
                    else
                        return true;
                case CHR_QUOTE:
                    if (typePrev !=CHR_NUMBER)
                        return true;
                    else
                        return false;
                default:
                    return true;
                }
            }

            bool CSymbolDetect::detectPunctuation(const std::wstring &wstrText, int &nSymStartPos, int &nSymLength)
            {
                // detect all punctuations
                static const cmn::CRegexp regAllPunc((m_str_terminator_paragraph + L"|" + m_str_terminator_sentence).c_str(), false);

                const wchar_t *pText = wstrText.c_str();
                while (*pText != L'\0')
                {
                    // detect the punctuations (for paragraph and sentence)
                    if (regAllPunc.compiled() && regAllPunc.match(pText))
                    {
                        nSymStartPos = regAllPunc.substart(0) + pText - wstrText.c_str();
                        nSymLength   = regAllPunc.sublength(0);
                    }
                    else
                    {
                        // no punctuation is found
                        return false;
                    }

                    // detect the real format of punctuation
                    size_t idxFormat = 0;
                    const cmn::CRegexp *pRegExpFormat = &regTerminator[idxFormat];
                    while (pRegExpFormat->compiled())
                    {
                        if (pRegExpFormat->match(pText + regAllPunc.substart(0)))
                        {
                            // punctuation is matched in the text
                            if (pRegExpFormat->sublength(0) == nSymLength)
                                break;
                        }
                        pRegExpFormat = &regTerminator[++idxFormat];
                    }
                    if (!pRegExpFormat->compiled())
                        return false;

                    // judge further whether it really is punctuation
                    if (judgePunctuation(wstrText, nSymStartPos, nSymLength))
                        return true;

                    // look for the remaining part of the text
                    pText = wstrText.c_str() + nSymStartPos + nSymLength;
                }
                return false;
            }

            bool CSymbolDetect::detectFormat(const std::wstring &wstrSymbol, std::wstring &wstrInterpretAs, std::wstring &wstrFormat, std::wstring &wstrDetail)
            {
                const cmn::CRegexp *pRegExpFormat = NULL;
                int idxInterpret = 0;
                int idxFormat = 0;
                while (regExpArrayStrict[idxInterpret] != NULL)
                {
                    pRegExpFormat = &regExpArrayStrict[idxInterpret][idxFormat];
                    while (pRegExpFormat != NULL && pRegExpFormat->compiled())
                    {
                        if (isCertainFormat(wstrSymbol, pRegExpFormat))
                        {
                            wstrInterpretAs = wstrInterpretArray[idxInterpret];
                            wstrFormat = wstrFormatArray[idxInterpret][idxFormat];
                            wstrDetail = L"";
                            return true;
                        }
                        idxFormat ++;
                        pRegExpFormat = &regExpArrayStrict[idxInterpret][idxFormat];
                    }
                    idxInterpret ++;
                    idxFormat = 0;
                }
                wstrInterpretAs = L"symbol";
                wstrFormat = L"";
                wstrDetail = L"";
                return true;
            }

        }//namespace base
    }
}
