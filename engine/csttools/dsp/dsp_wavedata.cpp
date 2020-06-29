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
/// @brief  Implementation file for functions of wave data manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#include "dsp_wavedata.h"

namespace cst
{
    namespace dsp
    {
        CWaveData::CWaveData(const CWaveData &right)
        {
            this->clear();
            this->setFormat(right.m_nSamplesPerSec, right.m_wBitsPerSample, right.m_nChannels, right.m_wFormatTag);
            this->appendData(right.m_pData, right.m_nLength);
        }

        CWaveData &CWaveData::operator = (const CWaveData &right)
        {
            // handle self assignment
            if (this != &right)
            {
                this->clear();
                this->setFormat(right.m_nSamplesPerSec, right.m_wBitsPerSample, right.m_nChannels, right.m_wFormatTag);
                this->appendData(right.m_pData, right.m_nLength);
            }
            return (*this);
        }

        bool CWaveData::setFormat(unsigned long nSamplesPerSec, unsigned short wBitsPerSample, unsigned short nChannels, unsigned short wFormat)
        {
            if (wFormat != 1)
            {
                // currently, only WAVE_FORMAT_PCM(=1) is supported
                return false;
            }
            if (wBitsPerSample != 8 && wBitsPerSample != 16)
            {
                // only 8 or 16 is supported for WAVE_FORMAT_PCM
                return false;
            }

            // clear all the data
            clear();

            // set the format
            m_nSamplesPerSec = nSamplesPerSec;
            m_wBitsPerSample = wBitsPerSample;
            m_nChannels      = nChannels;
            m_wFormatTag     = wFormat;
            return true;
        }

        bool CWaveData::alignData()
        {
            if ((m_wBitsPerSample == 16) && (m_nLength & 1))
            {
                // the bits per sample is 16, the length is odd
                // append one zero byte
                appendData(0, 1);
            }
            return true;
        }

        int CWaveData::getSampleValue( unsigned long sampleIndex) const
        {
            int val = 0;
            if (getBitsPerSample() == 16)
            {
                val = *((const short*)getData() + sampleIndex);
            }
            else if (getBitsPerSample() == 8)
            {
                val = ((short)*(getData() + sampleIndex) - 128) << 8;
            }
            return val;
        }

        void CWaveData::setSampleValue( unsigned long sampleIndex, int sampleValue)
        {
            sampleValue = (sampleValue>  32767) ?  32767 : sampleValue;
            sampleValue = (sampleValue < -32768) ? -32768 : sampleValue;

            if (getBitsPerSample() == 16)
            {
                *((short*)getData() + sampleIndex) = sampleValue;
            }
            else if (getBitsPerSample() == 8)
            {
                *(getData() + sampleIndex) = (unsigned char)((sampleValue >> 8) + 128);
            }
        }

    }
}
