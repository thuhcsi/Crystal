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
/// @brief  Declaration of string manipulating utilities, including encode conversion between WCHAR and one of GBK, BIG5, UTF8.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Yongxin (fefe.wyx@gmail.com)
///   Date:     2006/07/21
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/06
///   Changed:  Modified by adding the namespaces
/// - Version:  0.2.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/05/22
///   Changed:  Modified by adding the support of HKSCS (the Hong Kong Supplementary Character Set) characters
///


#ifndef _CST_TOOLS_UTL_STRING_H_
#define _CST_TOOLS_UTL_STRING_H_

#include <string>
#include <vector>

namespace cst
{
    namespace str
    {
        ///
        /// @brief  Enumerations denoting the encodings currently supported
        ///
        enum EEncoding
        {
            ENC_AUTO=-1,///< Detect encoding automatically
            ENC_ANSI,   ///< ANSI encoding (default encoding with ANSI codepage)
            ENC_GB,     ///< GB2312, GBK, GB18030. The three are compatible, currently supported up to GBK in windows and will support GB18030 in Linux if possible
            ENC_BIG5,   ///< BIG5 with HKSCS (the Hong Kong Supplementary Character Set) characters
            ENC_UTF8,   ///< UTF-8
            ENC_UTF16,  ///< UTF-16 (little endian)
            ENC_UTF16BE,///< UTF-16 (big endian)
        };


        ///
        /// @brief  Find the source-sub-string in the string and replace it with destination-string
        ///
        /// @param  [in] srcOrg     The string whose sub-string is to be replaced
        /// @param  [in] strSrc     The source-sub-string to be replaced
        /// @param  [in] strDst     The destination-string to replace the source-sub-string
        ///
        /// @return Return the reference to the modified string
        ///
        std::wstring &replace(std::wstring &strOrg, const std::wstring &strSrc, const std::wstring &strDst);

        ///
        /// @brief  Trim the string by removing the eliminating-string from both front and back end
        ///
        /// @param  [in] strOrg     The string whose front and back sub-string is to be eliminated
        /// @param  [in] strToElim  The sub-string to be eliminated from the source string
        ///
        /// @return Return the reference to the modified string
        ///
        std::wstring &trim(std::wstring &strOrg, const std::wstring &strToElim);

        ///
        /// @brief  Convert to lower case for the string characters one by one
        ///
        /// @param  [in] strOrg     The string to be converted
        ///
        /// @return Return the reference to the modified string
        ///
        std::wstring &tolower(std::wstring &strOrg);

        ///
        /// @brief  Find the first non-blank (non-white-space) character start from posBeg
        ///
        /// Blanks (white spaces) are half-width and/or full-width blank characters and tab,
        /// not including line break.
        ///
        /// @param  [in] strLine    String input to find blank (white-space)
        /// @param  [in] posBeg     Start position for skipping space
        ///
        /// @return Where the next non-blank character is, or strLine.length() if no blank character
        ///
        std::wstring::size_type findNonSpace(const std::wstring &strLine, std::wstring::size_type posBeg);

        ///
        /// @brief  Format the string given the format and argument
        ///
        /// @return Return the composed string
        ///
        std::wstring format(const wchar_t *strFormat, ...);

        ///
        /// @brief  Tokenize the string into a vector of tokens separated by delimiters
        ///
        /// @param  [in] str        Input string to be tokenized
        /// @param  [in] delimiters String of delimiters to separate input string
        /// @param  [out] tokens    Return the vector of tokens
        ///
        /// @return Return the reference to the vector of tokens
        ///
        std::vector<std::wstring> &tokenize(const std::wstring &str, const std::wstring &delimiters, std::vector<std::wstring> &tokens);

        ///
        /// @brief  Converts wide-character string to corresponding multi-byte-character string with required encoding.
        /// @see    EEncoding for supported encodings
        ///
        std::string wcstombs(const std::wstring &wcs, EEncoding mbsenc=ENC_UTF8);

        ///
        /// @brief  Converts multi-byte-character string with input encoding to corresponding wide-character string.
        /// @see    EEncoding for supported encodings
        ///
        std::wstring mbstowcs(const std::string &mbs, EEncoding mbsenc=ENC_UTF8);


        //////////////////////////////////////////////////////////////////////////
        //
        //  UTF-16 character string processing related utilities
        //
        //////////////////////////////////////////////////////////////////////////

        ///
        /// @brief  Fetch one UTF-16 character from input string
        ///
        /// Deal with UTF-16 non-BMP characters:
        /// UTF-16 represents non-BMP characters (from U+10000 through U+10FFFF) using a pair of 16-bit words, known as a surrogate pair.
        /// To allow safe use of simple word-oriented string processing, separate ranges of values are used for the two surrogates:
        /// 0xD800¨C0xDBFF for the first, most significant surrogate and 0xDC00-0xDFFF for the second, least significant surrogate.
        ///
        /// @param [in]  wcsText    Input text from which one character to be fetched
        /// @param [in]  wcsChar    Pointer of the buffer to store the fetched character
        /// @param [out] wcsChar    Return the fetch character
        ///
        /// @return Return the pointer to the next characters of the input text
        ///
        const wchar_t *fetchCharacter(const wchar_t *wcsText, wchar_t *wcsChar);


        //////////////////////////////////////////////////////////////////////////
        //
        //  Encoding conversion related utilities
        //
        //////////////////////////////////////////////////////////////////////////

        ///
        /// @brief  Determine whether the multi-byte string is encoded by UTF-8
        ///
        bool is_utf8(const char *mbs);

        ///
        /// @brief  Converts wide-character string to corresponding multi-byte-character string with required encoding.
        /// @see    EEncoding for supported encodings
        ///
        /// @param  [in]  wcstr     Input wide-character string.
        /// @param  [in]  wcscnt    Number of wide-characters to be converted of the input wide-character string.
        /// @param  [in]  mbscnt    Maximum number of bytes that can be stored in the output multi-byte-character string.
        /// @param  [in]  mbsenc    Required encoding of the output multi-byte-character string
        /// @param  [out] mbstr     Output multi-byte-character string.
        ///
        /// @return Conversion will end if the input number of wide characters are converted or the ending zero is met.
        ///         If wcstombs successfully converts the source string, it returns the number of bytes (in char)
        ///         written into the multi-byte output string, EXCLUDing the terminating NULL (if any).
        ///         If the mbstr argument is NULL, wcstombs returns the required size of the destination string.
        ///         If wcstombs encounters a wide character it cannot be convert to a multi-byte character,
        ///         it returns ¨C1 cast to type size_t; and the multi-byte output string will stay unchanged.
        ///
        /// @note   The returned required size (i.e. queried with mbstr=NULL) is not NOT PRECISE but LARGE enough.
        ///         Please use the precise size after performing the real conversion (i.e. mbstr!=NULL).
        ///
        size_t wcstombs(const wchar_t *wcstr, size_t wcscnt, char *mbstr, size_t mbscnt, EEncoding mbsenc);


        ///
        /// @brief  Converts multi-byte-character string with input encoding to corresponding wide-character string.
        /// @see    EEncoding for supported encodings
        ///
        /// @param  [in]  mbstr     Input multi-byte-character string.
        /// @param  [in]  mbscnt    Number of multi-byte-characters to be converted of the input multi-byte-character string.
        /// @param  [in]  mbsenc    Encoding of the input multi-byte-character string
        /// @param  [in]  wcscnt    Maximum number of bytes that can be stored in the output wide-character string.
        /// @param  [out] wcstr     Output wide-character string.
        ///
        /// @return Conversion will end if the input number of characters are converted or the ending zero is met.
        ///         If mbstowcs successfully converts the source string, it returns the number of characters (in wchar_t)
        ///         written into the wide-character output string, EXCLUDing the terminating NULL (if any).
        ///         If the wcstr argument is NULL, mbstowcs returns the required size of the destination string (in wchar_t).
        ///         If mbstowcs encounters an invalid multi-byte character, it returns -1 case to type size_t;
        ///         and the wide-character output string will stay unchanged.
        ///
        /// @note   The returned required size (i.e. queried with wcstr=NULL) is not NOT PRECISE but LARGE enough.
        ///         Please use the precise size after performing the real conversion (i.e. wcstr!=NULL).
        ///         And there may be some leading Byte Order Mark (BOM) in Linux version (see following description).
        ///
        size_t mbstowcs(const char *mbstr, size_t mbscnt, wchar_t *wcstr, size_t wcscnt, EEncoding mbsenc);

    }//namespace str
}

#endif//_CST_TOOLS_UTL_STRING_H_
