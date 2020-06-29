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
/// @brief  Implementation file for speech related digital signal processing algorithm functions
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Cong Honglei (conghonglei@gmail.com)
///   Date:     2007/06/13
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Refined to static function for all signal processing algorithms
///


#include <math.h>
#include <stdlib.h>
#include "dsp_wavedata.h"
#include "dsp_dspalgorithm.h"

namespace cst
{
    namespace dsp
    {
        const double CDSPAlgorithm::PI = 3.14159265358979;

        void CDSPAlgorithm::getHanningWindow(float *pHanning, unsigned long nLength)
        {
            double multiply = 2 * PI / nLength;

            for (unsigned long i=0; i<nLength; i++)
            {
                pHanning[i] = (float)(0.5 - 0.5 * cos(i * multiply));
            }
        }

        void CDSPAlgorithm::getHanningUpHill(float *pHanning, unsigned long nLength)
        {
            double multiply = PI / (nLength-1); // 2 * PI / (2 * (nLength-1))

            for (unsigned long i=0; i<nLength; i++)
            {
                pHanning[i] = (float)(0.5 - 0.5 * cos(i * multiply));
            }
        }

        void CDSPAlgorithm::getHanningDownHill(float *pHanning, unsigned long nLength)
        {
            double multiply = PI / (nLength-1);    // 2 * PI / (2 * (nLength-1))

            for (unsigned long i=0; i<nLength; i++)
            {
                pHanning[i] = (float)(0.5 - 0.5 * cos((i + nLength-1) * multiply));
            }
        }

        bool CDSPAlgorithm::multiplyWindow(CWaveData &waveData, float *pWindow, unsigned long sampleOffset, unsigned long sampleLength)
        {
            unsigned long nSamples = waveData.getLength() / (waveData.getBitsPerSample() / 8);
            if (sampleOffset + sampleLength > nSamples)
                return false;

            for (unsigned long i=0, sampleIdx=sampleOffset; i<sampleLength; i++, sampleIdx++)
            {
                int val = (int)(pWindow[i] * waveData.getSampleValue(sampleIdx));
                waveData.setSampleValue(sampleIdx, val);
            }
            return true;
        }

        bool CDSPAlgorithm::hanningNormal(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd)
        {
            if (sampleEnd <= sampleStart)
                return false;

            unsigned long sampleLength = sampleEnd - sampleStart;
            float *pHanning = new float[sampleLength];
            if (pHanning == NULL)
                return false;

            getHanningWindow(pHanning, sampleLength);
            bool res = multiplyWindow(waveData, pHanning, sampleStart, sampleLength);
            delete[] pHanning;

            return res;
        }

        bool CDSPAlgorithm::hanningUp(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd)
        {
            if (sampleEnd <= sampleStart)
                return false;

            unsigned long sampleLength = sampleEnd - sampleStart;
            float *pHanning = new float[sampleLength];
            if (pHanning == NULL)
                return false;

            getHanningUpHill(pHanning, sampleLength);
            bool res = multiplyWindow(waveData, pHanning, sampleStart, sampleLength);
            delete[] pHanning;

            return res;
        }

        bool CDSPAlgorithm::hanningDown(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd)
        {
            if (sampleEnd <= sampleStart)
                return false;

            unsigned long sampleLength = sampleEnd - sampleStart;
            float *pHanning = new float[sampleLength];
            if (pHanning == NULL)
                return false;

            getHanningDownHill(pHanning, sampleLength);
            bool res = multiplyWindow(waveData, pHanning, sampleStart, sampleLength);
            delete[] pHanning;

            return res;
        }

        bool CDSPAlgorithm::amplify(CWaveData &waveData, unsigned long sampleStart, unsigned long sampleEnd, float ampScale)
        {
            unsigned long nSamples = waveData.getLength() / (waveData.getBitsPerSample() / 8);
            if (sampleStart > nSamples || sampleEnd > nSamples || sampleEnd < sampleStart)
                return false;

            for (unsigned long i=sampleStart; i<sampleEnd; i++)
            {
                int val = (int)(waveData.getSampleValue(i) * ampScale);
                waveData.setSampleValue(i, val);
            }
            return true;
        }

        bool CDSPAlgorithm::overlapAdd(CWaveData &waveTo, const CWaveData &waveFrom)
        {
            if (waveTo.getLength() < waveFrom.getLength())
            {
                // left (output) wave data having no enough space
                return false;
            }

            // overlap adding now
            unsigned long nFromSamples = waveFrom.getLength() / (waveFrom.getBitsPerSample() / 8);
            for (unsigned long i=0; i<nFromSamples; i++)
            {
                int val = (int)(waveTo.getSampleValue(i) + waveFrom.getSampleValue(i));
                waveTo.setSampleValue(i, val);
            }
            return true;
        }

        bool CDSPAlgorithm::hanningOverlap(const CWaveData &waveLeft, const CWaveData &waveRight, CWaveData &waveOut)
        {
            CWaveData waveTmp;
            // adding the Hanning window
            if (waveLeft.getLength() >= waveRight.getLength())
            {
                waveOut = waveLeft;
                waveTmp = waveRight;
                hanningUp(waveOut, 0, waveOut.getLength() / (waveOut.getBitsPerSample() / 8));
                hanningDown(waveTmp, 0, waveTmp.getLength() / (waveTmp.getBitsPerSample() / 8));
            }
            else
            {
                waveOut = waveRight;
                waveTmp = waveLeft;
                hanningUp(waveTmp, 0, waveTmp.getLength() / (waveTmp.getBitsPerSample() / 8));
                hanningDown(waveOut, 0, waveOut.getLength() / (waveOut.getBitsPerSample() / 8));
            }
            // overlap adding
            return overlapAdd(waveOut, waveTmp);
        }
    }
}
