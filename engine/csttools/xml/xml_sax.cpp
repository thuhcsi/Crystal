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
/// @brief  Implementation of SAX specification for XML (eXtensible Markup Language) manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   www.sourceforge.net/projects/tinyxml
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2006/05/01
///   Changed:  Re-organize and simplify the tinyXml to match our own requirement
/// - Version:  0.3.0
///   Author:   Dezhi HUANG, Yongxin WANG (fefe.wyx@gmail.com)
///   Date:     2006/07/27
///   Change:   Changed to std::wstring to support UNICODE directly
/// - Version:  0.3.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2010/11/10
///   Change:   Added forward class CElemStack to prevernt include <stack> <string> in head file
///

#include <vector>
#include <stack>
#include <string>
#include <cwctype>      // for isw**() functions
#include "xml_sax.h"
#include "cmn/cmn_string.h"

namespace cst
{
    namespace xml
    {
        //////////////////////////////////////////////////////////////////////////
        //
        //  CElemStack
        //
        //////////////////////////////////////////////////////////////////////////

        class CXMLSAXParser::CElemStack : public std::stack<std::wstring>
        {

        };


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLSAXParser
        //
        //////////////////////////////////////////////////////////////////////////

        CXMLSAXParser::CXMLSAXParser()
            : m_pElemStack(NULL), m_pDataReader(NULL), m_pDocHandler(NULL), m_bContAfterError(false) 
        {
        }

        CXMLSAXParser::~CXMLSAXParser()
        {
            delete m_pDataReader;
            delete m_pElemStack;
            m_pDataReader     = NULL;
            m_pElemStack      = NULL;
            m_pDocHandler     = NULL;
            m_bContAfterError = false;
        }

        void CXMLSAXParser::setHandler(CXMLSAXHandler* pHandler)
        {
            m_pDocHandler = pHandler;
        }

        void CXMLSAXParser::setContinueAfterError(bool bContinue)
        {
            m_bContAfterError = bContinue;
        }

        void CXMLSAXParser::parse(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer)
        {
            // reset the parser and reader
            if (!reset(pDataSource, eDataEncoding, bCreateBuffer))
            {
                // data reader or element stack create failed
                // do not continue
                emitError(ERR_OUTOF_MEMORY, L"Internal buffer initialization failed");
                return;
            }

            // invoke the startDocument call
            if (m_pDocHandler && !m_pDocHandler->startDocument())
            {
                emitError(ERR_FROM_USER, L"startDocument");
            }

            // loop to parse the data now
            bool  bGetData = true;
            bool bInMarkup = false;
            while (bGetData)
            {
                const ETokens eCurToken = senseNextToken();
                if (eCurToken == TOKEN_CHARDATA)
                {
                    // scan the character data and call appropriate events
                    scanCharData();
                    continue;
                }
                if (eCurToken == TOKEN_EOF)
                {
                    if (!m_pElemStack->empty())
                    {
                        emitError(ERR_ENDED_WITH_TAGS_NOT_PROCESSED, m_pElemStack->top().c_str());
                    }
                    // end of file, clear the get data flag
                    bGetData = false;
                    continue;
                }
                // we are in some sort of markup now
                bInMarkup = true;
                switch (eCurToken)
                {
                case TOKEN_COMMENT:     scanComment();      break;
                case TOKEN_DECLARATION: scanDeclaration();  break;
                case TOKEN_ENDTAG:      scanEndTag();       break;
                case TOKEN_STARTTAG:    scanStartTag();     break;
                default:                scanUnknownTag();   break;
                }
                bInMarkup = false;
            }

            // invoke the endDocument call
            if (m_pDocHandler && !m_pDocHandler->endDocument())
            {
                emitError(ERR_FROM_USER, L"endDocument");
            }
        }

        void CXMLSAXParser::scanCharData()
        {
            const wchar_t *pCharData = m_pDataReader->getChars();
            size_t        nDataStart = m_pDataReader->getCursor();

            m_pDataReader->readUntil(L"<", NULL, false);

            if (m_pDocHandler)
            {
                // notify the document handler
                std::wstring strChar(pCharData, m_pDataReader->getCursor()-nDataStart);
                bool bSucc = m_pDocHandler->characters(strChar.c_str());
                if (!bSucc)
                {
                    emitError(ERR_FROM_USER, L"characters");
                }
            }
        }

        void CXMLSAXParser::scanUnknownTag()
        {
            const wchar_t *pTagData = m_pDataReader->getChars();
            size_t       nDataStart = m_pDataReader->getCursor();

            if (!m_pDataReader->readUntil(L">", L"<", false))
            {
                // '>' must be met!
                emitError(ERR_PARSING_UNKNOWN, L"'>' expected");
                return;
            }

            if (m_pDocHandler)
            {
                // pass the string to the handler
                std::wstring strTag(pTagData, m_pDataReader->getCursor()-nDataStart);
                bool bSucc = m_pDocHandler->unknownTag(strTag.c_str());
                if (!bSucc)
                {
                    emitError(ERR_FROM_USER, L"unknownTag");
                }
                // skip ">"
                m_pDataReader->getChar();
            }
        }

        void CXMLSAXParser::scanComment()
        {
            const wchar_t *pTagData = m_pDataReader->getChars();
            size_t       nDataStart = m_pDataReader->getCursor();

            //if (!m_pDataReader->readUntil(L"-->", L"<", false))
            if (!m_pDataReader->readUntil(L"-->", NULL, false))
            {
                // we just look right until "-->"
                emitError(ERR_PARSING_COMMENT, L"'-->' expected");
                return;
            }

            if (m_pDocHandler)
            {
                // pass the string to the handler
                std::wstring strComment(pTagData, m_pDataReader->getCursor()-nDataStart);
                bool bSucc = m_pDocHandler->comment(strComment.c_str());
                if (!bSucc)
                {
                    emitError(ERR_FROM_USER, L"comment");
                }
                // skip "-->"
                m_pDataReader->peekString(L"-->", true);
            }
        }

        void CXMLSAXParser::scanDeclaration()
        {
            std::wstring strVersion = L"";
            std::wstring strEncoding = L"";
            std::wstring strStandalone = L"";
            const wchar_t *pAttPtr;
            size_t nValueLen;

            for (;;)
            {
                m_pDataReader->skipWhiteSpaces();
                // the end of declaration "?>", break loop
                if (m_pDataReader->peekString(L"?>", true))
                {
                    break;
                }
                pAttPtr = m_pDataReader->getChars();

                // version
                if (m_pDataReader->peekString(L"version", true))
                {
                    m_pDataReader->skipWhiteSpaces();
                    pAttPtr = senseAttribValue(ERR_PARSING_DECLARATION, nValueLen);
                    if (pAttPtr==NULL)
                        return;
                    strVersion = std::wstring(pAttPtr, nValueLen);
                    continue;
                }
                // encoding
                if (m_pDataReader->peekString(L"encoding", true))
                {
                    m_pDataReader->skipWhiteSpaces();
                    pAttPtr = senseAttribValue(ERR_PARSING_DECLARATION, nValueLen);
                    if (pAttPtr==NULL)
                        return;
                    strEncoding = std::wstring(pAttPtr, nValueLen);
                    continue;
                }
                // standalone
                if (m_pDataReader->peekString(L"standalone", true))
                {
                    m_pDataReader->skipWhiteSpaces();
                    pAttPtr = senseAttribValue(ERR_PARSING_DECLARATION, nValueLen);
                    if (pAttPtr==NULL)
                        return;
                    strStandalone = std::wstring(pAttPtr, nValueLen);
                    continue;
                }
                // error
                if (m_pDataReader->peekChar() == L'<')
                {
                    // "<" encountered
                    emitError(ERR_PARSING_DECLARATION, L"'?>' expected");
                    return;
                }
                else
                {
                    // unexpected attribute
                    emitError(ERR_PARSING_DECLARATION, L"Unexpected attribute in declaration");
                    return;
                }
            }

            if (m_pDocHandler)
            {
                // notify the handler: declaration
                bool bSucc = m_pDocHandler->declaration(strVersion.c_str(), strEncoding.c_str(), strStandalone.c_str());
                if (!bSucc)
                {
                    emitError(ERR_FROM_USER, L"declaration");
                }
            }
        }

        void CXMLSAXParser::scanStartTag()
        {
            // vecAttributes[2*i+1] is the value of the attribute name vecAttributes[2*i]
            // vecAttributes always have even number of elements when existing successfully
            std::wstring strElement;        // Element name
            std::vector<std::wstring> vecAttributes; // attribute name and value of the element
            vecAttributes.reserve(25*2);    // reserve space for 25 attributes, avoid time consuming allocation

            const wchar_t *pStrPtr = NULL;  // pointer to the begin of a string (element name, attribute name, or attribute value)            
            size_t nStartPos = 0;           // position of the begin of a string in the input data, used to calculate string length
            size_t nValueLen = 0;           // attribute value length

            // we should already get rid of the '<' as the begin of the element tag.
            // read the element name
            pStrPtr   = m_pDataReader->getChars();
            nStartPos = m_pDataReader->getCursor();
            while (m_pDataReader->isNameChar(m_pDataReader->peekChar()))
            {
                m_pDataReader->getChar();
            }
            strElement = std::wstring(pStrPtr, m_pDataReader->getCursor() - nStartPos);

            // check for and read attributes, also look for an empty tag or end tag
            m_pDataReader->skipWhiteSpaces();
            bool bEmptyElem = false;    // whether this is an empty tag

            for (wchar_t nxtchr; nxtchr = m_pDataReader->peekChar();)
            {
                if (nxtchr == L'/')
                {
                    //---- met '/', we have an empty tag ------------------------------
                    m_pDataReader->getChar();
                    // check for '>'
                    if (m_pDataReader->peekChar() != L'>')
                    {
                        emitError(ERR_PARSING_START_TAG, L"'/>' expected");
                        return;
                    }
                    // just set the empty element flag, process it in '>' of the next loop
                    bEmptyElem = true;
                }
                else if (nxtchr == L'>')
                {
                    //---- the end of the tag -----------------------------------------
                    // notify the handler: startElement and endElement if any
                    if (m_pDocHandler)
                    {
                        bool bSuccStart = true; // return of startElement
                        bool bSuccEnd = true;   // return of endElement, if called

                        // create the attribute list
                        size_t nAttribNum = vecAttributes.size();
                        const wchar_t **pAttributes = new const wchar_t*[nAttribNum];
                        for (size_t i=0; i<nAttribNum; i++)
                            pAttributes[i] = vecAttributes[i].c_str();

                        // notify the handler
                        bSuccStart = m_pDocHandler->startElement(strElement.c_str(), pAttributes, nAttribNum/2);

                        // delete the attribute list
                        delete []pAttributes;

                        if (bEmptyElem)
                        {
                            // this is an empty tag, end element should be called
                            bSuccEnd = m_pDocHandler->endElement(strElement.c_str());
                        }
                        else
                        {
                            // save to the Element stack!
                            m_pElemStack->push(strElement);
                        }

                        if (!bSuccStart)
                        {
                            emitError(ERR_FROM_USER, L"startElement");
                        }
                        if (!bSuccEnd)
                        {
                            emitError(ERR_FROM_USER, L"endElement");
                        }
                    }
                    // skip '>'
                    m_pDataReader->getChar();
                    //---- normal exit ---- normal exit ---- normal exit --------------
                    // we have parse the tag successfully.
                    return;
                }
                else
                {
                    //---- attribute name ---------------------------------------------
                    if (!m_pDataReader->isNameStart(m_pDataReader->peekChar()))
                    {
                        // illegal character at the start of tag
                        emitError(ERR_PARSING_START_TAG, L"Attribute name expected");
                        return;
                    }

                    // read attribute name
                    pStrPtr   = m_pDataReader->getChars();
                    nStartPos = m_pDataReader->getCursor();
                    while (m_pDataReader->isNameChar(m_pDataReader->peekChar()))
                    {
                        m_pDataReader->getChar();
                    }
                    vecAttributes.push_back(std::wstring(pStrPtr, m_pDataReader->getCursor() - nStartPos));

                    // read attribute value
                    m_pDataReader->skipWhiteSpaces();
                    pStrPtr = senseAttribValue(ERR_PARSING_START_TAG, nValueLen);
                    if (pStrPtr==NULL)
                    {
                        // error has been emitted in senseAttribValue
                        return;
                    }
                    vecAttributes.push_back(std::wstring(pStrPtr, nValueLen));

                    // check whether current attribute is already in the list
                    m_pDataReader->skipWhiteSpaces();
                    size_t AttNum = vecAttributes.size();
                    for (size_t i = 0; i < AttNum - 2; i += 2)
                    {
                        if (vecAttributes[i] == vecAttributes[AttNum-1])
                        {
                            emitError(ERR_PARSING_START_TAG, L"Multiple value for one attribute specified");
                            // not break, continue parsing
                        }
                    }
                }
            }

            // if goes here: unexpected EOF
            emitError(ERR_PARSING_START_TAG, L"Attribute, '>' or '/>' expected");
            return;
        }

        void CXMLSAXParser::scanEndTag()
        {
            // read the element name
            if (!m_pDataReader->isNameStart(m_pDataReader->peekChar()))
            {
                emitError(ERR_END_TAG_NAME_EXPECTED, m_pElemStack->top().c_str());
                return;
            }
            const wchar_t *pName = m_pDataReader->getChars();
            size_t nNameStartPos = m_pDataReader->getCursor();
            while (m_pDataReader->isNameChar(m_pDataReader->peekChar()))
            {
                m_pDataReader->getChar();
            }
            size_t nNameEndPos   = m_pDataReader->getCursor();

            // ">" expected
            m_pDataReader->skipWhiteSpaces();
            if (m_pDataReader->peekChar() != L'>')
            {
                emitError(ERR_PARSING_END_TAG, L"'>' expected");
                return;
            }
            m_pDataReader->getChar();

            // start tag matched?
            if (m_pElemStack->empty())
            {
                emitError(ERR_PARSING_END_TAG, L"End tag unexpected");
                return;
            }
            std::wstring strName(pName, nNameEndPos-nNameStartPos);
            if (strName != m_pElemStack->top())
            {
                emitError(ERR_END_TAG_NAME_EXPECTED, m_pElemStack->top().c_str());
                return;
            }

            // notify the handler
            if (m_pDocHandler)
            {
                bool bSucc = m_pDocHandler->endElement(strName.c_str());
                if (!bSucc)
                {
                    emitError(ERR_FROM_USER, L"endElement");
                }
            }
            // pop from the Element stack!
            m_pElemStack->pop();
        }

        const wchar_t *CXMLSAXParser::senseAttribValue(EErrorCodes eErrNo, size_t &nValueLen)
        {
            const wchar_t *pRetVal = NULL;
            size_t nStartPos = 0;
            size_t nEndPos = 0;

            // '=' between attribute name and value
            if (m_pDataReader->peekChar() != L'=')
            {
                emitError(eErrNo, L"'=' in attribute pairs expected");
                return NULL;
            }
            m_pDataReader->getChar(); // skip '='
            m_pDataReader->skipWhiteSpaces();

            // attribute value
            wchar_t chTempChar = m_pDataReader->peekChar();
            if (chTempChar == L'\'' || chTempChar == L'"')
            {
                // attribute value in single or double quotes
                m_pDataReader->getChar();
                pRetVal   = m_pDataReader->getChars();
                nStartPos = m_pDataReader->getCursor();

                // read until '\'' or '"' is met
                if (chTempChar == L'\'')
                    m_pDataReader->readUntil(L"\'", NULL, false);
                else
                    m_pDataReader->readUntil(L"\"", NULL, false);
                nEndPos = m_pDataReader->getCursor();

                // skip '\'' or '"'
                if (m_pDataReader->getChar()==0)
                {
                    emitError(eErrNo, (chTempChar == L'\'') ? L"'\'' expected" : L"'\"' expected");
                    return NULL;
                }
            }
            else
            {
                // all attribute values should be in single or double quotes.
                // but it is such a common error to forget quotes that parser will try its best, even without them.
                pRetVal   = m_pDataReader->getChars();
                nStartPos = m_pDataReader->getCursor();

                // read until white space or tag end ('/','>') is met
                while (chTempChar && !m_pDataReader->isWhiteSpace(chTempChar) && chTempChar != L'/' && chTempChar != L'>')
                {
                    m_pDataReader->getChar();
                    chTempChar = m_pDataReader->peekChar();
                }
                nEndPos = m_pDataReader->getCursor();
            }

            // set value information
            nValueLen = nEndPos - nStartPos;
            return pRetVal;
        }

        CXMLSAXParser::ETokens CXMLSAXParser::senseNextToken()
        {
            static const wchar_t *pXMLHeader = L"<?xml";
            static const wchar_t *pCmtHeader = L"<!--";
            static const wchar_t *pDTDHeader = L"<!";
            static const wchar_t *pStartTag  = L"<";
            static const wchar_t *pEndTag    = L"</";

            // we are going to skip white spaces.
            // they may have to be kept(in Raw Text) in character data, so save the current cursor
            size_t nOrgPos = m_pDataReader->getCursor();
            // skip white spaces
            m_pDataReader->skipWhiteSpaces();

            // check against each token
            wchar_t chNextChar = m_pDataReader->peekChar();
            if (chNextChar == 0)
            {
                // no more data, parsing end
                return TOKEN_EOF;
            }
            if (chNextChar != L'<')
            {
                // character data, keep the leading white spaces if any
                m_pDataReader->setCursor(nOrgPos);
                return TOKEN_CHARDATA;
            }

            // after parsing, cursor points to the main part of the token (skipping white space)
            if (m_pDataReader->peekString(pXMLHeader, true))
            {
                return TOKEN_DECLARATION;
            }
            if (m_pDataReader->peekString(pCmtHeader, true))
            {
                return TOKEN_COMMENT;
            }
            if (m_pDataReader->peekString(pEndTag, true))
            {
                return TOKEN_ENDTAG;
            }

            // element start tag must be 'Name' following '<'
            m_pDataReader->getChar(); // skip '<'
            chNextChar = m_pDataReader->peekChar();
            if (m_pDataReader->isNameStart(chNextChar))
            {
                return TOKEN_STARTTAG;
            }

            // unknown tag (including DTD tag)
            return TOKEN_UNKNOWN;
        }

        bool CXMLSAXParser::reset(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer)
        {
            if (m_pDocHandler)
            {
                if (!m_pDocHandler->resetDocument())
                    emitError(ERR_FROM_USER, L"resetDocument");
            }
            // internal data
            delete m_pDataReader;
            delete m_pElemStack;
            m_pDataReader = new CXMLReader(pDataSource, eDataEncoding, bCreateBuffer);
            m_pElemStack  = new CElemStack();
            if (m_pDataReader == NULL || m_pElemStack == NULL)
            {
                delete m_pDataReader;
                delete m_pElemStack;
                m_pDataReader = NULL;
                m_pElemStack = NULL;
                return false;
            }
            return true;
        }

        void CXMLSAXParser::emitError(EErrorCodes eErrNo, const wchar_t *pErrParam)
        {
            if (m_pDocHandler)
            {
                std::wstring errorMsg;
                switch (eErrNo)
                {
                case ERR_ENDED_WITH_TAGS_NOT_PROCESSED:
                    errorMsg = errorMsg + L"Expect end of tag: " + pErrParam;
                    break;
                case ERR_END_TAG_NAME_EXPECTED:
                    errorMsg = errorMsg + L"Expect end of tag: " + pErrParam;
                    break;
                case ERR_FROM_USER:
                    errorMsg = errorMsg + L"Error in user defined handler: " + pErrParam;
                    break;
                default:
                    errorMsg = pErrParam;
                    break;
                }
                m_pDocHandler->error(errorMsg.c_str(), m_pDataReader->getCursor());
            }
            if (!m_bContAfterError && m_pDataReader)
            {
                // not continue after error occurred
                // skip all the source data
                m_pDataReader->readUntil(L"", L"", true);
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  CXMLReader
        //
        //////////////////////////////////////////////////////////////////////////

        CXMLReader::CXMLReader(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer)
        {
            // save pointer to the source data buffer
            if (pDataSource == NULL)
            {
                wchar_t *pTempBuf = new wchar_t[1];
                pTempBuf[0]       = 0;
                m_pDataBuffer     = pTempBuf;
                m_nDataAvail      = 0;
                m_nDataCursor     = 0;
                m_bInternalBuffer = true;
            }
            else if (!bCreateBuffer)
            {
                m_pDataBuffer     = pDataSource;
                m_nDataAvail      = wcslen(pDataSource);
                m_nDataCursor     = 0;
                m_bInternalBuffer = false;
            }
            else
            {
                m_nDataAvail      = wcslen(pDataSource);
                wchar_t *pTempBuf = new wchar_t[m_nDataAvail+1];
                str::snwprintf(pTempBuf, m_nDataAvail, pDataSource);
                pTempBuf[m_nDataAvail] = 0;
                m_pDataBuffer     = pTempBuf;
                m_nDataCursor     = 0;
                m_bInternalBuffer = true;
            }

            // check for the UTF-16 leading byte (0xFE 0xFF)
            if (m_nDataAvail >= 1 && m_pDataBuffer[0] == L'\ufeff')
            {
                m_nDataCursor += 1;
            }
        }

        CXMLReader::~CXMLReader()
        {
            if (m_bInternalBuffer)
                delete []m_pDataBuffer;
        }

        const wchar_t* CXMLReader::getChars () const
        {
            return m_pDataBuffer + m_nDataCursor;
        }

        const size_t CXMLReader::getCursor() const
        {
            return m_nDataCursor;
        }

        bool CXMLReader::skipWhiteSpaces()
        {
            size_t orgpos = m_nDataCursor;
            while (m_nDataCursor < m_nDataAvail && isWhiteSpace(m_pDataBuffer[m_nDataCursor]))
                m_nDataCursor++;
            return (orgpos!=m_nDataCursor);
        }

        const wchar_t CXMLReader::peekChar() const
        {
            if (m_nDataCursor >= m_nDataAvail)
                return 0;
            return m_pDataBuffer[m_nDataCursor];
        }

        const wchar_t CXMLReader::getChar()
        {
            if (m_nDataCursor >= m_nDataAvail)
                return 0;
            return m_pDataBuffer[m_nDataCursor++];
        }

        bool CXMLReader::setCursor(size_t nNewPos)
        {
            if (nNewPos > m_nDataAvail)
                return false;
            m_nDataCursor = nNewPos;
            return true;
        }

        bool CXMLReader::peekString(const wchar_t *pToPeek, bool bSkip)
        {
            size_t nPeekLen = wcslen(pToPeek);
            // to see if the current reader has enough chars to test against this string
            if ((m_nDataAvail - m_nDataCursor) < nPeekLen)
            {
                return false;
            }

            // compare the string now (case insensitive)
            if (str::wcsnicmp(m_pDataBuffer + m_nDataCursor, pToPeek, nPeekLen) != 0)
            {
                return false;
            }

            // skip the matched string
            if (bSkip)
            {
                m_nDataCursor += nPeekLen;
            }

            return true;
        }

        bool CXMLReader::readUntil(const wchar_t *pToMatch, const wchar_t *pToBreak, bool bSkip)
        {
            size_t nMatchLen = wcslen(pToMatch);
            size_t nBrkLen   = pToBreak ? wcslen(pToBreak) : 0;

            bool bMatched = false; // pattern pToMatch matched?
            bool bBreaked = false; // pattern pToBreak matched?

            while (m_nDataAvail - m_nDataCursor >= nMatchLen)
            {
                bBreaked = (pToBreak != NULL) && (wcsncmp(m_pDataBuffer+m_nDataCursor, pToBreak, nBrkLen) == 0);
                if (bBreaked)
                {
                    break;
                }

                bMatched = (wcsncmp(m_pDataBuffer+m_nDataCursor, pToMatch, nMatchLen) == 0);
                if (bMatched)
                {
                    break;
                }

                m_nDataCursor++;
            }
            if (bMatched && bSkip)
            {
                // skip the matched pattern
                m_nDataCursor += nMatchLen;
            }
            if (!bMatched && !bBreaked)
            {
                // no pattern matched, put the cursor at the end of the input, as we should consume all the input.
                m_nDataCursor += (nMatchLen-1);
            }
            return bMatched;
        }

        bool CXMLReader::isWhiteSpace(wchar_t c)
        {
            return ((c>0) && (iswspace(c) || c==L'\n' || c==L'\r'));
        }

        bool CXMLReader::isNameStart(wchar_t c)
        {
            return ((c>0) && (iswalpha(c) || c==L'_'));
        }

        bool CXMLReader::isNameChar(wchar_t c)
        {
            return ((c>0) && (iswalnum(c) || c==L'.' || c==L'-' || c==L'_' || c==L':'));
        }

    }
}
