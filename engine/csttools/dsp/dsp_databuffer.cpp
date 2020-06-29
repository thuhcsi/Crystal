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
/// @brief  Implementation file for functions of data buffer manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#include <memory.h>
#include "dsp_databuffer.h"

namespace cst
{
    namespace dsp
    {
        CDataBuffer::CDataBuffer(const CDataBuffer &right)
        {
            this->clear();
            this->appendData(right.m_pData, right.m_nLength);
        }

        CDataBuffer &CDataBuffer::operator = (const CDataBuffer &right)
        {
            // handle self assignment
            if (this != &right)
            {
                this->clear();
                this->appendData(right.m_pData, right.m_nLength);
            }
            return (*this);
        }

        bool CDataBuffer::clear()
        {
            delete[] m_pData;
            m_nLength =0;
            m_pData   = 0;
            return true;
        }

        bool CDataBuffer::resize(unsigned long nNewByteSize)
        {
            if (nNewByteSize <= m_nLength)
            {
                // truncate the old data
                m_nLength = nNewByteSize;
                return true;
            }
            else
            {
                // append the zero
                return appendData(0, nNewByteSize-m_nLength);
            }
        }

        bool CDataBuffer::appendData(const unsigned char *pData, unsigned long nByteLen)
        {
            // create buffer
            unsigned char *pTmpData = new unsigned char[m_nLength+nByteLen];
            if (pTmpData==0)
            {
                // memory overflow
                return false;
            }
            memcpy(pTmpData, m_pData, sizeof(unsigned char)*m_nLength);
            if (pData==0)
            {
                // append the zero
                memset(pTmpData+m_nLength, 0, sizeof(unsigned char)*nByteLen);
            }
            else
            {
                // append the data
                memcpy(pTmpData+m_nLength, pData, sizeof(unsigned char)*nByteLen);
            }
            delete[] m_pData;
            m_pData    = pTmpData;
            m_nLength += nByteLen;
            return true;
        }

        unsigned long CDataBuffer::assignData(const unsigned char *pData, unsigned long nByteLen, unsigned long nByteOffset)
        {
            // detect the correct length to copy
            unsigned long toCopy = m_nLength - nByteOffset;
            if (m_nLength <= nByteOffset)
            {
                toCopy = 0;
            }
            else if (m_nLength-nByteOffset > nByteLen)
            {
                toCopy = nByteLen;
            }

            if (pData == 0)
            {
                // assign the zero
                memset(m_pData+nByteOffset, 0, sizeof(unsigned char)*toCopy);
            }
            else
            {
                // copy the data
                memcpy(m_pData+nByteOffset, pData, sizeof(unsigned char)*toCopy);
            }
            return toCopy;
        }
    }
}
