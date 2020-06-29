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
/// @brief  Definition of DOM specification for XML (eXtensible Markup Language) manipulation.
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


#ifndef _CST_TOOLS_XML_DOM_H_
#define _CST_TOOLS_XML_DOM_H_

#include <string>
#include "xml_sax.h"
#include "utils/utl_string.h"

namespace cst
{
    namespace xml
    {
        ///
        /// @brief  A base class for XML DOM specification
        ///
        /// The class is an implementation of the XML DOM specification.
        /// DOM is the Document Object Model (http://www.w3.org/TR/).
        ///
        /// In XML, the document and elements can contain other elements and other types of nodes.
        /// @verbatim
        ///     A Document can contain: Declaration (leaf)
        ///                             Comment     (leaf)
        ///                             Unknown     (leaf)
        ///                             Element     (container or leaf)
        ///     An Element can contain: Element     (container or leaf)
        ///                             Text        (leaf)
        ///                             Attributes  (not on tree)
        ///                             Comment     (leaf)
        ///                             Unknown     (leaf)
        ///     A Declaration contains: Attributes  (not on tree)
        /// @endverbatim
        ///
        class CXMLBase
        {
        public:
            ///
            /// @brief  Constructor
            ///
            CXMLBase() {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLBase() {}

        public:
            ///
            /// @brief  All XML DOM classes can print themselves to a stream.
            ///
            /// This is a formatted print, and will insert indent tabs and newlines.
            ///
            /// @param  [in] outStream  Stream to write to
            /// @param  [in] nDepth     Depth level (used for indent in formatted print)
            ///
            /// @return Whether procedure is successful or not
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const = 0;

            ///
            /// @brief  Used to print the debug information inside a XML document/node
            ///
            /// @param  [in] outStream  Stream to write to
            ///
            /// @note   Debug use only, do not call this function for general use
            ///
            virtual bool debug(std::wostream &outStream) const {return true;}
        };


        ///
        /// @brief  The XML attribute
        ///
        /// An attribute is a name-value pair.
        /// XML elements have an arbitrary number of attributes, each with a unique name.
        ///
        /// @note   The attributes are not CXMLNodes, since they are not part of XML DOM specification.
        ///
        class CXMLAttribute : public CXMLBase
        {
            friend class CXMLAttribList;
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] strName    Name of the attribute
            /// @param  [in] strValue   Value of the attribute
            ///
            CXMLAttribute(const std::wstring &strName, const std::wstring &strValue);

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLAttribute() {}

            ///
            /// @brief  Get the attribute name
            ///
            const std::wstring &name() const {return m_strName;}

            ///
            /// @brief  Get the attribute value
            ///
            const std::wstring &value() const {return m_strValue;}

            ///
            /// @brief  Set the attribute name
            ///
            void setName(const std::wstring &strName) {m_strName=strName;}

            ///
            /// @brief  Set the attribute value
            ///
            /// @note   The function won't change &quot; to '"'
            ///
            virtual void setValue(const std::wstring &strValue) {m_strValue=strValue;}

            ///
            /// @brief  Get next attribute in attribute list, NULL at end
            ///
            /// This is used in CXMLAttribList that has a circular implementation. The attribute
            /// with name of "" (string of length zero) is both boundaries as beginning and ending.
            ///
            /// @return Pointer to next attribute, NULL for end of list
            ///
            CXMLAttribute* next() const {if (m_pNext->m_strName.length()==0) return NULL; return m_pNext;}

            ///
            /// @brief  Get previous attribute in attribute list, NULL at beginning
            ///
            /// This is used in CXMLAttribList that has a circular implementation. The attribute
            /// with name of "" (string of length zero) is both boundaries as beginning and ending.
            ///
            /// @return Pointer to previous attribute, NULL for beginning of list
            ///
            inline CXMLAttribute* prev() const {if (m_pPrev->m_strName.length()==0) return NULL; return m_pPrev;}

            ///
            /// @brief  Print the attribute
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;

        protected:
            std::wstring    m_strName;  ///< Attribute name
            std::wstring    m_strValue; ///< Attribute value
            CXMLAttribute  *m_pNext;    ///< Next attribute
            CXMLAttribute  *m_pPrev;    ///< Previous attribute
        };


        ///
        /// @brief  The class to manage a list of attributes
        ///
        /// The list is implemented in circular list for easier boundary checking
        ///
        class CXMLAttribList : public CXMLBase
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// Initialize the Sentinel element as the base element in the circular list
            ///
            CXMLAttribList() : m_sentinel(L"",L"sentinel") {m_sentinel.m_pNext=&m_sentinel; m_sentinel.m_pPrev=&m_sentinel;}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLAttribList() {}

            ///
            /// @brief  Find a specific attribute in the list with attribute name
            ///
            /// @param  [in] strName  Name of the attribute to be searched
            ///
            /// @return Whether attribute is found. NULL if not found.
            ///
            CXMLAttribute* find(const std::wstring &strName) const;

            ///
            /// @brief  Add a new attribute to the list
            ///
            /// @param  [in] pAttrib    Attribute to be added to the list
            ///
            /// @return Whether the operation is successful
            ///
            bool add(CXMLAttribute *pAttrib);

            ///
            /// @brief If the attribute is in the list, remove it. But the attribute itself is not deleted.
            ///
            /// @param [in] pAttrib     The attribute to be removed from the attribute list
            ///
            /// @return Whether it is removed from list
            /// @note   The removed attribute (pAttrib) itself is not destroyed!
            ///
            bool remove(CXMLAttribute *pAttrib);

            ///
            /// @brief  Get first attribute in the list, NULL for empty list
            ///
            CXMLAttribute* first() const {return (m_sentinel.m_pNext==&m_sentinel) ? NULL : m_sentinel.m_pNext;}

            ///
            /// @brief  Get last attribute in the list, NULL for empty list
            ///
            CXMLAttribute* last() const {return (m_sentinel.m_pPrev==&m_sentinel) ? NULL : m_sentinel.m_pPrev;}

            ///
            /// @brief  Print the attribute list, by printing each attribute one by one
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;

        protected:
            CXMLAttribute m_sentinel;  ///< The list sentinel
        };


        ///
        /// @brief  The parent class for everything in the XML DOM implementation (Except for Attribute).
        ///
        /// Each node has siblings, a parent, and children.
        ///
        /// A node can be in a document, or stand on its own.
        ///
        /// The type of the node can be queried, and it can be cast to its derived type.
        /// (Please enable RTTI and use dynamic_cast when you want to do this kind of cast).
        ///
        class CXMLNode : public CXMLBase
        {
        public:
            /// The types of supported XML nodes. (All the unsupported types are picked by UNKNOWN)
            enum ENodeType
            {
                XML_DOCUMENT,    ///< root node of the document
                XML_ELEMENT,     ///< node begin with <..> and ends with </..>
                XML_COMMENT,     ///< comment, <!-- ... -->
                XML_UNKNOWN,     ///< unknown node type
                XML_TEXT,        ///< raw text
                XML_DECLARATION  ///< declaration node <?xml ...?>
            };

        protected:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] eType      The node type
            /// @param  [in] strValue   Node value, has different meaning in different type of node
            ///
            CXMLNode(ENodeType eType, const std::wstring &strValue) : m_eNodeType(eType), m_strValue(strValue)
            {
                m_pParent = m_pFstChild = m_pLstChild = m_pNext = m_pPrev = NULL;
            }

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLNode() {clearChildren();}

        protected:
            std::wstring  m_strValue;   ///< The value of the XML node
            ENodeType     m_eNodeType;  ///< Type of this node
            CXMLNode     *m_pParent;    ///< Parent node
            CXMLNode     *m_pFstChild;  ///< First child
            CXMLNode     *m_pLstChild;  ///< Last child
            CXMLNode     *m_pNext;      ///< Next sibling
            CXMLNode     *m_pPrev;      ///< Previous sibling

        public:
            ///
            /// @brief  Get node type
            ///
            ENodeType type() const {return m_eNodeType;}

            ///
            /// @brief  Get node value
            ///
            const std::wstring &value() const {return m_strValue;}

            ///
            /// @brief  Set node value
            ///
            virtual void setValue(const std::wstring &strValue) {m_strValue = strValue;}

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  The following procedures manipulate myself
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Unlink (remove) this node and all its children from the node tree.
            ///
            /// This node and all children will be unlinked from the node tree.
            /// The content of this node and all children is kept unchanged.
            ///
            void unlink();

            ///
            /// @brief  Unlink (remove) this node and all children from the node tree.
            ///         And the node would be deleted (memory released);
            ///
            /// This node and all children will be unlinked from the node tree, and the memory would be released.
            ///
            static void remove(CXMLNode *pNode);

            ///
            /// @brief  Insert this node after the node pNode. This node will be the next sibling of pNode.
            ///
            /// @param  [in] pOrphanNode  The node to be inserted after.
            ///
            /// @note   This node must be allocated using operator new
            ///
            void insertBefore(CXMLNode *pOrphanNode);// insert pOrphanNode after before me

            ///
            /// @brief  Insert this node before the node pNode. This node will be the prev sibling of pNode.
            ///
            /// @param  [in] pOrphanNode  The node to be inserted before.
            ///
            /// @note   This node must be allocated using operator new 
            ///
            void insertAfter(CXMLNode *pOrphanNode);// insert pOrphanNode after me

            /// @brief Insert this node as the parent of the node pXMLNode, and retain the sibling relationship of pNode
            ///
            /// @param  [in] pOrphanNode
            ///
            /// @note   This node must be allocated using operator new 
            ///
            void insertAsParent(CXMLNode *pOrphanNode);// insert pOrphanNode as my parent, and retain the sibling (to pOrphanNode) and child (remains me)

            //////////////////////////////////////////////////////////////////////////
            //
            //  The following procedures manipulate the child
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Delete all the children of this node. Does not affect 'this'.
            ///
            /// @note   Does not affect 'this'
            ///
            void clearChildren();

            ///
            /// @brief  Add a new child node as the last child.
            ///
            /// The node to be added is passed by pointer, and will be henceforth owned (and deleted) by this node.
            /// This method is efficient, and avoids an extra copy, but should be used with care.
            ///
            /// @param  [in] pNode  The node to be inserted. This pointer MUST be allocated using operator new.
            ///
            /// @return The inserted node
            ///
            /// @note   The parameter pNode must be allocated using operator new
            ///
            CXMLNode* linkLastChild(CXMLNode *pNode);// link pNode as my last child

            CXMLNode * linkFirstChild(CXMLNode * pNode);// link pNode as my first child

        public:
            /// Parent, or NULL if no parent
            CXMLNode* parent() const {return m_pParent;}

            /// Next sibling, or NULL if no next sibling
            CXMLNode* nextSibling() const {return m_pNext;}

            /// Previous sibling, or NULL if no previous sibling
            CXMLNode* prevSibling() const {return m_pPrev;}

            /// First child, or NULL if no any child
            CXMLNode* firstChild() const {return m_pFstChild;}

            /// Last child, or NULL if no any child
            CXMLNode* lastChild() const {return m_pLstChild;}

            /// First child which matches the value, or NULL if no matched
            CXMLNode* firstChild(const std::wstring &strValue) const;
            CXMLNode *firstChild(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const;
            CXMLNode *firstChild(CXMLNode::ENodeType eNodeType) const;

            /// Last child which matches the value, or NULL if no matched
            CXMLNode* lastChild(const std::wstring &strValue) const;

            /// Next sibling which matches the value, or NULL if no matched
            CXMLNode* nextSibling(const std::wstring &strValue) const;
            CXMLNode *nextSibling(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const;
            CXMLNode *nextSibling(CXMLNode::ENodeType eNodeType) const;
            CXMLNode *nextNode(CXMLNode::ENodeType eNodeType, const std::wstring &strValue, CXMLNode *pRootNode, bool bIgnoreCurrentNode, bool bIgnoreSubTree) const;

            /// Previous sibling which matches the value, or NULL of no matched
            CXMLNode* prevSibling(const std::wstring &strValue) const;
            CXMLNode *prevSibling(const std::wstring &strValue, CXMLNode::ENodeType eNodeType) const;
            CXMLNode *prevNode(CXMLNode::ENodeType eNodeType, const std::wstring &strValue, CXMLNode *pRootNode) const;

        public:
            ///@todo inline? (Linux warning)
            virtual bool isCertainElement(std::wstring wstrElementName) const;

            ///@todo to be removed!
            CXMLNode *setAsLastChild();

        };


        ///
        /// @brief  The XML element
        ///
        /// The element is a container. It has a value (the element name), and can
        /// contain other elements, text, comments, and unknowns.
        /// Element can also contain an arbitrary number of attributes.
        ///
        class CXMLElement : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] strName    The element name (node value)
            ///
            CXMLElement(const std::wstring &strName) : CXMLNode(XML_ELEMENT, strName) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLElement();

            ///
            /// @brief  Set the attribute of name to a given value.
            ///         The attribute will be created if it does not exist, or updated if it does.
            ///
            /// @param  [in] strName    Attribute name
            /// @param  [in] strValue   Attribute value
            ///
            /// @return Whether the operations is successful
            ///
            bool setAttribute(const std::wstring &strName, const std::wstring &strValue);

            ///
            /// @brief  Remove the attribute with the specified name. Do nothing if it does not exist
            ///
            /// @param  [in] strName    Name of the attribute to delete
            ///
            void removeAttribute(const std::wstring &strName);

            ///
            /// @brief  Get the attribute value given the name, or "" if not found
            ///
            /// @param  [in] strName    The name of the attribute to query
            ///
            /// @return The attribute value. Empty string ("") is return if the given attribute name not found.
            ///
            const std::wstring &getAttribute(const std::wstring &strName) const;

        public:
            ///
            /// @brief  Get the first attribute
            ///
            CXMLAttribute* firstAttribute() const {return m_attribList.first();}

            ///
            /// @brief  Get the last attribute
            ///
            CXMLAttribute* lastAttribute() const {return m_attribList.last();}

            ///
            /// @brief  Print the element
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;

        protected:
            CXMLAttribList m_attribList; ///< The attribute list
        };


        ///
        /// @brief  The XML declaration (without leading "<?xml" and ending "?>")
        ///
        /// In well formated XML, the declaration is the first entry in the file.
        /// @verbatim
        ///     <?xml version = "1.0" encoding="UTF-16" standalone = "yes"?>
        /// @endverbatim
        /// However, this implementation will happily read or write files without declaration.
        ///
        /// There are 3 possible attributes to the declaration: "version", "encoding", and "standalone".
        ///
        class CXMLDeclaration : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] strVersion     Version attribute value
            /// @param  [in] strEncoding    Encoding attribute value
            /// @param  [in] strStandalone  Standalone attribute value
            ///
            CXMLDeclaration(const std::wstring &strVersion, const std::wstring &strEncoding, const std::wstring &strStandalone) : CXMLNode(XML_DECLARATION, L"")
            {
                m_strVersion = strVersion; m_strEncoding = strEncoding; m_strStandalone = strStandalone;
            }

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLDeclaration() {}

            ///
            /// @brief  XML version
            ///
            const std::wstring &version() const {return m_strVersion;}

            ///
            /// @brief  XML character encoding
            ///
            const std::wstring &encoding() const {return m_strEncoding;}

            ///
            /// @brief  XML declaration standalone
            ///
            const std::wstring &standalone() const {return m_strStandalone;}

            ///
            /// @brief  Print the declaration
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;

        protected:
            std::wstring m_strVersion;     ///< version
            std::wstring m_strEncoding;    ///< encoding
            std::wstring m_strStandalone;  ///< standalone
        };


        ///
        /// @brief  The XML comment (without leading "<--" and ending "-->")
        ///
        class CXMLComment : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] strComment Comment content (node value) (without leading "<--" and ending "-->")
            ///
            CXMLComment(const std::wstring &strComment) : CXMLNode(XML_COMMENT, strComment) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLComment() {}

            ///
            /// @brief  Print the comment
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;
        };


        ///
        /// @brief  Any tag that this implementation does not recognized is an unknown (without leading "<" and ending ">")
        ///
        /// An unknown is a tag of text, and should NOT be modified.
        /// It will be written back to the XML (unchanged) when the file is saved.
        ///
        class CXMLUnknown : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] strContent The unknown tag content (node value) (without leading "<" and ending ">")
            ///
            CXMLUnknown(const std::wstring &strContent) : CXMLNode(XML_UNKNOWN, strContent) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLUnknown() {}

            ///
            /// @brief  Print the unknown tag
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;
        };


        ///
        ///  @brief  The XML text, contained in an element.
        ///
        class CXMLText : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// The "&amp; &gt; &lt;" should already be decoded into "&, >, <" for the input text
            ///
            /// @param  [in] strText    The text content (node value)
            ///
            CXMLText(const std::wstring &strText) : CXMLNode(XML_TEXT, strText) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLText() {}

            ///
            /// @brief  Print the text, will encode "&, >, <" into "&amp; &gt; &lt;"
            ///
            virtual bool print(std::wostream &outStream, int nDepth) const;

            ///
            /// @brief  Set text content
            ///
            /// The "&amp; &gt; &lt;" should already be decoded into "&, >, <" for the input text
            ///
            virtual void setValue(const std::wstring &strValue) {m_strValue = strValue;}

            ///
            /// @brief  Return true if all white spaces or new lines
            ///
            bool isBlank() const;

        public:
            ///
            /// @brief  Encode the escape characters "&, >, <" into "&amp; &gt; &lt;"
            ///
            /// Encode the HTML entities: "&lt; &gt;"
            /// 1) keep "&lt;" and "&gt;" in the input text by replacing "&" to "&amp;"
            /// 2) convert "<" to "&lt;"
            /// 3) convert ">" to "&gt;"
            ///
            static void encodeEscapes(std::wstring &strText);

            ///
            /// @brief  Decode the escape characters "&amp; &gt; &lt;" into "&, >, <"
            ///
            /// Decode the HTML entities: during XML parsing, "<", ">", "&lt;" and "&gt;" should be stored
            /// 1) convert "&lt;"  to "<"
            /// 2) convert "&gt;"  to ">"
            /// 3) convert "&amp;" to "&"
            ///
            static void decodeEscapes(std::wstring &strText);
        };


        // forward class reference
        class CXMLDocument;

        ///
        ///  @brief  Class for XML document and error handling
        ///
        class CXMLDOMHandler : public CXMLSAXHandler
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] pDocument  The XML document tree that will operated on
            ///
            CXMLDOMHandler(CXMLDocument* pDocument);

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLDOMHandler();

            ///
            /// @brief  Return whether error is occurred
            ///
            bool haveError() const {return m_bHaveError;}

        protected:
            //////////////////////////////////////////////////////////////////////////
            //
            // XML handler overriding
            //
            //////////////////////////////////////////////////////////////////////////

            /// Receive notification of the beginning of the document
            virtual bool startDocument();

            /// Receive notification of the end of the document
            virtual bool endDocument();

            /// Receive notification of the start of an element
            virtual bool startElement(const wchar_t *pName, const wchar_t **pAttributes, size_t nAttrib);

            /// Receive notification of the end of an element
            virtual bool endElement(const wchar_t *pName);

            /// Receive notification of a comment
            virtual bool comment(const wchar_t *pCmt);

            /// Receive notification of an XML declaration in "<?xml" and "?>"
            virtual bool declaration(const wchar_t *pVersion, const wchar_t* pEncoding, const wchar_t *pStandalone);

            /// Receive notification of character data inside an element
            virtual bool characters(const wchar_t *pChars);

            /// Receive notification of an unknown tag
            virtual bool unknownTag(const wchar_t *pChars);

            /// Receive notification of a parser error
            virtual void error(const wchar_t *pErrMsg, const size_t nCurPos);

        protected:
            /// Stack for storing XML nodes during parsing
            class CNodeStack;

        protected:
            /// Whether there is an error occurred when parsing document
            bool m_bHaveError;
            /// The pointer to the related XML document, new elements will be added as the document children
            CXMLDocument* m_pDocument;
            /// The XML node stack
            CNodeStack* m_pNodeStack;
        };


        ///
        /// @brief  A top level class for XML document parsing using XML DOM specification.
        ///
        /// This is always the top level node. A document binds together all the XML pieces.
        /// It can be saved, loaded, and printed to the screen.
        /// The 'value' of a document node is the XML file name.
        ///
        /// In XML, the document and elements can contain other elements and other types of nodes.
        /// @verbatim
        ///     A Document can contain: Declaration (leaf)
        ///                             Comment     (leaf)
        ///                             Unknown     (leaf)
        ///                             Element     (container or leaf)
        ///     An Element can contain: Element     (container or leaf)
        ///                             Text        (leaf)
        ///                             Attributes  (not on tree)
        ///                             Comment     (leaf)
        ///                             Unknown     (leaf)
        ///     A Declaration contains: Attributes  (not on tree)
        /// @endverbatim
        ///
        /// Here is an example. If the input document is: (demo.xml)
        /// @verbatim
        ///     <?xml version = '1.0' standalone = no?>
        ///     <!-- our to do list data -->
        ///     <ToWork>
        ///         <Item prioprity = '1'>Go to the <bold>Toy Store!</bold></Item>
        ///         <Item prioprity = '2'>Do bills</Item>
        ///     </ToWork>
        /// @endverbatim
        ///
        /// The parsing result will be: (loaded from "demo.xml")
        /// @verbatim
        ///     XMLDocument                 "demo.xml"
        ///         XMLDeclaration          "version = '1.0'" "standalone = no"
        ///         XMLComment              " our to do list data "
        ///         XMLElement              "ToWork"
        ///             XMLElement          "Item"      Attributes: priprioty = 1
        ///                 XMLText         "Go to the"
        ///                 XMLElement      "bold"
        ///                     XMLText     "Toy Store!"
        ///             XMLElement          "Item"      Attributes: priprioty = 2
        ///                 XMLText         "Do bills"
        /// @endverbatim
        ///
        class CXMLDocument : public CXMLNode
        {
        public:
            ///
            /// @brief  Constructor
            ///
            CXMLDocument() : CXMLNode(XML_DOCUMENT, L""), m_bHaveError(false) {}

            ///
            /// @brief  Destructor
            ///
            virtual ~CXMLDocument() {}

        public:
            ///
            /// @brief  Parse the given null terminated XML data buffer.
            ///
            /// Passing in an encoding (only XML_ENCODING_UTF16 is supported) will
            /// force parser to use that encoding, regardless of what encoding might
            /// automatically detected.
            ///
            /// The parameter bCreateNewBuf indicate whether the parser should create an internal
            /// buffer for the data source. Set to true will cause more memory to be used.
            ///
            /// @param  [in] pDataSource     The input source of the XML document
            /// @param  [in] eDataEncoding   The character encoding of the input data source
            /// @param  [in] bCreateNewBuf   Whether the parser should create an internal buffer for the data source
            ///
            /// @note   It is recommended not to create an internal buffer (2nd parameter set to be false).\n
            ///         If you are writing a multi-thread program, the other threads might change the buffer
            ///         passed in before parsing is done, you should set this to true.\n
            ///
            /// @return Whether XML document is parsed successfully
            ///
            bool parse(const wchar_t *pDataSource, bool bCreateNewBuf=false, EXMLEncoding eDataEncoding=XML_ENCODING_DEFAULT);

            ///
            /// @brief  Load an XML file and parse it, return true if successful
            ///
            /// @param  [in] strFileName    XML file name to be loaded
            /// @param  [in] eEncoding      The character encoding of the file
            ///
            bool load(const wchar_t *strFileName, str::EEncoding eEncoding=str::ENC_AUTO);

            ///
            /// @brief  Save XML data to the file, return true if successful
            ///
            /// @param  [in] strFileName    XML file name
            /// @param  [in] eEncoding      The character encoding of the file
            ///
            bool save(const wchar_t *strFileName, str::EEncoding eEncoding=str::ENC_UTF8) const;

            ///
            /// @brief  Print the document with formatted XML data
            ///
            /// If the document is to be output to console (i.e. std::wcout),
            /// the locales must be properly set for "std::wcout" before calling the function.
            /// Local for "std::wcout" can be set as follows:
            /// @verbatim
            ///     std::wcout.imbue(std::locale("chs")); // set the character-handling functions to use Simplified Chinese
            ///     std::wcout.imbue(std::locale("cht")); // set the character-handling functions to use Traditional Chinese
            /// @endverbatim
            ///
            /// @param  [in] outStream  Stream to which the document is to be written
            /// @param  [in] nDepth     Depth level (used for indent in formatted print)
            ///
            virtual bool print(std::wostream &outStream, int nDepth=0) const;

            /// Print to console
            bool print() const;

        protected:
            /// Whether there is an error occurred when parsing document
            bool m_bHaveError;
        };

    }
}

#endif//_CST_TOOLS_XML_DOM_H_
