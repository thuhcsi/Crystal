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
/// @brief    Implementation file for document structure analysis interface
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/06/07
///   Changed:  Created
///

#include "cmn/cmn_error.h"
#include "dsa_docstruct.h"


namespace cst
{
    namespace tts
    {
        namespace base
        {
            int CDocStruct::process(xml::CSSMLDocument *pSSMLDocument)
            {
                int ret = ERROR_SUCCESS;

                // segment into paragraphs
                m_wstrParsingFragmentTag = L"p";
                m_numFragments.clear();
                ret = CSSMLTraversal::traverse(pSSMLDocument);
                if (ret != ERROR_SUCCESS)
                {
                    return ret;
                }

                // segment into sentences
                m_wstrParsingFragmentTag = L"s";
                m_numFragments.clear();
                ret = CSSMLTraversal::traverse(pSSMLDocument);
                if (ret != ERROR_SUCCESS)
                {
                    return ret;
                }

                // detect special constructs

                return ERROR_SUCCESS;
/*

                if( pSSMLDocument == NULL )
                {
                    return ERROR_SUCCESS;
                }
                m_wstrTerminatorsForSentence =L"\u3002";//¡£
                xml::CXMLNode *pCurrentNode = pSSMLDocument->firstChild();
                while (pCurrentNode != NULL)
                {
                    bool child_processed = false;
                    processNode( pCurrentNode, child_processed );
                    if(pCurrentNode->firstChild() != NULL && !child_processed )
                    {
                        // start the children processing
                        pCurrentNode = pCurrentNode->firstChild();
                        continue;
                    }

                    while (pCurrentNode != NULL)
                    {
                        // current-node processing is over, do post-processing
                        postProcessNode( pCurrentNode );

                        if (pCurrentNode->nextSibling() != NULL)
                        {
                            // next sibling first
                            pCurrentNode = pCurrentNode->nextSibling();
                            break;
                        }
                        else 
                        {
                            // back to parent
                            pCurrentNode = pCurrentNode->parent();
                        }
                    }
                }

                return ERROR_SUCCESS;
                */
            }


            int CDocStruct::processNode(xml::CXMLNode *pNode, bool &childProcessed)
            {
                const std::wstring &fragmentTag = m_wstrParsingFragmentTag;

                if (pNode->type() == xml::CXMLNode::XML_TEXT)
                {
                    // "text"
                    // it is text to be segmented
                    m_numFragments.push_back(std::make_pair(pNode, 0));
                }
                else if (pNode->type() == xml::CXMLNode::XML_ELEMENT)
                {
                    const std::wstring &value = pNode->value();

                    if (value == L"speak" && fragmentTag == L"p")
                    {
                        // "speak"
                        // it is paragraph root
                        m_numFragments.push_back(std::make_pair(pNode, 1)); // there must always be one "p"
                    }
                    else if (value == L"p" && fragmentTag == L"s")
                    {
                        // "paragraph"
                        // it is sentence root
                        m_numFragments.push_back(std::make_pair(pNode, 1)); // there must alway be one "s"
                    }
                    else
                    {
                        if (m_numFragments.size() != 0)
                        {
                            // if input node is already "p" or "s",
                            // set the fragment number of current node to 1,
                            // else set the fragment number to 0
                            int fragmentNum = (value == fragmentTag) ? 1 : 0;

                            // the root has occurred,
                            // current node will be parsed further
                            m_numFragments.push_back(std::make_pair(pNode, fragmentNum));
                        }

                        if (value == L"p" && fragmentTag == L"p")
                        {
                            // already paragraph
                            // no need go further down to the children
                            childProcessed = true;
                        }
                        else if (value == L"s")
                        {
                            // already sentence
                            // no need go further down to the children
                            // special constructs detection is required here
                            childProcessed = true;
                        }
                        else if (value == L"w"
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
                        else
                        {
                            // other elements, e.g. "emphasis", "prosody", etc
                            // contained text should be segmented further
                        }
                    }
                }

                return ERROR_SUCCESS;
            }

            int CDocStruct::postProcessNode(xml::CXMLNode *pNode)
            {
                const std::wstring &fragmentTag = m_wstrParsingFragmentTag;

                // find the parent node in the fragment number "stack"
                std::vector< std::pair<xml::CXMLNode*, int> >::reverse_iterator ritCurrent = m_numFragments.rbegin();
                std::vector< std::pair<xml::CXMLNode*, int> >::reverse_iterator ritParent  = ritCurrent;
                if (ritCurrent == m_numFragments.rend())
                {
                    // the input "pNode" is not the children of insertion root
                    // it will not be processed
                    return ERROR_SUCCESS;
                }
                if (ritCurrent->first != pNode)
                {
                    // it should be the newly created "p" or "s" node
                    return ERROR_SUCCESS;
                }
                while (ritParent != m_numFragments.rend() && ritParent->first != pNode->parent())
                    ritParent ++;
                if (ritParent == m_numFragments.rend())
                {
                    // it should be the insertion root of "p" or "s" node
                    // clear for new root
                    m_numFragments.resize(0);
                    return ERROR_SUCCESS;
                }

                //
                // There are 3 different conditions where the "p" or "s" node is to be created
                // Condition 1: see below
                // Condition 2: see below
                // Condition 3: see below
                //

                if (ritCurrent->second > 0) // Condition 1
                {
                    //
                    // Condition 1:
                    //
                    // There are already fragment(s) in current node (i.e. current.fragmentNum>0),
                    //  (1) left sibling(s) in the "stack" (if any) should belong to one new fragment of "p" or "s",
                    //  (2) pop up current node and all left sibling(s) as they have been processed
                    //
                    // Examples (for sentence "s"):
                    //  (1) <s> This is the first simple example </s>
                    //      ==> <s> This is the first simple example </s>
                    //  (2) This is the <emphasis> second <s> complicated example </s> </emphasis>
                    //      ==> <s> This is the </s> <emphasis> <s> second </s> <s> complicated example </s> </emphasis>
                    //

                    int numFragments = ritCurrent->second;

                    // if and only if there are left sibling(s) in "stack"
                    if (ritParent-1 != ritCurrent)
                    {
                        // create "p"/"s" for left siblings
                        xml::CXMLElement *pFragNode = new xml::CXMLElement(fragmentTag);
                        std::vector< std::pair<xml::CXMLNode*, int> >::reverse_iterator rit = ritParent-1;
                        rit->first->insertBefore(pFragNode);
                        for (; rit != ritCurrent; rit--)
                        {
                            rit->first->unlink();
                            pFragNode->linkLastChild(rit->first);
                        }
                        numFragments ++;
                    }

                    // update the fragment number of parent
                    ritParent->second += numFragments;

                    // pop up nodes (until parent) from "stack"
                    m_numFragments.resize(m_numFragments.size() - (ritParent - ritCurrent));

                    return ERROR_SUCCESS;
                }

                if (   (pNode->nextSibling() == NULL && ritParent->second > 0) // Condition 2
                    || (pNode->nextSibling() != NULL && isTerminator(pNode, fragmentTag)) ) // Condition 3
                {
                    //
                    // Condition 2:
                    //
                    // There is no right sibling, and
                    // there are already fragment(s) in the level of current node (i.e. parent.fragmentNum>0),
                    //  (1) current node and left sibling(s) in the "stack" (if any) should belong to one new fragment of "p" or "s",
                    //  (2) pop up current node and all left sibling(s) as they have been processed
                    //
                    // Examples (for sentence "s"):
                    //  (1) This is a pure text example
                    //      ==> <s> This is a pure text example </s>
                    //  (2) <s> This is the second </s> simple example
                    //      ==> <s> This is the second </s> <s> simple example </s>
                    //  (3) <s> This is </s> the third <emphasis> complicated example </emphasis>
                    //      ==> <s> This is </s> <s> the third <emphasis> complicated example </emphasis> </s>
                    //

                    //
                    // Condition 3:
                    //
                    // Current node is terminator for the fragment, and
                    // there are still right sibling(s),
                    //  (1) current node and left sibling(s) in the "stack" (if any) should belong to one new fragment of "p" or "s",
                    //  (2) pop up current node and all left sibling(s) as they have been processed
                    //
                    // Examples (for sentence "s"):
                    //  (1) This is part1. This is part2
                    //      ==> <s> This is part1 <say-as...>.</say-as> </s> <s> This is part2 </s>
                    //

                    // create "p"/"s" for current node and left siblings
                    xml::CXMLElement *pFragNode = new xml::CXMLElement(fragmentTag);
                    std::vector< std::pair<xml::CXMLNode*, int> >::reverse_iterator rit = ritParent-1;
                    rit->first->insertBefore(pFragNode);
                    for (; rit != ritCurrent; rit --)
                    {
                        rit->first->unlink();
                        pFragNode->linkLastChild(rit->first);
                    }
                    rit->first->unlink();
                    pFragNode->linkLastChild(rit->first);

                    // update the fragment number of parent
                    ritParent->second ++;

                    // pop up nodes (until parent) from "stack"
                    m_numFragments.resize(m_numFragments.size() - (ritParent - ritCurrent));

                    return ERROR_SUCCESS;
                }

                if (pNode->nextSibling() == NULL)
                {
                    //
                    // Condition 4:
                    //
                    // There is no right sibling
                    //  (1) pop up current node and all left sibling(s) as they have been processed
                    //
                    // Examples (for sentence "s"):
                    //  (1) This is <emphasis> an example. </emphasis>
                    //      ==> <s> This is <emphasis> an example <say-as...>.</say-as> </emphasis> </s>
                    //

                    // pop up nodes (until parent) from "stack"
                    m_numFragments.resize(m_numFragments.size() - (ritParent - ritCurrent));
                }
                return ERROR_SUCCESS;
            }

            bool CDocStruct::isTerminator(const xml::CXMLNode *pNode, const std::wstring &fragmentTag)
            {
                if (pNode->type() == xml::CXMLNode::XML_ELEMENT && pNode->value() == L"say-as")
                {
                    xml::CXMLElement *pSayAs = (xml::CXMLElement*)pNode;
                    if ( pSayAs->getAttribute(L"interpret-as") == L"punctuation" )
                    {
                        const std::wstring &format = pSayAs->getAttribute(L"format");
                        if (format == L"p")
                        {
                            // paragraph must be sentence
                            return true;
                        }
                        if (format == fragmentTag)
                        {
                            // for sentence
                            return true;
                        }
                    }
                }
                return false;
            }

        }//namespace base
    }
}
