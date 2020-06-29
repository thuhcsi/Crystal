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

/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                       Copyright (c) 1996,1997                         */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                 Authors:  Alan W Black                                */
/*                 Date   :  July 1996                                   */
/*-----------------------------------------------------------------------*/
/*  A Viterbi decoder                                                    */
/*                                                                       */
/*  User provides the candidates, target and combine score function      */
/*  and it searches for the best path through the candidates             */
/*                                                                       */
/*=======================================================================*/

///
/// @file
///
/// @brief  Head file for Viterbi Search algorithm implementation
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Cong Honglei (conghonglei@gmail.com)
///   Date:     2007/11/02
///   Changed:  Created
/// <b>History:</b>
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/31
///   Changed:  Changed some interfaces for IVTDataSource and CViterbiDecoder,
///             Fixed the memory leak bug in CViterbiDecoder destructor.
///

#ifndef _CST_TTS_BASE_UTL_VITERBI_H_
#define _CST_TTS_BASE_UTL_VITERBI_H_

namespace cst
{
    ///
    /// @brief  The namespace for Viterbi search dynamic programming algorithm
    ///
    namespace Viterbi
    {
        ///
        /// @brief  The Viterbi item for data input/output in CVTList
        ///
        class CVTItem
        {
        private:
            friend class CVTList;
            CVTItem *pnext;

        public:
            CVTItem() : data(NULL), pnext(NULL) {}
            ~CVTItem() {}

        public:
            /// Return the next item
            CVTItem *next() const {return pnext;}
            /// Caller specific data
            void *data;
        };

        ///
        /// @brief  The Viterbi item list for data input/output in the Decoder
        ///
        class CVTList
        {
        private:
            CVTItem *phead;
            CVTItem *ptail;

        public:
            CVTList() : phead(NULL), ptail(NULL) {}
            ~CVTList() {clear();}

        public:
            /// Remove all the items in the list
            void clear();
            /// Get the head item of the list
            CVTItem *head() const {return phead;}
            /// Append a new item as the tail of the list
            CVTItem *append();
            /// Insert a new item as the head of the list
            CVTItem *prepend();
        };

        ///
        /// @brief  The candidate information of an input item in the Viterbi searching path
        ///
        /// The object should to be created and returned by ITVDataSource::getCandidateList()
        /// from the user (caller) of the Viterbi decoder
        ///
        class CVTCandidate
        {
        private:
            friend class CViterbiDecoder;
            unsigned int pos; ///< State index in the state paths under the related decoder table node CVTPoint

        public:
            CVTCandidate() : score(0), pos(-1), name(NULL), item(NULL), next(NULL) {}
            ~CVTCandidate() {}

        public:
            double        score;    ///< Candidate score (target score) between the target and the input item
            void          *name;    ///< Identity (name, ID) of the candidate, whose type is user defined
            const CVTItem *item;    ///< Related input item
            CVTCandidate  *next;    ///< Next candidate information in the candidate list
        };


        ///
        /// @brief  The data source of the Viterbi decoder where 
        ///         candidates, target/candidate and combine/transition score functions are specified
        ///
        class IVTDataSource
        {
        public:
            /// Destructor
            virtual ~IVTDataSource() {}

            ///
            /// @brief  Build the candidate list from the user defined input item
            ///
            /// The procedure must use "new/delete" operator to create or destroy the CVTCandidate objects.
            /// The returned candidate list will be maintained by the CViterbiDecoder and 
            /// be deleted by calling freeCandidateList().
            ///
            /// If the user definded data {name} is created dynamically,
            /// freeCandidateList() must be overridden to delete such user defined data {name}.
            ///
            /// Each candidate is filled up with {score}, {name}, {item} and {next}. Where
            /// {score} is the candidate (target) score,
            /// {item} is the user defined item related to this candidate,
            /// {name} is the candidate identity (name) to distinguish it from other candidates.
            ///
            /// @note   The return candidates should NOT be NULL, otherwise Viterbi algorithm will fail!
            ///
            /// @param  [in] item   Input item for which the candidates are built
            ///
            /// @return The candidates (pointer to the first candidate of the list) related to the user defined input item
            ///
            virtual CVTCandidate *getCandidateList(const CVTItem *item) const = 0;

            ///
            /// @brief  Release the candidate list and especially the user defined data {name} if any
            ///
            /// This function must be overridden to delete user defined data {name} if it is created dynamically in getCandidateList().
            ///
            /// @param  [in] cand   Candidate list to be released
            ///
            virtual void freeCandidateList(CVTCandidate *cand) const
            {
                // delete candidate list
                CVTCandidate *cnext;
                for (; cand != NULL; cand = cnext)
                {
                    // {name} is assumed to be not created by "new".
                    // Otherwise, this function must be overridden to delete {name} with user defined type
                    cnext = cand->next;
                    delete cand;
                }
            }

            ///
            /// @brief  Return the transition (combine) score between current and previous candidate
            ///
            /// @param  [in] prevCand   The previous candidate information
            /// @param  [in] curCand    The current  candidate information
            ///
            /// @return The transition (combine) score between the two candidates
            ///
            virtual double getTransitionScore(const CVTCandidate *prevCand, const CVTCandidate *curCand) const = 0;

        public:
            /// The maximum score used for score comparison
            static const double MAX_SCORE;
        };


        ///
        /// @brief  The Viterbi algorithm decoder
        ///
        /// This class can be used to find the best path through a set of candidates based on
        /// likelihoods of the candidates and combination (transition) function.
        ///
        /// The candidates, target/candidate and combine/transition score functions are input through IVTDataScoure.
        ///
        /// Following is an example illustrating the usage of the decoder and related objects:
        /// @verbatim
        ///     // define the input and output list
        ///     CVTList vitInput, vitOutput;
        ///
        ///     // initialize the input list
        ///     while (pInputData != NULL)
        ///     {
        ///         vitInput.append()->data = (void*)pInputData;
        ///     }
        ///
        ///     // define the data source and decoder
        ///     IVTDataScoure vitSource;
        ///     CViterbiDecoder vitDecoder(&vitSource, true);
        ///
        ///     // perform the Viterbi search and get result
        ///     vitDecoder.initialize(vitInput);
        ///     vitDecoder.search();
        ///     vitDecoder.getResult(vitOutput);
        ///
        ///     // extract the user specific data from the output list.
        ///     // the user specific {data} is actually the identity (i.e. name) of the CVTCandidate
        ///     for (pItem = vitOutput.head(); pItem != NULL; pItem = pItem->next)
        ///     {
        ///         // retrieve user specific data (i.e. name of the CVTCandiate)
        ///         pItem->data;
        ///     }
        /// @endverbatim
        ///
        class CViterbiDecoder
        {
        public:
            ///
            /// @brief  Constructor
            ///
            /// @param  [in] dataSrc    Data source where candidates, target and combine score function are specified
            /// @param  [in] bigIsGood  Whether bigger (e.g. likelihood) or smaller (e.g. cost) score is better
            ///
            CViterbiDecoder(const IVTDataSource *dataSrc, bool bigIsGood) : dataSource(dataSrc), timeline(NULL), big_is_good(bigIsGood) {}

            ///
            /// @brief  Destructor
            ///
            ~CViterbiDecoder();

        public:
            ///
            /// @brief  Build the initial Viterbi decoder table from the input item list
            ///
            /// @param  [in] input  The input item list which stores the caller specific data
            ///
            /// @return Whether initialization is successful
            ///
            bool initialize(const CVTList &input);

            ///
            /// @brief  Perform the actual Viterbi search
            ///
            void search();

            ///
            /// @brief  Extract the result (at most N best paths) from the Viterbi decoder table
            ///
            /// @param  [in]  n       The maximum number of results to be returned
            /// @param  [in]  results The array buffer to store the returned result (at most N best paths)
            /// @param  [out] scores  Return the accumulative score of the results (for each path of the N best paths)
            /// @param  [out] results Return the result (at most N best paths) with best score.
            ///                       The {data} in the returned result is actually the identity (i.e. name) of
            ///                       the candidate in the best Viterbi decoding path.
            ///
            /// @return The number of the best paths actually returned
            ///
            unsigned int getResult(CVTList *results, double *scores, unsigned int n) const;

        protected:
            ///
            /// @brief  The class representing a link in a path related to a candidate
            ///
            class CVTPath
            {
            public:
                CVTPath() : cand(NULL), from(NULL), next(NULL), score(0), state(0) {}
                ~CVTPath() {}

            public:
                unsigned int state;     ///< State number (index) in the state paths of CVTPoint
                double       score;     ///< Accumulative score for the path
                CVTCandidate *cand;     ///< Related candidate of this path
                CVTPath      *from;     ///< Preceding node in the path
                CVTPath      *next;     ///< Succeeding node in the path
            };

            ///
            /// @brief  The class representing a node in the decoder table,
            ///         which maintains the path node in the Viterbi decoding path
            ///
            class CVTPoint
            {
            public:
                CVTPoint(): item(NULL), cands(NULL), state_paths(NULL), next(NULL), num_states(0) {}
                ~CVTPoint() {}

            public:
                const CVTItem *item;        ///< Related input item at this decoder node
                CVTCandidate  *cands;       ///< Related candidate list
                unsigned int   num_states;  ///< State number, the number of the state paths
                CVTPath      **state_paths; ///< State paths array for Viterbi decoding
                CVTPoint      *next;        ///< Next (succeeding) node in the decoder table
            };

        protected:
            ///
            /// @brief  Create the state paths array and initialize it
            ///
            /// @param  [in] point      Point node for which state paths array is initialized
            /// @param  [in] num_states Number (dimension) of the state paths array
            ///
            void initPathsArray(CVTPoint *point, unsigned int num_states);

            ///
            /// @brief  Initialize the state paths array according to the candidates
            ///
            void initDynamicStates(CVTPoint *point, CVTCandidate *cands);

            ///
            /// @brief  Add several new paths to the decoder table node
            ///
            /// @param  [in] point      Decoder table node to which new paths are added
            /// @param  [in] newpaths   Several new paths (using the "next") to be added
            ///
            /// @see    addPath
            ///
            void addPaths(CVTPoint *point, CVTPath *newpaths);

            ///
            /// @brief  Add one new path to the state paths in the decoder table node, 
            ///         update the original state path if necessary
            ///
            /// @param  [in] point      The decoder table node to which new path is added
            /// @param  [in] newpath    New path (only one) to be added to the decoder table
            ///
            void addPath(CVTPoint *point, CVTPath *newpath);

            ///
            /// @brief  Determine whether a is "better" than b according to "big_is_good" flag
            ///
            /// Some times, the bigger data is better (i.e. likelihood probabilities or scores);
            /// while others might require the smaller data is better (i.e. cost vaues).
            ///
            bool betterThan(double a, double b) const {return (big_is_good) ? (a>b) : (a<b);}

            ///
            /// @brief  Find at most N best paths from the end of the decoder table
            ///
            /// @param  [in]  n     The maximum number of the best paths to be returned
            /// @param  [in]  paths The array to store the pointer to the best paths
            /// @param  [out] paths Return the pointer to the best paths
            ///
            /// @return The number of the best paths actually returned
            ///
            unsigned int findBestEnd(CVTPath **paths, unsigned int n) const;

        protected:
            ///
            /// @brief  The timeline decoder table
            ///
            CVTPoint *timeline;

            ///
            /// @brief  Define whether bigger or smaller score is better
            ///
            /// This allows the search to work for
            /// likelihoods probabilities or scores (i.e. big_is_good=true),
            /// and costs (i.e. big_is_good=false) etc.
            ///
            bool big_is_good;

            ///
            /// @brief  The data source
            ///
            const IVTDataSource *dataSource;
        };
    }
}

#endif //_CST_TTS_BASE_UTL_VITERBI_H_
