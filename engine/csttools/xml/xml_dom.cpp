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

/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
*/


///
/// @file
///
/// @brief  Implementation of DOM specification for XML (eXtensible Markup Language) manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   www.sourceforge.net/projects/tinyxml
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2006/05/01
///   Changed:  Re-organize and simplify the tinyXml to match our own reqirement
/// - Version:  0.3.0
///   Author:   Dezhi HUANG, Yongxin WANG (fefe.wyx@gmail.com)
///   Date:     2006/07/27
///   Change:   Changed to std::wstring to support UNICODE directly
/// - Version:  0.3.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2010/11/10
///   Change:   Added forward class CNodeStack to prevernt include <stack> in head file
///

#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include "xml_dom.h"
#include "cmn/cmn_textfile.h"

namespace cst
{
    namespace xml
    {
        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLDOMHandler: XML document and error handling
        //
        //////////////////////////////////////////////////////////////////////////

        class CXMLDOMHandler::CNodeStack : public std::stack<CXMLNode*>
        {

        };

        CXMLDOMHandler::CXMLDOMHandler(CXMLDocument* pDocument) : m_pDocument(pDocument), m_bHaveError(false)
        {
            m_pNodeStack = new CNodeStack();
        }

        CXMLDOMHandler::~CXMLDOMHandler()
        {
            delete m_pNodeStack;
            m_pNodeStack = NULL;
            m_pDocument  = NULL;
            m_bHaveError = false;
        }

        bool CXMLDOMHandler::startDocument()
        {
            // m_pDocument must NOT be NULL
            // It is user's responsibility to check the validation
            m_pDocument->clearChildren();
            m_bHaveError = false;
            // clear stack
            while (m_pNodeStack->size()>0)
            {
                m_pNodeStack->pop();
            }
            m_pNodeStack->push(m_pDocument);
            return true;
        }

        bool CXMLDOMHandler::endDocument()
        {
            if (m_pNodeStack->empty() || m_pNodeStack->top()->type()!=CXMLNode::XML_DOCUMENT)
                return false;
            return true;
        }

        bool CXMLDOMHandler::startElement(const wchar_t *pName, const wchar_t **pAttributes, size_t nAttrib)
        {
            CXMLElement* pElement = new CXMLElement(pName);
            for (size_t i=0; i<nAttrib; i++)
            {
                pElement->setAttribute(pAttributes[2*i], pAttributes[2*i+1]);
            }
            m_pNodeStack->top()->linkLastChild(pElement);
            m_pNodeStack->push(pElement);
            return true;
        }

        bool CXMLDOMHandler::endElement(const wchar_t *pName)
        {
            m_pNodeStack->pop();
            return true;
        }

        bool CXMLDOMHandler::characters(const wchar_t *pChars)
        {
            std::wstring strText = pChars;
            CXMLText::decodeEscapes(strText);
            CXMLText* pTextObj = new CXMLText(strText);
            m_pNodeStack->top()->linkLastChild(pTextObj);
            return true;
        }

        bool CXMLDOMHandler::comment(const wchar_t *pCmt)
        {
            CXMLComment* pComment = new CXMLComment(pCmt);
            m_pNodeStack->top()->linkLastChild(pComment);
            return true;
        }

        bool CXMLDOMHandler::declaration(const wchar_t *pVersion, const wchar_t* pEncoding, const wchar_t *pStandalone)
        {
            CXMLDeclaration* pDeclaration = new CXMLDeclaration(pVersion, pEncoding, pStandalone);
            m_pNodeStack->top()->linkLastChild(pDeclaration);
            return true;
        }

        bool CXMLDOMHandler::unknownTag(const wchar_t *pChars)
        {
            CXMLUnknown* pUnknown = new CXMLUnknown(pChars);
            m_pNodeStack->top()->linkLastChild(pUnknown);
            return true;
        }

        void CXMLDOMHandler::error(const wchar_t *pErrMsg, const size_t nCurPos)
        {
            m_bHaveError = true;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLNode
        //
        //////////////////////////////////////////////////////////////////////////

        void CXMLNode::clearChildren()
        {
            CXMLNode *node=m_pFstChild, *temp=NULL;
            while (node)
            {
                temp = node;
                node = node->m_pNext;
                delete temp;
            }
            m_pFstChild = NULL;
            m_pLstChild = NULL;
        }

        CXMLNode* CXMLNode::linkLastChild(CXMLNode *pNode)
        {
            pNode->m_pParent = this;
            pNode->m_pNext   = NULL;
            pNode->m_pPrev   = m_pLstChild;
            if (m_pLstChild)
                m_pLstChild->m_pNext = pNode;
            else
                m_pFstChild = pNode;    // empty list
            m_pLstChild = pNode;
            return pNode;
        }

        CXMLNode* CXMLNode::linkFirstChild(CXMLNode *pNode)
        {
            pNode->m_pParent = this;
            pNode->m_pPrev   = NULL;
            pNode->m_pNext   = m_pFstChild;
            if (m_pFstChild)
                m_pFstChild->m_pPrev = pNode;
            else
                m_pLstChild = pNode;    // empty list
            m_pFstChild = pNode;
            return pNode;
        }


        CXMLNode* CXMLNode::firstChild(const std::wstring &strValue) const
        {
            for (CXMLNode *pNode=m_pFstChild; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::firstChild(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const
        {
            for (CXMLNode *pNode=m_pFstChild; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_eNodeType == eNodeType && pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::firstChild(CXMLNode::ENodeType eNodeType) const
        {
            for (CXMLNode *pNode=m_pFstChild; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_eNodeType == eNodeType)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode* CXMLNode::lastChild(const std::wstring &strValue) const
        {
            for (CXMLNode *pNode=m_pLstChild; pNode; pNode=pNode->m_pPrev)
            {
                if (pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode* CXMLNode::nextSibling(const std::wstring &strValue) const
        {
            for (CXMLNode *pNode=m_pNext; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::nextSibling(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const
        {
            for (CXMLNode *pNode=m_pNext; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_eNodeType == eNodeType && pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::nextSibling(CXMLNode::ENodeType eNodeType) const
        {
            for (CXMLNode *pNode=m_pNext; pNode; pNode=pNode->m_pNext)
            {
                if (pNode->m_eNodeType == eNodeType)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::nextNode(CXMLNode::ENodeType eNodeType, const std::wstring &strValue, CXMLNode *pRootNode, bool bIgnoreCurrentNode, bool bIgnoreSubTree) const
        {
            const CXMLNode *pNode;
            /*
            //never enters a tree whose root is a say-as node
            if ( pRootNode->m_eNodeType ==CXMLNode::XML_ELEMENT && pRootNode->m_strValue ==L"say-as")
            {
            return NULL;
            }
            */
            //need to check the current node
            if ( !bIgnoreCurrentNode)
            {
                pNode =this;
            }
            //when the current node is ignored and the following three conditions are met, enter into the sub tree of the current node
            //1.firstchild exits
            //2.the current node is not a say-as node(we won't enter into the sub tree of a say-as node)
            //3.user specified not to ignore the subtree
            else if ( m_pFstChild != NULL && !((m_eNodeType ==CXMLNode::XML_ELEMENT) &&(m_strValue ==L"say-as"))&&!bIgnoreSubTree)
            {
                pNode = m_pFstChild;
            }
            //not enter into the subtree of current node, try to move to it's next sibling node 
            else if ( m_pNext != NULL)
            {
                pNode = m_pNext;
            }
            else
            {
                pNode = m_pParent;
                while ( pNode != NULL && pNode != pRootNode)
                {
                    if ( pNode->m_pNext != NULL)
                    {
                        pNode = pNode->m_pNext;
                        break;
                    }
                    else
                    {
                        pNode = pNode->m_pParent;
                    }
                }
            }
            while ( pNode != NULL && pNode != pRootNode)
            {
                if ( pNode->m_eNodeType == eNodeType && (strValue.empty()?true:(pNode->m_strValue == strValue)))
                {
                    return (CXMLNode*)pNode;
                }
                if (!(pNode->m_eNodeType == CXMLNode::XML_ELEMENT && pNode->m_strValue ==L"say-as") &&pNode->m_pFstChild != NULL)
                {
                    pNode = pNode->m_pFstChild;
                    continue;
                }
                //while ( pNode != NULL && (pNode->m_eNodeType != CXMLNode::XML_ELEMENT || pNode->m_strValue != L"s") && pNode != pRootNode)
                while ( pNode !=NULL && pNode !=pRootNode)
                {
                    if ( pNode->m_pNext != NULL)
                    {
                        pNode = pNode->m_pNext;
                        break;
                    }
                    else
                    {
                        pNode = pNode->m_pParent;
                    }
                }
            }
            return NULL;
        }

        CXMLNode *CXMLNode::prevNode(CXMLNode::ENodeType eNodeType, const std::wstring &strValue, CXMLNode *pRootNode) const
        {
            CXMLNode *pNode =NULL;
            CXMLNode *pAnchorNode =NULL;
            bool bFound =false;
            if ( pRootNode ==NULL || pRootNode->firstChild() ==NULL)
            {
                return NULL;
            }
            pNode =pRootNode->nextNode(eNodeType, strValue, pRootNode, true, false);
            while ( pNode !=NULL && pNode !=this && pNode !=this->nextNode(eNodeType,strValue,pRootNode,true,false))
            {
                pAnchorNode =pNode;
                pNode =pNode->nextNode(eNodeType, strValue, pRootNode, true, false);
            }
            return pAnchorNode;
        }
        /*
        CXMLNode *CXMLNode::prevNode(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const
        {
        CXMLNode *pNode;
        CXMLNode *pTemp1;
        CXMLNode *pTemp2;
        CXMLNode *pEndpos;
        if ( m_pPrev != NULL)
        {
        pNode = m_pPrev;
        }
        else
        {
        pNode = m_pParent;
        while ( pNode != NULL)
        {
        if ( pNode->m_eNodeType == eNodeType && (strValue.empty()?true:pNode->m_strValue == strValue))
        {
        return pNode;
        }
        else if ( pNode->m_pPrev != NULL)
        {
        pNode = pNode->m_pPrev;
        break;
        }
        else
        {
        pNode = pNode->m_pParent;
        }
        }
        }
        while ( pNode != NULL)
        {
        //retrieve the sub tree which the pNode point to 
        pEndpos = pNode->m_pNext;
        pTemp1 = NULL;
        pTemp2 = pNode->nextElement( strValue, eNodeType, pEndpos);
        while ( pTemp2 != NULL)
        {
        pTemp1 =pTemp2;
        pTemp2 =pTemp2->nextElement(strValue, eNodeType, pEndpos);
        }
        if ( pTemp1 != NULL)
        {
        return pTemp1;
        }
        else if ( pNode->m_eNodeType == eNodeType && pNode->m_strValue == strValue)
        {
        return pNode;
        }
        //if not found
        while (pNode != NULL )
        {
        if ( pNode->m_eNodeType == eNodeType && (strValue.empty()?true:pNode->m_strValue == strValue))
        {
        return pNode;
        }
        else if ( pNode->m_pPrev != NULL)
        {
        pNode = pNode->m_pPrev;
        break;
        }
        else
        {
        pNode = pNode->m_pParent;
        }
        }
        }
        return NULL;
        }
        */

        CXMLNode* CXMLNode::prevSibling(const std::wstring &strValue) const
        {
            for (CXMLNode* pNode=m_pPrev; pNode; pNode=pNode->m_pPrev)
            {
                if (pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        CXMLNode *CXMLNode::prevSibling(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const
        {
            for (CXMLNode *pNode=m_pPrev; pNode; pNode=pNode->m_pPrev)
            {
                if (pNode->m_eNodeType == eNodeType && pNode->m_strValue == strValue)
                    return pNode;
            }
            return NULL;
        }

        void CXMLNode::unlink()
        {
            CXMLNode *pParent = m_pParent;
            if (pParent != NULL)
            {
                if (pParent->m_pFstChild == this)
                    pParent->m_pFstChild = m_pNext;
                if (pParent->m_pLstChild == this)
                    pParent->m_pLstChild = m_pPrev;
            }
            if (m_pPrev != NULL)
            {
                m_pPrev->m_pNext = m_pNext;
            }
            if (m_pNext != NULL)
            {
                m_pNext->m_pPrev = m_pPrev;
            }

            // Clear
            m_pParent = NULL;
            m_pPrev = NULL;
            m_pNext = NULL;
        }

        void CXMLNode::remove(CXMLNode *pNode)
        {
            pNode->unlink();
            delete pNode;
        }

        void CXMLNode::insertBefore(CXMLNode *pOrphanNode)
        {
            pOrphanNode->m_pParent = m_pParent;
            if (m_pParent != NULL && m_pParent->m_pFstChild == this)
            {
                m_pParent->m_pFstChild = pOrphanNode;
            }

            if (m_pPrev != NULL)
            {
                m_pPrev->m_pNext = pOrphanNode;
            }
            pOrphanNode->m_pPrev = m_pPrev;

            m_pPrev = pOrphanNode;
            pOrphanNode->m_pNext = this;
        }

        //void CXMLNode::insertBefore(CXMLNode *pNode)
        //{
        //    m_pParent = pNode->m_pParent;
        //    if (m_pParent != NULL && m_pParent->m_pLstChild == pNode)
        //    {
        //        m_pParent->m_pLstChild = this;
        //    }

        //    m_pNext = pNode->m_pNext;
        //    pNode->m_pNext = this;

        //    if (m_pNext != NULL)
        //    {
        //        m_pNext->m_pPrev = this;
        //    }
        //    m_pPrev = pNode;
        //}

        void CXMLNode::insertAfter(CXMLNode *pOrphanNode)
        {
            pOrphanNode->m_pParent = m_pParent;
            if (m_pParent != NULL && m_pParent->m_pLstChild == this)
            {
                m_pParent->m_pLstChild = pOrphanNode;
            }

            if (m_pNext != NULL)
            {
                m_pNext->m_pPrev = pOrphanNode;
            }
            pOrphanNode->m_pNext = m_pNext;

            m_pNext = pOrphanNode;
            pOrphanNode->m_pPrev = this;
        }

        //void CXMLNode::insertAfter(CXMLNode *pNode)
        //{
        //    m_pParent = pNode->m_pParent;
        //    if (m_pParent != 0 && m_pParent->m_pFstChild == pNode)
        //    {
        //        m_pParent->m_pFstChild = this;
        //    }
        //    m_pPrev = pNode->m_pPrev;
        //    pNode->m_pPrev = this;
        //    if (m_pPrev)
        //    {
        //        m_pPrev->m_pNext = this;
        //    }
        //    m_pNext = pNode;
        //}

        void CXMLNode::insertAsParent(CXMLNode *pOrphanNode)
        {
            // insert this
            pOrphanNode->m_pParent = m_pParent;
            pOrphanNode->m_pPrev = m_pPrev;
            pOrphanNode->m_pNext = m_pNext;

            pOrphanNode->m_pFstChild = this;
            pOrphanNode->m_pLstChild = this;

            // modifications on the original tree
            if (pOrphanNode->m_pParent)
            {
                if (pOrphanNode->m_pParent->m_pFstChild == this)
                {
                    pOrphanNode->m_pParent->m_pFstChild = pOrphanNode;
                }
                if (pOrphanNode->m_pParent->m_pLstChild == this)
                {
                    pOrphanNode->m_pParent->m_pLstChild = pOrphanNode;
                }
            }
            if (pOrphanNode->m_pPrev && pOrphanNode->m_pPrev->m_pNext == this)
            {
                pOrphanNode->m_pPrev->m_pNext = pOrphanNode;
            }
            if (pOrphanNode->m_pNext && pOrphanNode->m_pNext->m_pPrev == this)
            {
                pOrphanNode->m_pNext->m_pPrev = pOrphanNode;
            }

            // modification on pXMLNode
            m_pParent = pOrphanNode;
            m_pPrev = 0;
            m_pNext = 0;
        }

        bool CXMLNode::isCertainElement(std::wstring wstrElementName) const
        {
            return ( (type()==XML_ELEMENT) && (value()==wstrElementName) );
        }

        CXMLNode *CXMLNode::setAsLastChild()
        {
            if ( m_pParent !=NULL)
            {
                CXMLNode *pNextNode;
                m_pParent->m_pLstChild =this;
                pNextNode =m_pNext;
                m_pNext =NULL;
                return pNextNode;
            }
            return NULL;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLDeclaration & CXMLComment & CXMLUnknown & CXMLText
        //
        //////////////////////////////////////////////////////////////////////////

        bool CXMLDeclaration::print(std::wostream &outStream, int nDepth) const
        {
            for (int i=0; i<nDepth; i++)
                outStream << L"    ";
            // declaration
            outStream << L"<?xml ";
            if (m_strVersion.length()!=0)
            {
                outStream << L"version=\"" << m_strVersion << L"\" ";
            }
            if (m_strEncoding.length()!=0)
            {
                outStream << L"encoding=\"" << m_strEncoding << L"\" ";
            }
            if (m_strStandalone.length()!=0)
            {
                outStream << L"standalone=\"" << m_strStandalone << L"\" ";
            }
            outStream << L"?>";
            return true;
        }

        bool CXMLComment::print(std::wostream &outStream, int nDepth) const
        {
            for (int i=0; i<nDepth; i++)
                outStream << L"    ";
            // comment
            outStream << L"<!--" << m_strValue << L"-->";
            return true;
        }

        bool CXMLUnknown::print(std::wostream &outStream, int nDepth) const
        {
            for (int i=0; i<nDepth; i++)
                outStream << L"    ";
            // unknown
            outStream << L"<" << m_strValue << L">";
            return true;
        }

        bool CXMLText::print(std::wostream &outStream, int nDepth) const
        {
            // text
            std::wstring tmpValue(m_strValue);
            encodeEscapes(tmpValue);
            outStream << tmpValue;
            return true;
        }

        void CXMLText::encodeEscapes(std::wstring &strText)
        {
            // encode "&, >, <" into "&amp; &gt; &lt;"
            str::replace(strText, L"&", L"&amp;");
            str::replace(strText, L">", L"&gt;");
            str::replace(strText, L"<", L"&lt;");
        }

        void CXMLText::decodeEscapes(std::wstring &strText)
        {
            // decode "&amp; &lt; &gt;" into "&, <, >"
            str::replace(strText, L"&lt;", L"<");
            str::replace(strText, L"&gt;", L">");
            str::replace(strText, L"&amp;", L"&");
        }

        bool CXMLText::isBlank() const
        {
            for (size_t i=0; i <m_strValue.length(); i++)
            {
                if (!CXMLReader::isWhiteSpace(m_strValue[i]))
                    return false;
            }
            return true;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLAttribute & CDOMAttribList
        //
        //////////////////////////////////////////////////////////////////////////

        CXMLAttribute::CXMLAttribute(const std::wstring &strName, const std::wstring &strValue)
        {
            m_pNext    = NULL;
            m_pPrev    = NULL;
            m_strName  = strName;
            m_strValue = strValue;
        }

        bool CXMLAttribute::print(std::wostream &outStream, int nDepth) const
        {
            // attribute
            if (m_strValue.find(L'\"') == m_strValue.npos)
            {
                outStream << m_strName << L"=\"" << m_strValue << L"\"";
            }
            else
            {
                outStream << m_strName << L"=\'" << m_strValue << L"\'";
            }
            return true;
        }

        bool CXMLAttribList::print(std::wostream &outStream, int nDepth) const
        {
            // attribute list
            // print attribute one by one
            for (CXMLAttribute *pNode=m_sentinel.m_pNext; pNode!=&m_sentinel; pNode=pNode->m_pNext)
            {
                outStream << L" ";
                pNode->print(outStream, nDepth);
            }
            return true;
        }

        CXMLAttribute* CXMLAttribList::find(const std::wstring &strName) const
        {
            for (CXMLAttribute *pNode=m_sentinel.m_pNext; pNode!=&m_sentinel; pNode=pNode->m_pNext)
            {
                if (pNode->m_strName==strName)
                {
                    return pNode;
                }
            }
            return NULL;
        }

        bool CXMLAttribList::add(CXMLAttribute* pAttrib)
        {
            if (find(pAttrib->m_strName.c_str())!=NULL)    // Should not be added multiply
                return false;
            pAttrib->m_pNext = &m_sentinel;
            pAttrib->m_pPrev = m_sentinel.m_pPrev;
            m_sentinel.m_pPrev->m_pNext = pAttrib;
            m_sentinel.m_pPrev          = pAttrib;
            return true;
        }

        bool CXMLAttribList::remove(CXMLAttribute* pAttrib)
        {
            for (CXMLAttribute *pNode=m_sentinel.m_pNext; pNode!=&m_sentinel; pNode=pNode->m_pNext)
            {
                if (pNode==pAttrib)
                {
                    pNode->m_pPrev->m_pNext = pNode->m_pNext;
                    pNode->m_pNext->m_pPrev = pNode->m_pPrev;
                    pNode->m_pNext = NULL;
                    pNode->m_pPrev = NULL;
                    return true;
                }
            }
            // tried to remove a non-linked attribute
            return false;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLElement
        //
        //////////////////////////////////////////////////////////////////////////

        CXMLElement::~CXMLElement()
        {
            // Do not clear children here, because CXMLNode has done this!
            // clearChildren();
            CXMLAttribute *pNode = NULL;
            while ((pNode = m_attribList.first()) != NULL)
            {
                m_attribList.remove(pNode);
                delete pNode;
            }
        }

        bool CXMLElement::print(std::wostream &outStream, int nDepth) const
        {
            for (int i=0; i<nDepth; i++)
                outStream << L"    ";
            // element start
            outStream << L"<" << m_strValue;
            // attributes
            m_attribList.print(outStream, nDepth);
            // There are 3 different formatting approaches:
            // 1) An element without children is printed as a <foo /> node
            // 2) An element with only a text child is printed as <foo> text </foo>
            // 3) An element with children is printed on multiple lines.
            if (m_pFstChild == NULL)
            {
                outStream << L" />";
            }
            else if (m_pFstChild==m_pLstChild && m_pFstChild->type()==XML_TEXT)
            {
                outStream << L">";
                m_pFstChild->print(outStream, nDepth+1);
                outStream << L"</" << m_strValue << L">";
            }
            else
            {
                outStream << L">";
                for (CXMLNode* pNode=m_pFstChild; pNode; pNode=pNode->nextSibling())
                {
                    if (pNode->type()!=XML_TEXT)
                        outStream << L"\r\n";
                    pNode->print(outStream, nDepth+1);
                }
                outStream << L"\r\n";
                for (int i=0; i<nDepth; i++)
                    outStream << L"    ";
                outStream << L"</" << m_strValue << L">";
            }
            return true;
        }

        bool CXMLElement::setAttribute(const std::wstring &strName, const std::wstring &strValue)
        {
            CXMLAttribute* node = m_attribList.find(strName);
            if (node!=NULL)
            {
                // attribute exist, update
                node->setValue(strValue);
                return true;
            }
            // create new attribute
            CXMLAttribute* attrib = new CXMLAttribute(strName, strValue);
            if (attrib!=NULL)
            {
                m_attribList.add(attrib);
            }
            return (attrib!=NULL);
        }

        void CXMLElement::removeAttribute(const std::wstring &strName)
        {
            CXMLAttribute* node = m_attribList.find(strName);
            if (node!=NULL)
            {
                m_attribList.remove(node);
                delete node;
            }
        }

        const std::wstring& CXMLElement::getAttribute(const std::wstring &strName) const
        {
            const static std::wstring wstrEmpty; // empty return value
            CXMLAttribute* node = m_attribList.find(strName);
            if (node!=NULL)
                return node->value();
            return wstrEmpty;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLDocument
        //
        //////////////////////////////////////////////////////////////////////////

        bool CXMLDocument::parse(const wchar_t *pDataSource, bool bCreateNewBuf, EXMLEncoding eDataEncoding)
        {
            if (eDataEncoding != XML_ENCODING_UTF16)
                return false;

            // parse the data
            CXMLDOMHandler handler(this);
            CXMLSAXParser  parser;
            parser.setHandler(&handler);
            parser.parse(pDataSource, eDataEncoding, bCreateNewBuf);
            m_bHaveError = handler.haveError();
            return !m_bHaveError;
        }

        bool CXMLDocument::load(const wchar_t *strFileName, str::EEncoding eEncoding)
        {
            // load the data from text file
            std::wstring buf;
            cmn::CTextFile fp;
            if (!fp.open(strFileName, L"rb", eEncoding))
            {
                return false;
            }
            fp.readString(buf);

            // parse now
            bool succ = parse(buf.c_str(), false, XML_ENCODING_UTF16);
            return succ;
        }

        bool CXMLDocument::save(const wchar_t *strFileName, str::EEncoding eEncoding) const
        {
            // store xml data to string buffer
            std::wostringstream stream;
            if (!print(stream, 0))
            {
                return false;
            }

            // save the data to text file
            cmn::CTextFile fp;
            if (!fp.open(strFileName, L"wb", eEncoding))
            {
                return false;
            }
            fp.writeString(stream.str().c_str());
            return true;
        }

        bool CXMLDocument::print(std::wostream &outStream, int nDepth) const
        {
            for (CXMLNode* pNode=m_pFstChild; pNode; pNode=pNode->nextSibling())
            {
                pNode->print(outStream, nDepth);
                outStream << L"\r\n";
            }
            return true;
        }

        bool CXMLDocument::print() const
        {
            return print(std::wcout, 0);
        }

    }
}
