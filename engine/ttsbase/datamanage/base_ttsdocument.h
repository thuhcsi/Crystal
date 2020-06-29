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
/// @brief      Head file for the basic template class for internal TTS document manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/25
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_BASE_TTSDOCUMENT_H_
#define _CST_TTS_BASE_BASE_TTSDOCUMENT_H_

namespace cst
{
    namespace tts
    {
        namespace base
        {
            ///
            /// @brief  The basic class for TTS data, which is used for user specific data manipulation
            ///
            class CTTSData
            {
            public:
                /// Destructor
                virtual ~CTTSData() {}
            };

            ///
            /// @brief  The template class for internal document information manipulation during the processing of TTS modules
            ///
            /// This template class does NOT check the validity of the operations.
            /// User (caller) should ensure the validity of the operations themselves.
            ///
            template <class _T>
            class CTTSDocument
            {
            public:
                /// Constructor
                CTTSDocument() : m_iteParagraph(m_paragraphs.end()) {}

            public:
                ///
                /// @brief  Reset the cursor, so that getSentence() could return the first sentence of the document
                ///
                virtual bool resetCursor()
                {
                    m_iteParagraph = m_paragraphs.begin();
                    if (m_iteParagraph == m_paragraphs.end())
                    {
                        // no paragraph in document
                        return false;
                    }
                    m_iteSentence  = m_iteParagraph->begin();
                    if (m_iteSentence == m_iteParagraph->end())
                    {
                        // no sentence in paragraph
                        m_iteParagraph = m_paragraphs.end();
                        return false;
                    }
                    return true;
                }

                ///
                /// @brief  Get one sentence and advance to next sentence
                ///
                /// @retval NULL    Means no sentence in the document any more.
                ///
                virtual std::vector<_T> *getSentence()
                {
                    if (m_iteParagraph == m_paragraphs.end())
                    {
                        // no data any more
                        return NULL;
                    }

                    // store current sentence
                    sentence_t &retSen = *m_iteSentence;

                    // advance to next sentence, and next paragraph if necessary
                    m_iteSentence ++;
                    if (m_iteSentence == m_iteParagraph->end())
                    {
                        // advance to next paragraph
                        m_iteParagraph ++;
                        if (m_iteParagraph != m_paragraphs.end())
                            m_iteSentence = m_iteParagraph->begin();
                    }

                    // return current sentence
                    return &retSen;
                }

            public:
                ///
                /// @brief  Create and append a new paragraph in the document
                ///
                virtual void appendParagraph()
                {
                    // append a new paragraph to the document
                    m_paragraphs.resize(m_paragraphs.size() + 1);
                    m_iteParagraph = m_paragraphs.end();
                }

                ///
                /// @brief  Create and append a new sentence in the current paragraph of the document
                ///
                /// @note   Will raise exception if not valid (i.e. no paragraph exists)
                ///
                virtual void appendSentence()
                {
                    paragraph_t &lastParagraph = m_paragraphs.back();

                    // append a new sentence to the last paragraph of the document
                    lastParagraph.resize(lastParagraph.size() + 1);
                }

                ///
                /// @brief  Get the reference to the last sentence
                ///
                /// @note   Will raise exception of not valid (i.e. no paragraph or sentence exists)
                ///
                virtual std::vector<_T> &getLastSentence()
                {
                    paragraph_t &lastParagraph = m_paragraphs.back();
                    sentence_t  &lastSentence  = lastParagraph.back();
                    return lastSentence;
                }

                ///
                /// @brief  Append a single item to the current sentence of the document
                ///
                /// @note   Will raise exception of not valid (i.e. no paragraph or sentence exists)
                ///
                virtual void appendItem(const _T &oneItem)
                {
                    paragraph_t &lastParagraph = m_paragraphs.back();
                    sentence_t  &lastSentence  = lastParagraph.back();

                    // append a new word to the last sentence of the document
                    lastSentence.push_back(oneItem);
                }

            protected:
                /// Item type, the basic unit of the item
                typedef _T item_t;
                /// Sentence type, a list of items
                typedef std::vector<item_t> sentence_t;
                /// Paragraph type, a list of sentences
                typedef std::vector<sentence_t> paragraph_t;
                /// Document type, a list of paragraphs
                typedef std::vector<paragraph_t> document_t;

            protected:
                /// The internal document information
                document_t  m_paragraphs;
                /// The iterator for paragraph traversal in the document
                typename document_t::iterator  m_iteParagraph;
                /// The iterator for sentence traversal in the paragraph
                typename paragraph_t::iterator m_iteSentence;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_BASE_TTSDOCUMENT_H_
