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
/// @brief  Implementation file of the helper functions for SSML document operation.
///
/// @version    0.1.0
/// @date       2008/05/28
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Created
///


#include "ssml_document.h"
#include "ssml_helper.h"
#include <cstdarg>
#include <cmath>

namespace cst
{
    namespace xml
    {
        int CSSMLHelper::composeString(std::wstring &wstrResult, const wchar_t *format, ...)
        {
            static const int SBUF_SIZE = 260;
            static wchar_t sBuf[SBUF_SIZE];

#pragma warning( push )
#pragma warning( disable : 4996 )
            va_list arglist;
            va_start(arglist, format);
#if defined(WIN32) || defined(WINCE)
            int ret = _vsnwprintf(sBuf, SBUF_SIZE, format, arglist);
#else
            int ret = vswprintf(sBuf, SBUF_SIZE, format, arglist);
#endif
            va_end(arglist);
#pragma warning( pop )
            wstrResult = sBuf;
            return ret;
        }


        float CSSMLHelper::parseFloatNumber(const std::wstring  &wstrFloat)
        {
            float flNum;
            if (std::swscanf(wstrFloat.c_str(), L"%f", &flNum) < 1)
            {
                return 0;
            }
            return flNum;
        }


        float CSSMLHelper::parseTime(const std::wstring  &wstrTime)
        {
            static const int SBUF_SIZE = 64;
            static wchar_t s_buf[SBUF_SIZE];
            wchar_t *pBuf = s_buf;
            float flTime = 0;

            if (wstrTime.length() >= SBUF_SIZE)
            {
                pBuf = new wchar_t[wstrTime.length() + 1];
            }

            if (std::swscanf(wstrTime.c_str(), L"%f%S", &flTime, pBuf) < 2)
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


        float CSSMLHelper::parseFrequency(const std::wstring  &wstrFreq)
        {
            float flFreq;
            if (std::swscanf(wstrFreq.c_str(), L"%f", &flFreq) < 1)
            {
                return 0;
            }
            return flFreq;
        }

        std::vector<float> CSSMLHelper::parseFrequencyList(const std::wstring &wstrContour)
        {
            std::vector<float> retFreqs;
            size_t curPos = 0, lstPos = 0;
            while ((curPos=wstrContour.find(L" ", lstPos)) != std::wstring::npos)
            {
                float oneFreq = parseFrequency(wstrContour.substr(lstPos, curPos-lstPos));
                retFreqs.push_back(oneFreq);
                lstPos = curPos +1;
            }
            if (lstPos<wstrContour.length())
            {
                float oneFreq = parseFrequency(wstrContour.substr(lstPos, curPos-lstPos));
                retFreqs.push_back(oneFreq);
            }
            return retFreqs;
        }

        int CSSMLHelper::parseUnitID(const std::wstring &wstrUnitID, std::wstring &wstrPhoneme)
        {
            wstrPhoneme = wstrUnitID;

            size_t sep = wstrUnitID.find_last_of(L':');
            if (sep == std::string::npos)
                return -1;

            // assign phoneme
            wstrPhoneme.assign(wstrUnitID.c_str(), sep);

            // get unit id
            int id = 0;
            if (std::swscanf(wstrUnitID.c_str()+sep+1, L"%d", &id) < 1)
                return -1;

            return id;
        }

        float CSSMLHelper::parseVolume(const std::wstring& strVolume)
        {
            float ratio = 1;

            // numerical value (in dB)
            if (!strVolume.empty() && strVolume.find(L"dB") == strVolume.length()-2)
            {
                // convert from dB to ratio
                ratio = xml::CSSMLHelper::parseFloatNumber(strVolume.substr(0, strVolume.length()-2));
                ratio = pow(10, ratio/20);
            }
            // numerical value ("+number%", "-number%", "number%")
            else if (!strVolume.empty() && strVolume.find(L"%") == strVolume.length()-1)
            {
                // ratio can be 0
                if (strVolume.compare(0, 1, L"+") == 0 || strVolume.compare(0, 1, L"-") == 0)
                {
                    // relative change ("+number%", "-number%")
                    // convert from +%/-% to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strVolume.substr(0, strVolume.length()-1));
                    ratio = 1 + ratio/100;
                    if (ratio < 0)
                        ratio = -1.0f;  // invalid value
                }
                else
                {
                    // non-negative percentage ("number%")
                    // convert from % to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strVolume.substr(0, strVolume.length()-1));
                    if (ratio < 0)
                        ratio = -1.0f;  // invalid negative value
                    else
                        ratio = ratio/100;
                }
            }
            // non-numerical value
            else if (strVolume.empty() || strVolume == L"default")
                ratio = 1.0f;
            else if (strVolume == L"x-loud")
                ratio = 2.0f;
            else if (strVolume == L"loud")
                ratio = 1.6f;
            else if (strVolume == L"medium")
                ratio = 1.4f;
            else if (strVolume == L"soft")
                ratio = 0.7f;
            else if (strVolume == L"x-soft")
                ratio = 0.5f;
            else if (strVolume == L"silent")
                ratio = 0.0f;
            // invalid value
            else
                ratio = -1.0f;

            return ratio;
        }

        float CSSMLHelper::parseRate(const std::wstring& strRate)
        {
            float ratio = 1;

            // numerical value ("+number%", "-number%", "number%")
            if (!strRate.empty() && strRate.find(L"%") == strRate.length()-1)
            {
                if (strRate.compare(0, 1, L"+") == 0 || strRate.compare(0, 1, L"-") == 0)
                {
                    // relative change ("+number%", "-number%")
                    // convert from +%/-% to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strRate.substr(0, strRate.length()-1));
                    ratio = 1 + ratio/100;
                    if (ratio <= 0)
                        ratio = -1.0f;  // invalid negative value
                    else
                        ratio = 1/ratio;
                }
                else
                {
                    // non-negative percentage ("number%")
                    // convert from % to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strRate.substr(0, strRate.length()-1));
                    if (ratio <= 0)
                        ratio = -1.0f;  // invalid negative value
                    else
                        ratio = 100/ratio;
                }
            }
            // non-numerical value
            else if (strRate.empty() || strRate == L"default")
                ratio = 1.0f;
            else if (strRate == L"x-fast")
                ratio = 0.625f; // 160%
            else if (strRate == L"fast")
                ratio = 0.714f; // 140%
            else if (strRate == L"medium")
                ratio = 0.833f; // 120%
            else if (strRate == L"slow")
                ratio = 1.250f; // 80%
            else if (strRate == L"x-slow")
                ratio = 1.667f; // 60%
            // invalid value
            else
                ratio = -1.0f;

            return ratio;
        }

        float CSSMLHelper::parsePitch(const std::wstring& strPitch)
        {
            float ratio = 1;

            // numerical value ("+number%", "-number%", "number%")
            if (!strPitch.empty() && strPitch.find(L"%") == strPitch.length()-1)
            {
                if (strPitch.compare(0, 1, L"+") == 0 || strPitch.compare(0, 1, L"-") == 0)
                {
                    // relative change ("+number%", "-number%")
                    // convert from +%/-% to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strPitch.substr(0, strPitch.length()-1));
                    ratio = 1 + ratio/100;
                    if (ratio <= 0)
                        ratio = -1.0f;  // invalid value
                }
                else
                {
                    // non-negative percentage ("number%")
                    // convert from % to ratio
                    ratio = xml::CSSMLHelper::parseFloatNumber(strPitch.substr(0, strPitch.length()-1));
                    if (ratio <= 0)
                        ratio = -1.0f;  // invalid negative value
                    else
                        ratio = ratio/100;
                }
            }
            // non-numerical value
            else if (strPitch.empty() || strPitch == L"default")
                ratio = 1.0f;
            else if (strPitch == L"x-high")
                ratio = 1.3f;
            else if (strPitch == L"high")
                ratio = 1.2f;
            else if (strPitch == L"medium")
                ratio = 1.1f;
            else if (strPitch == L"low")
                ratio = 0.9f;
            else if (strPitch == L"x-low")
                ratio = 0.8f;
            // invalid value
            else
                ratio = -1.0f;

            return ratio;
        }



        int CSSMLHelper::getBoundaryType(const CXMLElement *pSSMLBreak)
        {
            if (pSSMLBreak == NULL || pSSMLBreak->value() != L"break")
            {
                return CSSMLDocument::PROSBOUND_NONE;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"none")
            {
                return CSSMLDocument::PROSBOUND_NONE;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"x-weak")
            {
                return CSSMLDocument::PROSBOUND_X_WEAK;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"weak")
            {
                return CSSMLDocument::PROSBOUND_WEAK;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"strong")
            {
                return CSSMLDocument::PROSBOUND_STRONG;
            }
            else if(pSSMLBreak->getAttribute(L"strength") == L"x-strong")
            {
                return CSSMLDocument::PROSBOUND_X_STRONG;
            }
            else
            {
                // default is medium
                return CSSMLDocument::PROSBOUND_MEDIUM;
            }
        }

        void CSSMLHelper::setBoundaryType(CXMLElement *pSSMLBreak, int iBoundaryType)
        {
            const wchar_t *wstrBoundaryType = NULL;

            switch (iBoundaryType)
            {
            case CSSMLDocument::PROSBOUND_NONE:     wstrBoundaryType = L"none";     break;
            case CSSMLDocument::PROSBOUND_X_WEAK:   wstrBoundaryType = L"x-weak";   break;
            case CSSMLDocument::PROSBOUND_WEAK:     wstrBoundaryType = L"weak";     break;
            case CSSMLDocument::PROSBOUND_MEDIUM:   wstrBoundaryType = L"medium";   break;
            case CSSMLDocument::PROSBOUND_STRONG:   wstrBoundaryType = L"strong";   break;
            case CSSMLDocument::PROSBOUND_X_STRONG: wstrBoundaryType = L"x-strong"; break;
            }

            pSSMLBreak->setAttribute(L"strength", wstrBoundaryType);
        }

    }//xml
}
/*
CXMLNode *CSSMLHelper::getNextNodeDepthFirst(CXMLNode *pRoot, CXMLNode *pCurr, bool bGoPastRoot)
{
    CXMLNode * pNode;
    if (pCurr == 0)
    {
        pNode = pRoot;
    }
    else
    {
        pNode = pCurr;
    }

    if (pNode->firstChild() != NULL)
    {
        pNode = pNode->firstChild();
    }
    else
    {
        while (pNode != NULL && pNode->nextSibling() == NULL)
        {
            if (pNode == pRoot && bGoPastRoot == false)
            {
                return NULL;
            }
            pNode = pNode->parent();
        }
        if (pNode != NULL)
        {
            pNode = pNode->nextSibling();
        }
    }
    return pNode;
}

CXMLElement *CSSMLHelper::findElementDepthFirst(CXMLNode *pRoot, std::vector<std::wstring> wstrName, CXMLNode *pCurrent, bool bGoPastRoot)
{
    CXMLNode *pNode = pCurrent;
    for (;;)
    {
        pNode = getNextNodeDepthFirst(pRoot, pNode, bGoPastRoot);
        if (pNode == NULL)
        {
            break;
        }
        if (wstrName.size() == 0)
        {
            if (pNode->type() == CXMLNode::XML_ELEMENT)
            {
                break;
            }
        }
        else
        {
            size_t i;
            for (i = 0; i < wstrName.size(); ++i)
            {
                if (isElement(pNode, wstrName[i].c_str()))
                {
                    break;
                }
            }
            if (i < wstrName.size())
            {
                break;
            }
        }
    }
    return dynamic_cast<CXMLElement *>(pNode);
}

CXMLElement *CSSMLHelper::findElementDepthFirst(CXMLNode *pRoot, const wchar_t *wstrName, CXMLNode *pCurrent, bool bGoPastRoot)
{
    return findElementDepthFirst(pRoot, std::vector<std::wstring>(1, wstrName), pCurrent, bGoPastRoot);
}

//CXMLElement *CSSMLHelper::findElementInSiblings(const CXMLNode *pCurrent, const wchar_t *wstrName)
//{
//    CXMLNode *pNode = pCurrent->nextSibling();
//    while ( pNode && pNode->type() == CXMLNode::XML_ELEMENT && pNode->value() != wstrName )
//    {
//        pNode = pNode->nextSibling();
//    }
//
//    if ( isElement(pNode, wstrName) )
//    {
//        return (CXMLElement *)pNode;
//    }
//
//    return NULL;
//}

CXMLElement *CSSMLHelper::findElementInChildren(CXMLNode *pParent, const wchar_t *wstrName, bool bRecursive)
{
    return findElementDepthFirst(pParent, wstrName);
}

//CXMLElement *CSSMLHelper::findElementInLevel1Children(CXMLNode *pParent, const wchar_t *wstrName)
//{
//    CXMLNode *pNode = pParent->firstChild();
//    while ( pNode && p)
//}

bool CSSMLHelper::isElement(const CXMLNode *pNode, const wchar_t *wstrName)
{
     return pNode&&pNode->type()==CXMLNode::XML_ELEMENT&&pNode->value()==wstrName;
}

bool CSSMLHelper::isBreak(const CXMLNode *pNode)
{
    return isElement(pNode, L"break");
}

bool CSSMLHelper::isProsody(const CXMLNode *pNode)
{
    return isElement(pNode, L"prosody");
}

bool CSSMLHelper::isWord(const CXMLNode *pNode)
{
    return isElement(pNode, L"w");
}

bool CSSMLHelper::isSyllable(const CXMLNode *pNode)
{
    return isElement(pNode, L"syllable");
}

bool CSSMLHelper::isPhoneme(const CXMLNode *pNode)
{
    return isElement(pNode, L"phoneme");
}

bool CSSMLHelper::getSelectionResult(CXMLElement *pSSMLSyl, IndexCode &iCode, int &iIdx)
{
    if ( pSSMLSyl && pSSMLSyl->value() == L"syllable" )
    {
        const std::wstring &wstrSylID = pSSMLSyl->attribute(L"id");
        int tempicode;
        if ( std::swscanf(wstrSylID.c_str(), L"%d:%d", &tempicode, &iIdx) < 2 )
        {
            return false;
        }
        iCode = (IndexCode)tempicode;
        if ( iCode == (IndexCode)-1 )
        {
            return false;
        }
        return true;
    }
    return false;
}

void CSSMLHelper::setSelectionResult(CXMLElement *pSSMLSyl, IndexCode iCode, int iIdx)
{
    static const int BUFFER_SIZE = 64;
    wchar_t buffer[BUFFER_SIZE];
    std::swprintf(
        buffer,
        BUFFER_SIZE,
        L"%d:%d",
        iCode,
        iIdx
        );
    pSSMLSyl->setAttribute(L"id", buffer);
}

float CSSMLHelper::translateTime(const std::wstring &wstrTime)
{
    static const unsigned int SBUF_SIZE = 64;
    wchar_t s_buf[SBUF_SIZE];
    wchar_t *pBuf = s_buf;
    float fDuration = 0;

    if ( wstrTime.length() >= SBUF_SIZE )
    {
        pBuf = new wchar_t[wstrTime.length() + 1];
    }

    if ( std::swscanf(wstrTime.c_str(), L"%f%S", &fDuration, pBuf) < 2 )
    {
        if ( pBuf != s_buf )
        {
            delete pBuf;
        }
        return 0;
    }

    if ( std::wcscmp(pBuf, L"s") == 0 )
    {
        fDuration *= 1000;
    }

    if ( pBuf != s_buf )
    {
        delete pBuf;
    }
    return fDuration;
}

std::wstring CSSMLHelper::constructTime(float fDuration)
{
    static const int SBUF_SIZE = 64;
    wchar_t s_buf[SBUF_SIZE];
    std::swprintf(
        s_buf,
        SBUF_SIZE,
        L"%.1fms",
        fDuration
        );

    return s_buf;
}

void CSSMLHelper::setBreakTime(CXMLElement *pSSMLBreak, float fDuration)
{
    if ( fDuration > 0 )
    {
        pSSMLBreak->setAttribute(L"time", constructTime(fDuration));
    }
}

bool CSSMLHelper::getBreakTime(CXMLElement *pSSMLBreak, float &fDuration)
{
    fDuration = translateTime(pSSMLBreak->attribute(L"time"));
    if ( fDuration == 0 )
    {
        return false;
    }
    return true;
}

bool CSSMLHelper::getBoundaryType(CXMLElement *pSSMLBreak, int &iBoundaryType)
{
    if ( pSSMLBreak == 0 )
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_NONE;
        return false;
    }
    else if( pSSMLBreak->attribute(L"strength") == L"x-weak" )
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_PWORD;
    }
    else if( pSSMLBreak->attribute(L"strength") == L"weak" )
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_PPHRASE;
    }
    else if( pSSMLBreak->attribute(L"strength") == L"medium" )
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_MEDIUM;
    }
    else if( pSSMLBreak->attribute(L"strength") == L"strong" )
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_STRONG;
    }
    else
    {
        iBoundaryType = CSSMLDocument::PROSBOUND_NONE;
        return false;
    }
    return true;
}

void CSSMLHelper::setBoundaryType(CXMLElement *pSSMLBreak, int iBoundaryType)
{
    const wchar_t *pwstrBoundaryType = 0;
    switch (iBoundaryType)
    {
    case CSSMLDocument::PROSBOUND_PWORD:
        pwstrBoundaryType = L"x-weak";
        break;
    case CSSMLDocument::PROSBOUND_PPHRASE:
        pwstrBoundaryType = L"weak";
        break;
    case CSSMLDocument::PROSBOUND_MEDIUM:
        pwstrBoundaryType = L"medium";
        break;
    case CSSMLDocument::PROSBOUND_STRONG:
        pwstrBoundaryType = L"strong";
        break;
    default:
        return;
    }

    pSSMLBreak->setAttribute(L"strength", pwstrBoundaryType);
}

float CSSMLHelper::translateFloat(const std::wstring &wstrFloat)
{
    float fValue = 0;
    if ( std::swscanf(wstrFloat.c_str(), L"%f", &fValue) < 1 )
    {
        return 0;
    }

    return fValue;
}

std::wstring CSSMLHelper::constructFloat(float fValue)
{
    static const int SBUF_SIZE = 64;
    wchar_t s_buf[SBUF_SIZE];
    std::swprintf(
        s_buf,
        SBUF_SIZE,
        L"%.1f",
        fValue
        );

    return s_buf;
}

bool CSSMLHelper::getProsodyValues(CXMLElement *pSSMLProsody, float &fPitch, float &fDuration, float &fAmplitude)
{
    int i = 0;

    fPitch = translateFloat(pSSMLProsody->attribute(L"pitch"));
    if ( FLOAT_EQUAL(fPitch, 0) )
    {
        i++;
    }

    fDuration = translateTime(pSSMLProsody->attribute(L"duration"));
    if ( FLOAT_EQUAL(fDuration, 0) )
    {
        i++;
    }

    fAmplitude = translateFloat(pSSMLProsody->attribute(L"amplitude"));
    if ( FLOAT_EQUAL(fAmplitude, 0) )
    {
        i++;
    }

    if ( i == 3 )
    {
        return false;
    }

    return true;
}

bool CSSMLHelper::getProsodyValues(CXMLElement *pSSMLProsody, std::wstring &strPitch, float &fDuration, float &fAmplitude)
{
    int i = 0;

    strPitch = pSSMLProsody->attribute(L"pitch");

    fDuration = translateTime(pSSMLProsody->attribute(L"duration"));
    if ( FLOAT_EQUAL(fDuration, 0) )
    {
        i++;
    }

    fAmplitude = translateFloat(pSSMLProsody->attribute(L"amplitude"));
    if ( FLOAT_EQUAL(fAmplitude, 0) )
    {
        i++;
    }

    if ( i == 2 )
    {
        return false;
    }

    return true;
}

void CSSMLHelper::setProsodyValues(CXMLElement *pSSMLProsody, const std::wstring &strPitch, float fDuration, float fAmplitude)
{
    if ( strPitch.length() != 0 )
    {
        pSSMLProsody->setAttribute(L"pitch", strPitch);
    }
    
    if ( FLOAT_GREATERTHAN(fDuration, 0) )
    {
        pSSMLProsody->setAttribute(L"duration", constructTime(fDuration));
    }

    if ( FLOAT_GREATERTHAN(fAmplitude, 0) )
    {
        pSSMLProsody->setAttribute(L"amplitude", constructFloat(fAmplitude));
    }
}

void CSSMLHelper::setProsodyValues(CXMLElement *pSSMLProsody, float fPitch, float fDuration, float fAmplitude)
{
    if ( FLOAT_GREATERTHAN(fPitch, 0) )
    {
        pSSMLProsody->setAttribute(L"pitch", constructFloat(fPitch));
    }

    if ( FLOAT_GREATERTHAN(fDuration, 0) )
    {
        pSSMLProsody->setAttribute(L"duration", constructTime(fDuration));
    }

    if ( FLOAT_GREATERTHAN(fAmplitude, 0) )
    {
        pSSMLProsody->setAttribute(L"amplitude", constructFloat(fAmplitude));
    }
}

std::wstring CSSMLHelper::constructSyllablePitch(const std::map<std::string, std::string> &params)
{
    if ( params.size() == 0 )
    {
        return L"";
    }

    wchar_t buf[1024];
    std::wstring temp;
    std::map<std::string, std::string>::const_iterator iter = params.begin();
    while ( iter != params.end() )
    {
        swprintf(buf, 1024, L"%hs:%hs,", iter->first.c_str(), iter->second.c_str());
        temp.append(buf);
        iter++;
    }
    return temp.substr(0, temp.length()-1);
}

bool CSSMLHelper::translateSyllablePitch(const std::wstring &strPitch, std::map<std::string, std::string> &params)
{
    params.clear();
    char tempbuf[1024];
    char *pbuf = tempbuf;
    if ( strPitch.length() >= 512 )
    {
        pbuf = new char[strPitch.length()*2+1];
    }
    sprintf(pbuf, "%ls", strPitch.c_str());

    char *ptr = strtok(pbuf, ", \t");
    while (ptr)
    {
        std::string temp = ptr;
        int nidx = temp.find(':'); 
        if ( nidx != (int)std::string::npos && nidx != 0 && nidx != (int)temp.length()-1 )
        {
            params.insert(std::make_pair(temp.substr(0, nidx), temp.substr(nidx+1, temp.length()-nidx)));
        }
        ptr = strtok(NULL, ",");
    }
    if ( params.size() != 0 )
    {
        return true;
    }
    return false;
}

//std::wstring CSSMLHelper::constructSyllablePitch(float pratio_mean, float pratio_stddev)
//{
//    static const int SBUF_SIZE = 256;
//    wchar_t s_buf[SBUF_SIZE];
//    std::swprintf(
//        s_buf,
//        SBUF_SIZE,
//        L"%.1f:%.1f",
//        pratio_mean, 
//        pratio_stddev
//        );
//
//    return s_buf;
//}
//
//bool CSSMLHelper::translateSyllablePitch(const std::wstring &strPitch, float &pratio_mean, float &pratio_stddev)
//{
//    if ( std::swscanf(strPitch.c_str(), L"%f:%f", &pratio_mean, &pratio_stddev) < 2 )
//    {
//        return false;
//    }
//
//    return true;
//}

bool CSSMLHelper::getContinuousFlag(CXMLElement *pSSMLProsody)
{
    std::wstring val = pSSMLProsody->attribute(L"continuous");
    if ( val == L"yes" )
    {
        return true;
    }
    return false;
}

void CSSMLHelper::setContinuousFlag(CXMLElement *pSSMLProsody, bool bContinous)
{
    if ( bContinous )
    {
        pSSMLProsody->setAttribute(L"continuous", L"yes");
        return;
    }

    pSSMLProsody->removeAttribute(L"continuous");
}
*/
