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
/// @brief  Implementation for operation on SSML document.
///
/// @version    0.3.0
/// @date       2007/05/21
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Dezhi HUANG
///   Date:     2006/7/24
///   Changed:  Create
/// - Version:  0.2.0
///   Author:   Yongxin WANG (fefe.wyx@gmail.com)
///   Date:     2006/07/27
///   Change:   Changed to std::wstring
/// - Version:  0.3.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/21
///   Changed:  Changed to cmn::wstring
///


#include "ssml_document.h"

namespace cst
{
    namespace xml
    {
        bool CSSMLDocument::parse(const wchar_t *pDataSource)
        {
            return CXMLDocument::parse(pDataSource, false);
        }

        CXMLNode *CSSMLDocument::findNextWord(const CXMLNode *pWord)
        {
            CXMLNode *pNextWord = (pWord==NULL) ? NULL : pWord->nextSibling();
            while (pNextWord != NULL)
            {
                if (pNextWord->type() == XML_ELEMENT && pNextWord->value() == L"w")
                {
                    return pNextWord;
                }
                pNextWord = pNextWord->nextSibling();
            }
            return NULL;
        }

        CXMLNode *CSSMLDocument::findPrevWord(const CXMLNode *pWord)
        {
            CXMLNode *pPrevWord = (pWord==NULL) ? NULL : pWord->prevSibling();
            while (pPrevWord != NULL)
            {
                if (pPrevWord->type() == XML_ELEMENT && pPrevWord->value() == L"w")
                {
                    return pPrevWord;
                }
                pPrevWord = pPrevWord->prevSibling();
            }
            return NULL;
        }

        CXMLElement *CSSMLDocument::findFirstWord(const CXMLNode *pSentence)
        {
            CXMLNode *pFirstWord = (pSentence==NULL) ? NULL : pSentence->firstChild();
            while (pFirstWord != NULL)
            {
                if (pFirstWord->type() == XML_ELEMENT && pFirstWord->value() == L"w")
                {
                    return cst_dynamic_cast<CXMLElement *>(pFirstWord);
                }
                pFirstWord = pFirstWord->nextSibling();
            }
            return NULL;
        }

        const cmn::wstring &CSSMLDocument::getWordPOS(const CXMLNode *pWord)
        {
            static const cmn::wstring wstrDefaultPOS(L"w");
            if (pWord == NULL || pWord->type() != XML_ELEMENT || pWord->value() != L"w")
            {
                return wstrDefaultPOS;
            }

            const CXMLElement  *pElementWord = cst_dynamic_cast<const CXMLElement *>(pWord);
            const cmn::wstring &wstrPOS = pElementWord->getAttribute(L"pos");
            if (wstrPOS.length() == 0)
            {
                return wstrDefaultPOS;
            }
            else
            {
                return wstrPOS;
            }
        }

        const cmn::wstring &CSSMLDocument::getWordContent(const CXMLElement *pSSMLWord)
        {
            CXMLNode *pNode;
            static const cmn::wstring &wstrEmptyContent = L"";

            while (pSSMLWord)
            {
                for (pNode = pSSMLWord->firstChild();
                    pNode != 0;
                    pNode = pNode->nextSibling())
                {
                    if (pNode->type() == XML_TEXT)
                    {
                        return pNode->value();
                    }
                    else if(pNode->type() == XML_ELEMENT && pNode->value() == L"say-as")
                    {
                        return cst_dynamic_cast<CXMLElement *>(pNode)->getAttribute(L"read-as");
                    }
                    else if(pNode->type() == XML_ELEMENT && pNode->value() == L"phoneme")
                    {
                        pSSMLWord = cst_dynamic_cast<CXMLElement *>(pNode);
                        break;
                    }
                }
                if (pNode == 0)
                {
                    break;
                }
            }
            return wstrEmptyContent;
        }

        CXMLElement *CSSMLDocument::findProsodyBoundaryElemenet(CXMLElement *pSSMLWord)
        {
            if (pSSMLWord == 0)
            {
                return 0;
            }

            if (pSSMLWord->value() == L"break")
            {
                return pSSMLWord;
            }

            CXMLNode *pSSMLW = pSSMLWord->nextSibling();

            while (pSSMLW != 0)
            {
                if (pSSMLW->type() == CXMLNode::XML_ELEMENT)
                {
                    if (pSSMLW->value() == L"break")
                    {
                        return cst_dynamic_cast<CXMLElement *>(pSSMLW);
                    }
                    break;
                }
                pSSMLW = pSSMLW->nextSibling();
            }

            return 0;
        }

        int CSSMLDocument::getBoundaryType(CXMLElement *pSSMLBreak)
        {
            if (pSSMLBreak == NULL || pSSMLBreak->value() != L"break")
            {
                return PROSBOUND_NONE;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"none")
            {
                return PROSBOUND_NONE;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"x-weak")
            {
                return PROSBOUND_X_WEAK;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"weak")
            {
                return PROSBOUND_WEAK;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"strong")
            {
                return PROSBOUND_STRONG;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"x-strong")
            {
                return PROSBOUND_X_STRONG;
            }
            else
            {
                // default is medium
                return PROSBOUND_MEDIUM;
            }
        }

        int CSSMLDocument::getProsodyBoundary(CXMLElement *pSSMLWord)
        {
            CXMLElement *pSSMLBreak = findProsodyBoundaryElemenet(pSSMLWord);
            if (pSSMLBreak == 0)
            {
                return PROSBOUND_NONE;
            }
            return getBoundaryType(pSSMLBreak);
        }

        void CSSMLDocument::clearProsodyBoundary(CXMLElement *pSSMLWord)
        {
            CXMLElement *pSSMLBreak = findProsodyBoundaryElemenet(pSSMLWord);
            if (pSSMLBreak != 0)
            {
                pSSMLBreak->unlink();
                delete pSSMLBreak;
            }
        }

        void CSSMLDocument::setProsodyBoundary(CXMLElement *pSSMLWord, int iBoundaryType)
        {
            if (iBoundaryType == PROSBOUND_NONE)
            {
                clearProsodyBoundary(pSSMLWord);
                return;
            }

            if (iBoundaryType < 0 || iBoundaryType >= PROSBOUND_LAST_ITEM)
            {
                return;
            }

            CXMLElement *pSSMLBreak = findProsodyBoundaryElemenet(pSSMLWord);
            if (pSSMLBreak == 0)
            {
                pSSMLBreak = new CXMLElement(L"break");
                pSSMLWord->insertAfter(pSSMLBreak);
                //pSSMLBreak->insertBefore(pSSMLWord);
            }
            const wchar_t *pwstrBoundaryType = 0;

            switch (iBoundaryType)
            {
            case PROSBOUND_NONE:
                pwstrBoundaryType = L"none";
                break;
            case PROSBOUND_X_WEAK:
                pwstrBoundaryType = L"x-weak";
                break;
            case PROSBOUND_WEAK:
                pwstrBoundaryType = L"weak";
                break;
            case PROSBOUND_MEDIUM:
                pwstrBoundaryType = L"medium";
                break;
            case PROSBOUND_STRONG:
                pwstrBoundaryType = L"strong";
                break;
            case PROSBOUND_X_STRONG:
                pwstrBoundaryType = L"x-strong";
                break;
            }

            pSSMLBreak->setAttribute(L"strength", pwstrBoundaryType);
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  The following functions are used for the assistance
        //  to access the data in SSML recommendation
        //
        //////////////////////////////////////////////////////////////////////////

        float CSSMLDocument::parseTime(const cmn::wstring &wstrTime)
        {
            static const int SBUF_SIZE = 64;
            wchar_t s_buf[SBUF_SIZE];
            wchar_t *pBuf = s_buf;
            float flTime = 0;

            if (wstrTime.length() >= SBUF_SIZE)
            {
                pBuf = new wchar_t[wstrTime.length() + 1];
            }

            if (std::swscanf(wstrTime.c_str(), L"%f%s", &flTime, pBuf) < 2)
            {
                if (pBuf != s_buf)
                {
                    delete pBuf;
                }
                return 0;
            }

            if (std::wcscmp(pBuf, L"s") == 0)
            {
                flTime *= 1000;
            }

            if (pBuf != s_buf)
            {
                delete pBuf;
            }
            return flTime;
        }


    } // End of namespace xml
} // End of namespace cst
