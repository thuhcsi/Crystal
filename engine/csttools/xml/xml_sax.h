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
/// @brief  Definition of SAX specification for XML (eXtensible Markup Language) manipulation.
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


#ifndef _CST_TOOLS_XML_SAX_H_
#define _CST_TOOLS_XML_SAX_H_

namespace cst
{
    ///
    /// @brief  The namespace for XML (eXtensible Markup Language) manipulation
    ///
    namespace xml
    {
        ///
        /// @brief  The basic class for document and error handing for XML SAX specification.
        ///
        /// This class is an implementation of the XML SAX specification.
        /// SAX is the Simple API for XML (http://sax.sourceforge.net/).
        ///
        /// All the XML parsing result is passed to this handler by calling corresponding functions.
        /// It is a virtual base class and will do nothing by returning true to all events.
        /// User can implement his own class by overriding this class to get the parsing result.
        ///
        /// For sample code:
        /// @see    CXMLSAXParser
        ///
        class CXMLSAXHandler
        {
        public:
            /// @brief  Constructor
            CXMLSAXHandler() {}

            /// @brief  Destructor
            virtual ~CXMLSAXHandler() {}

        public:
            ///
            /// @brief  Reset the Document on its reuse
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool resetDocument() {return true;}

            ///
            /// @brief  Receive notification of the beginning of the document
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool startDocument() {return true;}

            ///
            /// @brief  Receive notification of the end of the document
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool endDocument() {return true;}

            ///
            /// @brief  Receive notification of the start of an element
            ///
            /// @param  [in] pName          Name of the element
            /// @param  [in] pAttributes    The attributes of the element (attribute name and value pair)
            /// @param  [in] nAttrib        The number of the attributes
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            /// Sample code:
            /// @verbatim
            /// bool startElement(const wchar_t *pName, const wchar_t **pAttributes, size_t nAttrib)
            /// {
            ///     std::wcout << pName << std::endl;       // output element name
            ///     for (size_t i=0; i<nAttrib; i++)
            ///     {
            ///         std::wcout << pAttributes[2*i];     // output name of attribute i
            ///         std::wcout << pAttributes[2*i+1];   // output value of attribute i
            ///     }
            ///     return true;
            /// }
            /// @endverbatim
            ///
            virtual bool startElement(const wchar_t *pName, const wchar_t **pAttributes, size_t nAttrib) {return true;}

            ///
            /// @brief  Receive notification of the end of an element
            ///
            /// @param  [in] pName      Name of the element
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool endElement(const wchar_t *pName) {return true;}

            ///
            /// @brief  Receive notification of a comment
            ///
            /// @param  [in] pCmt       The comment string, without leading "<!--" and ending "-->"
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool comment(const wchar_t *pCmt) {return true;}

            ///
            /// @brief  Receive notification of an XML declaration in "<?xml" and "?>"
            ///
            /// @param  [in] pVersion       The version attribute value
            /// @param  [in] pEncoding      The encoding attribute value
            /// @param  [in] pStandalone    The standalone attribute value
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool declaration(const wchar_t *pVersion, const wchar_t* pEncoding, const wchar_t *pStandalone) {return true;}

            ///
            /// @brief  Receive notification of character data inside an element
            ///
            /// @param  [in] pChars     The character data
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool characters(const wchar_t *pChars) {return true;}

            ///
            /// @brief  Receive notification of an unknown tag
            ///
            /// @param  [in] pChars     The tag string, without leading "<" and ending ">"
            ///
            /// @return Tell the Parser whether error is occurred or not. \n If error occurred, 
            ///         parser will not continue excepting that the contAfterError is set to true.
            ///
            virtual bool unknownTag(const wchar_t *pChars) {return true;}

            ///
            /// @brief  Receive notification of a parser error
            ///
            /// @param  [in] pErrMsg    Detailed error message
            /// @param  [in] nCurPos    The cursor position where error takes place
            ///
            virtual void error(const wchar_t *pErrMsg, const size_t nCurPos) {}
        };


        ///
        /// @brief  The XML character encoding of the input data.
        ///
        /// @note   Currently, only XML_ENCODING_UTF16 (Unicode Little Endian) is supported.
        ///         This enumeration is provided for possible later extension.
        ///
        enum EXMLEncoding
        {
            XML_ENCODING_UTF16  = 0,                    ///< The UTF16 (Unicode) encoding
            XML_ENCODING_DEFAULT = XML_ENCODING_UTF16,  ///< Default encoding
        };


        ///
        /// @brief  The class to perform the XML source data reading
        ///
        class CXMLReader
        {
        public:
            ///
            /// @brief  Constructing a data source reader
            ///
            /// @param  [in] pDataSource    Pointer to the input XML data source.
            ///                             The data source will never be changed by this class and the parser.
            /// @param  [in] eDataEncoding  The character encoding of the input data source.
            /// @param  [in] bCreateBuffer  Whether to create an internal buffer for the data source.
            ///                             If the data source might be changed by other threads during parsing, set this to true.
            ///
            CXMLReader(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer);

            ///
            /// @brief  Destructor
            ///
            /// If internal buffer is created, destroy it.
            ///
            ~CXMLReader();

        public:
            /// Get data pointer from cursor
            inline const wchar_t* getChars () const;

            /// Get current data cursor position
            inline const size_t getCursor() const;

            /// Peak one char from the buffer, return 0 if no more character
            inline const wchar_t peekChar() const;

            /// Get one char from the buffer (advance cursor accordingly), return 0 if no more character
            inline const wchar_t getChar();

            /// @brief  Set the data cursor position
            /// @return Whether input is a legal position
            inline bool setCursor(size_t nNewPos);

            /// @brief  Skip white spaces
            /// @retval true If some white spaces are skipped
            inline bool skipWhiteSpaces();

            ///
            /// @brief  Read the data buffer until the specified pattern is met.
            ///
            /// The function only sets the cursor to the end of the data that have been
            /// read, and returns whether pToMatch is matched.
            ///
            /// Match is done as case sensitive string comparison.
            ///
            /// The function will stop either pToMatch or pToBreak is met in the source.
            /// The cursor will set to the character after pToMatch if pToMatch is matched and bSkip is true.
            /// Otherwise, the cursor will be set to the first character where either pattern is matched.
            /// If nothing is matched, cursor will be set to the end of the data source.
            ///
            /// @param  [in] pToMatch   The pattern we want to match. Cannot be NULL.
            /// @param  [in] pToBreak   The pattern which we do not want to find before pToMatch is met.
            ///                         If this is met, matching ends, and functions returns false.
            /// @param  [in] bSkip      If true, skip the pToMatch pattern if met
            ///
            /// @return Whether the pToMatch pattern is matched
            ///
            bool readUntil(const wchar_t *pToMatch, const wchar_t *pToBreak, bool bSkip);

            ///
            /// @brief  Peek if the next coming buffer matches the string pToPeek (case insensitive)
            ///
            /// @param  [in] pToPeek    The string to be matched (case insensitive). Cannot be NULL.
            /// @param  [in] bSkip      If true, skip the string if matched
            ///
            /// @return Whether the pattern is matched and peeked
            ///
            bool peekString(const wchar_t *pToPeek, bool bSkip);

        public:
            /// Whether it is an XML white space: 0x0D, 0x0A, 0x09, 0x20
            static bool isWhiteSpace(wchar_t c);

            /// Whether it is an XML name start char: (Letter | '_')
            static bool isNameStart (wchar_t c);

            /// Whether it is an XML name body char: (Letter | Digit | '.' | '-' | '_' | ':')
            static bool isNameChar (wchar_t c);

        private:
            const wchar_t *m_pDataBuffer;       ///< Data buffer. It will never be changed whether internal or not.
            size_t         m_nDataCursor;       ///< Data cursor in the buffer.
            size_t         m_nDataAvail;        ///< Number of characters currently available in the buffer.
            bool           m_bInternalBuffer;   ///< Whether the data buffer is internal or not.
        };


        ///
        /// @brief  The main entry class to perform the XML document parsing for SAX specification
        ///
        /// This class is very simple and efficient, and is an implementation of the SAX specification.
        /// SAX is the Simple API for XML (http://sax.sourceforge.net/).
        ///
        /// The sample code:
        /// @verbatim
        ///     CXMLSAXHandler xmlHandler;
        ///     CXMLSAXParser  xmlParser;
        ///     EXMLEncoding   dataEncoding = XML_ENCODING_UTF16;
        ///     xmlParser.setHandler(&xmlHandler);
        ///     xmlParser.parse(dataSource, dataEncoding, createNewBuf);
        /// @endverbatim
        ///
        class CXMLSAXParser
        {
        public:
            ///
            /// @brief  Default constructor
            ///
            CXMLSAXParser();
            ///
            /// @brief  Destructor
            ///
            ~CXMLSAXParser();

        public:
            ///
            /// @brief Parse an XML document. The XML document is stored in a wchar_t array buffer.
            ///
            /// The bCreateBuffer parameter indicate whether the parser should create an internal buffer
            /// for containing the data source. \n
            /// Generally, it is <b>NOT</b> necessary to create the internal buffer, excepting that
            /// the source might be changed by other threads during parsing.
            ///
            /// @param  [in] pDataSource    The input source of the XML document (wchar_t string).
            /// @param  [in] eDataEncoding  The character encoding of the input data source (only support: XML_ENCODING_UTF16)
            /// @param  [in] bCreateBuffer  Whether the parser should create an internal buffer for data source
            ///
            void parse(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer);

            ///
            /// @brief  Set the handler dealing the parsing events
            ///
            /// @param  [in] pHandler   The handle to the handler
            ///
            void setHandler(CXMLSAXHandler* pHandler);

            ///
            /// @brief  Set whether parsing should continue after an error is met
            ///
            void setContinueAfterError(bool bContinue = true);

        protected:
            /// XML tokens to deal with
            enum ETokens
            {
                TOKEN_CHARDATA,     ///< Raw text data between tags (element characters)
                TOKEN_COMMENT,      ///< Comments, "<-- -->"
                TOKEN_DECLARATION,  ///< Declaration, "<?xml ...?>"
                TOKEN_ENDTAG,       ///< End tag, "</...>"
                TOKEN_EOF,          ///< End of input buffer
                TOKEN_STARTTAG,     ///< Start tag "<...>"
                TOKEN_UNKNOWN       ///< Unknown tag
            };

            /// Error codes indicating errors during parsing
            enum EErrorCodes
            {
                ERR_OUTOF_MEMORY,                   ///< Internal buffer create failed
                ERR_ENDED_WITH_TAGS_NOT_PROCESSED,  ///< Expect end of tag '...'
                ERR_PARSING_UNKNOWN,                ///< Error parsing unknown tag
                ERR_PARSING_COMMENT,                ///< Error parsing comment
                ERR_PARSING_DECLARATION,            ///< Error parsing declaration
                ERR_PARSING_START_TAG,              ///< Error parsing StartTag
                ERR_PARSING_END_TAG,                ///< Error parsing EndTag
                ERR_END_TAG_NAME_EXPECTED,          ///< End tag name '...' expected
                ERR_FROM_USER                       ///< Error in user defined handler
            };

            /// Stack for storing XML element name during parsing
            class CElemStack;

        protected:
            ///
            /// @brief  Detect the attribute value
            ///
            /// For input, the cursor points to the non-space characters after the attribute name.
            /// After parsing, cursor points to where after the attribute value if no error occurred.
            ///
            /// @param  [in]  eErrNo    Error number to be emitted when an error is met
            /// @param  [out] nValueLen The length of the value string
            ///
            /// @return Pointer to the attribute value, or NULL if error occurred.
            ///         The return value is NOT NULL terminated.
            ///
            /// @pre    The cursor should point to the equal sign('=') between attribute name and attribute value
            /// @post   The cursor points to the character after the closing quotes of the attribute value.
            ///
            /// @note   The return value is NOT null terminated
            ///
            const wchar_t *senseAttribValue(EErrorCodes eErrNo, size_t &nValueLen);

            ///
            /// @brief  Detect what the next top level token is in the input source.
            ///
            /// After parsing, the cursor points to the main token part without leading tag
            ///
            ETokens senseNextToken();

            ///
            /// @brief  Reset the Parser and the XML data Reader, and notify the Handler
            ///
            /// The function will clear any content and temporary state in the parser, and
            /// initialize the parser with new content given.
            ///
            /// @param  [in] pDataSource    The new data that is to be parsed
            /// @param  [in] eDataEncoding  The character encoding of the input data
            /// @param  [in] bCreateBuffer  Whether an internal buffer should be created
            ///
            /// @return Whether operation is completed successfully 
            ///         (i.e. data reader, element stack created successfully)
            ///
            /// @note   The function creates the data reader.
            ///
            /// @see    CXMLReader::CXMLReader
            ///
            bool reset(const wchar_t *pDataSource, EXMLEncoding eDataEncoding, bool bCreateBuffer);

            ///
            /// @brief  Emit an error message to the error Handler
            ///
            /// @param  [in] eErrNo     The error code
            /// @param  [in] pErrParam  The error parameter.
            ///                         For XMLERR_ENDED_WITH_TAGS_NOT_PROCESSED and XMLERR_END_TAG_NAME_EXPECTED,
            ///                         the parameter should be the tag name
            ///
            void emitError(EErrorCodes eErrNo, const wchar_t *pErrParam);

            ///
            /// @brief  Scan the raw character data in an element, pass the data as a string to the Handler
            ///
            /// @pre    The cursor should point to the first character of the character data
            /// @post   The cursor points to the next character after data (usually a '<')
            ///
            void scanCharData();

            ///
            /// @brief  Scan the unknown tag, just pass the main token string to the Handler
            ///
            /// @pre    The cursor points to the character after the starting '<'
            /// @post   The cursor points to the character after the ending '>'
            ///
            void scanUnknownTag();

            ///
            /// @brief  Scan the comment, pass the comment string to the Handler
            ///
            /// @pre    The cursor points to the character after "<!--"
            /// @post   The cursor points to the character after "-->"
            ///
            void scanComment();

            ///
            /// @brief  Scan the XML declaration, pass the version, encoding, and standalone to the Handler
            ///
            /// @pre    The cursor points to the character after "<?xml"
            /// @post   The cursor points to the character after "?>"
            ///
            void scanDeclaration();

            ///
            /// @brief  Parse the element start tag, push element name to stack if it is not an empty tag, and notify the Handler
            ///
            /// @pre    The cursor points to the character after the beginning "<"
            /// @post   The cursor points to the character after the ending ">"
            ///
            void scanStartTag();

            ///
            /// @brief  Parse the element end tag, pop element name from stack, and notify the Handler
            ///
            /// @pre    The cursor points to the character after the beginning "</"
            /// @post   The cursor points to the character after the ending ">"
            ///
            void scanEndTag();

        private:
            CElemStack     *m_pElemStack;   ///< XML element stack. Initialized in reset()
            CXMLReader     *m_pDataReader;  ///< The module to read the XML data. Initialized in reset()
            CXMLSAXHandler *m_pDocHandler;  ///< The client code's document handler
            bool        m_bContAfterError;  ///< Whether continue parsing after error occurred
        };

    }
}

#endif//_CST_TOOLS_XML_SAX_H_
