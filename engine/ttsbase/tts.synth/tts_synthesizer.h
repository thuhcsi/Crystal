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
/// @brief  Head file defining the speech synthesis interfaces for Text-to-Speech (TTS) engine
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

#ifndef _CST_TTS_BASE_TTS_SYNTHESIZER_H_
#define _CST_TTS_BASE_TTS_SYNTHESIZER_H_

#include "ttsbase/datamanage/module_manager.h"
#include "ttsbase/datamanage/data_datamanager.h"
#include "xml/ssml_document.h"
#include "dsp/dsp_wavedata.h"

namespace cst
{
    namespace dsp {class CWaveData;}
    namespace xml {class CSSMLDocument;}

    namespace tts
    {
        ///
        /// @brief  The namespace for interface definition of a basic TTS engine
        ///
        namespace base
        {
            // forward class reference
            class CVoiceData;
            class CUnitSegment;
            class CProsodyPredict;
            class CSynthesize;


            ///
            /// @brief  The base class defining the speech synthesis interfaces for Text-to-Speech (TTS) engine
            ///
            /// @warning    This class is not thread safe because of the global member variable,
            ///             please keep only one instance of the class for one thread.
            ///
            /// @todo   Is this warning message true?
            ///
            class CSynthesizer : public CModuleManager
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CSynthesizer();

                ///
                /// @brief  Destructor
                ///
                virtual ~CSynthesizer();

                ///
                /// @brief  Initialize the speech synthesis modules for TTS engine
                ///
                /// @param  [in] configFile     Configuration file for initializing speech synthesizer
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int initialize(const wchar_t *configFile);

                ///
                /// @brief  Terminate the speech synthesizer, and close all data
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int terminate();

                ///
                /// @brief  Performing speech synthesis: synthesize wave data according to input SSML document
                ///
                /// @param  [in]  ssmlDocument  SSML document containing the related information for speech synthesis
                /// @param  [out] ssmlDocument  Return the SSML document containing the related information of synthetic result
                /// @param  [out] waveData      Return the synthetic wave data
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int process(xml::CSSMLDocument &ssmlDocument, dsp::CWaveData &waveData);

                ///
                /// @brief  Get the format of the synthetic wave result
                ///
                /// @param  [out] nChannels         Return the number of channels of the wave result
                /// @param  [out] nBitsPerSample    Return the number of bits per sample of the wave result
                /// @param  [out] nSamplesPerSec    Return the number of samples per second (sampling rate) of the wave result
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int getWaveFormat(ushort& nChannels, ushort& nBitsPerSample, ulong& nSamplesPerSec);

                ///
                /// @brief  Get global variables and settings for TTS engine
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                virtual int getSetting(TTSSetting& setting);

                ///
                ///
                /// @brief  Set global variables and settings for TTS engine
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                ///
                virtual int setSetting(const TTSSetting& setting);

            public:
                int unitSegment(xml::CSSMLDocument *pSSMLDocument);
                int prosodyPredict(xml::CSSMLDocument *pSSMLDocument);
                int waveSynthesize(xml::CSSMLDocument *pSSMLDocument, dsp::CWaveData &waveData);

            protected:
                ///
                /// @brief  Set the default modules
                ///
                void setDefaultModules(std::map<std::string, ModuleInfo> &modules);

            protected:
                /// whether engine is initialized
                bool initialized;
                /// data manager
                CVoiceData *dataManager;
                /// unit segmentation
                CUnitSegment *unitSegmentor;
                /// prosodic prediction
                CProsodyPredict *prosodyPredictor;
                /// speech synthesize
                CSynthesize *waveSynthesizer;
            };

        }//namespace base
    }
}

#endif//_CST_TTS_BASE_TTS_SYNTHESIZER_H_
