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
/// @brief  Implementation file for grapheme to phoneme conversion interface
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
///

#include "gtp_grapheme2phoneme.h"
#include "cmn/cmn_error.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CGrapheme2Phoneme::CG2PDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
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
                        CWordItem wordItem;
                        processWordNode((xml::CXMLElement*)pNode, wordItem);
                        appendItem(wordItem);

                        childProcessed = true;
                    }
                }
                return ERROR_SUCCESS;
            }

            int CGrapheme2Phoneme::CG2PDocument::processWordNode(xml::CXMLElement *pWord, CWordItem &wordItem)
            {
                // currently, the "w" element can only contain text and the "phoneme" element
                // the "phoneme" element can only contain text

                wordItem.pWordNode = pWord;
                wordItem.bFixed    = false;
                wordItem.wstrPOS   = pWord->getAttribute(L"role");

                xml::CXMLNode *pChild = pWord->firstChild();
                if (pChild == NULL || pChild->nextSibling() != NULL)
                {
                    // only one child is allowed
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                if (pChild->type() == xml::CXMLNode::XML_ELEMENT && pChild->value() == L"phoneme")
                {
                    // under "phoneme"
                    xml::CXMLElement *pPhoneme = (xml::CXMLElement*)pChild;
                    wordItem.wstrAlphabet = pPhoneme->getAttribute(L"alphabet");
                    wordItem.wstrPhoneme  = pPhoneme->getAttribute(L"ph");
                    wordItem.bFixed       = true;

                    // advance to text
                    pChild = pChild->firstChild();
                    if (pChild == NULL || pChild->nextSibling() != NULL)
                    {
                        return ERROR_INVALID_SSML_DOCUMENT;
                    }
                }

                if (pChild->type() != xml::CXMLNode::XML_TEXT)
                {
                    return ERROR_INVALID_SSML_DOCUMENT;
                }

                // the text
                wordItem.wstrWord = pChild->value();
                return ERROR_SUCCESS;
            }

            int CGrapheme2Phoneme::process(xml::CSSMLDocument *pSSMLDocument)
            {
                CG2PDocument g2pDocument;

                // retrieve data from SSML document
                g2pDocument.traverse(pSSMLDocument);

                // process internal data
                getPhoneme(g2pDocument);

                // write result back to SSML document
                writeResult(pSSMLDocument, g2pDocument);

                return ERROR_SUCCESS;
            }

            int CGrapheme2Phoneme::writeResult(xml::CSSMLDocument *pSSMLDocument, CG2PDocument &g2pDocument)
            {
                // write the non-fixed phoneme to word directly
                // the original "phoneme" will be kept unchanged

                g2pDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CWordItem> *pSentence = g2pDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    for (std::vector<CWordItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CWordItem &wordItem = *it;
                        if (wordItem.bFixed)
                        {
                            // the fixed flag indicates the existence of the original "phoneme" element
                            // the original "phoneme" will be kept unchanged
                            continue;
                        }

                        // create new "phoneme" as the child of "w"
                        xml::CXMLElement *pPhoneme = new xml::CXMLElement(L"phoneme");
                        pPhoneme->setAttribute(L"alphabet", wordItem.wstrAlphabet);
                        pPhoneme->setAttribute(L"ph", wordItem.wstrPhoneme);

                        xml::CXMLNode *pText = wordItem.pWordNode->firstChild();
                        pText->insertBefore(pPhoneme);
                        pText->unlink();
                        pPhoneme->linkLastChild(pText);
                    }
                }

                return ERROR_SUCCESS;
            }

            int CGrapheme2Phoneme::getPhoneme(CG2PDocument &g2pDocument)
            {
                // get the phoneme for every non-fixed word item
                g2pDocument.resetCursor();
                for (;;)
                {
                    std::vector<CWordItem> *pSentence = g2pDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // get phoneme for one sentence
                    getPhoneme(*pSentence);
                }

                return ERROR_SUCCESS;
            }

            int CGrapheme2Phoneme::getPhoneme(std::vector<CWordItem> &sentenceInfo)
            {
                // make sure that lexicon is initialized
                const CLexicon &lexicon = getDataManager<CTextData>()->getLexicon();

                for (std::vector<CWordItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    CWordItem &wordItem = *it;
                    if (!wordItem.bFixed)
                    {
                        // get the phoneme for the word text
                        wordItem.wstrAlphabet = lexicon.getAlphabet();
                        wordItem.wstrPhoneme  = lexicon.getPhoneme(wordItem.wstrWord, wordItem.wstrPOS, wordItem.wstrPhoneme);
                    }
                }

                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
