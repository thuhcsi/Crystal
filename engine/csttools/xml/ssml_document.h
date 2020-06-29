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
/// @brief  Definition for operation on SSML document.
///
/// @version    0.3.0
/// @date       2007/05/21
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Dezhi HUANG
///   Date:     2006/7/24
///   Changed:  Create
/// - Version:  0.2.0
///   Author:   Yongxin WANG (fefe.wyx@gmail.com)
///   Date:     2006/07/27
///   Change:   Changed to std::wstring
/// - Version:  0.3.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/21
///   Changed:  Changed to cmn::wstring
/// - Version:  0.4.0
///   Author:   Yongxin Wang (fefe.wyx@gmail.com)
///   Data:     2007/06/13
///   Changed:  Added getBoundaryType to take a "break" element as parameter
///


#ifndef _CST_TTS_BASE_SSML_DOCUMENT_H_
#define _CST_TTS_BASE_SSML_DOCUMENT_H_

#include "xml_dom.h"
#include "cmn/cmn_type.h"

#include "cmn/cmn_string.h"
#include <vector>

namespace cst
{
    namespace xml
    {
        ///
        /// @brief  The class for SSML (Speech Synthesis Markup Language) document manipulation
        ///
        /// This class manipulates the binary (in memory) SSML document tree.
        ///
        /// For the SSML specification, please visit:
        /// http://www.w3.org/TR/2007/WD-speech-synthesis11-20070110/
        ///
        class CSSMLDocument : public CXMLDocument
        {
        public:
            /// Constructor
            CSSMLDocument() {}

            /// Destructor
            virtual ~CSSMLDocument() {}

        public:
            ///
            /// @brief  Create an SSML document from a wide char (wchar_t) string
            ///
            /// @param  [in] pDataSource    The wide char string containing the SSML document
            ///
            /// @return Whether the operation is successful or not.
            ///
            bool parse(const wchar_t *pDataSource);

            ///
            /// @brief  Find next word of pWord
            ///
            /// @param  [in] pWord  The word whose next word will be found and returned
            ///
            /// @return The next word of pWord. NULL if pWord is the last word of sentence
            ///
            CXMLNode *findNextWord(const CXMLNode *pWord);

            ///
            /// @brief  Find previous word of pWord
            ///
            /// @param  [in] pWord  The word whose previous word will be found and returned
            ///
            /// @return The previous word of pWord. NULL if pWord is the first word of sentence
            ///
            CXMLNode *findPrevWord(const CXMLNode *pWord);

            ///
            /// @brief  Find the first word element in a sentence element
            ///
            /// @param  [in] pSentence  The sentence element in which to find the first word element
            ///
            /// @return Pointer to the found word element. NULL if not found
            ///
            CXMLElement *findFirstWord(const CXMLNode *pSentence);

            ///
            /// @brief  Get the part of speech (POS) of a word
            ///
            /// @pre    pWord MUST be a node at the word level, that is, one level below 's' with value 'w'
            ///
            /// @param  [in] pWord  The input word whose POS will be returned
            ///
            /// @return The Part of speech (POS) of the word
            ///
            const cmn::wstring &getWordPOS(const CXMLNode *pWord);

            ///
            /// @brief  Get the content of the word
            ///
            /// Any word element must and must only have either a Text element or a
            /// say-as element as a child. It can have arbitrary number of comment
            /// element children also.
            ///
            /// The function will retrieve the content of the text element, or the
            /// read-as attribute value in the say-as element.
            ///
            /// @param  [in] pSSMLWord  The word element. This must be a word element.
            ///
            /// @return The content of the word.
            ///         Content in the text or read-as value of the say-as label.
            ///
            /// @todo   How to deal with, if the word is split into syllables ("<syllable>")?
            ///
            static const cmn::wstring &getWordContent(const CXMLElement *pSSMLWord);


            /// @brief find the prosody label at the given position
            static CXMLElement *findProsodyBoundaryElemenet(CXMLElement *pSSMLWord);


            /// @brief retrieve the prosody boundary type
            static int getProsodyBoundary(CXMLElement *pSSMLWord);

            /// @brief clear the prosody boundary at the specified position
            void clearProsodyBoundary(CXMLElement *pSSMLWord);


        protected:
            static int getBoundaryType(CXMLElement *pSSMLBreak);

            /// @brief set the prosody boundaries at the specified position
            void setProsodyBoundary(CXMLElement *pSSMLWord, int iBoundaryType);

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  The following helper functions are used for the assistance
            //  to access the data in SSML recommendation
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Translate the time from string format in the SSML document into numeral value, return in millisecond
            ///
            /// @param  [in] wstrTime   Time in string format
            ///
            /// @return Time in float, in millisecond
            ///
            static float parseTime(const cmn::wstring &wstrTime);


        public:
            enum EProsodyBoudaryType
            {
                PROSBOUND_NONE = 0,
                PROSBOUND_SYLLABLE = PROSBOUND_NONE,    ///< Syllable boundary
                PROSBOUND_X_WEAK = 1,
                PROSBOUND_LWORD = PROSBOUND_X_WEAK,     ///< Lexicon word boundary
                PROSBOUND_WEAK = 2,
                PROSBOUND_PWORD = PROSBOUND_WEAK,       ///< Prosody word boundary
                PROSBOUND_MEDIUM = 3,
                PROSBOUND_PPHRASE = PROSBOUND_MEDIUM,   ///< Prosody phrase boundary
                PROSBOUND_STRONG = 4,
                PROSBOUND_IPHRASE = PROSBOUND_STRONG,   ///< Intonation phrase boundary
                PROSBOUND_X_STRONG = 5,
                PROSBOUND_SENTENCE = PROSBOUND_X_STRONG,///< Utterance boundary
                PROSBOUND_LAST_ITEM = 6                 ///< used only for boundary check
            };


        private:

        };//CSSMLDocument
    }
}

#endif//_CST_TTS_BASE_SSML_DOCUMENT_H_
