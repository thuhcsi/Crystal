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
/// @brief  Head file for wave file manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_DSP_WAVEFILE_H_
#define _CST_TTS_BASE_DSP_WAVEFILE_H_

namespace cst
{
    namespace dsp
    {
        // forward class reference
        class CWaveData;

        ///
        /// @brief  The class which manipulates wave file
        ///
        class CWaveFile
        {
        public:
            ///
            /// @brief  Definition of open flags for accessing data or specifying the data type
            ///
            /// @note   The explanation of the opening flags combination:<br>
            /// <ul>
            /// <li><b>modeRead</b>: open the file for read only, combination with "modeWrite" or "modeAppend" will discard "modeRead"</li>
            /// <li><b>modeWrite</b>: open empty file for writeing data, destry the content if file exists</li>
            /// <li><b>modeAppend</b>: open file for appending data at end, original data will be kept <i>UNCHANGED</i>, create file if it does not exist</li>
            /// <li><b>modeWrite | modeAppend</b>: same as modeAppend</li>
            /// <li><b>modeRead | modeWrite</b>: same as modeWrite</li>
            /// <li><b>modeRead | modeAppend</b>: same as modeAppend</li>
            ///
            /// <li><b>typeRawData</b>: only write raw wave data, used with "modeRead" will discard "typeRawData"</li>
            /// <li><b>modeRead | typeRawData</b>: discard typeRawData</li>
            /// <li><b>modeWrite | typeRawData</b>: only write raw wave data (no wave file header)</li>
            /// <li><b>modeAppend | typeRawData</b>: depends on the content of the original file, oroginal file head will be kept if it exists</li>
            /// </ul>
            ///
            enum EOpenFlags
            {
                modeRead    =   0x0000, ///< Open the file for reading only, failed if file not found
                modeWrite   =   0x0001, ///< Open an empty file for writing data, destroy the content if file exist
                modeAppend  =   0x2000, ///< Open the file for appending data at end, create the file if it does not exist
                typeRawData =   0x4000, ///< Only write raw wave data (no wave file header), used with modeWrite or modeAppend
            };

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  Static operations for wave data load and save
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Load the wave data from file, original wave data will be destroyed
            ///
            /// @param  [in]  fileName  The name of the wave file to load data from
            /// @param  [out] waveData  Return the loaded wave data, original data will be destroyed
            ///
            /// @return Whether operation is successful
            ///
            static bool load(const wchar_t *fileName, CWaveData &waveData);

            ///
            /// @brief  Save the wave data to file
            ///
            /// @param  [in] fileName   The name of the wave file to save data to
            /// @param  [in] waveData   The wave data to be saved
            /// @param  [in] flags      The saving flags, one of "modeWrite" or "modeAppend" must be used, "typeRawData" is optional
            ///
            /// @see    EOpenFlags
            ///
            /// @return Whether operation is successful
            ///
            static bool save(const wchar_t *fileName, const CWaveData &waveData, unsigned int flags);

            ///
            /// @brief  Test whether the specified file exist
            ///
            /// @param  [in] fileName   Name of the file to be tested
            ///
            static bool fileExist(const wchar_t *fileName);


        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  File related operations
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Default constructor
            ///
            CWaveFile();

            ///
            /// @brief  Destructor, close file
            ///
            virtual ~CWaveFile() {close();}

            ///
            /// @brief  Open the file with specified mode
            ///
            /// @param  [in] fileName   The name of the file to be opened
            /// @param  [in] openFlags  The open flags, supporting the open flags in EOpenFlags
            ///
            /// @see    EOpenFlags
            ///
            /// @return Whether file is opened successfully or not
            ///
            virtual bool open(const wchar_t *fileName, unsigned int openFlags);

            ///
            /// @brief  Close the file if opened
            ///
            /// @return Whether file is closed successfully or not
            ///
            virtual bool close();

            ///
            /// @brief  Read the wave data from file
            ///
            /// @param  [in]  size      Size of the wave data to read (Unit: byte)
            /// @param  [out] buffer    Pointer to the storage for wave data
            ///
            /// @return The number of wave data actually read, in bytes
            ///
            virtual unsigned long read(void *buffer, unsigned long size);

            ///
            /// @brief  Write the wave data to file
            ///
            /// @param  [in] buffer     Pointer to the wave data to be written
            /// @param  [in] size       Size of the wave data to be written (Unit: byte)
            ///
            /// @return The number of wave data actually written, in bytes
            ///
            virtual unsigned long write(const void *buffer, unsigned long size);

            ///
            /// @brief  Seek the file cursor to the position in <b> wave DATA block </b>
            ///
            /// @param  [in] offset     The offset in wave DATA block (Unit: byte)
            /// @param  [in] origin     The origin in wave DATA block, currently <b>only SEEK_SET</b> is supported
            ///
            /// @return Whether operation is successful or not
            ///
            virtual bool seek(long offset, int origin);

            ///
            /// @brief  Return the length of the wave data (only data block) of the opened wave file (in byte)
            ///
            unsigned long getWaveLength() const {return m_nWaveLength;}

        protected:
            ///
            /// @brief  Read the wave file header information from the file
            ///
            /// @return Whether file header does exist and is read successfully
            ///
            bool readFileHeader();

            ///
            /// @brief  Write the wave file header information to the file
            ///
            /// @return Whether file header is written successfully
            ///
            bool writeFileHeader();

            ///
            /// @brief  Search a specific chunk (given the chunk ID) in the file
            ///
            /// @return Return whether the specific chunk is found.
            ///         If chunk found, the file cursor points to the start of this chunk.
            ///
            bool searchChunk(const char *chunkID);

            ///
            /// @brief  Check whether ID parameter is valid (equal to id value)
            ///
            bool checkID(const char *idParam, const char *idValue);

        protected:
            FILE           *m_pFile;            ///< File handle
            bool            m_bWriting;         ///< Indicating whether file is opened for writing
            bool            m_bFormatFixed;     ///< Indicating whether format is fixed (unchangeable for accessing existing file)
            unsigned long   m_nWaveLength;      ///< The wave data length
            unsigned long   m_nHeaderLength;    ///< The wave file header length

        public:
            //////////////////////////////////////////////////////////////////////////
            //
            //  Format related procedures
            //
            //////////////////////////////////////////////////////////////////////////

            ///
            /// @brief  Set the format of the wave data file
            ///
            /// @param  [in] nSamplesPerSec Sample rat, in samples per second (hertz, Hz)
            /// @param  [in] wBitsPerSample Bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
            /// @param  [in] nChannels      Number of channels in the waveform-audio data
            /// @param  [in] wFormat        Waveform-audio format type. Currently, only WAVE_FORMAT_PCM(=1) is supported
            ///
            /// @return Whether operation is successful
            ///
            bool setFormat(unsigned long nSamplesPerSec, unsigned short wBitsPerSample, unsigned short nChannels, unsigned short wFormat=1/*WAVE_FORMAT_PCM*/);

            ///
            /// @brief  Get the waveform-audio format type, currently only WAVE_FORMAT_PCM is supported
            ///
            unsigned short getFormatTag() const {return m_wFormatTag;}

            ///
            /// @brief  Get the number of channels in the waveform-audio data
            ///
            unsigned short getChannels() const {return m_nChannels;}

            ///
            /// @brief  Get the bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
            ///
            unsigned short getBitsPerSample() const {return m_wBitsPerSample;}

            ///
            /// @brief  Get the sample rat, in samples per second (hertz, Hz)
            ///
            unsigned long  getSamplesPerSec() const {return m_nSamplesPerSec;}

        protected:
            unsigned short  m_wFormatTag;       ///< Waveform-audio format type, currently only WAVE_FORMAT_PCM is supported
            unsigned short  m_nChannels;        ///< Number of channels in the waveform-audio data
            unsigned long   m_nSamplesPerSec;   ///< Sampling rate, in samples per second (hertz, Hz)
            unsigned short  m_wBitsPerSample;   ///< Bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16.
        };
    }
}

#endif//_CST_TTS_BASE_DSP_WAVEFILE_H_
