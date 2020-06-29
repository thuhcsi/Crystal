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
/// @brief  Implementation file for wave file manipulation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/07/13
///   Changed:  Created
///


#include <string.h>
#include <stdio.h>
#include "dsp_wavedata.h"
#include "dsp_wavefile.h"
#include "cmn/cmn_file.h"

namespace cst
{
    namespace dsp
    {
        ///
        /// @brief  Define the necessary format for wave file headers
        ///
        namespace NWaveFileFmt
        {
            ///
            /// @brief  The format for RIFF header
            ///
            struct TRiffHeader
            {
                char riffID[4];             ///< Riff ID: "RIFF"
                unsigned int  riffSize;     ///< Riff block size from next byte (excluding RIFF ID and this size = whole file size - 8)
                char riffFormat[4];         ///< Wave indicator: "WAVE"
            };

            ///
            /// @brief  The format for RIFF chunks
            ///
            struct TRiffChunk
            {
                char chunkID[4];            ///< Chunk ID: "fmt " (including space) for format chunk, "data" for data chunk
                unsigned int  chunkSize;    ///< Size of the chunk data from next byte (excluding chunk ID and this size: -8)
            };

            ///
            /// @brief  The wave format information
            ///
            struct TWaveFormat
            {
                unsigned short wFormatTag;      ///< Waveform-audio format type, WAVE_FORMAT_PCM=1
                unsigned short nChannels;       ///< Number of audio channels (i.e. mono=1, stereo=2, etc)
                unsigned int   nSamplePerSec;   ///< Sample rate, in samples per second (hertz)
                unsigned int   nAvgBytesPerSec; ///< For buffer rate estimation
                unsigned short nBlockAlign;     ///< Block alignment, in bytes
                unsigned short wBitsPerSample;  ///< Bits per sample. If wFormatTag is WAVE_FORMAT_PCM, it should be 8 or 16
            };
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  Wave file manipulation
        //
        //////////////////////////////////////////////////////////////////////////

        CWaveFile::CWaveFile()
        {
            m_pFile = NULL;
            m_bWriting = false;
            m_bFormatFixed = false;
            m_nWaveLength = 0;
            m_nHeaderLength = 0;
            setFormat(16000, 16, 1);
        }

        bool CWaveFile::setFormat(unsigned long nSamplesPerSec, unsigned short wBitsPerSample, unsigned short nChannels, unsigned short wFormat)
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

            if (m_bFormatFixed)
            {
                // if format is read from existing file, it can not be changed
                bool bSameFormat = (
                    (m_nSamplesPerSec == nSamplesPerSec) &&
                    (m_wBitsPerSample == wBitsPerSample) &&
                    (m_nChannels      == nChannels ) &&
                    (m_wFormatTag     == wFormat)
                    );
                return bSameFormat;
            }

            // set new format
            m_nSamplesPerSec = nSamplesPerSec;
            m_wBitsPerSample = wBitsPerSample;
            m_nChannels      = nChannels;
            m_wFormatTag     = wFormat;
            return true;
        }

        bool CWaveFile::open(const wchar_t *fileName, unsigned int openFlags)
        {
            if (!close())
                return false;

            // get the file open mode
            // the default mode is reading
            const wchar_t *mode = L"rb";
            m_bWriting = false;
            if ((openFlags & modeAppend) != 0)
            {
                // create file if necessary
                FILE *pTmpFile = cmn::wfopen(fileName, L"a+");
                fclose(pTmpFile);

                // now open for both reading and writing
                mode = L"r+b";
                m_bWriting = true;
            }
            else if ((openFlags & modeWrite) != 0)
            {
                // open new file for writing
                mode = L"w+b";
                m_bWriting = true;
                m_bFormatFixed = false;
                m_nHeaderLength = 0;
                m_nWaveLength = 0;
            }

            // open file
            m_pFile = cmn::wfopen(fileName, mode);
            if (m_pFile == NULL)
            {
                close();
                return false;
            }

            if (mode[0] == L'r')
            {
                // reading or appending data
                // read information from file, including
                // (1) format, (2) header length, (3) wave length
                //
                // if file header exists (function returns true), format is not changeable
                // otherwise, format is changeable
                m_bFormatFixed = readFileHeader();
            }

            if (m_bWriting && (openFlags & typeRawData)==0 && m_nHeaderLength==0)
            {
                // writing data with file head
                // write the file head to reserve space for later update
                //
                // after function returns, the necessary information is updated, including
                // (1) format, (2) header length, (3) wave length
                writeFileHeader();
            }

            if (m_bWriting)
            {
                // if writing, moving the cursor to the end
                // and for appending, it is also necessary to switch from reading (file header) to writing
                fseek(m_pFile, m_nHeaderLength + m_nWaveLength, SEEK_SET);
            }
            return true;
        }

        bool CWaveFile::close()
        {
            if (m_pFile != NULL)
            {
                // try to write file head
                if (m_bWriting && m_nHeaderLength!=0)
                {
                    writeFileHeader();
                }
                fclose(m_pFile);
            }

            // clear data
            m_pFile = NULL;
            m_bWriting = false;
            m_bFormatFixed = false;
            m_nWaveLength = 0;
            m_nHeaderLength = 0;
            return true;
        }

        unsigned long CWaveFile::write(const void *buffer, unsigned long size)
        {
            if (m_pFile == NULL)
                return 0;

            unsigned long ret = (unsigned long)fwrite(buffer, sizeof(char), size, m_pFile);

            // update wave data length
            m_nWaveLength += ret;
            return ret;
        }

        unsigned long CWaveFile::read(void *buffer, unsigned long size)
        {
            if (m_pFile == NULL)
                return 0;

            return (unsigned long)fread(buffer, sizeof(char), size, m_pFile);
        }

        bool CWaveFile::seek(long offset, int origin)
        {
            if (origin != SEEK_SET)
                return false;
            // add the file head size
            offset += m_nHeaderLength;
            return (fseek(m_pFile, offset, SEEK_SET) == 0);
        }

        bool CWaveFile::checkID(const char *idParam, const char *idValue)
        {
            return (
                (idParam[0] == idValue[0]) &&
                (idParam[1] == idValue[1]) &&
                (idParam[2] == idValue[2]) &&
                (idParam[3] == idValue[3])
                );
        }

        bool CWaveFile::searchChunk(const char *chunkID)
        {
            using namespace NWaveFileFmt;

            // search for the chunk with the input ID
            TRiffChunk riffChunk;
            while (fread(&riffChunk, sizeof(TRiffChunk), 1, m_pFile) == 1)
            {
                if (checkID(riffChunk.chunkID, chunkID))
                {
                    // found the chunk ID, break
                    break;
                }
                // skip the unknown block with the chunk data size
                fseek(m_pFile, riffChunk.chunkSize, SEEK_CUR);
            }
            if (!checkID(riffChunk.chunkID, chunkID))
                return false;

            // chunk found, set the cursor to the start of this chunk
            fseek(m_pFile, -(long)sizeof(TRiffChunk), SEEK_CUR);
            return true;
        }

        bool CWaveFile::readFileHeader()
        {
            using namespace NWaveFileFmt;

            // read and check the "RIFF" format
            // if not valid "RIFF" format, then treat the input file as raw wave file

            // set the default value in case of failure:
            // (1) set header length to 0,
            // (2) set wave length to file size
            fseek(m_pFile, 0, SEEK_END);
            m_nHeaderLength = 0;
            m_nWaveLength = ftell(m_pFile);

            // now begin to check "RIFF" format chunks one by one:
            // (1) "RIFF" chunk is always the first chunk,
            // (2) "fmt " chunk should be present before the "data" chunk

            // check "RIFF" block
            TRiffHeader riffHeader;
            fseek(m_pFile, 0, SEEK_SET);
            if (fread(&riffHeader, sizeof(TRiffHeader), 1, m_pFile) != 1)
                return false;
            if (!checkID(riffHeader.riffID, "RIFF"))
                return false;
            if (!checkID(riffHeader.riffFormat, "WAVE"))
                return false;

            // search for "fmt " block
            if (!searchChunk("fmt "))
                return false;

            // read "fmt " chunk header
            TRiffChunk fmtBlock;
            if (fread(&fmtBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                return false;
            if (!checkID(fmtBlock.chunkID, "fmt "))
                return false;
            if (fmtBlock.chunkSize < sizeof(TWaveFormat))
            {
                // the format size can be 16 (sizeof(TWaveFormat)) or 18 (for misalignment)
                return false;
            }

            // now it is format block, read wave format
            char *dummy = new char[fmtBlock.chunkSize];
            if (fread(dummy, sizeof(char), fmtBlock.chunkSize, m_pFile) != fmtBlock.chunkSize)
            {
                delete[] dummy;
                return false;
            }
            TWaveFormat waveFormat;
            memcpy(&waveFormat, dummy, sizeof(TWaveFormat));
            delete[] dummy;

            // search for "data" block
            if (!searchChunk("data"))
                return false;

            // read "data" chunk header
            TRiffChunk dataBlock;
            if (fread(&dataBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                return false;
            if (!checkID(dataBlock.chunkID, "data"))
                return false;

            // now both data and header read successfully
            m_nWaveLength   = dataBlock.chunkSize;
            m_nHeaderLength = ftell(m_pFile);

            return setFormat(waveFormat.nSamplePerSec, waveFormat.wBitsPerSample, waveFormat.nChannels, waveFormat.wFormatTag);
        }

        bool CWaveFile::writeFileHeader()
        {
            using namespace NWaveFileFmt;

            if (m_nHeaderLength != 0)
            {
                // now is append data to existing file
                // update header information if necessary

                // "RIFF"
                TRiffHeader riffHeader;
                strncpy(riffHeader.riffID, "RIFF", 4);
                riffHeader.riffSize = m_nHeaderLength + m_nWaveLength - 8;
                strncpy(riffHeader.riffFormat, "WAVE", 4);

                fseek(m_pFile, 0, SEEK_SET);
                if (fwrite(&riffHeader, sizeof(TRiffHeader), 1, m_pFile) != 1)
                    return false;

                // "fmt "
                fseek(m_pFile, 0, SEEK_CUR);    // needed to switch between writing and reading
                if (!searchChunk("fmt "))
                    return false;

                // write "fmt " chunk
                TRiffChunk fmtBlock;
                strncpy(fmtBlock.chunkID, "fmt ", 4);
                fmtBlock.chunkSize = sizeof(TWaveFormat);
                TWaveFormat waveFormat;
                waveFormat.wFormatTag       = m_wFormatTag;
                waveFormat.nChannels        = m_nChannels;
                waveFormat.nSamplePerSec    = m_nSamplesPerSec;
                waveFormat.nBlockAlign      = (m_wBitsPerSample>>3) * m_nChannels;
                waveFormat.nAvgBytesPerSec  = waveFormat.nBlockAlign * m_nSamplesPerSec;
                waveFormat.wBitsPerSample   = m_wBitsPerSample;

                if (fwrite(&fmtBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                    return false;
                if (fwrite(&waveFormat, sizeof(TWaveFormat), 1, m_pFile) != 1)
                    return false;

                // "data"
                fseek(m_pFile, 0, SEEK_CUR);    // needed to switch between writing and reading
                if (!searchChunk("data"))
                    return false;

                // write "data" chunk
                TRiffChunk dataBlock;
                strncpy(dataBlock.chunkID, "data", 4);
                dataBlock.chunkSize = m_nWaveLength;

                if (fwrite(&dataBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                    return false;
            }
            else
            {
                // writing new header
                m_nHeaderLength = sizeof(TRiffHeader) + sizeof(TRiffChunk) + sizeof(TWaveFormat) + sizeof(TRiffChunk);

                // "RIFF"
                TRiffHeader riffHeader;
                strncpy(riffHeader.riffID, "RIFF", 4);
                riffHeader.riffSize = m_nHeaderLength + m_nWaveLength - 8;
                strncpy(riffHeader.riffFormat, "WAVE", 4);
                
                // "fmt "
                TRiffChunk fmtBlock;
                strncpy(fmtBlock.chunkID, "fmt ", 4);
                fmtBlock.chunkSize = sizeof(TWaveFormat);
                TWaveFormat waveFormat;
                waveFormat.wFormatTag       = m_wFormatTag;
                waveFormat.nChannels        = m_nChannels;
                waveFormat.nSamplePerSec    = m_nSamplesPerSec;
                waveFormat.nBlockAlign      = (m_wBitsPerSample>>3) * m_nChannels;
                waveFormat.nAvgBytesPerSec  = waveFormat.nBlockAlign * m_nSamplesPerSec;
                waveFormat.wBitsPerSample   = m_wBitsPerSample;

                // "data"
                TRiffChunk dataBlock;
                strncpy(dataBlock.chunkID, "data", 4);
                dataBlock.chunkSize = m_nWaveLength;

                // write header now
                fseek(m_pFile, 0, SEEK_SET);
                if (fwrite(&riffHeader, sizeof(TRiffHeader), 1, m_pFile) != 1)
                    return false;
                if (fwrite(&fmtBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                    return false;
                if (fwrite(&waveFormat, sizeof(TWaveFormat), 1, m_pFile) != 1)
                    return false;
                if (fwrite(&dataBlock, sizeof(TRiffChunk), 1, m_pFile) != 1)
                    return false;
            }
            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        //
        //  Static operations for wave data load and save
        //
        //////////////////////////////////////////////////////////////////////////

        bool CWaveFile::load(const wchar_t *fileName, CWaveData &waveData)
        {
            // clear data first
            waveData.clear();

            // open file for reading only
            CWaveFile waveFile;
            if (!waveFile.open(fileName, modeRead))
                return false;

            // set data format
            waveData.setFormat(waveFile.getSamplesPerSec(), waveFile.getBitsPerSample(), waveFile.getChannels(), waveFile.getFormatTag());

            // malloc buffer
            unsigned long dataLength = waveFile.getWaveLength();
            if (!waveData.resize(dataLength))
                return false;

            // read data
            if (waveFile.read(waveData.getData(), dataLength) != dataLength)
                return false;

            return true;
        }

        bool CWaveFile::save(const wchar_t *fileName, const CWaveData &waveData, unsigned int flags)
        {
            // one of "modeWrite" or "modeAppend" must be set
            if ((flags & modeWrite) == 0 && (flags & modeAppend) == 0)
                return false;

            // first open file if existing,
            // then check format for matching or not, (return false immediately if format not match)
            // then write data
            CWaveFile waveFile;
            if (!waveFile.open(fileName, flags))
                return false;
            if (!waveFile.setFormat(waveData.getSamplesPerSec(), waveData.getBitsPerSample(), waveData.getChannels(), waveData.getFormatTag()))
                return false;
            if (waveFile.write(waveData.getData(), waveData.getLength()) != waveData.getLength())
                return false;
            if (!waveFile.close())
                return false;

            return true;
        }

        bool CWaveFile::fileExist(const wchar_t *fileName)
        {
            FILE * inf = NULL;
            if (fileName == NULL || (inf = cmn::wfopen(fileName, L"rb")) == NULL)
            {
                return false;
            }
            fclose(inf);
            return true;
        }
    }
}
