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
/// @brief  Implementation file for prosodic prediction interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///

#include "../datavoice/data_voicedata.h"
#include "psp_prosodypredict.h"
#include "xml/ssml_helper.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CProsodyPredict::process(xml::CSSMLDocument *pSSMLDocument)
            {
                int retVal = ERROR_SUCCESS;
                CPSPDocument prosodyDocument;

                // retrieve data from SSML document
                retVal = prosodyDocument.traverse(pSSMLDocument);
                if (retVal != ERROR_SUCCESS)
                    return retVal;

                // process internal data for prosody prediction
                retVal = predictProsody(prosodyDocument);
                if (retVal != ERROR_SUCCESS)
                    return retVal;

                // write result back to SSML document
                return writeResult(pSSMLDocument, prosodyDocument);
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Definitions for internal document processing
            //
            //////////////////////////////////////////////////////////////////////////

            void CProsodyPredict::CUnitItem::clear()
            {
                wstrPhoneme.clear();
                wstrText.clear();
                pXMLNode      = NULL;
                boundaryType  = xml::CSSMLDocument::PROSBOUND_SYLLABLE;
                isStressed    = false;
                isBreak       = false;
                duration      = 0;
                amplitude     = 0;
                pitchmean     = 0;
                memset(contour, 0, sizeof(float)*10);
            }

            int CProsodyPredict::CPSPDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                childProcessed = false;
                if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    if (pNode->value() == L"p")
                    {
                        // paragraph
                        appendParagraph();
                    }
                    else if (pNode->value() == L"s")
                    {
                        // sentence
                        appendSentence();
                    }
                    else if (pNode->value() == L"unit")
                    {
                        // get unit information
                        CUnitItem unitInfo;
                        int succ = parseUnitInfo((xml::CXMLElement*)pNode, unitInfo);
                        if (succ != ERROR_SUCCESS)
                            return succ;
                        // apply user specified prosody
                        if (m_stackProsody.size() != 0)
                        {
                            unitInfo.ssmlProsody = m_stackProsody.top();
                        }
                        appendItem(unitInfo);
                        childProcessed = true;
                    }
                    else if (pNode->value() == L"break")
                    {
                        // break
                        CUnitItem unitInfo;
                        unitInfo.isBreak = true;
                        unitInfo.pXMLNode = (xml::CXMLElement*)pNode;
                        unitInfo.boundaryType = xml::CSSMLHelper::getBoundaryType(unitInfo.pXMLNode);
                        // set the boundary information of the previous word
                        std::vector<CUnitItem> &sentence = getLastSentence();
                        if (sentence.size() != 0)
                        {
                            sentence.back().boundaryType = unitInfo.boundaryType;
                        }
                        appendItem(unitInfo);
                    }
                    else if (pNode->value() == L"prosody")
                    {
                        // prosody
                        SSMLProsody prosInfo;
                        int succ = parseProsodyInfo((xml::CXMLElement*)pNode, prosInfo);
                        if (succ != ERROR_SUCCESS)
                            return succ;
                        m_stackProsody.push(prosInfo);
                    }
                    else if (pNode->value() == L"emphasis")
                    {
                        // emphasis
                        SSMLProsody prosInfo;
                        int succ = parseEmphasisInfo((xml::CXMLElement*)pNode, prosInfo);
                        if (succ != ERROR_SUCCESS)
                            return succ;
                        m_stackProsody.push(prosInfo);
                    }
                }
                return ERROR_SUCCESS;
            }

            int CProsodyPredict::CPSPDocument::postProcessNode(xml::CXMLNode *pNode)
            {
                if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    if (pNode->value() == L"prosody")
                    {
                        // end of prosody
                        m_stackProsody.pop();
                    }
                    else if (pNode->value() == L"emphasis")
                    {
                        // end of emphasis
                        m_stackProsody.pop();
                    }
                }
                return ERROR_SUCCESS;
            }

            int CProsodyPredict::CPSPDocument::parseUnitInfo(xml::CXMLElement *pWordNode, CUnitItem &unitInfo) const
            {
                xml::CXMLElement *pPhoneme = (xml::CXMLElement*)pWordNode->firstChild(L"phoneme", xml::CXMLNode::XML_ELEMENT);
                if (pPhoneme == NULL || pPhoneme->nextSibling() != NULL)
                {
                    // only one child "phoneme" is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                xml::CXMLText *pText = (xml::CXMLText*)pPhoneme->firstChild(xml::CXMLNode::XML_TEXT);
                if (pText == NULL || pText->nextSibling() != NULL)
                {
                    // only "text" is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // get phoneme, text, boundary information
                unitInfo.wstrText    = pText->value();
                unitInfo.wstrPhoneme = pPhoneme->getAttribute(L"ph");
                unitInfo.pXMLNode    = pWordNode;
                unitInfo.boundaryType= xml::CSSMLDocument::PROSBOUND_SYLLABLE;
                unitInfo.isStressed  = false;
                unitInfo.isBreak     = false;

                return ERROR_SUCCESS;
            }

            int CProsodyPredict::CPSPDocument::parseEmphasisInfo(xml::CXMLElement *pEmphasisNode, SSMLProsody &prosInfo) const
            {
                // get level attribute, default is "moderate"
                const std::wstring& level = pEmphasisNode->getAttribute(L"level");
                std::wstring pitch, rate, volume;

                // set prosody value
                if (level.empty() || level == L"moderate")
                {
                    // default or moderate
                    pitch  = L"medium";
                    rate   = L"slow";
                    volume = L"loud";
                }
                else if (level == L"strong")
                {
                    // strong
                    pitch  = L"high";
                    rate   = L"x-slow";
                    volume = L"x-loud";
                }
                else if (level == L"none")
                {
                    // none
                    pitch  = L"default";
                    rate   = L"default";
                    volume = L"default";
                }
                else if (level == L"reduced")
                {
                    // reduced
                    pitch  = L"default";
                    rate   = L"fast";
                    volume = L"soft";
                }
                else
                {
                    // invalid level value
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                prosInfo.pitch = xml::CSSMLHelper::parsePitch(pitch);
                prosInfo.rate  = xml::CSSMLHelper::parseRate(rate);
                prosInfo.volume= xml::CSSMLHelper::parseVolume(volume);

                return ERROR_SUCCESS;
            }

            int CProsodyPredict::CPSPDocument::parseProsodyInfo(xml::CXMLElement *pProsodyNode, SSMLProsody &prosInfo) const
            {
                // get pitch
                prosInfo.pitch = xml::CSSMLHelper::parsePitch(pProsodyNode->getAttribute(L"pitch"));
                if (prosInfo.pitch == -1)
                {
                    // invalid pitch
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // get rate
                prosInfo.rate = xml::CSSMLHelper::parseRate(pProsodyNode->getAttribute(L"rate"));
                if (prosInfo.rate == -1)
                {
                    // invalid rate
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // get volume
                prosInfo.volume = xml::CSSMLHelper::parseVolume(pProsodyNode->getAttribute(L"volume"));
                if (prosInfo.volume == -1)
                {
                    // invalid volume
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                return ERROR_SUCCESS;
            }



            //////////////////////////////////////////////////////////////////////////
            //
            //  Operations for performing prosody prediction
            //
            //////////////////////////////////////////////////////////////////////////

            int CProsodyPredict::writeResult(xml::CSSMLDocument *pSSMLDocument, CPSPDocument &unitDocument)
            {
                unitDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CUnitItem> *pSentence = unitDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    for (std::vector<CUnitItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CUnitItem &unitInfo = *it;

                        if (unitInfo.isBreak && unitInfo.pXMLNode != NULL && unitInfo.pXMLNode->getAttribute(L"time").length() == 0)
                        {
                            // set break time information
                            // if original time exist, leave it unchanged!
                            std::wstring times;
                            xml::CSSMLHelper::composeString(times, L"%dms", (int)unitInfo.duration);
                            unitInfo.pXMLNode->setAttribute(L"time", times);
                            continue;
                        }

                        // unit information
                        {
                            // get original "phoneme" element
                            // unlink it from "unit", for later being linked as the child of "prosody"
                            xml::CXMLNode *pPhoneme = unitInfo.pXMLNode->firstChild();
                            pPhoneme->unlink();

                            // create "prosody" element
                            std::wstring attVal, tmpVal;
                            xml::CXMLElement *pProsody = new xml::CXMLElement(L"prosody");
                            xml::CSSMLHelper::composeString(attVal, L"%.3f", unitInfo.ssmlProsody.rate);
                            pProsody->setAttribute(L"dratio", attVal);
                            xml::CSSMLHelper::composeString(attVal, L"%.3f", unitInfo.ssmlProsody.volume);
                            pProsody->setAttribute(L"vratio", attVal);
                            xml::CSSMLHelper::composeString(attVal, L"%.3f", unitInfo.ssmlProsody.pitch);
                            pProsody->setAttribute(L"pratio", attVal);
                            xml::CSSMLHelper::composeString(attVal, L"%dms", (int)unitInfo.duration);
                            pProsody->setAttribute(L"duration", attVal);
                            xml::CSSMLHelper::composeString(attVal, L"%d", (int)unitInfo.amplitude);
                            pProsody->setAttribute(L"volume", attVal);
                            xml::CSSMLHelper::composeString(attVal, L"%dHz", (int)unitInfo.pitchmean);
                            pProsody->setAttribute(L"pitch", attVal);
                            attVal = L"";
                            for (int i=0; i<10; i++)
                            {
                                xml::CSSMLHelper::composeString(tmpVal, L"%dHz ", (int)unitInfo.contour[i]);
                                attVal += tmpVal;
                            }
                            pProsody->setAttribute(L"contour", attVal);
                            pProsody->linkLastChild(pPhoneme);

                            // insert "prosody" as child of "unit" element
                            unitInfo.pXMLNode->linkLastChild(pProsody);
                        }
                    }
                }

                return ERROR_SUCCESS;
            }

            int CProsodyPredict::predictProsody(CPSPDocument &unitDocument)
            {
                int retVal = ERROR_SUCCESS;

                unitDocument.resetCursor();
                for (;;)
                {
                    std::vector<CUnitItem> *pSentence = unitDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // perform prosody prediction for a sentence
                    retVal = predictProsody(*pSentence);
                    if (retVal != ERROR_SUCCESS)
                        break;
                }

                return retVal;
            }

            int CProsodyPredict::predictProsody(std::vector<CUnitItem> &sentenceInfo)
            {
                // the default implementation of prosody predict just sets the default duration

                const CWavSynthesizer &wavsyn = getDataManager<CVoiceData>()->getWavSynthesizer();
                float bytesPerMilliSecond = wavsyn.getSamplesPerSec() * wavsyn.getBitsPerSample() / 8.0f / 1000;

                for (std::vector<CUnitItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    CUnitItem &unitInfo = *it;

                    // get the wave length
                    uint32 waveLen = wavsyn.getWaveLength(unitInfo.wstrPhoneme);

                    // set default duration
                    unitInfo.duration = waveLen / bytesPerMilliSecond;
                }

                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
