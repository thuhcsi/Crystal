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

#ifndef _BL_DATA_BIGRAM_H_INCLUDED_
#define _BL_DATA_BIGRAM_H_INCLUDED_

#include "cmn/cmn_type.h"
#include <vector>
#include <string>

namespace cst
{

    namespace cmn
    {
        /// 
        /// @brief NGM file-format, a binary file storing the unigram & bi-gram
        /// 
        /// This file contains 3 section:
        /// 1. TNGMFileHeader
        /// 2. Unigram
        /// 3. Bigram
        /// An important problem is that unigram section contains a textual POS with arbitrary
        /// length, instead of using a fixed length structure to store them, we keep one line for 
        /// each POS
        /// 
        struct TNGMFileHeader
        {
            uint32 posCount;
            uint32 unigramBuffLength;
        };

        class CBigram
        {
        public:
            CBigram(){m_vecBigram = NULL;}
            virtual ~CBigram() {clear();}

        public:
            ///
            /// @brief initialized the bigram module with the specified file
            ///
            /// The method will first clear everything that is in the lexicon
            ///
            /// @param [in] strFile   input file name
            ///
            /// @return whether the intialization is sucessful
            ///
            /// @warning Whether the method is successfull or not, every thing that is
            ///         already in the lexicon will be CLEARED!!
            ///
            bool loadBigramFromARPA(const std::wstring &strFile);

            ///
            /// @brief initialized the bigram module with the specified file
            ///
            /// The method will first clear everything that is in the lexicon
            ///
            /// @param [in] strFile   input file name
            ///
            /// @return whether the intialization is sucessful
            ///
            /// @warning Whether the method is successfull or not, every thing that is
            ///         already in the lexicon will be CLEARED!!
            ///
            bool loadBigramFromNGM(const std::string &strFile);

        public:
            bool saveBigramToNGM(const std::string &strFile);

        public:
            ///
            /// @brief get the POS index in this specific module
            ///
            /// The function will check the wstrPOS through the POS vector, and return
            /// the index. If it is not in the vector, the last character will be
            /// eliminated(to get the parent POS) and search again.
            ///
            /// @return the index value of the given POS(or its parent POS) in the POS
            ///         vector.
            /// @retval -1 None of the POS and any of its prefix are in the POS vector.
            ///
            int getPOSIndex(const std::wstring &wstrPOS) const;

            ///
            /// @brief get the bigram value of the given POS's
            ///
            /// @param [in] wstrPOS1 the first element of the bigram
            /// @param [in] wstrPOS2 the second element of the bigram
            ///
            /// @return The bigram value of the give pair of POS's, in logrithm.\n
            ///         The return will be -1e30 if not found.
            ///
            double getBigramValue(const std::wstring &wstrPOS1, const std::wstring &wstrPOS2) const;

            ///
            /// @brief get the bigram value of the given POS's
            ///
            /// @param [in] idxPOS1 the first element of the bigram
            /// @param [in] idxPOS2 the second element of the bigram
            ///
            /// @return The bigram value of the give pair of POS's, in logrithm.\n
            ///         The return will be -1e30 if not found.
            ///
            double getBigramValue(int idxPOS1, int idxPOS2) const;

            ///
            /// @brief clear the Bigram model
            ///
            /// @return whether the model is cleared.
            ///
            bool clear();

        private:
            std::vector<std::wstring> m_vecPOS; ///< an array of all the POS in the bigram module
            std::vector<double> m_vecUnigram;  ///< unigram module
            std::vector<double> m_vecUnigramBackOff; ///< back off value, use to calculate bigram when
            ///< the bigram itself is missing
            // std::vector<double> m_vecBigram;  ///< bigram module
            double *m_vecBigram;
        };
    }

}

#endif // _BL_DATA_BIGRAM_H_INCLUDED_
