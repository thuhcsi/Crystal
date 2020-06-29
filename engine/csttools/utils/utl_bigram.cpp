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
/// @version 0.1
/// @author Yongxin Wang<fefe.wyx@gmail.com>
/// @date 2006/08/07
///
/// <b>History:</b>
/// - Version: 0.1 \n
///   Author: Yongxin WANG \n
///   Date: 2006/08/07 \n
///   Changed: create
///
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstring>
#include "utl_bigram.h"
#include "utils/utl_string.h"

namespace cst
{

    namespace cmn
    {

        bool CBigram::loadBigramFromARPA(const std::wstring &strFile)
        {
            // clear every thing
            clear();

            std::ifstream infBigram;
            infBigram.open(str::wcstombs(strFile).c_str());
            if (!infBigram)
            {   // open file failed
                return false;
            }

            std::string strLineBuf;
            std::string strTempWord[4]; // to save the words of a line, before knowing there meaning
            int iNGramCount; // how many modle do we have? we should have a unigram modle and a bigram one

            std::getline(infBigram, strLineBuf);

            while (std::strncmp("\\data\\", strLineBuf.c_str(), 6) != 0)
            {
                if (infBigram)
                {
                    std::getline(infBigram, strLineBuf);
                }
                else
                {   // oops, we didn't get what we want.
                    infBigram.close();
                    return false;
                }
            }

            std::getline(infBigram, strLineBuf);

            iNGramCount = 0;
            while (std::strncmp("\\1-grams:", strLineBuf.c_str(), 8) != 0)
            {
                if (std::strncmp("ngram ", strLineBuf.c_str(), 6) == 0)
                {
                    int n; //`n'-gram
                    n = strLineBuf[6] - '0';
                    if (n == 1)
                    {
                        int size;
                        std::istringstream(strLineBuf.c_str() + 8) >> size;

                        if (size > 0)
                        {
                            m_vecPOS.assign(size, L"");
                            m_vecUnigram.assign(size, 0.0);
                            m_vecUnigramBackOff.assign(size, 0.0);
                        }
                        else
                        {
                            clear();
                            return false;
                        }
                    }
                    if (n == 2)
                    {
                        int size = m_vecPOS.size();
                        if (size > 0)
                        {
                            // 1.0 indicates no data
                            // m_vecBigram.assign(size * size, 1.0);
                            m_vecBigram = new double[size*size];
                            for ( int i = 0; i < size*size; i++ )
                            {
                                m_vecBigram[i] = 1.0;
                            }
                        }
                        else
                        {
                            clear();
                            return false;
                        }
                    }
                    iNGramCount++;
                }
                std::getline(infBigram, strLineBuf);
            }

            if (iNGramCount != 2)
            {
                return false;
            }

            // load 1 gram
            std::getline(infBigram, strLineBuf);

            size_t iPosIdx = 0; // index of each POS
            // should make sure in the file that <UNK> has the index of zero
            while (strncmp("\\2-grams:", strLineBuf.c_str(), 8))
            {
                double dUnigramValue;
                double dUnigramBackoff;
                std::istringstream InputLine(strLineBuf);
                InputLine>> dUnigramValue >> strTempWord[0] >> dUnigramBackoff;

                if (InputLine)
                {
                    if (iPosIdx < m_vecPOS.size())
                    {
                        m_vecUnigram[iPosIdx] = dUnigramValue;
                        m_vecUnigramBackOff[iPosIdx] = dUnigramBackoff;
                        m_vecPOS[iPosIdx] = str::mbstowcs(strTempWord[0], str::ENC_GB);
                        iPosIdx++;
                    }
                }
                std::getline(infBigram, strLineBuf);
            }


            // load 2 gram
            std::getline(infBigram, strLineBuf);
            while (strncmp("\\end\\", strLineBuf.c_str(), 5))
            {
                double dBigramValue;

                std::istringstream InputLine(strLineBuf);
                InputLine>> dBigramValue >> strTempWord[0] >> strTempWord[1];

                if (InputLine)
                {
                    int idxFst = getPOSIndex(str::mbstowcs(strTempWord[0], str::ENC_GB));
                    int idxSnd = getPOSIndex(str::mbstowcs(strTempWord[1], str::ENC_GB));

                    if (idxFst == -1 || idxSnd == -1)
                    {
                        clear();
                        return false;
                    }

                    m_vecBigram[idxFst * m_vecPOS.size() + idxSnd] = dBigramValue;
                }
                std::getline(infBigram, strLineBuf);
            }

            return true;
        }

        bool CBigram::saveBigramToNGM(const std::string &strFile)
        {
            std::ofstream outfBigram;
            outfBigram.open(strFile.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
            if (!outfBigram)
            {
                // open file failed
                return false;
            }

            if ( m_vecPOS.size() == 0 || m_vecBigram == NULL )
            {
                // no data
                return false;
            }

            TNGMFileHeader fheader;
            fheader.posCount = m_vecPOS.size();
            fheader.unigramBuffLength = 0;
            outfBigram.seekp(sizeof(fheader));

            for ( uint32 i = 0; i < fheader.posCount; i++ )
            {
                char unigramLine[1024];
                int rlen = sprintf(unigramLine, "%S\t%f\t%f\n",
                    m_vecPOS[i].c_str(),
                    m_vecUnigram[i],
                    m_vecUnigramBackOff[i]);
                outfBigram.write(unigramLine, rlen);
                fheader.unigramBuffLength += rlen;
            }

            outfBigram.write((char *)m_vecBigram, sizeof(double)*fheader.posCount*fheader.posCount);
            outfBigram.seekp(0);
            outfBigram.write((char *)&fheader, sizeof(TNGMFileHeader));
            return true;
        }

        bool CBigram::loadBigramFromNGM(const std::string &strFile)
        {
            // clear every thing
            clear();

            std::ifstream infBigram;
            infBigram.open(strFile.c_str(), std::ios::in|std::ios::binary);
            if (!infBigram)
            {
                // open file failed
                return false;
            }

            // read file header
            TNGMFileHeader fheader;
            infBigram.read((char *)&fheader, sizeof(TNGMFileHeader));
            if ( infBigram.rdstate()&std::ios::eofbit )
            {
                infBigram.close();
                return false;
            }

            // read and parse uni-gram
            if ( true )
            {
                char *buff = new char[fheader.unigramBuffLength+1];
                infBigram.read(buff, fheader.unigramBuffLength);
                if ( infBigram.rdstate()&std::ios::eofbit )
                {
                    delete []buff;
                    infBigram.close();
                    return false;
                }
                buff[fheader.unigramBuffLength] = 0;

                char *ptr = strtok(buff, "\n");
                while ( ptr )
                {
                    wchar_t ptrpos[128];
                    double unigram;
                    double backoff;
                    int fn = sscanf(ptr, "%S\t%lf\t%lf", ptrpos, &unigram, &backoff);
                    if ( fn < 3 )
                    {
                        delete []buff;
                        infBigram.close();
                        return false;
                    }
                    m_vecPOS.push_back(ptrpos);
                    m_vecUnigram.push_back(unigram);
                    m_vecUnigramBackOff.push_back(backoff);

                    ptr = strtok(NULL, "\n");
                }

                delete []buff;
            }

            if ( m_vecPOS.size() != fheader.posCount )
            {
                infBigram.close();
                return false;
            }

            // read 2-gram
            m_vecBigram = new double[fheader.posCount*fheader.posCount];
            infBigram.read((char *)m_vecBigram, sizeof(double)*fheader.posCount*fheader.posCount);
            if ( infBigram.rdstate()&std::ios::eofbit )
            {
                clear();
                infBigram.close();
                return false;
            }

            return true;
        }

        bool CBigram::clear()
        {
            m_vecPOS.clear();
            m_vecUnigram.clear();
            m_vecUnigramBackOff.clear();
            if ( m_vecBigram )
            {
                delete []m_vecBigram;
                m_vecBigram = NULL;
            }
            return true;
        }

        int CBigram::getPOSIndex(const std::wstring &wstrPOS) const
        {
            std::vector<std::wstring>::const_iterator posIt;

            posIt = std::find(m_vecPOS.begin(), m_vecPOS.end(), wstrPOS);
            if (posIt != m_vecPOS.end())
            {
                return (int)(posIt - m_vecPOS.begin());
            }

            std::wstring tmpPOS = wstrPOS.substr(0, wstrPOS.length() - 1);

            while (tmpPOS.length() > 0)
            {
                posIt = std::find(m_vecPOS.begin(), m_vecPOS.end(), tmpPOS);
                if (posIt != m_vecPOS.end())
                {
                    return (int)(posIt - m_vecPOS.begin());
                }
                tmpPOS.erase(tmpPOS.length() - 1, 1);
            }

            return 0; // this should be <UNK>
        }

        double CBigram::getBigramValue(int idxPOS1, int idxPOS2) const
        {
            const int sizeVec = m_vecPOS.size();
            if (idxPOS1 < 0 || idxPOS1 >= sizeVec ||
                idxPOS2 < 0 || idxPOS2 >= sizeVec)
            {
                return -1E30;
            }

            double retVal = m_vecBigram[idxPOS1 * sizeVec + idxPOS2];
            if (retVal > 0.0)
            {
                retVal = m_vecUnigramBackOff[idxPOS1] + m_vecUnigram[idxPOS2];
                // update bigam module
                // m_vecBigram[idxPOS1 * sizeVec + idxPOS2] = retVal;
            }
            return retVal;
        }

        double CBigram::getBigramValue(const std::wstring &wstrPOS1, const std::wstring &wstrPOS2) const
        {
            int idxPOS1 = getPOSIndex(wstrPOS1);
            int idxPOS2 = getPOSIndex(wstrPOS2);
            // leave the check to the index version
            return getBigramValue(idxPOS1, idxPOS2);
        }

    }

}
