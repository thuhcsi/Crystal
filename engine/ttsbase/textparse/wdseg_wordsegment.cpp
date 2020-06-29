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
/// @brief  Implementation file for word segmentation interface
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

#include "wdseg_wordsegment.h"
#include "cmn/cmn_error.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CWordSegment::CWdSegDocument::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                childProcessed = false;
                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // text
                    if (!m_tmpItem.bPhoneme && !m_tmpItem.bWord)
                        m_tmpItem.pXMLNode = pNode;
                    m_tmpItem.wstrText = pNode->value();
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
                        if (!m_tmpItem.bPhoneme)
                        {
                            m_tmpItem.bWord = true;
                            m_tmpItem.pXMLNode = pNode;
                            m_tmpItem.wstrPOS = ((xml::CXMLElement*)pNode)->getAttribute(L"role");
                        }
                        else
                        {
                            // "w" can not be under "phoneme"
                            return ERROR_INVALID_SSML_DOCUMENT;
                        }
                    }
                    else if (pNode->value() == L"phoneme")
                    {
                        // phoneme
                        if (!m_tmpItem.bWord)
                        {
                            m_tmpItem.bPhoneme = true;
                            m_tmpItem.pXMLNode = pNode;
                        }
                    }
                    else if (pNode->value() == L"sub")
                    {
                        // sub
                        if (!m_tmpItem.bPhoneme && !m_tmpItem.bWord)
                            m_tmpItem.pXMLNode = pNode;
                        m_tmpItem.wstrText = ((xml::CXMLElement*)pNode)->getAttribute(L"alias");
                        appendItem(m_tmpItem);
                        childProcessed = true;
                    }
                }
                return ERROR_SUCCESS;
            }

            int CWordSegment::CWdSegDocument::postProcessNode(xml::CXMLNode *pNode)
            {
                if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    // restore the flag for "w" and "phoneme"
                    if (pNode->value() == L"w")
                    {
                        m_tmpItem.bWord = false;
                        m_tmpItem.wstrPOS = L"";
                    }
                    else if (pNode->value() == L"phoneme")
                    {
                        m_tmpItem.bPhoneme = false;
                    }
                }
                return ERROR_SUCCESS;
            }

            int CWordSegment::process(xml::CSSMLDocument * pSSMLDocument)
            {
                CWdSegDocument  wdsegDocument;

                // retrieve data from SSML document
                wdsegDocument.traverse(pSSMLDocument);

                // process internal data for word segmentation
                segmentToWord(wdsegDocument);

                // write result back to SSML document
                writeResult(pSSMLDocument, wdsegDocument);

                return ERROR_SUCCESS;
            }

            int CWordSegment::writeResult(xml::CSSMLDocument *pSSMLDocument, CWdSegDocument &wdsegDocument)
            {
                // write the word segmentation result to the SSML document
                // for existing "w" (bWord=true), update POS and convert name "token" to "w" if necessary;
                // for existing "phoneme" (bPhoneme=true), add "w" element and link "phoneme" to the new "w";
                // for pure text, create new "w" element (with text only)

                wdsegDocument.resetCursor();
                for (;;)
                {
                    const std::vector<CTextItem> *pSentence = wdsegDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    xml::CXMLNode *pLastNode = NULL;
                    for (std::vector<CTextItem>::const_iterator it = pSentence->begin(); it != pSentence->end(); it++)
                    {
                        const CTextItem &wordItem = *it;
                        if (wordItem.bWord)
                        {
                            // set POS for word if necessary
                            xml::CXMLElement *pWord = (xml::CXMLElement*)wordItem.pXMLNode;
                            pWord->setAttribute(L"role", wordItem.wstrPOS);
                        }
                        else if (wordItem.bPhoneme)
                        {
                            // phoneme, add "w" element
                            xml::CXMLElement *pWord = new xml::CXMLElement(L"w");
                            pWord->setAttribute(L"role", wordItem.wstrPOS);

                            // link "phoneme"
                            wordItem.pXMLNode->insertBefore(pWord);
                            wordItem.pXMLNode->unlink();
                            pWord->linkLastChild(wordItem.pXMLNode);
                        }
                        else
                        {
                            // text, create "w" element
                            xml::CXMLText *pText = new xml::CXMLText(wordItem.wstrText);
                            xml::CXMLElement *pWord = new xml::CXMLElement(L"w");
                            pWord->setAttribute(L"role", wordItem.wstrPOS);
                            pWord->linkLastChild(pText);

                            // insert before current "text" node
                            wordItem.pXMLNode->insertBefore(pWord);

                            // remove the original "text" node only after all "words" have been added
                            if (pLastNode && pLastNode != wordItem.pXMLNode)
                            {
                                xml::CXMLNode::remove(pLastNode);
                            }
                            pLastNode = wordItem.pXMLNode;
                        }
                    }
                    if (pLastNode)
                    {
                        xml::CXMLNode::remove(pLastNode);
                    }
                }

                return ERROR_SUCCESS;
            }

            int CWordSegment::segmentToWord(CWdSegDocument &wdsegDocument)
            {
                // perform the word segmentation for each non-word text
                wdsegDocument.resetCursor();
                for (;;)
                {
                    std::vector<CTextItem> *pSentence = wdsegDocument.getSentence();
                    if (pSentence == NULL)
                        break;

                    // perform the word segmentation for a sentence
                    segmentSentence(*pSentence);
                }

                return ERROR_SUCCESS;
            }

            int CWordSegment::segmentSentence(std::vector<CTextItem> &sentenceInfo)
            {
                const CLexicon &lexicon = getDataManager<CTextData>()->getLexicon();

                for (std::vector<CTextItem>::iterator it = sentenceInfo.begin(); it != sentenceInfo.end(); )
                {
                    CTextItem &textItem = *it;
                    if (textItem.bWord || textItem.bPhoneme)
                    {
                        // advance directly, no necessary for processing
                        it ++;
                    }
                    else
                    {
                        // perform the word segmentation
                        std::vector<CTextItem> wdsegResult;
                        forwardMatch(lexicon, textItem, wdsegResult);

                        // insert the result to original vector
                        size_t span = it - sentenceInfo.begin();
                        sentenceInfo.insert(it, wdsegResult.begin(), wdsegResult.end());
                        it = sentenceInfo.begin() + span + wdsegResult.size();
                        it = sentenceInfo.erase(it);
                    }
                }
                return ERROR_SUCCESS;
            }

            int CWordSegment::forwardMatch(const CLexicon &lexicon, const CTextItem &srcText, std::vector<CTextItem> &resTextList)
            {
                size_t lastPos = 0, totalTextLen = srcText.wstrText.length(), maxWordLen = lexicon.getWordMaxLen(), curWordLen;
                size_t nCount = 0;
                std::wstring wstrDupPiece;
                std::vector<CLexeme> wordEntries;

                while (lastPos < totalTextLen)
                {
                    // duplicate the original piece with the maximum word length
                    curWordLen = totalTextLen - lastPos;
                    wstrDupPiece = srcText.wstrText.substr(lastPos, (curWordLen > maxWordLen) ? maxWordLen : curWordLen);

                    // query the words from maximum length until 1 or word is matched
                    while (wstrDupPiece.length() >= 1)
                    {
                        nCount = lexicon.lookupWord(wstrDupPiece, wordEntries);
                        if (nCount > 0)
                            break;
                        wstrDupPiece.erase(wstrDupPiece.length()-1, 1);
                        nCount = 0;
                    }

                    // save word information
                    resTextList.push_back(srcText);
                    CTextItem &wordItem = resTextList.back();
                    if (nCount > 0)
                    {
                        // word matched in lexicon
                        wordItem.wstrText = wstrDupPiece;
                        wordItem.wstrPOS  = wordEntries[0].wstrPOS;
                    }
                    else
                    {
                        // process unknown character
                        wstrDupPiece = srcText.wstrText.substr(lastPos, 1);
                        wordItem.wstrText = wstrDupPiece;
                        wordItem.wstrPOS  = L"x";   // "x" represents unknown word
                    }

                    // advance to next word
                    lastPos += wstrDupPiece.length();
                }
                return ERROR_SUCCESS;
            }

        }//namespace base
    }
}
