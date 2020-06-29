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
/// @brief
///
/// @version    0.1.0
/// @date       2007/08/03
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/08/03
///   Changed:  Created
///

#include "cmn/cmn_error.h"
#include "cmn/cmn_textfile.h"

#include "utl_chineseconv.h"

namespace cst
{
    namespace cmn
    {
        void CChineseConvert::terminate()
        {
            m_S2T_STable = L"";
            m_S2T_TTable = L"";
            m_S2T_TTable = L"";
            m_S2T_STable = L"";
            m_bInitialized = false;
        }

        bool CChineseConvert::initialize(const wchar_t *wstrFileName)
        {
            if ( m_bInitialized )
            {
                terminate();
            }

            // load text
            std::wstring buf;
            CTextFile file;
            if ( !file.open(wstrFileName, L"rb") || !file.readString(buf) )
            {
                return false;
            }

            // get each conversion table
            std::wstring deliminators = L"\r\n";
            std::vector<std::wstring> tokens;
            str::tokenize(buf, deliminators, tokens);

            if (   tokens.size() != 4
                || tokens[0].length() != tokens[1].length()
                || tokens[2].length() != tokens[3].length() )
            {
                return false;
            }

            m_S2T_STable = tokens[0];
            m_S2T_TTable = tokens[1];
            m_T2S_TTable = tokens[2];
            m_T2S_STable = tokens[3];

            m_bInitialized = true;
            return true;
        }

        int CChineseConvert::findInTable(const wstring &table, wchar_t chr) const
        {
            // the conversion table is sorted ascending
            const wchar_t* ptable = table.data();
            int idxFst = 0;
            int idxLst = (int)table.length()-1;
            int idxTmp = -1;

            if (chr < ptable[idxFst] || chr > ptable[idxLst])
                return -1;
            else if (chr == ptable[idxFst])
                return idxFst;
            else if (chr == ptable[idxLst])
                return idxLst;

            while (idxFst < idxLst)
            {
                idxTmp = (idxFst + idxLst) / 2;
                if (idxTmp == idxFst)
                    return -1;
                if (chr == ptable[idxTmp])
                    return idxTmp;
                else if (chr < ptable[idxTmp])
                    idxLst = idxTmp;
                else
                    idxFst = idxTmp;
            }
            return -1;
        }

        void CChineseConvert::toTraditional(wstring &str) const
        {
            if (!m_bInitialized)
                return;

            int iTable;
            for (size_t i=0; i<str.length(); i++)
            {
                iTable = findInTable(m_S2T_STable, str[i]);
                if (iTable != -1)
                {
                    str[i] = m_S2T_TTable[iTable];
                }
            }
        }

        void CChineseConvert::toSimplified(wstring &str) const
        {
            if ( !m_bInitialized )
                return;

            int iTable;
            for (size_t i=0; i<str.length(); i++)
            {
                iTable = findInTable(m_T2S_TTable, str[i]);
                if (iTable != -1)
                {
                    str[i] = m_T2S_STable[iTable];
                }
            }
        }

    } // End of namespace cmn
} // End of namespace cst
