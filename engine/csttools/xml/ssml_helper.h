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
/// @brief  Definition file of the helper functions for SSML document operation.
///
/// @version    0.1.0
/// @date       2008/05/28
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_SSML_HELPER_H_
#define _CST_TTS_BASE_SSML_HELPER_H_

#include <map>
#include <vector>
#include <string>

namespace cst
{
    namespace xml
    {
        ///
        /// @brief  The class containing the helper functions for SSML document operation
        ///
        class CSSMLHelper
        {
        public:
            ///
            /// @brief  Compose the string given the format and arguments
            ///
            /// @param  [out] wstrResult    Return the composed string
            ///
            static int composeString(std::wstring &wstrResult, const wchar_t *format, ...);


        public:
            ///
            /// @brief  Parse the simple floating number.
            ///
            /// The legal format of the number could be any floating point commonly used.
            ///
            /// @param  [in] wstrFloat  The floating number string to be parsed
            ///
            /// @return The parsed floating number
            ///
            static float parseFloatNumber(const std::wstring &wstrFloat);

            ///
            /// @brief  Parse the time information. The unit of the returned time is: ms.
            ///
            /// The format of the "time" string is defined as a non-negative real number followed by
            /// a time unit identifier.  The time unit identifiers include: "s" for secondes, "ms" for milliseconds.
            /// Examples include: 3s, 500ms, etc.
            ///
            /// The "time" is a value in seconds (ended with "s") or milliseconds (ended with "ms").
            /// The units (i.e. "s" or "ms") are case sensitive.
            ///
            /// @param  [in] wstrTime   The time string to be parsed
            ///
            /// @return The parsed time information (Unit: ms)
            ///
            static float parseTime(const std::wstring &wstrTime);

            ///
            /// @brief  Parse the frequency information. The unit of the returned frequency is: Hz.
            ///
            /// The format of the "frequency" string is defined as a "number" followed by "Hz".
            /// A number is a simple positive floating point value without exponentials.
            /// Examples include: 150Hz, 112.6Hz, etc.
            ///
            /// @param  [in] wstrFrequency  The frequency string to be parsed
            ///
            /// @return The parsed frequency information (Unit: Hz)
            ///
            static float parseFrequency(const std::wstring &wstrFrequency);

            ///
            /// @brief  Parse a list of frequency information. The unit of the returned frequency is: Hz.
            ///
            /// The format of the input string is a list of "frequency" separated by blank space (" ").
            /// For example: "150Hz 112.5Hz 30Hz".
            ///
            /// @param  [in] wstrFreqList   The frequency list string
            ///
            /// @return The parsed frequency list in vector (Unit: Hz)
            ///
            static std::vector<float> parseFrequencyList(const std::wstring &wstrFreqList);

            ///
            /// @brief  Parse the unit selection identifier.
            ///
            /// The format of the input string is "phoneme:id".
            ///
            /// @param  [in]  wstrUnitID    The input string of the unit selection identifier
            /// @param  [out] wstrPhoneme   Return the phoneme string of the unit selection identifier
            ///
            /// @return The unit "id" of the unit selection identifier
            /// @retval -1  Input string is not valid format
            ///
            static int parseUnitID(const std::wstring &wstrUnitID, std::wstring &wstrPhoneme);

            ///
            /// @brief  Parse the volume string, and convert to float value that represents the relative change
            ///
            /// The format of the "volume" string is defined as:
            ///   (1) a dB value: a "number" preceded by "+" or "-" and immediately followed by "dB";
            ///   (2) a non-negative percentage: an "unsigned number" immediately followed by "%";
            ///   (3) "silent", "x-soft", "soft", "medium", "loud", "x-loud", or "default".
            ///
            /// @param  [in] strVolume  The volume string to be parsed
            ///
            /// @return The float value that represents the relative change
            ///
            static float parseVolume(const std::wstring& strVolume);

            ///
            /// @brief  Parse the rate string, and convert to float value that represents the relative change
            ///
            /// The format of the "rate" string is defined as:
            ///   (1) a non-negative percentage: an "unsigned number" immediately followed by "%";
            ///   (2) "x-slow", "slow", "medium", "fast", "x-fast", or "default".
            ///
            /// @param  [in] strRate    The rate string to be parsed
            ///
            /// @return The float value that represents the relative change
            ///
            static float parseRate(const std::wstring& strRate);

            ///
            /// @brief  Parse the pitch string, and convert to float value that represents the relative change
            ///
            /// The format of the "pitch" string is defined as:
            ///   (1) a relative value: a "number" preceded by "+" or "-" and immediately followed by "%";
            ///   (2) a non-negative percentage: an "unsigned number" immediately followed by "%";
            ///   (3) "x-low", "low", "medium", "high", "x-high", or "default".
            ///
            /// @param  [in] strPitch   The pitch string to be parsed
            ///
            /// @return The float value that represents the relative change
            ///
            static float parsePitch(const std::wstring& strPitch);



        public:
            ///
            /// @brief  Get the boundary type information related to "break" element
            ///
            /// @param  [in] pSSMLBreak     Break element whose boundary information is to be retrieved
            ///
            /// @return Return the boundary type defined in CSSMLDocument::EProsodyBoudaryType
            ///
            static int getBoundaryType(const CXMLElement *pSSMLBreak);

            ///
            /// @brief  Set the boundary type for "break" element
            ///
            /// @param  [in] pSSMLBreak     Break element whose boundary information is to be set
            /// @param  [in] iBoundaryType  Boundary type as defined in CSSMLDocument::EProsodyBoudaryType
            ///
            static void setBoundaryType(CXMLElement *pSSMLBreak, int iBoundaryType);


        /*
        public:
            /// Find methods
            static CXMLElement *findElementInChildren(CXMLNode *pParent, const wchar_t *wstrName, bool bRecursive = true);
            //static CXMLElement *findElementInSiblings(const CXMLNode *pCurrent, const wchar_t *wstrName);
            static CXMLElement *findElementDepthFirst(CXMLNode *pRoot, const wchar_t *wstrName, CXMLNode *pCurrent = 0, bool bGoPastRoot = false);
            static CXMLElement *findElementDepthFirst(CXMLNode *pRoot, std::vector<std::wstring> wstrName, CXMLNode *pCurrent = 0, bool bGoPastRoot = false);
            static CXMLNode *getNextNodeDepthFirst(CXMLNode *pRoot, CXMLNode *pCurr, bool bGoPastRoot);
            //static CXMLElement *findElementInLevel1Children(CXMLNode *pParent, const wchar_t *wstrName);

            /// Check element type
            static bool     isElement(const CXMLNode *pNode, const wchar_t *wstrName);
            static bool     isBreak(const CXMLNode *pNode);
            static bool     isProsody(const CXMLNode *pNode);
            static bool     isWord(const CXMLNode *pNode);
            static bool     isSyllable(const CXMLNode *pNode);
            static bool     isPhoneme(const CXMLNode *pNode);

            /// For <break>, setting or getting its boundary type and break time
            static bool     getBreakTime(CXMLElement *pSSMLBreak, float &fDuration);
            static void     setBreakTime(CXMLElement *pSSMLBreak, float fDuration);
            static bool     getBoundaryType(CXMLElement *pSSMLBreak, int &iBoundaryType);
            static void     setBoundaryType(CXMLElement *pSSMLBreak, int iBoundaryType);

            /// For <syllable>, setting or getting its selection result
            static bool     getSelectionResult(CXMLElement *pSSMLSyl, IndexCode &iCode, int &iIdx);
            static void     setSelectionResult(CXMLElement *pSSMLSyl, IndexCode iCode, int iIdx);

            /// For <prosody>, setting or getting its duration/pitch/amplitude
            static bool     getProsodyValues(CXMLElement *pSSMLProsody, float &fPitch, float &fDuration, float &fAmplitude);
            static void     setProsodyValues(CXMLElement *pSSMLProsody, float fPitch, float fDuration, float fAmplitude);
            static bool     getProsodyValues(CXMLElement *pSSMLProsody, std::wstring &strPitch, float &fDuration, float &fAmplitude);
            static void     setProsodyValues(CXMLElement *pSSMLProsody, const std::wstring &strPitch, float fDuration, float fAmplitude);

            /// For <prosody>, judge or set whether the sub units are continous
            static bool     getContinuousFlag(CXMLElement *pSSMLProsody);
            static void     setContinuousFlag(CXMLElement *pSSMLProsody, bool bContinous = true);

        public:
            static std::wstring constructTime(float fDuration);
            /// @brief translates time written in strings in the SSML document into
            ///        numeral value, in miliseconds
            /// @param [in] wstrTime time in string format
            /// @return floating point time in miliseconds
            static float    translateTime(const std::wstring &wstrTime);

            static std::wstring constructFloat(float fValue);
            static float    translateFloat(const std::wstring &wstrFloat);

            //static std::wstring constructSyllablePitch(float pratio_mean, float pratio_stddev);
            //static bool translateSyllablePitch(const std::wstring &strPitch, float &pratio_mean, float &pratio_stddev);

            static std::wstring constructSyllablePitch(const std::map<std::string, std::string> &params);
            static bool translateSyllablePitch(const std::wstring &strPitch, std::map<std::string, std::string> &params);
            */

        };//CSSMLHelper
    }
}

#endif//_CST_TTS_BASE_SSML_HELPER_H_
