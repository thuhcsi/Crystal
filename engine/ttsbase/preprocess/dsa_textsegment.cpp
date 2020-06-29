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
/// @brief    Implementation file for segmenting text into pieces for later document structure analysis module
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/02/01
///   Changed:  Created
///


#include "cmn/cmn_error.h"
#include "dsa_textsegment.h"
#include "dsa_symboldetect.h"

namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CTextSegment::process(xml::CSSMLDocument *pSSMLDocument)
            {
                // get text items
                m_textItems.clear();
                int ret = CSSMLTraversal::traverse(pSSMLDocument);
                if (ret != ERROR_SUCCESS)
                {
                    return ret;
                }

                // segment text into pieces, and
                // write back the result
                std::vector<xml::CXMLNode*>::iterator itText;
                for (itText = m_textItems.begin(); itText != m_textItems.end(); itText++)
                {
                    xml::CXMLNode *pNode = *itText;

                    // segment text into pieces
                    std::vector<CTextPiece> textPieces;
                    segmentText(pNode->value(), textPieces);

                    // write back the result
                    std::vector<CTextPiece>::iterator it;
                    for (it = textPieces.begin(); it != textPieces.end(); it++)
                    {
                        // build new node if necessary
                        xml::CXMLNode *pNewNode = NULL;

                        if (it->wstrInterpret != L"")
                        {
                            // it is special constructs
                            // create "say-as" node for this
                            xml::CXMLText *pText = new xml::CXMLText(it->wstrText);
                            xml::CXMLElement *pSayAs = new xml::CXMLElement(L"say-as");
                            pSayAs->setAttribute(L"interpret-as", it->wstrInterpret);
                            if (it->wstrFormat != L"")
                                pSayAs->setAttribute(L"format", it->wstrFormat);
                            if (it->wstrDetail != L"")
                                pSayAs->setAttribute(L"detail", it->wstrDetail);
                            pSayAs->linkLastChild(pText);
                            pNewNode = pSayAs;
                        }
                        else
                        {
                            // it is text node
                            xml::CXMLText *pText = new xml::CXMLText(it->wstrText);
                            pNewNode = pText;
                        }

                        // insert new node as previous sibling
                        pNode->insertBefore(pNewNode);
                    }

                    // delete old text node
                    xml::CXMLNode::remove(pNode);
                }

                return ERROR_SUCCESS;
            }

            int CTextSegment::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // "text"
                    // it is text to be segmented
                    m_textItems.push_back(pNode);
                }
                else if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    const std::wstring &value = pNode->value();

                    if (   value == L"w"
                        || value == L"break"
                        || value == L"say-as"
                        || value == L"phoneme"
                        || value == L"sub")
                    {
                        // "w", "break", "say-as", "phoneme", "sub"
                        // should be children of "p" or "s"
                        // contained text should belong to one unit (should NOT be segmented further)
                        childProcessed = true;
                    }
                }

                return ERROR_SUCCESS;
            }

            void CTextSegment::segmentText(const std::wstring &wstrInput, std::vector<CTextPiece> &textPieces)
            {
                textPieces.clear();
                std::wstring wstrText = wstrInput;

                // punctuation(s) is detected first to avoiding mixing it with other symbols
                // (e.g. "24.жа" might be detected as "24." and "жа" if we use "detectSymbols" directly)

                // segment into text pieces by punctuation
                int nPuncStart, nPuncLength;
                while (wstrText.length() > 0 && CSymbolDetect::detectPunctuation(wstrText, nPuncStart, nPuncLength))
                {
                    // leading text
                    if (nPuncStart != 0)
                    {
                        CTextPiece onePiece;
                        onePiece.wstrText = wstrText.substr(0, nPuncStart);
                        textPieces.push_back(onePiece);
                    }

                    // punctuation string
                    if (nPuncLength != 0)
                    {
                        CTextPiece onePiece;
                        onePiece.wstrText = wstrText.substr(nPuncStart, nPuncLength);
                        CSymbolDetect::detectFormat(onePiece.wstrText, onePiece.wstrInterpret, onePiece.wstrFormat, onePiece.wstrDetail);
                        textPieces.push_back(onePiece);
                    }

                    // succeeding text
                    wstrText = wstrText.substr(nPuncStart + nPuncLength);
                }

                // left text
                if (wstrText.length() > 0)
                {
                    CTextPiece onePiece;
                    onePiece.wstrText = wstrText.substr(0, nPuncStart);
                    textPieces.push_back(onePiece);
                }
            }

        }//namespace base
    }
}
