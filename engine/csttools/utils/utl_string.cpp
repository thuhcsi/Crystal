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
/// - Version:  0.2.2
///   Author:   John (john.zywu@gmail.com)
///   Date:     2010/11/12
///   Changed:  Load HKSCS library (hkscs04.dll) dynamically. BIG5 will be used if HKSCS library is not found
///

#include <string>
#include <vector>
#include <cstdarg>
#include <cstring>  // memmove()
#include "utl_string.h"

#if defined(WINCE)
#   include <windows.h>
#elif defined(WIN32)
#   include <windows.h>
# if defined(HKSCS)
#   include "hkscs04.h" // for HKSCS characters
# endif
#elif defined(__GNUC__)
#   include <iconv.h>
#else
#   error "Operating system or Complier not supported!"
#endif

namespace cst
{
    namespace str
    {
        std::wstring &replace(std::wstring &strOrg, const std::wstring &strSrc, const std::wstring &strDst)
        {
            std::wstring::size_type nDstLen = strDst.length();
            std::wstring::size_type nSrcLen = strSrc.length();
            std::wstring::size_type nOccurPos = 0;   // occurrence position of strSrc
            std::wstring::size_type nCurrentPos = 0; // current position for scanning

            nOccurPos = strOrg.find(strSrc, nCurrentPos);
            while (nOccurPos != std::wstring::npos)
            {
                strOrg.replace(nOccurPos, nSrcLen, strDst);
                // jump over the strDst
                nCurrentPos = nOccurPos + nDstLen;
                nOccurPos = strOrg.find(strSrc, nCurrentPos);
            }
            return strOrg;
        }

        std::wstring &trim(std::wstring &strOrg, const std::wstring &strToElim)
        {
            std::wstring::size_type nPosStart = strOrg.find_first_not_of(strToElim);
            if (nPosStart == std::wstring::npos)
            {
                // nothing to keep
                strOrg.clear();
                return strOrg;
            }

            std::wstring::size_type nPosEnd = strOrg.find_last_not_of(strToElim);

            // nPosEnd should not be npos, as there are some characters not in strElim
            strOrg = strOrg.substr(nPosStart, nPosEnd-nPosStart+1);
            return strOrg;
        }

        std::wstring &tolower(std::wstring &strOrg)
        {
            wchar_t ch = L'a';
            for (std::wstring::iterator it = strOrg.begin(); it != strOrg.end(); ++it)
            {
                ch = *it;
                if (ch >= L'A' && ch <= L'Z')
                    *it = ch - L'A' + L'a';
            }
            return strOrg;
        }

        std::wstring::size_type findNonSpace(const std::wstring &strLine, std::wstring::size_type posBeg)
        {
            const std::wstring blanks = L" \u3000\t";

            std::wstring::size_type nPos = strLine.find_first_not_of(blanks, posBeg);
            if (nPos == std::wstring::npos)
                return strLine.length();
            else
                return nPos;
        }

        std::wstring format(const wchar_t *strFormat, ...)
        {
            static const int SBUF_SIZE = 260;
            wchar_t sBuf[SBUF_SIZE];

#pragma warning( push )
#pragma warning( disable : 4996 )
            va_list arglist;
            va_start(arglist, strFormat);
#if defined(WIN32) || defined(WINCE)
            int ret = _vsnwprintf(sBuf, SBUF_SIZE, strFormat, arglist);
#else
            int ret = vswprintf(sBuf, SBUF_SIZE, strFormat, arglist);
#endif
            va_end(arglist);
#pragma warning( pop )

            return sBuf;
        }

        std::vector<std::wstring> &tokenize(const std::wstring &str, const std::wstring &delimiters, std::vector<std::wstring> &tokens)
        {
            tokens.clear();

            std::wstring::size_type start = 0;
            std::wstring::size_type pos = str.find_first_of(delimiters, start);

            while (std::wstring::npos != pos)
            {
                // found a token, add it to the vector.
                tokens.push_back(str.substr(start, pos - start));
                // skip delimiters
                start = pos+1;
                // find next delimiter
                pos = str.find_first_of(delimiters, start);
            }
            if (str.length() > start)
            {
                // last token
                tokens.push_back(str.substr(start));
            }

            return tokens;
        }

        std::string wcstombs(const std::wstring &wcs, EEncoding mbsenc)
        {
            // get required length for multi-byte-character string
            size_t mbscnt = wcstombs(wcs.c_str(), wcs.length(), NULL, 0, mbsenc);

            // perform conversion
            char *tmpbuf = new char[mbscnt+5];
            mbscnt = wcstombs(wcs.c_str(), wcs.length(), tmpbuf, mbscnt+1, mbsenc);
            tmpbuf[mbscnt] = 0;
            std::string tmpstr(tmpbuf);
            delete []tmpbuf;
            return tmpstr;

        }

        std::wstring mbstowcs(const std::string &mbs, EEncoding mbsenc)
        {
            // get required length for wide-character string
            size_t wcscnt = mbstowcs(mbs.c_str(), mbs.length(), NULL, 0, mbsenc);

            //
            // attention:
            //  (1) to prevent twice buffer allocation operation,
            //      (const wchar_t*) has been forced converted to (wchar_t*)
            //  (2) however, it will cause memory crash!
            //  (3) so, the common method is finally used
            //std::wstring tmpstr(wcscnt+1, L'\0');
            //wcscnt = mbstowcs(mbs.c_str(), mbs.length(), (wchar_t*)tmpstr.c_str(), wcscnt, mbsenc);

            // perform conversion
            wchar_t *tmpbuf = new wchar_t[wcscnt+5];
            wcscnt = mbstowcs(mbs.c_str(), mbs.length(), tmpbuf, wcscnt+1, mbsenc);
            tmpbuf[wcscnt] = 0;
            std::wstring tmpstr(tmpbuf);
            delete []tmpbuf;
            return tmpstr;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  UTF-16 character string processing related utilities
        //
        //////////////////////////////////////////////////////////////////////////

        const wchar_t *fetchCharacter(const wchar_t *wcsText, wchar_t *wcsChar)
        {
            if (*wcsText >= 0xD800 && *wcsText <= 0xDBFF && *(wcsText+1) >= 0xDC00 && *(wcsText+1) <= 0xDFFF)
            {
                *wcsChar++ = *wcsText++;
                *wcsChar++ = *wcsText++;
                *wcsChar = 0;
                return wcsText;
            }
            else
            {
                *wcsChar++ = *wcsText++;
                *wcsChar = 0;
                return wcsText;
            }
        }



        //////////////////////////////////////////////////////////////////////////
        //
        //  Encoding conversion related utilities
        //
        //////////////////////////////////////////////////////////////////////////

        // mbs points to multi-byte text string
        // wchr points to a variable which will store the converted Unicode character
        // the function returns how many bytes of sz are converted to wchr Unicode character
        int utf8_to_unicode(const char *mbs, unsigned int *wchr)
        {
            if (mbs == NULL || wchr == NULL)
                return 0;

            unsigned char *u = (unsigned char *)mbs;
            int len=0;
            *wchr = 0;

            if ((u[0]&0x80) == 0x00)
            {
                // U-00000000 - U-0000007F : 0xxxxxxx
                // ASCII code
                *wchr = u[0];
                return 1;
            }

            if ((u[0]&0xE0) == 0xC0)
            {
                // U-00000080 - U-000007FF : 110xxxxx 10xxxxxx
                len = 2;
                *wchr = u[0]&0x1F;
            }
            else if ((u[0]&0xF0) == 0xE0)
            {
                // U-00000800 - U-0000FFFF : 1110xxxx 10xxxxxx 10xxxxxx
                len = 3;
                *wchr = u[0]&0x0F;
            }
            else if ((u[0]&0xF8) == 0xF0)
            {
                // U-00010000 - U-001FFFFF : 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                len = 4;
                *wchr = u[0]&0x07;
            }
            else
            {
                // UTF-8 character is malformed
                // return it as an extended-ASCII
                *wchr = u[0];
                return 1;
            }

            // read the following bytes
            for (int i=1; i<len; ++i)
            {
                if ((u[i]&0xC0) != 0x80)
                {
                    // UTF-8 code is malformed
                    // return it as an extended-ASCII
                    *wchr = u[0];
                    return 1;
                }
                // so far, everything seems OK
                // safely build the Unicode
                *wchr = (*wchr<<6) | (u[i]&0x3F);
            }

            // According to Unicode 5.0
            // codes in the range 0xD800 to 0xDFFF are not allowed
            if ((*wchr >= 0xD800) && (*wchr <= 0xDFFF))
            {
                // In this case, our UTF-8 code was well formed.
                // So, or we break it into 2 extended ASCII codes,
                // or we display an other symbol instead ...
                // We should read the Unicode 5.0 book to
                // to know their official recommendations though ...
                *wchr = '?';
                return 1;
            }
            // it is done!
            // *wchr contains Unicode.
            // simply return how many bytes of sz have been converted to *wchr Unicode character.
            return len;
        }

        // return the number of characters of the UTF-8 encoded string
        int utf8_strlen(const char *mbs)
        {
            if (mbs == NULL)
                return 0;
            unsigned int wchr = 0;
            int len = 0;
            int cnt = 0;
            while (*mbs != 0 )
            {
                cnt = utf8_to_unicode(mbs, &wchr);
                if (cnt == 0 || wchr == 0)
                    return len;
                mbs += cnt;
                ++len;
            }
            return len;
        }

        bool is_utf8(const char *mbs)
        {
#if 1
            // Quick detection of UTF-8 encoding
            // Return true when any of the UTF-8 character is matched

            // BYTE1 BYTE2 BYTE3 BYTE4
            // 00-7F
            // C2-DF 80-BF
            // E0-EF 80-BF 80-BF
            // F0-F4 80-BF 80-BF 80-BF
            const unsigned char * bytes = (const unsigned char *)mbs;
            while (*bytes)
            {
                // do not worry about the possibility of bytes[1],[2],[3]
                // exceeds the memory of string mbs,
                // because the terminal zero of string (0x00) will be first matched
                // to ensure the condition will return false

                if( // 2 bytes
                    (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                    ) {
                        return true;
                }
                if( // 3 bytes
                    (0xE0 <= bytes[0] && bytes[0] <= 0xEF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                    ) {
                        return true;
                }
                if( // 4 bytes
                    (0xF0 <= bytes[0] && bytes[0] <= 0xF4) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                    ) {
                        return true;
                }
                ++ bytes;
            }
            return false;
#else
            // This is the full implementation
            // that detects all the bytes in the input string

            const unsigned char * bytes = (const unsigned char *)mbs;
            while(*bytes)
            {
                if( (   // ASCII
                    bytes[0] == 0x09 ||
                    bytes[0] == 0x0A ||
                    bytes[0] == 0x0D ||
                    (0x20 <= bytes[0] && bytes[0] <= 0x7E)
                    )
                    ) {
                        bytes += 1;
                        continue;
                }

                if( (   // non-overlong 2-byte
                    (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                    )
                    ) {
                        bytes += 2;
                        continue;
                }

                if( (   // excluding overlong
                    bytes[0] == 0xE0 &&
                    (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                    ) ||
                    (   // straight 3-byte
                    ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                    ) ||
                    (   // excluding surrogates
                    bytes[0] == 0xED &&
                    (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                    )
                    ) {
                        bytes += 3;
                        continue;
                }

                if( (   // planes 1-3
                    bytes[0] == 0xF0 &&
                    (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                    ) ||
                    (   // planes 4-15
                    (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                    ) ||
                    (   // plane 16
                    bytes[0] == 0xF4 &&
                    (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                    )
                    ) {
                        bytes += 4;
                        continue;
                }

                // find a non-UTF-8 character
                return false;
            }

            // match all the UTF-8 characters
            return true;
#endif
        }

        size_t wcstombs(const wchar_t *wcstr, size_t wcscnt, char *mbstr, size_t mbscnt, EEncoding mbsenc)
        {
            if (wcstr == NULL || wcscnt == 0)
            {
                // no input
                return 0;
            }
            if (mbstr == NULL)
            {
                // User is querying the required size of the destination string.
                // Set mbscnt as zero because WideCharToMultiByte use this parameter 
                // indicating that we are querying the size
                mbscnt = 0;
            }

#if defined (WIN32) || defined(WINCE)

            if (mbsenc == ENC_UTF16)
            {
                // destination is already UTF16, no conversion is needed
                if (mbstr == NULL)
                {
                    return wcscnt * 2;
                }
                else
                {
                    size_t iConvLength = wcscnt * 2;
                    if (mbscnt < iConvLength)
                        iConvLength = mbscnt;
                    memcpy(mbstr, wcstr, iConvLength);
                    return iConvLength;
                }
            }

            // convert now
            unsigned int codePage;
            switch (mbsenc)
            {
            case ENC_UTF8:   codePage = CP_UTF8;    break; // UTF-8
            case ENC_GB:     codePage = 936;        break; // GB2312/GBK, GB18030 is not supported in windows
            case ENC_BIG5:   codePage = 950;        break; // BIG5
            case ENC_ANSI:   codePage = CP_ACP;     break; // ANSI
            case ENC_UTF16BE:codePage = 1201;       break; // Unicode UCS-2 Big-Endian
            default:         return 0;                     // this should not happen.
            }

            size_t iConvLength = WideCharToMultiByte(codePage, 0, wcstr, (int)wcscnt, mbstr, (int)mbscnt, NULL, false);
            return iConvLength;

#elif defined(__GNUC__)

            if (mbstr == NULL)
            {
                // @bug: this is terrible, but may work.
                // usually, multi-byte encoding won't use bytes four times greater of their wide characters
                return wcscnt * 4;
            }

            // set source code
            // detect the size of the UNICODE wide-character
            // detect the endian? (big-endian or little-endian)
            const char *CodeSrc = 0;
            wchar_t a = L'a';
            if (sizeof(wchar_t) == 2)
            {
                CodeSrc = (a == 0x0061) ? "UTF-16LE" : "UTF-16BE";
            }
            else
            {
                CodeSrc = (a == 0x0061) ? "UTF-32LE" : "UTF-32BE";
            }

            // set destinate code
            const char *CodeDest = 0;
            switch (mbsenc)
            {
            case ENC_UTF8:   CodeDest = "UTF-8";      break;
            case ENC_GB:     CodeDest = "GB18030";    break;
            case ENC_BIG5:   CodeDest = "BIG5-HKSCS"; break;
            case ENC_UTF16:  CodeDest = "UTF-16LE";   break;
            case ENC_UTF16BE:CodeDest = "UTF-16BE";   break;
            case ENC_ANSI:   CodeDest = "CP1250";     break;
            default:         return -1; // this should not happen.
            }

            char      *pInBuf = reinterpret_cast<char *>(const_cast<wchar_t *>(wcstr));
            size_t  sizeInBuf = wcscnt * sizeof(wchar_t);
            char  *pOutputBuf = mbstr;
            size_t sizeOutBuf = mbscnt;
            iconv_t iConvT = iconv_open(CodeDest, CodeSrc);
            iconv(iConvT, &pInBuf, &sizeInBuf, &pOutputBuf, &sizeOutBuf);
            iconv_close(iConvT);

            // do not move the '/4' inside, as they are all integer.
            // that will affect the result
            return mbscnt - sizeOutBuf;
#endif
        }

        size_t mbstowcs(const char *mbstr, size_t mbscnt, wchar_t *wcstr, size_t wcscnt, EEncoding mbsenc)
        {
            if (mbstr == NULL && mbscnt == 0)
            {
                // no input
            }
            if (wcstr == NULL)
            {
                // User is querying the required size of the destination string.
                // Set wcscnt as zero because WideCharToMultiByte use this parameter 
                // indicating that we are querying the size
                wcscnt = 0;
            }


#if defined(WIN32) || defined(WINCE)

            if (mbsenc == ENC_UTF16)
            {
                // input is already UTF16, no conversion is needed
                if (wcstr == NULL)
                {
                    return mbscnt / 2;
                }
                else
                {
                    size_t iConvLength = mbscnt / 2;
                    if (wcscnt < iConvLength)
                        iConvLength = wcscnt;
                    memcpy(wcstr, mbstr, iConvLength*2);
                    return iConvLength;
                }
            }

#if defined(WIN32)
            // special processing for HKSCS
            if (mbsenc == ENC_BIG5)
            {
                //size_t iConvLength = HKSCS_Big5ToUnicode41(HKSCS_ERR_INVALID_CHARS, mbstr, (int)mbscnt, wcstr, (int)wcscnt);
                //return iConvLength;

#if defined(HKSCS)
                // Load DLL dynamically
                HINSTANCE hkscs = LoadLibrary(L"hkscs04.dll");
                if (hkscs)
                {
                    typedef DWORD(WINAPI *HKSCS_Big5ToUtf16)(DWORD, LPCSTR, int, LPWSTR, int);
                    HKSCS_Big5ToUtf16 lpBig5ToUtf16 = (HKSCS_Big5ToUtf16)GetProcAddress(hkscs, "HKSCS_Big5ToUnicode41");
                    if (lpBig5ToUtf16)
                    {
                        size_t iConvLength = lpBig5ToUtf16(HKSCS_ERR_INVALID_CHARS, mbstr, (int)mbscnt, wcstr, (int)wcscnt);
                        FreeLibrary(hkscs);
                        return iConvLength;
                    }
                    else
                    {
                        FreeLibrary(hkscs);
                    }
                }
                // If goes here,
                // prompt: no HKSCS is supported
#endif
            }
#endif

            // conversion is needed
            unsigned int codePage;
            switch (mbsenc)
            {
            case ENC_UTF8:   codePage = CP_UTF8;    break; // UTF-8
            case ENC_GB:     codePage = 936;        break; // GB2312/GBK, GB18030 is not supported in windows
            case ENC_BIG5:   codePage = 950;        break; // BIG5
            case ENC_ANSI:   codePage = CP_ACP;     break; // ANSI
            case ENC_UTF16BE:codePage = 1201;       break; // Unicode UCS-2 Big-Endian
            default:         return 0;                     // this should not happen.
            }

            size_t iConvLength = MultiByteToWideChar(codePage, 0, mbstr, (int)mbscnt, wcstr, (int)wcscnt);
            return iConvLength;

#elif defined(__GNUC__)

            if (wcstr == NULL)
            {
                // @bug: this is terrible, but may work.
                // usually, wide-character string contains less character than its multi-byte counterpart
                return mbscnt;
            }

            // set destinate code
            // detect the size of the UNICODE wide-character
            // detect the endian? (big-endian or little-endian)
            const char *CodeDest = 0;
            wchar_t a = L'a';
            if (sizeof(wchar_t) == 2)
            {
                CodeDest = (a == 0x0061) ? "UTF-16LE" : "UTF-16BE";
            }
            else
            {
                CodeDest = (a == 0x0061) ? "UTF-32LE" : "UTF-32BE";
            }

            // set source code
            const char *CodeSrc = 0;
            switch (mbsenc)
            {
            case ENC_UTF8:   CodeSrc = "UTF-8";      break;
            case ENC_GB:     CodeSrc = "GB18030";    break;
            case ENC_BIG5:   CodeSrc = "BIG5-HKSCS"; break;
            case ENC_UTF16:  CodeSrc = "UTF-16LE";   break;
            case ENC_UTF16BE:CodeSrc = "UTF-16BE";   break;
            case ENC_ANSI:   CodeSrc = "CP1250";     break;
            default:         return -1; // this should not happen.
            }

            char      *pInBuf = const_cast<char *>(mbstr);
            size_t  sizeInBuf = mbscnt;
            char  *pOutputBuf = reinterpret_cast<char *>(wcstr);
            size_t sizeOutBuf = wcscnt * sizeof(wchar_t);
            iconv_t iConvT = iconv_open(CodeDest, CodeSrc);
            iconv(iConvT, &pInBuf, &sizeInBuf, &pOutputBuf, &sizeOutBuf);
            iconv_close(iConvT);

            // The Linux version of the function uses iconv directly, and 
            // iconv will add a leading Byte Order Mark (BOM) in the output string.
            // The function will not return the BOM, now removing the leading BOM.
            //
            // But it should be noted that, as this function uses wcstr directly
            // for buffering the output of iconv, there must be one more space in 
            // wcstr than the finally returned wide-character string.

            // do not move the '/sizeof(wchar_t)' inside, as they are all integer.
            // that will affect the result
            int iConvertLength = (wcscnt * sizeof(wchar_t) - sizeOutBuf) / sizeof(wchar_t);
            if (wcstr[0] == 0xFEFF || wcstr[0] == 0xFFFE)
            {
                // might be Big Endian or Little Endian
                // removing the leading BOM
                iConvertLength --;
                memmove(wcstr, wcstr + 1, iConvertLength * sizeof(wchar_t));
            }
            return iConvertLength;
#endif
        }

    }//namespace str
}
