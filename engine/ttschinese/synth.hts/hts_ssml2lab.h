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
/// @brief  Head file for SSML to LAB file conversion for HMM based speech synthesis of Chinese TTS engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///

#ifndef _CST_TTS_CHINESE_HTS_SSML2LAB_H_
#define _CST_TTS_CHINESE_HTS_SSML2LAB_H_

#include "ttsbase/synthesize/synth_synthesize.h"

namespace cst
{
    namespace tts
    {
        namespace Chinese
        {
            ///
            /// @brief  The class for converting SSML document to LAB file
            ///
            class CSSML2Lab : public base::CSynthesize
            {
            public:
                ///
                /// @brief  Constructor
                ///
                CSSML2Lab(const base::CDataManager *pDataManager) : base::CSynthesize(pDataManager) {}

            protected:
                ///
                /// @brief  Information of a specific label, which will be output to LAB file with specific format
                ///
                class CSegInfo
                {
                public:
                    // unit information
                    CUnitItem *pUnit;       ///< Handle to the unit information for original SSML document
                    // context information
                    const CSegInfo *preSeg; ///< Previous unit
                    const CSegInfo *nxtSeg; ///< Next unit
                    const CSegInfo *preSyl; ///< Previous syllable
                    const CSegInfo *nxtSyl; ///< Next syllable
                    const CSegInfo *prePWd; ///< Previous prosodic word
                    const CSegInfo *nxtPWd; ///< Next prosodic word
                    const CSegInfo *prePPh; ///< Previous prosodic phrase
                    const CSegInfo *nxtPPh; ///< Next prosodic phrase
                    // time information
                    int begTime;            ///< Begin time of current unit
                    int endTime;            ///< End time of current unit
                    // segment information
                    std::string segPhoneme; ///< Phoneme of current segment
                    int segPos2SylHead;     ///< Phoneme (segment) position counting from the head of syllable (Unit: phoneme)
                    int segPos2SylTail;     ///< Phoneme (segment) position counting from the tail of syllable (Unit: phoneme)
                    // syllable information
                    std::string sylPhoneme; ///< Phoneme of current syllable
                    std::string sylFinal;   ///< Final of current syllable
                    std::vector<std::string> sylSegs; ///< Segment list of current syllable
                    int sylTone;            ///< Tone of current syllable
                    int sylSegNumber;       ///< Segment number of current syllable
                    int sylPos2PWdHead;     ///< Syllable position counting from the head of prosodic word (Unit: syllable)
                    int sylPos2PWdTail;     ///< Syllable position counting from the tail of prosodic word (Unit: syllable)
                    int sylPos2PPhHead;     ///< Syllable position from the head of prosodic phrase (Unit: syllable)
                    int sylPos2PPhTail;     ///< Syllable position from the tail of prosodic phrase (Unit: syllable)
                    int sylPos2IPhHead;     ///< Syllable position from the head of intonation phrase (Unit: syllable)
                    int sylPos2IPhTail;     ///< Syllable position from the tail of intonation phrase (Unit: syllable)
                    int sylPreBoundaryType; ///< Boundary type before current syllable (syl: 0, pwd: 1, pph: 2, sp: 3, lp: 4, sil: 5)
                    int sylNxtBoundaryType; ///< Boundary type after current syllable
                    // prosodic word information
                    int pwdSylNumber;       ///< Syllable number of current prosodic word
                    int pwdPos2PPhHead;     ///< Prosodic word position from the head of prosodic phrase (Unit: prosodic word)
                    int pwdPos2PPhTail;     ///< Prosodic word position from the tail of prosodic phrase (Unit: prosodic word)
                    int pwdPos2IPhHead;     ///< Prosodic word position from the head of intonation phrase (Unit: prosodic word)
                    int pwdPos2IPhTail;     ///< Prosodic word position from the tail of intonation phrase (Unit: prosodic word)
                    // prosodic phrase information
                    int pphSylNumber;       ///< Syllable number of current prosodic phrase
                    int pphPWdNumber;       ///< Prosodic word number of current prosodic phrase
                    int pphPos2IPhHead;     ///< Prosodic phrase position from the head of intonation phrase (Unit: prosodic phrase)
                    int pphPos2IPhTail;     ///< Prosodic phrase position from the tail of intonation phrase (Unit: prosodic phrase)
                    // intonation phrase information
                    int iphPWdNumber;       ///< Prosodic word number of current intonation phrase
                    int iphPPhNumber;       ///< Prosodic phrase number of current intonation phrase
                    int iphIntonationType;  ///< Intonation type of the intonation phrase (breath group) type (0 not at utterance ending; 1 declarative, 2 interrogative, 3 exclamatory; 0 for sil, lp, sp)
                public:
                    // constructor
                    CSegInfo() {clear();}
                    // set unit as break
                    void asBreak(const char *name) {clear(); segPhoneme=name; sylFinal="X";}
                    // write the unit information to file in LAB format
                    void print(std::ostream &fp) const;
                    // return label string in LAB format
                    const std::string &asLabel() const;
                    // clear data
                    void clear();
                };

                ///
                /// @brief  Build the LAB formated information from sentence unit information for HTS engine
                ///
                /// @param  [in]  inSenUnits    Target basic unit information for a sentence
                /// @param  [out] outSenSegs    Return the information for each segment corresponding to LAB string
                ///
                /// @return Whether operation is successful
                /// @retval ERROR_SUCCESS The operation is successful
                ///
                int buildLabInfo(std::vector<CUnitItem> &inSenUnits, std::vector<CSegInfo> &outSenSegs);

            protected:
                //////////////////////////////////////////////////////////////////////////
                //
                //  Definitions for internal processing
                //
                //////////////////////////////////////////////////////////////////////////

                ///
                /// @brief  Information of a specific unit
                ///
                /// It should be noted that for the context information,
                /// both positions to head and to tail are 0 based.
                /// In the example "中国人民 (zhong1 guo2 ren2 min2)",
                /// the second syllable (guo2) will have the following
                /// value: "sylPos2UttHead=1" and "sylPos2UttTail=2".
                /// Total syllable number in the utterance is 
                /// "the summation of two values + 1".
                ///
                class CUnitInfo
                {
                public:
                    // original unit item
                    CUnitItem* pUnit;           ///< Handle to the unit item for SSML document
                    // context unit
                    const CUnitInfo *preSyl;    ///< Previous syllable information
                    const CUnitInfo *nxtSyl;    ///< Next syllable information
                    const CUnitInfo *preLWd;    ///< Last syllable of previous lexicon word
                    const CUnitInfo *prePWd;    ///< Last syllable of previous prosodic word
                    const CUnitInfo *prePPh;    ///< Last syllable of previous prosodic phrase
                    const CUnitInfo *preIPh;    ///< Last syllable of previous intonation phrase
                    const CUnitInfo *nxtLWd;    ///< First syllable of next lexicon word
                    const CUnitInfo *nxtPWd;    ///< First syllable of next prosodic word
                    const CUnitInfo *nxtPPh;    ///< First syllable of next prosodic phrase
                    const CUnitInfo *nxtIPh;    ///< First syllable of next intonation phrase
                    // context position
                    int sylPos2LWdHead;     ///< Syllable position (index) counting from the head of lexicon  word (Unit: syllable)
                    int sylPos2LWdTail;     ///< Syllable position (index) counting from the tail of lexicon word (Unit: syllable)
                    int sylPos2PWdHead;     ///< Syllable position (index) counting from the head of prosodic word (Unit: syllable)
                    int sylPos2PWdTail;     ///< Syllable position (index) counting from the tail of prosodic word (Unit: syllable)
                    int sylPos2PPhHead;     ///< Syllable position from the head of prosodic phrase (Unit: syllable)
                    int sylPos2PPhTail;     ///< Syllable position from the tail of prosodic phrase (Unit: syllable)
                    int sylPos2IPhHead;     ///< Syllable position from the head of intonation phrase (Unit: syllable)
                    int sylPos2IPhTail;     ///< Syllable position from the tail of intonation phrase (Unit: syllable)
                    int sylPos2UttHead;     ///< Syllable position from the head of utterance (Unit: syllable)
                    int sylPos2UttTail;     ///< Syllable position from the tail of utterance (Unit: syllable)
                    int lwdPos2PWdHead;     ///< Lexicon  word position from the head of prosodic word (Unit: lexicon word)
                    int lwdPos2PWdTail;     ///< Lexicon  word position from the tail of prosodic word (Unit: lexicon word)
                    int pwdPos2PPhHead;     ///< Prosodic word position from the head of prosodic phrase (Unit: prosodic word)
                    int pwdPos2PPhTail;     ///< Prosodic word position from the tail of prosodic phrase (Unit: prosodic word)
                    int pwdPos2IPhHead;     ///< Prosodic word position from the head of intonation phrase (Unit: prosodic word)
                    int pwdPos2IPhTail;     ///< Prosodic word position from the tail of intonation phrase (Unit: prosodic word)
                    int pwdPos2UttHead;     ///< Prosodic word position from the head of utterance (Unit: prosodic word)
                    int pwdPos2UttTail;     ///< Prosodic word position from the tail of utterance (Unit: prosodic word)
                    int pphPos2IPhHead;     ///< Prosodic phrase position from the head of intonation phrase (Unit: prosodic phrase)
                    int pphPos2IPhTail;     ///< Prosodic phrase position from the tail of intonation phrase (Unit: prosodic phrase)
                    int pphPos2UttHead;     ///< Prosodic phrase position from the head of utterance (Unit: prosodic phrase)
                    int pphPos2UttTail;     ///< Prosodic phrase position from the tail of utterance (Unit: prosodic phrase)
                    int iphPos2UttHead;     ///< Intonation phrase position from the head of utterance (Unit: Intonation phrase)
                    int iphPos2UttTail;     ///< Intonation phrase position from the tail of utterance (Unit: Intonation phrase)
                    // other information
                    int iphIntonationType;  ///< Intonation type of the intonation phrase (breath group) type (0 not at utterance ending; 1 declarative, 2 interrogative, 3 exclamatory)
                public:
                    // constructor
                    CUnitInfo() {clear();}  ///< Constructor
                    void clear();           ///< Clear the content
                };

                ///
                /// @brief  The internal structure for building the context position information
                ///
                struct TPosContext
                {
                    int idxLastUttBound;    ///< Last boundary position of utterance (Unit: syllable)
                    int idxLastIPhBound;    ///< Last boundary position of intonation phrase (Unit: syllable)
                    int idxLastPPhBound;    ///< Last boundary position of prosodic phrase (Unit: syllable)
                    int idxLastPWdBound;    ///< Last boundary position of prosodic word (Unit: syllable)
                    int idxLastLWdBound;    ///< Last boundary position of lexicon  word (Unit: syllable)
                    int idxCurSyllable;     ///< Current syllable index (Unit: syllable)
                    int idxLWdInPWd;        ///< Current lexicon  word index in prosodic word (Unit: lexicon word)
                    int idxPWdInPPh;        ///< Current prosodic word index in prosodic phrase (Unit: prosodic word)
                    int idxPWdInIPh;        ///< Current prosodic word index in intonation phrase (Unit: prosodic word)
                    int idxPWdInUtt;        ///< Current prosodic word index in utterance (Unit: prosodic word)
                    int idxPPhInIPh;        ///< Current prosodic phrase index in intonation phrase (Unit: prosodic phrase)
                    int idxPPhInUtt;        ///< Current prosodic phrase index in utterance (Unit: prosodic phrase)
                    int idxIPhInUtt;        ///< Current intonation phrase index in utterance (Unit: intonation phrase)
                };

                ///
                /// @brief  Build the context information for the internal unit information
                ///
                /// @param  [in]  inSenInfo     The input syllable information for a sentence
                /// @param  [out] outSenInfo    Return the internal syllable information with context
                ///
                int buildContext(std::vector<CUnitItem> &inSenInfo, std::vector<CUnitInfo> &outSenInfo);

                ///
                /// @brief  Build the context position information for the internal unit information
                ///
                int buildPosContext(TPosContext &posContext, std::vector<CUnitInfo*> &outSenInfo);

                ///
                /// @brief  Build the LAB formated information from internal unit information 
                ///
                int buildLabInfo(std::vector<CUnitInfo> &inSenInfo, std::vector<CSegInfo> &outSenInfo);

            };//CSSML2Lab

        }//namespace Chinese
    }
}

#endif//_CST_TTS_CHINESE_HTS_SSML2LAB_H_
