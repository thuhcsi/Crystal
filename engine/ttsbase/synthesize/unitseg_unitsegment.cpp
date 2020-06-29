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
/// @brief  Implementation file for segment the input text pieces and phonemes into basic units
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

#include "unitseg_unitsegment.h"
#include "cmn/cmn_error.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CUnitSegment::process(xml::CSSMLDocument *pSSMLDocument)
            {
                int retVal = ERROR_SUCCESS;
                CUnitSegDocument unitsegDocument;

                // retrieve data from SSML document
                retVal = unitsegDocument.traverse(pSSMLDocument);
                if (retVal != ERROR_SUCCESS)
                    return retVal;

                // process internal data for unit segmentation
                retVal = convertToUnits(unitsegDocument);
                if (retVal != ERROR_SUCCESS)
                    return retVal;

                // write result back to SSML document
                return writeResult(pSSMLDocument, unitsegDocument);
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Definitions for internal document processing
            //
            //////////////////////////////////////////////////////////////////////////

            void CUnitSegment::CTextItem::clear()
            {
                wstrAlphabet.clear();
                wstrPhoneme.clear();
                wstrText.clear();
                pWordNode = NULL;
            }

            int CUnitSegment::CUnitSegDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
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
                    else if (pNode->value() == L"w")
                    {
                        // word
                        // get text information under word
                        CTextItem wordInfo;
                        int succ = parseWordInfo((xml::CXMLElement*)pNode, wordInfo);
                        if (succ != ERROR_SUCCESS)
                            return succ;
                        appendItem(wordInfo);
                        childProcessed = true;
                    }
                }
                return ERROR_SUCCESS;
            }

            int CUnitSegment::CUnitSegDocument::parseWordInfo(xml::CXMLElement *pWordNode, CTextItem &wordInfo) const
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
                wordInfo.wstrText    = pText->value();
                wordInfo.wstrPhoneme = pPhoneme->getAttribute(L"ph");
                wordInfo.wstrAlphabet= pPhoneme->getAttribute(L"alphabet");
                wordInfo.pWordNode   = pWordNode;

                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Operations for performing unit segmentation
            //
            //////////////////////////////////////////////////////////////////////////

            int CUnitSegment::writeResult(xml::CSSMLDocument *pSSMLDocument, CUnitSegDocument &unitDocument)
            {
                unitDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CTextItem> *pSentence = unitDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    xml::CXMLNode *pLastNode = NULL;
                    for (std::vector<CTextItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CTextItem &textInfo = *it;

                        // word information
                        {
                            // create "phoneme" element
                            xml::CXMLText *pText = new xml::CXMLText(textInfo.wstrText);
                            xml::CXMLElement *pPhoneme = new xml::CXMLElement(L"phoneme");
                            pPhoneme->setAttribute(L"alphabet", textInfo.wstrAlphabet);
                            pPhoneme->setAttribute(L"ph", textInfo.wstrPhoneme);
                            pPhoneme->linkLastChild(pText);

                            // create "unit" element
                            xml::CXMLElement *pUnit = new xml::CXMLElement(L"unit");
                            pUnit->linkLastChild(pPhoneme);

                            // remove original "phoneme" node
                            if (pLastNode != textInfo.pWordNode)
                            {
                                xml::CXMLNode *pOldPhoneme = textInfo.pWordNode->firstChild();
                                xml::CXMLNode::remove(pOldPhoneme);
                            }
                            pLastNode = textInfo.pWordNode;

                            // insert "unit" as child or "w" element
                            textInfo.pWordNode->linkLastChild(pUnit);
                        }
                    }
                }

                return ERROR_SUCCESS;
            }

            int CUnitSegment::convertToUnits(CUnitSegDocument &unitDocument)
            {
                int retVal = ERROR_SUCCESS;

                unitDocument.resetCursor();
                for (;;)
                {
                    std::vector<CTextItem> *pSentence = unitDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // convert the text items to basic units
                    retVal = convertToUnits(*pSentence);
                    if (retVal != ERROR_SUCCESS)
                        break;
                }

                return retVal;
            }

            int CUnitSegment::convertToUnits(std::vector<CTextItem> &sentenceInfo)
            {
                std::vector<CTextItem> newSentence;
                for (std::vector<CTextItem>::const_iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    const CTextItem &textInfo = *it;

                    // segment to basic units for text items
                    std::vector<std::wstring> unitTexts, unitPhonemes;
                    if (!segmentToUnits(textInfo.wstrText, textInfo.wstrPhoneme, unitTexts, unitPhonemes))
                    {
                        return ERROR_INVALID_SSML_DOCUMENT;
                    }

                    // append the item
                    CTextItem unitItem;
                    for (size_t i=0; i<unitPhonemes.size(); i++)
                    {
                        unitItem.wstrText    = unitTexts[i];
                        unitItem.wstrPhoneme = unitPhonemes[i];
                        unitItem.wstrAlphabet= textInfo.wstrAlphabet;
                        unitItem.pWordNode   = textInfo.pWordNode;
                        newSentence.push_back(unitItem);
                    }
                }

                // store the new basic units
                // by swapping the two vectors
                std::swap(sentenceInfo, newSentence);

                return ERROR_SUCCESS;
            }

            bool CUnitSegment::segmentToUnits(const std::wstring &wstrText, const std::wstring &wstrPhoneme,
                std::vector<std::wstring> &unitTexts, std::vector<std::wstring> &unitPhonemes)
            {
                return false;
            }

        }//namespace base
    }
}
