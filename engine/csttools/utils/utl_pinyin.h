#ifndef _CST_TTS_BASE_UTL_PINYIN_H_
#define _CST_TTS_BASE_UTL_PINYIN_H_

//#include "cmn/cmn_string.h"
#include "xml/ssml_document.h"

using namespace cst::xml;

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  The class performs the conversion from SSML document to formatted pinyin string
        ///
        class CPinyinConvert
        {
		public:
			// PINYIN_WITH_TONE_NUMBER: e.g. ÂÀ lv3
			// PINYIN_WITH_TONE_SYMBOL: e.g. ÂÀ l¨·, only applicable to Putonghua
			enum PHONEME_OUTPUT_FORMAT {PINYIN_WITH_TONE_NUMBER, PINYIN_WITH_TONE_SYMBOL};

        public:
            ///
            /// @brief  Generate the phoneme string based of specific format, should be called after grapheme_to_phoneme module
            ///
			/// @param	[in]  pSSMLDoc			The SSML document contains the pinyin to be extracted
			/// @param	[in]  pof				The phoneme output format, as defined in enum PHONEME_OUTPUT_FORMAT
			/// @param	[in]  wstrSeparator		The separator used to separate the phonemes, space (" ") by default
            /// @param  [out] wstrPhonemeOutput	The output of phoneme string
            ///
            /// @return true if succeeded
            ///
			static bool generatePhonemeOutput(CSSMLDocument * pSSMLDoc, std::wstring& wstrPhonemeOutput, PHONEME_OUTPUT_FORMAT pof=PINYIN_WITH_TONE_SYMBOL, const std::wstring wstrSeparator=L" ");

		protected:
			static bool formatPhonemeToSymbol(std::wstring& wstrPhoneme);

        };
        
    } // End of namespace cmn
} // End of namespace cst

#endif // End of _CST_TTS_BASE_UTL_PINYIN_H_