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
/// @brief  Implementation file defining the text parsing interfaces for Text-to-Speech (TTS) engine
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

#include "tts_textparser.h"
#include "../datamanage/data_drivendata.h"
#include "../datatext/data_textdata.h"
#include "../preprocess/prep_preprocess.h"
#include "../preprocess/dsa_textsegment.h"
#include "../preprocess/dsa_docstruct.h"
#include "../preprocess/lang_langconvert.h"
#include "../textparse/norm_textnormalize.h"
#include "../textparse/wdseg_wordsegment.h"
#include "../textparse/psg_prosstructgen.h"
#include "../textparse/gtp_grapheme2phoneme.h"
#include <iostream>

namespace cst
{
    namespace tts
    {
        namespace base
        {
            CTextParser::CTextParser()
            {
                // initialize modules
                dataManager         = NULL;
                preProcessor        = NULL;
                langConverter       = NULL;
                textSegmenter       = NULL;
                dsaAnalyzer         = NULL;
                textNormalizer      = NULL;
                wordSegmenter       = NULL;
                psgGenerator        = NULL;
                gtpConverter        = NULL;

                initialized   = false;
            }

            CTextParser::~CTextParser()
            {
                terminate();
            }

            int CTextParser::initialize(const wchar_t *configFile)
            {
                // close engine first if initialized
                terminate();

                // set default modules in case the dynamic library only defines part of them
                setDefaultModules(modules);

                // load modules from dynamic library
                DataConfig dataConfig;
                int flag = loadModules(configFile, dataConfig);
                if (flag != ERROR_SUCCESS)
                {
                    return flag;
                }

                ///@todo log used modules
                {
                    std::wcout << str::mbstowcs(modules["CTextData"].name) << std::endl;
                    std::wcout << L"cst::tts::base::CPreProcess" << std::endl;
                    std::wcout << L"cst::tts::base::CLangConvert" << std::endl;
                    std::wcout << str::mbstowcs(modules["CTextSegment"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CDocStruct"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CTextNormalize"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CWordSegment"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CProsodicStructGenerate"].name) << std::endl;
                    std::wcout << str::mbstowcs(modules["CGrapheme2Phoneme"].name) << std::endl;
                }

                // initialize modules
                dataManager = dynamic_cast<CTextData*>(modules["CTextData"].createDataMan(dataConfig));
                if (dataManager == NULL)
                {
                    initialized = false;
                    return ERROR_OPEN_FAILED;
                }
                preProcessor    = new base::CPreProcess(dataManager, dataConfig.wstrLangTag.c_str());
                langConverter   = new base::CLangConvert((dataConfig.wstrLexiconPath + L"/sttable.dat").c_str());
                textSegmenter   = dynamic_cast<CTextSegment*>(modules["CTextSegment"].createModule(dataManager));
                dsaAnalyzer     = dynamic_cast<CDocStruct*>(modules["CDocStruct"].createModule(dataManager));
                textNormalizer  = dynamic_cast<CTextNormalize*>(modules["CTextNormalize"].createModule(dataManager));
                wordSegmenter   = dynamic_cast<CWordSegment*>(modules["CWordSegment"].createModule(dataManager));
                psgGenerator    = dynamic_cast<CProsodicStructGenerate*>(modules["CProsodicStructGenerate"].createModule(dataManager));
                gtpConverter    = dynamic_cast<CGrapheme2Phoneme*>(modules["CGrapheme2Phoneme"].createModule(dataManager));

                initialized = true;
                return ERROR_SUCCESS;
            }

            int CTextParser::terminate()
            {
                if (!initialized)
                    return ERROR_SUCCESS;

                // delete modules
                delete preProcessor;
                delete langConverter;
                modules["CTextSegment"].deleteModule(textSegmenter);
                modules["CDocStruct"].deleteModule(dsaAnalyzer);
                modules["CTextNormalize"].deleteModule(textNormalizer);
                modules["CWordSegment"].deleteModule(wordSegmenter);
                modules["CProsodicStructGenerate"].deleteModule(psgGenerator);
                modules["CGrapheme2Phoneme"].deleteModule(gtpConverter);

                // free the data manager
                modules["CTextData"].deleteDataMan(dataManager);

                // reset modules
                dataManager         = NULL;
                preProcessor        = NULL;
                langConverter       = NULL;
                textSegmenter       = NULL;
                dsaAnalyzer         = NULL;
                textNormalizer      = NULL;
                wordSegmenter       = NULL;
                psgGenerator        = NULL;
                gtpConverter        = NULL;

                initialized = false;
                return ERROR_SUCCESS;
            }

            int CTextParser::process(const wchar_t *szText, EInputTextType inputTextType, xml::CSSMLDocument &ssmlDocument)
            {
                if (!initialized)
                    return ERROR_NOT_INITIALIZED;

                // clear data first
                ssmlDocument.clearChildren();

                // the SSML document
                xml::CSSMLDocument *pDocument = &ssmlDocument;

                // pre-process
                int retCode = preProcess(pDocument, szText, inputTextType);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // encoding conversion
                retCode = langConvert(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // text piece segmentation
                retCode = textSegment(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // document structure analysis
                retCode = docStructAnalyze(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // text normalization
                retCode = textNormalize(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // word segmentation
                retCode = wordSegment(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // prosodic structure generation
                retCode = prosodyStructGenerate(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                // grapheme to phoneme
                retCode = graphemeToPhoneme(pDocument);
                if (retCode != ERROR_SUCCESS)
                    return retCode;

                return ERROR_SUCCESS;
            }

            int CTextParser::preProcess(xml::CSSMLDocument *pSSMLDocument, const cmn::wstring &wstrIn, EInputTextType inputTextType)
            {
                return preProcessor->process(pSSMLDocument, wstrIn, inputTextType);
            }

            int CTextParser::textSegment(xml::CSSMLDocument *pSSMLDocument)
            {
                return textSegmenter->process(pSSMLDocument);
            }

            int CTextParser::docStructAnalyze(xml::CSSMLDocument *pSSMLDocument)
            {
                return dsaAnalyzer->process(pSSMLDocument);
            }

            int CTextParser::langConvert(xml::CSSMLDocument *pSSMLDocument)
            {
                return langConverter->process(pSSMLDocument);
            }

            int CTextParser::textNormalize(xml::CSSMLDocument *pSSMLDocument)
            {
                return textNormalizer->process(pSSMLDocument);
            }

            int CTextParser::wordSegment(xml::CSSMLDocument *pSSMLDocument)
            {
                return wordSegmenter->process(pSSMLDocument);
            }

            int CTextParser::prosodyStructGenerate(xml::CSSMLDocument *pSSMLDocument)
            {
                return psgGenerator->process(pSSMLDocument);
            }

            int CTextParser::graphemeToPhoneme(xml::CSSMLDocument *pSSMLDocument)
            {
                return gtpConverter->process(pSSMLDocument);
            }


            base::CDataManager *CreateTextData(const base::DataConfig &dataConfig)
            {
                return new base::CTextData();
            }

            void DeleteTextData(base::CDataManager *pDataManager)
            {
                delete pDataManager;
            }

            base::CModule *CreateTextSegment(const base::CDataManager *pDataManager)
            {
                return new base::CTextSegment(pDataManager);
            }

            base::CModule *CreateDocStruct(const base::CDataManager *pDataManager)
            {
                return new base::CDocStruct(pDataManager);
            }

            base::CModule *CreateTextNormalize(const base::CDataManager *pDataManager)
            {
                return new base::CTextNormalize(pDataManager);
            }

            base::CModule *CreateWordSegment(const base::CDataManager *pDataManager)
            {
                return new base::CWordSegment(pDataManager);
            }

            base::CModule *CreateProsodicStructGenerate(const base::CDataManager *pDataManager)
            {
                return new base::CProsodicStructGenerate(pDataManager);
            }

            base::CModule *CreateGrapheme2Phoneme(const base::CDataManager *pDataManager)
            {
                return new base::CGrapheme2Phoneme(pDataManager);
            }

            void DeleteTextModule(base::CModule *pModule)
            {
                delete pModule;
            }
            
            void CTextParser::setDefaultModules(std::map<std::string, ModuleInfo> &modules)
            {
                modules["CTextData"] = ModuleInfo("cst::tts::base::CTextData", CreateTextData, DeleteTextData);
                modules["CTextSegment"] = ModuleInfo("cst::tts::base::CTextSegment", CreateTextSegment, DeleteTextModule);
                modules["CDocStruct"] = ModuleInfo("cst::tts::base::CDocStruct", CreateDocStruct, DeleteTextModule);
                modules["CTextNormalize"] = ModuleInfo("cst::tts::base::CTextNormalize", CreateTextNormalize, DeleteTextModule);
                modules["CWordSegment"] = ModuleInfo("cst::tts::base::CWordSegment", CreateWordSegment, DeleteTextModule);
                modules["CProsodicStructGenerate"] = ModuleInfo("cst::tts::base::CProsodicStructGenerate", CreateProsodicStructGenerate, DeleteTextModule);
                modules["CGrapheme2Phoneme"] = ModuleInfo("cst::tts::base::CGrapheme2Phoneme", CreateGrapheme2Phoneme, DeleteTextModule);
            }

        }//namespace base
    }
}
