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
/// @brief  Head file for functions of data buffer manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSP_DATABUFFER_H_
#define _CST_TTS_BASE_DSP_DATABUFFER_H_

namespace cst
{
    namespace dsp
    {
        ///
        /// @brief  The class which manipulates the data buffer automatically
        ///
        class CDataBuffer
        {
        public:
            ///
            /// @brief  Default constructor
            ///
            CDataBuffer () : m_pData(0), m_nLength(0) {}

            ///
            /// @brief  Copy constructor
            ///
            CDataBuffer(const CDataBuffer &right);

            ///
            /// @brief  Destructor, clear the data
            ///
            virtual ~CDataBuffer() {clear();}

            ///
            /// @brief  Assignment operator =, assign right operand to left operand.
            ///
            CDataBuffer &operator = (const CDataBuffer &right);

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  Data related procedures
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Clear the data buffer
            ///
            /// @return Whether operation is successful
            ///
            bool clear();

            ///
            /// @brief  Resize the data buffer to the new size
            ///
            /// If new size is smaller than old size, the data buffer will be truncated.
            /// If new size is greater than old size, the empty data (zero) will be appended.
            ///
            /// @param  [in] nNewByteSize   The new size of the data buffer (in byte)
            ///
            /// @return Whether operation is successful
            ///
            /// @note   If bits per sample of the audio data is 16, and the new size (in byte) is odd,
            ///         then the data might be mis-aligned. The caller must ensure to avoid such situation.
            /// @see    alignData
            ///
            bool resize(unsigned long nNewByteSize);

            ///
            /// @brief  Assign the new data to this data from the offset position
            ///
            /// This function will only fill the space from offset position as much as possible.
            /// If the new data size is greater than the space, only available space will be assigned.
            /// If new data is empty, zero will be assigned.
            /// If offset position is greater than the space, no data will be assigned.
            ///
            /// @param  [in] pData          The new data to be assigned, or NULL to assign zero
            /// @param  [in] nByteLen       The size of the new data to be assigned (in byte)
            /// @param  [in] nByteOffset    The offset position in the original data (in byte)
            ///
            /// @return The actual length of the assigned data
            ///
            unsigned long assignData(const unsigned char *pData, unsigned long nByteLen, unsigned long nByteOffset=0);

            ///
            /// @brief  Append the new data buffer to the end of this data
            ///
            /// if input new data is empty (NULL), the input length of zeros will be appended.
            ///
            /// @param  [in] pData      The new data to be appended, or NULL to append zero.
            /// @param  [in] nByteLen   The length of the data to be appended (in byte)
            ///
            /// @return Whether operation is successful
            ///
            bool appendData(const unsigned char *pData, unsigned long nByteLen);

            ///
            /// @brief  Get the data buffer (in byte, unsigned char), the return value is changeable
            ///
            unsigned char *getData() {return m_pData;}

            ///
            /// @brief  Get the data buffer (in byte), the return value is const, unchangeable
            ///
            const unsigned char *getData() const {return m_pData;}

            ///
            /// @brief  Get the length of the data buffer (in byte)
            ///
            unsigned long getLength() const {return m_nLength;}

        protected:
            unsigned char *m_pData;     ///< The data buffer
            unsigned long  m_nLength;   ///< The length of the data buffer (unit: in byte)
        };
    }
}

#endif//_CST_TTS_BASE_DSP_DATABUFFER_H_
