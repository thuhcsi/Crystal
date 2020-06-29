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
/// @brief  Definition of encapsulated file manipulation.
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/06/14
///   Changed:  Created
///

#ifndef _CST_TOOLS_CMN_FILE_H_
#define _CST_TOOLS_CMN_FILE_H_

#include <cstdio>

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  Open the file with specified mode (wide-character version).
        ///         Return the handle to the opened file, which can be closed with fclose().
        ///
        /// @param  [in] filename   The name of the file to be opened
        /// @param  [in] mode       Type of access permitted, takes the values as defined in "<stdio.h>"
        ///
        /// @return Handle to the opened file.  NULL if open failed.
        ///
        /// @note   On Windows, this function calls _wfopen() directly.
        ///         On Linux, this function first converts the parameters to UTF-8, and then calls fopen().
        ///
        FILE* wfopen(const wchar_t *filename, const wchar_t *mode);


        ///
        /// @brief  The base encapsulated class for file manipulation
        ///
        /// This encapsulated class is provided for easy file manipulation 
        /// by hiding the FILE related functions for later easy migration.
        ///
        class CFile
        {
        public:
            ///
            /// @brief  Default constructor
            ///
            CFile() : m_pFile(NULL) {}

            ///
            /// @brief  Default destructor
            ///
            virtual ~CFile() {close();}

            ///
            /// @brief  Open the file with specified mode
            ///
            /// @param  [in] filename   The name of the file to be opened
            /// @param  [in] mode       Type of access permitted, takes the values as defined in "<stdio.h>"
            ///
            /// @return Whether filename is opened successfully or not
            ///
            virtual bool open(const wchar_t *filename, const wchar_t *mode);

            ///
            /// @brief  Close the file if opened
            ///
            /// @return Whether file is closed successfully or not
            ///
            virtual bool close();

            ///
            /// @brief  Moves the file pointer to a specified location
            ///
            /// @param  [in] offset     Number of bytes from origin
            /// @param  [in] origin     Initial position, takes the values of SEEK_CUR, SEEK_END, SEEK_SET as defined in "<stdio.h>"
            ///
            /// @return Whether operation is successful or not
            ///
            virtual bool seek(long long offset, int origin) const;

            ///
            /// @brief  Gets the current position of the file pointer
            ///
            /// @return The current position of the file pointer
            ///
            virtual long long tell() const;

            ///
            /// @brief  Read the data from file
            ///
            /// @param  [out] buffer    Storage location for data
            /// @param  [in]  size      Item size in bytes
            /// @param  [in]  count     Maximum number of items to be read
            ///
            /// @return The number of full items actually read
            ///
            virtual size_t read(void *buffer, size_t size, size_t count) const;

            ///
            /// @brief  Write the data to file
            ///
            /// @param  [in] buffer     Pointer to data to be written
            /// @param  [in] size       Item size in bytes
            /// @param  [in] count      Maximum number of items to be written
            /// @return The number of full items actually written
            ///
            virtual size_t write(const void *buffer, size_t size, size_t count) const;

            ///
            /// @brief  Detect whether file has been opened
            ///
            virtual bool opened() const
            {
                return (m_pFile != NULL);
            }

        protected:
            FILE *m_pFile;  ///< File handle
        };

    }
}

#endif//_CST_TOOLS_CMN_FILE_H_
