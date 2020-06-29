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
/// @brief  Head file defining the text parsing interfaces for Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/11/16
///   Changed:  Implemented the framework and basic interfaces
///

#ifndef _CST_TTS_BASE_TTS_TEXTPARSER_H_
#define _CST_TTS_BASE_TTS_TEXTPARSER_H_

#include "ttsbase/datamanage/module_manager.h"
#include "xml/ssml_document.h"

namespace cst
{
    namespace xml {class CSSMLDocument;}

    namespace tts
    {
        ///
        /// @brief  Enumerations denoting the type of the input text string for TTS engine
        ///
        enum EInputTextType
        {
            ITT_TEXT_RAW = 0,           ///< Raw input text without any SSML tag
            ITT_TEXT_SSML_NO_ROOT,      ///< A input text with only partical SSML tags (without SSML "speak" root element)
            ITT_TEXT_SSML_WITH_ROOT,    ///< A well-formated SSML document with full SSML "speak" root element
        };

        ///
        /// @brief  The namespace for interface definition of a basic TTS engine
        ///
        namespace base
        {
            // forward class reference
            class CTextData;
            class CPreProcess;
            class CTextSegment;
            class CDocStruct;
            class CTextNormalize;
            class CLangConvert;
            class CWordSegment;
            class CProsodicStructGenerate;
            class CGrapheme2Phoneme;


            ///
            /// @brief  The base class defining the text parsing interfaces for Text-to-Speech (TTS) engine
            ///
            /// @warning    This class is not thread safe because of the global member variable,
            ///             please keep only one instance of the class for one thread.
            ///
            /// @todo   Is this warning message true?
            ///
            class CTextParser : public CModuleManager
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CTextParser();

                ///
                /// @brief  Destructor
                ///
                virtual ~CTextParser();

                ///
                /// @brief  Initialize the text parser for TTS engine
                ///
                /// @param  [in] configFile     Configuration file for initializing text parser
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int initialize(const wchar_t *configFile);

                ///
                /// @brief  Terminate the text parser, and close all data
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int terminate();

                ///
                /// @brief  Performing the text analysis
                ///
                /// @param  [in]  szText        The text to be analyzed
                /// @param  [in]  inputTextType Type of input text string. See declaration of "EInputTextType"
                /// @param  [out] ssmlDocument  Return the SSML document containing the related information of text analysis result
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(const wchar_t *szText, EInputTextType inputTextType, xml::CSSMLDocument &ssmlDocument);

            public:
                int preProcess(xml::CSSMLDocument *pSSMLDocument, const cmn::wstring &wstrIn, EInputTextType inputTextType);
                int textSegment(xml::CSSMLDocument *pSSMLDocument);
                int docStructAnalyze(xml::CSSMLDocument *pSSMLDocument);
                int langConvert(xml::CSSMLDocument *pSSMLDocument);
                int textNormalize(xml::CSSMLDocument *pSSMLDocument);
                int wordSegment(xml::CSSMLDocument *pSSMLDocument);
                int prosodyStructGenerate(xml::CSSMLDocument *pSSMLDocument);
                int graphemeToPhoneme(xml::CSSMLDocument *pSSMLDocument);

            protected:
                ///
                /// @brief  Set the default modules
                ///
                void setDefaultModules(std::map<std::string, ModuleInfo> &modules);

            protected:
                /// whether engine is initialized
                bool initialized;
                /// text data manager
                CTextData *dataManager;
                /// pre-process for text analysis
                CPreProcess *preProcessor;
                /// text piece segmentation
                CTextSegment *textSegmenter;
                /// document structure analysis
                CDocStruct *dsaAnalyzer;
                /// text normalization
                CTextNormalize *textNormalizer;
                /// encoding conversion (used for converting normalized text, to be deleted)
                CLangConvert *langConverter;
                /// word segmentation
                CWordSegment *wordSegmenter;
                /// prosodic structure generation
                CProsodicStructGenerate *psgGenerator;
                /// grapheme to phoneme
                CGrapheme2Phoneme *gtpConverter;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_TTS_TEXTPARSER_H_
