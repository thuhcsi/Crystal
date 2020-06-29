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
/// @brief  Implementation file for language conversion interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Re-implemented with new interface
///


#include "cmn/cmn_error.h"
#include "lang_langconvert.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            CLangConvert::CLangConvert(const wchar_t *strConvTableFile)
            {
                // initialize the Chinese converter
                m_convertChinese.initialize(strConvTableFile);
            }

            int CLangConvert::process(xml::CSSMLDocument *pSSMLDocument)
            {
                // perform language conversion
                return CSSMLTraversal::traverse(pSSMLDocument);
            }

            int CLangConvert::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // "text"

                    // perform language conversion
                    std::wstring wstrText = pNode->value();

                    int ret = doConversion(m_wstrSourceLang, m_wstrTargetLang, wstrText);
                    if (ret != ERROR_SUCCESS)
                        return ret;

                    pNode->setValue(wstrText);
                }
                else if (pNode->type() == xml::CXMLNode::XML_ELEMENT && pNode->value() == L"speak")
                {
                    // "speak"

                    const std::wstring &wstrSpeakLang = ((xml::CXMLElement*)pNode)->getAttribute(L"xml:lang");

                    // detect source written language
                    int ret = detectSourceLanguage(wstrSpeakLang, m_wstrSourceLang);
                    if (ret != ERROR_SUCCESS)
                        return ret;

                    // detect target written language
                    ret = detectTargetLanguage(wstrSpeakLang, m_wstrTargetLang);
                    if (ret != ERROR_SUCCESS)
                        return ret;
                }

                return ERROR_SUCCESS;
            }

            int CLangConvert::detectSourceLanguage(const std::wstring &speakLang, std::wstring &srcLang)
            {
                return ERROR_SUCCESS;
            }

            int CLangConvert::detectTargetLanguage(const std::wstring &speakLang, std::wstring &tgtLang)
            {
                if (speakLang == L"zh-cmn")
                {
                    tgtLang = L"zh-Hans";
                    return ERROR_SUCCESS;
                }
                else if (speakLang == L"zh-yue")
                {
                    tgtLang = L"zh-Hant";
                    return ERROR_SUCCESS;
                }
                else
                {
                    return ERROR_UNSUPPORTED_LANGUAGE;
                }
            }

            int CLangConvert::doConversion(const std::wstring &srcLang, const std::wstring &tgtLang, std::wstring &textString)
            {
                if (tgtLang == L"zh-Hans")
                {
                    m_convertChinese.toSimplified(textString);
                    return ERROR_SUCCESS;
                }
                else if (tgtLang == L"zh-Hant")
                {
                    m_convertChinese.toTraditional(textString);
                    return ERROR_SUCCESS;
                }
                else
                {
                    return ERROR_UNSUPPORTED_LANGUAGE;
                }
            }

        }//namespace base
    }
}
