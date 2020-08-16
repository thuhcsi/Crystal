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
/// @brief  Implementation file for Pinyin utility routines
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2018/12/20
///   Changed:  Created
///


#include "cmn/cmn_string.h"
#include "utils/utl_regexp.h"
#include "utl_pinyin.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            bool CPinyin::split(const wchar_t *pinyin, wchar_t *initial, wchar_t *final, wchar_t *retroflex, int &tone)
            {
                if (pinyin == 0 || pinyin[0] == 0)
                    return false;

                // try English letters first
                if (pinyin[0] == L'_')
                    return false;

                // internal buffer
                wchar_t pin[257], *p = pin;
                size_t n = wcslen(pinyin);
                if (n > 256)
                    return false;
                wcscpy(pin, pinyin);

                // extract tone
                // if no tone found, use neutral (5)
                tone = 0;
                if (p[n-1] >= L'0' && p[n-1] <= L'9')
                {
                    tone = p[n-1] - L'0';
                    p[--n] = 0;
                }
                if (tone == 0) tone = 5;
                if (n == 0)
                    return false;

                // get Erhua (retroflex): Pinyin is not "er" and ending with "r"
                retroflex[0] = 0;
                if (p[n-1] == L'r' && wcscmp(p, L"er") != 0)
                {
                    p[--n] = 0;
                    wcscpy(retroflex, L"rr");
                }
                if (n == 0)
                    return false;

                // get initial and final
                initial[0] = 0;
                if (p[0] == L'y')
                {
                    // ya->ia, yan->ian, yang->iang, yao->iao, ye->ie, yo->io, yong->iong, you->iou
                    // yi->i, yin->in, ying->ing
                    // yu->v, yuan->van, yue->ve, yun->vn
                    p[0] = L'i';
                    if (p[1] == L'u') p[1] = L'v';
                    if ((p[1] == L'i' || p[1] == L'v')) p++;
                    wcscpy(final, p);
                }
                else if (p[0] == L'w')
                {
                    // wa->ua, wo->uo, wai->uai, wei->uei, wan->uan, wen->uen, wang->uang, weng->ueng
                    // wu->u
                    // change 'w' to 'u', except 'wu->u'
                    p[0] = L'u';
                    if (p[1] == L'u') p++;
                    wcscpy(final, p);
                }
                else if (wcscmp(p, L"ng") == 0 || wcscmp(p, L"n") == 0 || wcscmp(p, L"m") == 0)
                {
                    // ng->ng, n->n, m->m
                    wcscpy(final, p);
                }
                else
                {
                    // with initial and final
                    // initial should be: b p m f d t n l g k h j q x z c s r zh ch sh
                    static std::wstring strInitial = L"b|p|m|f|d|t|n|l|g|k|h|j|q|x|zh?|ch?|sh?|r";
                    static std::wstring strPinyin = L"^("+strInitial+L")(.+)$";
                    static cmn::CRegexp regPinyin = cmn::CRegexp(strPinyin.c_str());
                    if (regPinyin.compiled() && regPinyin.match(p))
                    {
                        wcsncpy(initial, p+regPinyin.substart(1), regPinyin.sublength(1));
                        initial[regPinyin.sublength(1)] = 0;
                    }
                    wcscpy(final, p+wcslen(initial));

                    // special handling of final
                    if (wcscmp(final, L"i") == 0)
                    {
                        if (wcscmp(initial, L"z") == 0 || wcscmp(initial, L"c") == 0 || wcscmp(initial, L"s") == 0)
                        {
                            // the final of "zi, ci, si" should be "ix"
                            wcscpy(final, L"ix");
                        }
                        else if (wcscmp(initial, L"zh") == 0 || wcscmp(initial, L"ch") == 0 || wcscmp(initial, L"sh") == 0 || wcscmp(initial, L"r")  == 0)
                        {
                            // the final of "zhi, chi, shi, ri" should be "iy"
                            wcscpy(final, L"iy");
                        }
                    }
                    else if (final[0] == L'u' && (wcscmp(initial, L"j") == 0 || wcscmp(initial, L"q") == 0 || wcscmp(initial, L"x") == 0))
                    {
                        // ju->jv, jue->jve, juan->jvan, jun->jvn,
                        // qu->qv, que->qve, quan->qvan, qun->qun,
                        // xu->xv, xue->xve, xuan->xvan, xun->xun
                        // change all leading 'u' to 'v'
                        final[0] = L'v';
                    }
                    else if (wcscmp(final, L"ui") == 0)
                    {
                        // ui->uei
                        wcscpy(final, L"uei");
                    }
                    else if (wcscmp(final, L"iu") == 0)
                    {
                        // iu->iou
                        wcscpy(final, L"iou");
                    }
                    else if (wcscmp(final, L"un") == 0)
                    {
                        // un->uen
                        wcscpy(final, L"uen");
                    }
                }

                // special process for final "E, ng, m, n, ev"
                // full pinyin might be "hng", "hm", "ng", "m", "n"
                // as there are to few samples, treat final "E" as "ev", "ng" as "n", "ev" as "ei"
                if (wcscmp(final, L"E" ) == 0) wcscpy(final, L"ev");
                if (wcscmp(final, L"ng") == 0) wcscpy(final, L"n");
                if (wcscmp(final, L"ev") == 0) wcscpy(final, L"ei");

                return true;
            }

        }//namespace Chinese
    }
}
