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
/// @brief  Implementation file for prosodic structure generation interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Converted SSML document to internal representation for processing
/// - Version:  0.2.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/01/10
///   Changed:  For boundary type, consider the boundary after current word
/// - Version:  0.2.2
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/07/20
///   Changed:  The original user specific boundary type (i.e. bFixed is true) should be kept unchanged
///

#include "psg_prosstructgen.h"
#include "cmn/cmn_error.h"
#include "xml/ssml_helper.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CProsodicStructGenerate::CPSGDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                childProcessed = false;
                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // text
                    m_tmpItem.wstrText = pNode->value();
                    // decide whether it is BOS
                    std::vector<CWordItem> &sentence = getLastSentence();
                    m_tmpItem.bBOS = (sentence.size() == 0);
                    appendItem(m_tmpItem);
                }
                else if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
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
                        m_tmpItem.pWordNode = pNode;
                        m_tmpItem.wstrPOS   = ((xml::CXMLElement*)pNode)->getAttribute(L"role");

                        // only "break" after word is considered
                        // get the boundary type
                    }
                    else if (pNode->value() == L"break")
                    {
                        // break
                        // set the boundary information of the previous word
                        std::vector<CWordItem> &sentence = getLastSentence();
                        if (sentence.size() != 0)
                        {
                            int boundaryType = PROSBOUNDTYPE_SYLLABLE;
                            switch (xml::CSSMLHelper::getBoundaryType((xml::CXMLElement*)pNode))
                            {
                            case xml::CSSMLDocument::PROSBOUND_SYLLABLE: boundaryType = PROSBOUNDTYPE_SYLLABLE; break;
                            case xml::CSSMLDocument::PROSBOUND_PWORD:    boundaryType = PROSBOUNDTYPE_PWORD;    break;
                            case xml::CSSMLDocument::PROSBOUND_PPHRASE:  boundaryType = PROSBOUNDTYPE_PPHRASE;  break;
                            case xml::CSSMLDocument::PROSBOUND_SENTENCE: boundaryType = PROSBOUNDTYPE_SENTENCE; break;
                            }
                            sentence.back().nBoundaryType = boundaryType;
                            sentence.back().bFixed = true;
                        }
                    }
                }
                return ERROR_SUCCESS;
            }


            int CProsodicStructGenerate::process(xml::CSSMLDocument * pSSMLDocument)
            {
                CPSGDocument  psgDocument;

                // retrieve data from SSML document
                psgDocument.traverse(pSSMLDocument);

                // process internal data for prosodic structure generation
                generateProsodicStructure(psgDocument);

                // write result back to SSML document
                writeResult(pSSMLDocument, psgDocument);

                return ERROR_SUCCESS;
            }

            int CProsodicStructGenerate::writeResult(xml::CSSMLDocument *pSSMLDocument, CPSGDocument &psgDocument)
            {
                // write "break" to the document

                // the original boundary type (i.e. bFixed is true) should be kept unchanged
                // in prosodic structure generation procedure

                psgDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CWordItem> *pSentence = psgDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    for (std::vector<CWordItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CWordItem &wordItem = *it;

                        if (wordItem.bBOS)
                        {
                            // insert "break" element before BOS
                            xml::CXMLElement *pBreak = (xml::CXMLElement*)wordItem.pWordNode->prevSibling(L"break", xml::CXMLNode::XML_ELEMENT);
                            if (pBreak == NULL || pBreak != wordItem.pWordNode->prevSibling())
                            {
                                // "break" should be the immediate sibling of "w"
                                pBreak = new xml::CXMLElement(L"break");
                                wordItem.pWordNode->insertBefore(pBreak);
                            }
                            xml::CSSMLHelper::setBoundaryType(pBreak, xml::CSSMLDocument::PROSBOUND_SENTENCE);
                            // remove continuous "break" element
                            for (xml::CXMLNode *pNode = pBreak->prevSibling(); pNode != NULL && pNode->isCertainElement(L"break");)
                            {
                                xml::CXMLNode *pBrkNode = pNode;
                                pNode = pNode->prevSibling();
                                xml::CXMLNode::remove(pBrkNode);
                            }
                        }

                        if (wordItem.nBoundaryType == PROSBOUNDTYPE_SYLLABLE)
                        {
                            // none boundary (syllable / lexicon word)
                            continue;
                        }

                        // get "break" element after current word
                        xml::CXMLElement *pBreak = (xml::CXMLElement*)wordItem.pWordNode->nextSibling(L"break", xml::CXMLNode::XML_ELEMENT);
                        if (pBreak == NULL || pBreak != wordItem.pWordNode->nextSibling())
                        {
                            // "break" should be the immediate sibling of "w"
                            pBreak = new xml::CXMLElement(L"break");
                            wordItem.pWordNode->insertAfter(pBreak);
                        }

                        // set the "break" information
                        int boundaryType = xml::CSSMLDocument::PROSBOUND_PPHRASE;
                        switch (wordItem.nBoundaryType)
                        {
                        case PROSBOUNDTYPE_SYLLABLE: boundaryType = xml::CSSMLDocument::PROSBOUND_SYLLABLE;  break;
                        case PROSBOUNDTYPE_PWORD:    boundaryType = xml::CSSMLDocument::PROSBOUND_PWORD;     break;
                        case PROSBOUNDTYPE_PPHRASE:  boundaryType = xml::CSSMLDocument::PROSBOUND_PPHRASE;   break;
                        case PROSBOUNDTYPE_SENTENCE: boundaryType = xml::CSSMLDocument::PROSBOUND_SENTENCE;  break;
                        }
                        xml::CSSMLHelper::setBoundaryType(pBreak, boundaryType);
                        // remove continuous "break" element
                        for (xml::CXMLNode *pNode = pBreak->nextSibling(); pNode != NULL && pNode->isCertainElement(L"break");)
                        {
                            xml::CXMLNode *pBrkNode = pNode;
                            pNode = pNode->nextSibling();
                            xml::CXMLNode::remove(pBrkNode);
                        }
                    }
                }

                return ERROR_SUCCESS;
            }

            int CProsodicStructGenerate::generateProsodicStructure(CPSGDocument &psgDocument)
            {
                // generate prosodic structure for the entire document
                psgDocument.resetCursor();
                for (;;)
                {
                    std::vector<CWordItem> *pSentence = psgDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // generate prosodic structure for each sentence
                    generateProsodicStructure(*pSentence);
                }

                return ERROR_SUCCESS;
            }

            int CProsodicStructGenerate::generateProsodicStructure(std::vector<CWordItem> &sentenceInfo)
            {
                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
