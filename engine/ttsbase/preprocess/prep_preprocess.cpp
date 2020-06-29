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
/// @brief  Implementation file for preprocessing functions
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/06/05
///   Changed:  Implemented. See email for detailed change information.
///

#include "cmn/cmn_error.h"
#include "../datamanage/data_datamanager.h"
#include "../tts.text/tts_textparser.h"
#include "prep_preprocess.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            // 0x19 (ASCII 25)
            const cmn::wstring CPreProcess::m_strNewLine(L"\x19");

            CPreProcess::CPreProcess(const CDataManager *pDataManager, const wchar_t * pTTSEngineType)
                : m_wstrTTSEngineType(pTTSEngineType), CModule(pDataManager)
            {
            }

            int CPreProcess::process(xml::CSSMLDocument *pSSMLDocument, const cmn::string &strIn, str::EEncoding encodingIn, EInputTextType textTagIn)
            {
                // convert multi-byte string to wide-character string
                cmn::wstring wstrIn = str::mbstowcs(strIn, encodingIn);

                // process the wide-character string
                return process(pSSMLDocument, wstrIn, textTagIn);
            }

            int CPreProcess::process(xml::CSSMLDocument *pSSMLDocument, const cmn::wstring &wstrIn, EInputTextType textTagIn)
            {
                /// @todo detect! for partial SSML, some node like xml declaration or "<speak>" node already exist.
                //cmn::wstring wstrSSMLHeader = L"<?xml version=\"1.0\" encoding=\"UTF-16\" ?>\
                //<!DOCTYPE speak PUBLIC \"-/W3C/DTD SYNTHESIS 1.0/EN\"\t\"http://www.w3.org/TR/speech-synthesis/synthesis.dtd\">\
                //<speak version=\"1.1\" xmlns=\"http://www.w3.org/2001/10/synthesis\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.w3.org/TR/speech-synthesis/synthesis.xsd\" xml:lang=\""+m_wstrTTSEngineType+L"\">";

                if (pSSMLDocument == NULL)
                {
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // SSML "speak" root element
                cmn::wstring wstrSSMLHeader = L"<?xml version=\"1.0\" ?>\
                                                <speak version=\"1.1\" xml:lang=\""+m_wstrTTSEngineType+L"\">";
                cmn::wstring wstrSSMLTail = L"</speak>";

                // first replace all "\r\n" in wstrIn with 0x19 (ASCII 25)
                cmn::wstring wstrNew = wstrIn;
                str::replace(wstrNew,L"\r\n",m_strNewLine);
                str::replace(wstrNew,L"\n\r",m_strNewLine);
                str::replace(wstrNew,L"\r",m_strNewLine);
                str::replace(wstrNew,L"\n",m_strNewLine);

                // process text input
                // 1) deal with raw text input,
                // 2) followed by adding SSML "speak" root element,
                // 3) followed by parsing SSML document
                switch (textTagIn)
                {
                case ITT_TEXT_RAW:
                    // raw text input
                    {
                        // deal with HTML entities: "&lt; &gt;"
                        // 1) keep "&lt;" and "&gt;" in the input text by replacing "&" to "&amp;"
                        // 2) convert "<" to "&lt;"
                        // 3) convert ">" to "&gt;"

                        // during XML parsing, inversion processing will be conducted to restore "<", ">", "&lt;" and "&gt;"
                        // 1) convert "&lt;"  to "<"
                        // 2) convert "&gt;"  to ">"
                        // 3) convert "&amp;" to "&"

                        // test cases
                        // 数学表达式：1 + 1 < 3
                        // 为防止将“<”与HTML标记混淆，常用“&lt;”代替“<”，用“&amp;”代替“&”

                        xml::CXMLText::encodeEscapes(wstrNew);
                    }
                case ITT_TEXT_SSML_NO_ROOT:
                    // adding SSML "speak" root element
                    {
                        const TTSSetting &setting = getDataManager<CDataManager>()->getGlobalSetting();
                        std::wstring wstrProsodyHead = str::format(L"<prosody pitch=\"%.2f%%\" rate=\"%.2f%%\" volume=\"%.2f%%\">", setting.pitch*100, setting.rate*100, setting.volume*100);
                        std::wstring wstrProsodyTail = L"</prosody>";

                        // add root and global prosody control tag
                        if (setting.pitch != 1 || setting.rate != 1 || setting.volume != 1)
                            wstrNew = wstrSSMLHeader + wstrProsodyHead + wstrNew + wstrProsodyTail + wstrSSMLTail;
                        else
                            wstrNew = wstrSSMLHeader + wstrNew + wstrSSMLTail;
                    }
                case ITT_TEXT_SSML_WITH_ROOT:
                    // Parsing SSML document
                    {
                        // now it is a well formatted SSML text with "speak" root element
                        // parse the SSML text to SSML document

                        if (!pSSMLDocument->parse(wstrNew.c_str()))
                        {
                            return ERROR_INVALID_SSML_DOCUMENT;
                        }
                    }
                }

                // preprocessing successfully end
                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
