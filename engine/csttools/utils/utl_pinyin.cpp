
#include <stack>

#include "utl_pinyin.h"

#include "utl_string.h"

namespace cst
{
    namespace cmn
    {
		bool CPinyinConvert::generatePhonemeOutput(CSSMLDocument * pSSMLDoc, std::wstring& wstrPhonemeOutput, PHONEME_OUTPUT_FORMAT pof, const std::wstring wstrSeparator)
		{
			std::stack<CXMLNode *> nodesToVisit;
			std::deque<std::wstring> dequePhoneme;
			dequePhoneme.clear();

			std::wstring wstrTemp;
			std::wstring wstrRest;
			wstrTemp.clear();
			wstrRest.clear();
			wstrPhonemeOutput.clear();

			CXMLNode * pCurrentNode = pSSMLDoc;
			nodesToVisit.push(pCurrentNode);

			std::wstring::size_type posTemp = 0;

			std::wstring wstrTrim = L" ";

			// Use DFS to visit all <phoneme> node in the tree
			while ( !nodesToVisit.empty() )
			{
				pCurrentNode = nodesToVisit.top();
				nodesToVisit.pop();

				if ( pCurrentNode!=NULL && pCurrentNode->type() == CXMLNode::XML_ELEMENT && pCurrentNode->value() == L"phoneme" )
				{
					// Found the <phoneme> node
					wstrRest = ((CXMLElement *)pCurrentNode)->getAttribute(L"ph");
					wstrRest = str::trim(wstrRest,wstrTrim);

					// Remove '-' and divide them into separated phonemes
					do
					{
						posTemp = wstrRest.find_first_of(L"- ");

						if ( posTemp != std::wstring::npos )
						{
							wstrTemp = wstrRest.substr(0,posTemp);
							wstrRest = wstrRest.substr(posTemp+1,wstrRest.length()-posTemp);
						}
						else
						{
							wstrTemp = wstrRest;
							wstrRest.clear();

							if ( wstrTemp.empty() )
							{
								break;
							}
						}

						
						if ( pof == PINYIN_WITH_TONE_SYMBOL )
						{
							if ( ((CXMLElement *)pCurrentNode)->getAttribute(L"alphabet") == L"x-pinyin" )
							{
								formatPhonemeToSymbol(wstrTemp);
							}
							else
							{
								// PINYIN_WITH_TONE_SYMBOL is only available for Putonghua Pinyin
								// ignore error and continue
							}
						}

						dequePhoneme.push_back(wstrTemp);
					}while ( true );

				}

				if ( pCurrentNode->nextSibling()!=NULL )
				{
					nodesToVisit.push(pCurrentNode->nextSibling());
				}

				if ( pCurrentNode->firstChild()!=NULL )
				{
					nodesToVisit.push(pCurrentNode->firstChild());
				}
			}

			while ( !dequePhoneme.empty() )
			{
				wstrPhonemeOutput += dequePhoneme.front();
				wstrPhonemeOutput += wstrSeparator;
				dequePhoneme.pop_front();
			}

			return true;
		}

		bool CPinyinConvert::formatPhonemeToSymbol(std::wstring& wstrPhoneme)
		{
			if ( wstrPhoneme.empty() )
			{
				return false;
			}

			wchar_t wcVowelAfterModification = L'\0';
			unsigned int uiTone;
			std::wstring::size_type posVowel = std::wstring::npos;

			// Find the tone, and remove the tone
			uiTone = wstrPhoneme.at(wstrPhoneme.length()-1) - L'0';
			wstrPhoneme.erase(wstrPhoneme.length()-1);

			// Find the vowel to takes the diacritic

			// Refer to: http://zh.wikipedia.org/wiki/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3

			// Rule 1: If there is an "a", it will take the tone mark.
			// Rule 2: If there is an "o" or "e" when "a" is absent, it will take the tone mark. (not such combination: "oe" or "eo")
			// Rule 3: If there is a single"i" or single "u" or single "v" when "a" "o" "e" is absent, it will take the tone mark.

			// Speical Rule 1: "iu" and "ui", mark at the second one
			// Speical Rule 2: for "v", will be "u" for initials of "j", "q" and "x", otherwise will be "и╣"

			if ( (posVowel=wstrPhoneme.find('a')) != std::wstring::npos ) // Rule 1
			{
				wcVowelAfterModification = 'a';
			}
			else if ( (posVowel=wstrPhoneme.find('o')) != std::wstring::npos ) // Rule 2: "o"
			{
				wcVowelAfterModification = 'o';
			}
			else if ( (posVowel=wstrPhoneme.find('e')) != std::wstring::npos ) // Rule 2: "e"
			{
				wcVowelAfterModification = 'e';
			}
			else if ( (posVowel=wstrPhoneme.find(L"iu")) != std::wstring::npos ) // Special Rule 1: "iu"
			{
				posVowel ++;
				wcVowelAfterModification = 'u';
			}
			else if ( (posVowel=wstrPhoneme.find(L"ui")) != std::wstring::npos ) // Special Rule 1: "ui"
			{
				posVowel ++;
				wcVowelAfterModification = 'i';
			}
			else if ( (posVowel=wstrPhoneme.find('i')) != std::wstring::npos ) // Rule 3: "i"
			{
				wcVowelAfterModification = 'i';
			}
			else if ( (posVowel=wstrPhoneme.find('u')) != std::wstring::npos ) // Rule 3: "u"
			{
				wcVowelAfterModification = 'u';
			}
			else if ( (posVowel=wstrPhoneme.find('v')) != std::wstring::npos ) // Rule 3: "v"
			{
				// Find the initial
				wchar_t wcInitial = wstrPhoneme.at(0);

				switch ( wcInitial )
				{
				case 'j':
				case 'J':
				case 'q':
				case 'Q':
				case 'x':
				case 'X':
					wcVowelAfterModification = 'u'; // Special Rule 2
					break;
				default:
					wcVowelAfterModification = 'v'; // Special Rule 2
					break;
				}
			}

			// Replace the vowel
			switch ( wcVowelAfterModification )
			{
			case 'a': // ибивигид
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u0101':(uiTone==2)?L'\u00E1':(uiTone==3)?L'\u01CE':(uiTone==4)?L'\u00E0':L'a');
				break;
			case 'e': // иеижизии
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u0113':(uiTone==2)?L'\u00E9':(uiTone==3)?L'\u011B':(uiTone==4)?L'\u00E8':L'e');
				break;
			case 'i': // ийикилим
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u012B':(uiTone==2)?L'\u00ED':(uiTone==3)?L'\u01D0':(uiTone==4)?L'\u00EC':L'i');
				break;
			case 'o': // иниоипи░
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u014D':(uiTone==2)?L'\u00F3':(uiTone==3)?L'\u01D2':(uiTone==4)?L'\u00F2':L'o');
				break;
			case 'u': // и▒и▓и│и┤
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u016B':(uiTone==2)?L'\u00FA':(uiTone==3)?L'\u01D4':(uiTone==4)?L'\u00F9':L'u');
				break;
			case 'v': // и╡и╢и╖и╕и╣
				wstrPhoneme.replace(posVowel,1,1, (uiTone==1)?L'\u01D6':(uiTone==2)?L'\u01D8':(uiTone==3)?L'\u01DA':(uiTone==4)?L'\u01DC':L'\u00FC');
				break;
			default:
				// Error
				break;
			}

			return true;
		}

    } // End of namespace cmn
} // End of namespace cst