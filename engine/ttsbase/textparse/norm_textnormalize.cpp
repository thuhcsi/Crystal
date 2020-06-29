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
/// @brief  Implementation file for text normalization interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Added the default implementation by internal document processing
///

#include "cmn/cmn_error.h"
#include "norm_textnormalize.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CTextNormalize::process(xml::CSSMLDocument *pSSMLDocument)
            {
                CNormDocument normDocument;

                // retrieve data from SSML document
                normDocument.traverse(pSSMLDocument);

                // process internal data for text normalization
                doNormalize(normDocument);

                // write result back to SSML document
                writeResult(pSSMLDocument, normDocument);

                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Definitions for internal document processing
            //
            //////////////////////////////////////////////////////////////////////////

            int CTextNormalize::CNormDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                childProcessed = false;
                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // pure text
                    CTextItem textItem;
                    textItem.wstrText     = pNode->value();
                    appendItem(textItem);

                    childProcessed = true;
                }
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
                    else if (pNode->value() == L"say-as")
                    {
                        // say-as
                        xml::CXMLNode *pText = pNode->firstChild(xml::CXMLNode::XML_TEXT);
                        if (pText == NULL || pText->nextSibling() != NULL)
                        {
                            // only "text" is allowed
                            return ERROR_INVALID_SSML_DOCUMENT;
                        }
                        // store result
                        CTextItem textItem;
                        textItem.pSayasNode = (xml::CXMLElement*)pNode;
                        textItem.wstrText   = pText->value();
                        appendItem(textItem);

                        childProcessed = true;
                    }
                }
                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Operations for performing text normalization
            //
            //////////////////////////////////////////////////////////////////////////

            int CTextNormalize::writeResult(xml::CSSMLDocument *pSSMLDocument, CNormDocument &normDocument)
            {
                normDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CTextItem> *pSentence = normDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    for (std::vector<CTextItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        if (it->pSayasNode == NULL)
                            continue;

                        // store the normalized result

                        // create "sub" element
                        xml::CXMLText *pText = new xml::CXMLText(it->wstrText);
                        xml::CXMLElement *pSub = new xml::CXMLElement(L"sub");
                        pSub->setAttribute(L"alias", it->wstrNormWord);
                        pSub->linkLastChild(pText);

                        // insert before current "say-as" element
                        it->pSayasNode->insertBefore(pSub);

                        // remove the old "say-as" element
                        xml::CXMLNode::remove(it->pSayasNode);
                    }
                }

                return ERROR_SUCCESS;
            }

            int CTextNormalize::doNormalize(CNormDocument &normDocument)
            {
                normDocument.resetCursor();
                for (;;)
                {
                    std::vector<CTextItem> *pSentence = normDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // perform text normalization for a sentence
                    doNormalize(*pSentence);
                }

                return ERROR_SUCCESS;
            }

            int CTextNormalize::doNormalize(std::vector<CTextItem> &sentenceInfo)
            {
                // the default implementation of text normalization
                // just copy the original text to the normalized result, and set POS to L"x" (Unknown)

                for (std::vector<CTextItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); it++)
                {
                    if (it->pSayasNode == NULL)
                        continue;
                    it->wstrNormWord = it->wstrText;
                    it->wstrPOS      = L"x";
                }

                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
