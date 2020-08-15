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
/// @brief  Implementation file for speech synthesis interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/13
///   Changed:  Modified the interface to use CWaveData directly
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
///

#include "../datavoice/data_voicedata.h"
#include "synth_synthesize.h"
#include "xml/ssml_helper.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CSynthesize::process(xml::CSSMLDocument *pSSMLDocument, dsp::CWaveData &waveData)
            {
                CSynthDocument synthDocument;

                // retrieve data from SSML document
                synthDocument.traverse(pSSMLDocument);

                // clear the original wave data
                waveData.clear();

                // set the wave format
                const CWavSynthesizer &wavsyn = getDataManager<CVoiceData>()->getWavSynthesizer();
                int samplesPerSec = wavsyn.getSamplesPerSec();
                int bitsPerSample = wavsyn.getBitsPerSample();
                int channels      = wavsyn.getChannels();
                waveData.setFormat(samplesPerSec, bitsPerSample, channels);

                // perform synthesis based on the internal data
                synthesize(synthDocument, waveData);

                // save new duration
                writeResult(pSSMLDocument, synthDocument);

                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Definitions for internal document processing
            //
            //////////////////////////////////////////////////////////////////////////

            void CSynthesize::CUnitItem::clear()
            {
                wstrText.clear();
                wstrPhoneme.clear();
                pXMLNode    = NULL;
                unitID      = INVALID_UNITID;
                phonemeID   = INVALID_ICODE;
                isBreak     = false;
                boundaryType= xml::CSSMLDocument::PROSBOUND_SYLLABLE;
                isStressed  = false;
                duration    = 0;
                amplitude   = 0;
                pitchmean   = 0;
                rate        = 0;
                volume      = 0;
                pitch       = 0;
                memset(contour, 0, sizeof(float)*10);
            }

            int CSynthesize::CSynthDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
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
                    else if (pNode->value() == L"break")
                    {
                        // break
                        CUnitItem breakInfo;
                        getBreakInfo((xml::CXMLElement*)pNode, breakInfo);
                        appendItem(breakInfo);

                        childProcessed = true;
                    }
                    else if (pNode->value() == L"unit")
                    {
                        // get basic unit information
                        CUnitItem unitInfo;
                        getBasicUnitInfo((xml::CXMLElement*)pNode, unitInfo);
                        appendItem(unitInfo);

                        childProcessed = true;
                    }
                }
                return ERROR_SUCCESS;
            }

            int CSynthesize::CSynthDocument::postProcessNode(xml::CXMLNode *pNode)
            {
                // set the boundary information only after "s", "w" and "break" are processed
                if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    if (pNode->value() == L"s")
                    {
                        // set sentence boundary information
                        setBoundaryTypeTillNonbreak(getLastSentence(), xml::CSSMLDocument::PROSBOUND_SENTENCE);
                    }
                    else if (pNode->value() == L"break")
                    {
                        // set boundary information according to boundary type
                        setBoundaryTypeTillNonbreak(getLastSentence(), xml::CSSMLHelper::getBoundaryType((xml::CXMLElement*)pNode));
                    }
                    else if (pNode->value() == L"w")
                    {
                        // set lexicon word boundary information
                        setBoundaryTypeTillNonbreak(getLastSentence(), xml::CSSMLDocument::PROSBOUND_LWORD);
                    }
                }
                return ERROR_SUCCESS;
            }

            int CSynthesize::CSynthDocument::setBoundaryTypeTillNonbreak(std::vector<CUnitItem> &sentence, int boundaryType) const
            {
                 std::vector<CUnitItem>::reverse_iterator rit;
                 for (rit = sentence.rbegin(); rit != sentence.rend(); rit++)
                 {
                     // only set to larger boundary type
                     if (rit->boundaryType < boundaryType)
                        rit->boundaryType = boundaryType;
                     // till non-break
                     if (!rit->isBreak)
                         break;
                 }
                 return ERROR_SUCCESS;
            }

            int CSynthesize::CSynthDocument::getBreakInfo(xml::CXMLElement *pBreakNode, CUnitItem &breakInfo) const
            {
                float fBreakTime = xml::CSSMLHelper::parseTime(pBreakNode->getAttribute(L"time"));

                // get the break information
                breakInfo.clear();
                breakInfo.pXMLNode = pBreakNode;
                breakInfo.duration = fBreakTime;
                breakInfo.isBreak  = true;
                breakInfo.boundaryType = xml::CSSMLHelper::getBoundaryType(pBreakNode);

                return ERROR_SUCCESS;
            }

            int CSynthesize::CSynthDocument::getBasicUnitInfo(xml::CXMLElement *pUnitNode, CUnitItem &unitInfo) const
            {
                // Validation assurance:
                // The elements have been normalized.
                //  The "unit" element can only contain "prosody" element, and
                //  the "prosody" element can only contain "phoneme" element, and
                //  the "phoneme" element can only contain text

                xml::CXMLElement *pProsody = (xml::CXMLElement*)pUnitNode->firstChild(L"prosody", xml::CXMLNode::XML_ELEMENT);
                if (pProsody == NULL || pProsody->nextSibling() != NULL)
                {
                    // only one child "prosody" is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                xml::CXMLElement *pPhoneme = (xml::CXMLElement*)pProsody->firstChild(L"phoneme", xml::CXMLNode::XML_ELEMENT);
                if (pPhoneme == NULL || pPhoneme->nextSibling() != NULL)
                {
                    // only one child "phoneme" is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                xml::CXMLNode *pText = pPhoneme->firstChild(xml::CXMLNode::XML_TEXT);
                if (pText == NULL || pText->nextSibling() != NULL)
                {
                    // only "text" is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // "unit" element
                unitInfo.pXMLNode = pUnitNode;

                // get phoneme, text information
                unitInfo.wstrPhoneme= pPhoneme->getAttribute(L"ph");
                unitInfo.wstrText   = pText->value();
                unitInfo.phonemeID  = INVALID_ICODE;
                unitInfo.unitID     = INVALID_UNITID;

                // get prosody information
                unitInfo.duration   = xml::CSSMLHelper::parseTime(pProsody->getAttribute(L"duration"));
                unitInfo.amplitude  = xml::CSSMLHelper::parseFloatNumber(pProsody->getAttribute(L"volume"));
                unitInfo.pitchmean  = xml::CSSMLHelper::parseFrequency(pProsody->getAttribute(L"pitch"));
                if (pProsody->getAttribute(L"contour").length() > 0)
                    memcpy(unitInfo.contour, &(xml::CSSMLHelper::parseFrequencyList(pProsody->getAttribute(L"contour"))[0]), sizeof(float)*10);
                unitInfo.rate       = xml::CSSMLHelper::parseFloatNumber(pProsody->getAttribute(L"dratio"));
                unitInfo.volume     = xml::CSSMLHelper::parseFloatNumber(pProsody->getAttribute(L"vratio"));
                unitInfo.pitch      = xml::CSSMLHelper::parseFloatNumber(pProsody->getAttribute(L"pratio"));
                unitInfo.isStressed = false;

                // boundary information
                unitInfo.boundaryType = xml::CSSMLDocument::PROSBOUND_SYLLABLE;
                unitInfo.isBreak  = false;

                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Operations for performing the speech synthesis
            //
            //////////////////////////////////////////////////////////////////////////

            int CSynthesize::writeResult(xml::CSSMLDocument *pSSMLDocument, CSynthDocument &synthDocument)
            {
                // write the new prosody data
                // currently, only duration is written and kept

                synthDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CUnitItem> *pSentence = synthDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    for (std::vector<CUnitItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CUnitItem &unitInfo = *it;

                        // save prosody information
                        if (unitInfo.isBreak)
                        {
                            // set break time information
                            std::wstring times;
                            xml::CSSMLHelper::composeString(times, L"%dms", (int)unitInfo.duration);
                            unitInfo.pXMLNode->setAttribute(L"time", times);
                        }
                        else
                        {
                            // save uid
                            std::wstring wstrUid;
                            xml::CSSMLHelper::composeString(wstrUid, L":%d", unitInfo.unitID);
                            unitInfo.pXMLNode->setAttribute(L"uid", it->wstrPhoneme + wstrUid);

                            // set prosody information
                            // "unit" element can only contain "prosody" element
                            xml::CXMLNode *pChild = unitInfo.pXMLNode->firstChild(L"prosody");
                            if (pChild != NULL && pChild->type() == xml::CXMLNode::XML_ELEMENT)
                            {
                                xml::CXMLElement *pProsody = (xml::CXMLElement*)pChild;
                                std::wstring attVal;
                                xml::CSSMLHelper::composeString(attVal, L"%dms", (int)unitInfo.duration);
                                pProsody->setAttribute(L"duration", attVal);
                                pProsody->removeAttribute(L"dratio");
                                pProsody->removeAttribute(L"vratio");
                                pProsody->removeAttribute(L"pratio");
                                pProsody->removeAttribute(L"volume");
                                pProsody->removeAttribute(L"pitch");
                                pProsody->removeAttribute(L"contour");
                            }
                        }
                    }
                }

                return ERROR_SUCCESS;
            }

            int CSynthesize::synthesize(CSynthDocument &synthDocument, dsp::CWaveData &waveData)
            {
                synthDocument.resetCursor();
                for (;;)
                {
                    std::vector<CUnitItem> *pSentence = synthDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // perform speech synthesis for a sentence
                    synthesize(*pSentence, waveData);
                }
                return ERROR_SUCCESS;
            }

            int CSynthesize::synthesize(std::vector<CUnitItem> &sentenceInfo, dsp::CWaveData &waveData)
            {
                const CWavSynthesizer &wavsyn = getDataManager<CVoiceData>()->getWavSynthesizer();
                int samplesPerSec = wavsyn.getSamplesPerSec();
                int bitsPerSample = wavsyn.getBitsPerSample();
                int channels      = wavsyn.getChannels();

                // navigate all unit or break items
                dsp::CWaveData waveBuffer;
                for (std::vector<CUnitItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    CUnitItem &tgtUnit = *it;

                    if (tgtUnit.isBreak)
                    {
                        // synthesize break
                        uint32 silenceLength = (uint32)(tgtUnit.duration / 1000 * samplesPerSec * bitsPerSample * channels / 16) * 2; // align to even
                        if (!waveData.appendData(NULL, silenceLength))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                    else
                    {
                        // synthesize basic unit
                        uint32 waveLength = wavsyn.getWaveLength(tgtUnit.wstrPhoneme);
                        if (!waveBuffer.resize(waveLength))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                        // get data
                        if (!wavsyn.getWave(tgtUnit.wstrPhoneme, waveBuffer.getData(), waveLength))
                        {
                            return ERROR_DATA_READ_FAULT;
                        }
                        waveBuffer.resize(waveLength);
                        // append data
                        if (!waveData.appendData(waveBuffer.getData(), waveBuffer.getLength()))
                        {
                            return ERROR_OUTOFMEMORY;
                        }
                    }
                }
                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
