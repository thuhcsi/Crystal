// CrystalTest/main.cpp : Defines the entry point for the console application.
//

#include <locale.h>
#include <string>
#include <fstream>
#include <iostream>

#include "utils/utl_string.h"
#include "dsp/dsp_wavefile.h"
#include "dsp/dsp_waveplay.h"
#include "ttsbase/tts.text/tts_textparser.h"
#include "ttsbase/tts.synth/tts_synthesizer.h"

using namespace cst;
using namespace cst::tts;

#define SUPPORT_STEP

void displayUsage()
{
    std::wcout << std::endl;
    std::wcout << L"Welcome to Crystal TTS engine." << std::endl;
    std::wcout << L"Supporting commands:" << std::endl;
    std::wcout << L"  help - display usage information" << std::endl;
    std::wcout << L"  exit - exit the engine demo system" << std::endl;
#if defined(SUPPORT_STEP)
    std::wcout << L"  step - toggle step-by-step mode ON or OFF" << std::endl;
    std::wcout << L"  ssml - toggle the support of partial SSML tag ON or OFF" << std::endl;
#endif
    std::wcout << L"  speak  text - speak the input text" << std::endl;
    std::wcout << L"  speakW text - speak the input text to wave file" << std::endl;
    std::wcout << std::endl;
}

void displayStepMode(bool isStepMode)
{
    std::wcout << std::endl;
    if (isStepMode)
        std::wcout << L"Step-by-step mode: ON" << std::endl;
    else
        std::wcout << L"Step-by-step mode: OFF" << std::endl;
    std::wcout << std::endl;
}

int speakText(base::CTextParser *pTextParser, base::CSynthesizer *pSynthesizer, const std::wstring &strContent, bool isStepMode, bool isPartialSSML, const std::wstring &strOutWaveFile, bool playResult)
{
    // create new SSML document
    xml::CSSMLDocument *pDocument = new xml::CSSMLDocument();

    // pre-process
    pTextParser->preProcess(pDocument, strContent, isPartialSSML ? ITT_TEXT_SSML_NO_ROOT : ITT_TEXT_RAW);

    // encoding conversion
    pTextParser->langConvert(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after encoding conversion ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // text piece segmentation
    pTextParser->textSegment(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after text piece segmentation ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // document structure analysis
    pTextParser->docStructAnalyze(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after document structure analysis ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // text normalization
    pTextParser->textNormalize(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after text normalization ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // word segmentation
    pTextParser->wordSegment(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after word segmentation ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // prosodic structure generation
    pTextParser->prosodyStructGenerate(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after prosodic structure generation ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // grapheme to phoneme
    pTextParser->graphemeToPhoneme(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after grapheme to phoneme conversion ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // unit segment
    pSynthesizer->unitSegment(pDocument);
    // prosodic prediction
    pSynthesizer->prosodyPredict(pDocument);
    if (isStepMode)
    {
        std::wcout << L"========== Result after unit segment and prosodic prediction ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    // speech synthesize
    dsp::CWaveData waveData;
    pSynthesizer->waveSynthesize(pDocument, waveData);
    if (isStepMode)
    {
        std::wcout << L"========== Result after concatenation synthesis ==========" << std::endl;
        pDocument->print(std::wcout);
        getchar();
    }

    delete pDocument;

    // save to wave file
    if (strOutWaveFile.length() != 0)
    {
        // save to wave file
        dsp::CWaveFile::save(strOutWaveFile.c_str(), waveData, dsp::CWaveFile::modeWrite);
    }

    // play synthetic result
    if (playResult)
    {
        if (isStepMode)
        {
            std::wcout << L"========== Playing synthetic result ==========" << std::endl;
            std::wcout << strContent << std::endl;
        }
        //dsp::CWavePlay::playWaveFile(strOutWaveFile.c_str(), dsp::CWavePlay::ModeSync);
        dsp::CWavePlay::playWaveData(waveData, dsp::CWavePlay::ModeAsync | dsp::CWavePlay::ModeWait);
    }
    
    std::wcout << std::endl;
    return ERROR_SUCCESS;
}

void parseCommand(const std::wstring &cmdline, std::wstring &command, std::wstring &argument)
{
    size_t i = 0, j = 0, len = cmdline.length();
    const wchar_t *pcmd = cmdline.c_str();

    // get command
    for (; i<len && iswspace(pcmd[i]); i++);        // skip leading spaces
    for (j=i; i<len && !iswspace(pcmd[i]); i++);    // get command
    command = cmdline.substr(j, i-j);

    // get argument
    for (; i<len && iswspace(pcmd[i]); i++);        // skip middle spaces
    for (j=len; j>i && iswspace(pcmd[j-1]); j--);   // skip ending spaces

    argument = cmdline.substr(i, j-i);
}

#ifdef WINCE
int wmain(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
#if defined(WIN32)
    std::wcin.imbue(std::locale("chs", LC_CTYPE));
    std::wcout.imbue(std::locale("chs", LC_CTYPE));
    //setlocale(LC_CTYPE, "chs");//".936");
#elif defined(__GNUC__)
    std::setlocale(LC_CTYPE, "en_US.UTF-8");
#endif

    if (argc != 2)
    {
        std::wcout << L"Usage: CrystalTest config_file" << std::endl;
        std::wcout << L"       config_file: Configuration file for initializing TTS engine" << std::endl;
        return -1;
    }

    std::wcout << L"Initializing..." << std::endl;

    // initialize the TTS engine
    std::wstring strCfgFile = str::mbstowcs(argv[1]);
    base::CTextParser  *pTextParser  = new base::CTextParser();
    base::CSynthesizer *pSynthesizer = new base::CSynthesizer();
    bool bsucc =      pTextParser != NULL &&  pTextParser->initialize(strCfgFile.c_str()) == ERROR_SUCCESS; // initialize text parser
    bsucc = bsucc && pSynthesizer != NULL && pSynthesizer->initialize(strCfgFile.c_str()) == ERROR_SUCCESS; // initialize synthesizer
    if (!bsucc)
    {
        std::wcout << std::endl << L"TTS engine initialization error, please ensure the correct path." << std::endl;
        delete pTextParser;
        delete pSynthesizer;
        return -1;
    }
    std::wcout << L"Done!" << std::endl;

    //std::wstring input;
    //input = L"<s>太平山顶是香港最受欢迎的名胜景点之一</s><s>登临其间可俯瞰山下鳞次栉比的摩天高楼</s>";
    //input = L"<s><w><phoneme alphabet='pinyin' ph='hai6'>系</phoneme><phoneme alphabet='pinyin' ph='tung2'>统</phoneme><phoneme alphabet='pinyin' ph='gung1'>工</phoneme><phoneme alphabet='pinyin' ph='cing4'>程</phoneme></w></s>";
    //input = L"系統<prosody pitch='x-high'>工程</prosody>與<break/>工程管理學系";
    //input = L"系統工程與工程管理學系";
    //input = L"<s>太平山頂是香港最受歡迎的名勝景點之一</s><s>登臨其間可俯瞰山下鱗次櫛比的摩天高樓</s>";

    // main loop routine
    displayUsage();
    bool isStepMode = false;
    bool isPartialSSML = false;
    std::wstring commands = L"";
    std::wstring command  = L"";
    std::wstring strText  = L"";

    for (;;)
    {
        std::wcout << L"> ";
        std::getline(std::wcin, commands);
        parseCommand(commands, command, strText);

        if (command == L"exit")
        {
            break;
        }
        else if (command == L"help")
        {
            displayUsage();
        }
#if defined(SUPPORT_STEP)
        else if (command == L"step")
        {
            isStepMode = !isStepMode;
            displayStepMode(isStepMode);
        }
        else if (command == L"ssml")
        {
            isPartialSSML = !isPartialSSML;
            std::wcout << std::endl;
            if (isPartialSSML)
                std::wcout << "Support of partial SSML tag: ON" << std::endl;
            else
                std::wcout << "Support of partial SSML tag: OFF" << std::endl;
            std::wcout << std::endl;
        }
#endif
        else if (command == L"speak")
        {
            speakText(pTextParser, pSynthesizer, strText, isStepMode, isPartialSSML, L"", true);
        }
        else if (command == L"speakW")
        {
            std::wstring waveFile;
            std::wcout << L"Wave file name: ";
            std::getline(std::wcin, waveFile);
            speakText(pTextParser, pSynthesizer, strText, isStepMode, isPartialSSML, waveFile, false);
        }
        else
        {
            displayUsage();
        }
    }

    // close the TTS engine
    if (pTextParser->terminate() != ERROR_SUCCESS || pSynthesizer->terminate() != ERROR_SUCCESS)
    {
        std::wcout << L"Engine terminating failed!" << std::endl;
        return -1;
    }
    delete pTextParser;
    delete pSynthesizer;
    pTextParser  = NULL;
    pSynthesizer = NULL;

    // program ends
    std::wcout << L"Engine exits successfully. Bye-bye." << std::endl;
    std::wcout << std::endl;

    return 0;
}
