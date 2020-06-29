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
/// @brief  Implementation file for Viterbi Search algorithm implementation
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

#include <string.h>
#include "utl_viterbi.h"

namespace cst
{
    namespace Viterbi
    {
        const double IVTDataSource::MAX_SCORE = 1.0e10;

        void CVTList::clear()
        {
            for (CVTItem *pnext = NULL; phead != NULL; phead = pnext)
            {
                // delete all the items in iteration
                pnext = phead->pnext;
                delete phead;
            }
            phead = ptail = NULL;
        }

        CVTItem *CVTList::append()
        {
            CVTItem *n = new CVTItem();
            if (ptail == NULL)
                phead = n;
            else
                ptail->pnext = n;
            ptail = n;
            return n;
        }

        CVTItem *CVTList::prepend()
        {
            CVTItem *n = new CVTItem();
            if (phead == NULL)
                ptail = n;
            else
                n->pnext = phead;
            phead = n;
            return n;
        }

        CViterbiDecoder::~CViterbiDecoder()
        {
            CVTPoint *pnext, *point;
            for (point = timeline; point != NULL; point = pnext)
            {
                // delete user defined data in the candidate list
                if (dataSource != NULL)
                    dataSource->freeCandidateList(point->cands);
                point->cands = NULL;

                // delete state list
                for (unsigned int i = 0; i < point->num_states; ++i)
                {
                    delete point->state_paths[i];   // one state node
                }
                delete[] point->state_paths;    // the state node list
                point->state_paths = NULL;

                // delete point itself
                pnext = point->next;
                delete point;
            }
            timeline = NULL;
        }

        bool CViterbiDecoder::initialize(const CVTList &input)
        {
            if (dataSource == NULL)
                return false;

            // one extra point at the beginning
            CVTPoint *t = new CVTPoint();
            timeline = t;

            // the other points related to the input items
            for (CVTItem *p = input.head(); p != NULL; p = p->next())
            {
                CVTPoint * n = new CVTPoint();
                n->item = p;
                t->next = n;
                t = n;
            }

            // need init path on the start point so a search can start
            // initialize the start point
            initPathsArray(timeline, 1);
            return true;
        }

        void CViterbiDecoder::initPathsArray(CVTPoint *point, unsigned int num_states)
        {
            // create the state array and initialize it to NULL
            point->num_states = num_states;
            point->state_paths = new CVTPath*[num_states];
            memset(point->state_paths, 0, sizeof(CVTPath*)*num_states);
        }

        void CViterbiDecoder::initDynamicStates(CVTPoint *point, CVTCandidate *cands)
        {
            // in special case, the number of "states" is the number of candidates
            unsigned int nCands = 0;
            for (CVTCandidate *p = cands; p != NULL; p = p->next, ++nCands)
            {
                p->pos = nCands;
            }
            initPathsArray(point, nCands);
        }

        void CViterbiDecoder::search()
        {
            // search for the best path
            if (dataSource == NULL || timeline == NULL)
                return;

            CVTPoint *prev_point, *point;
            for (prev_point = timeline, point = timeline->next; point != NULL; prev_point = point, point = point->next)
            {
                // get the candidates
                point->cands = dataSource->getCandidateList(point->item);

                // initialize the sate paths array according to the candidates
                initDynamicStates(point, point->cands);

                // calculate the accumulative score for 
                // each "new candidate" and "previous path" combination
                for (unsigned int i = 0; i< prev_point->num_states; ++i)
                {
                    for (CVTCandidate *c = point->cands; c != NULL; c = c->next)
                    {
                        CVTPath *path = new CVTPath();
                        if (path == NULL)
                        {
                            // path creation error, just continue to next candidate
                            continue;
                        }

                        // fill up candidate information, including candidate (target) score
                        path->from  = prev_point->state_paths[i];
                        path->cand  = c;
                        path->state = c->pos;
                        path->score = c->score;

                        // compute the path accumulative score
                        if (prev_point->state_paths[i] != NULL)
                        {
                            // transition (combine) score
                            CVTCandidate *preCand = prev_point->state_paths[i]->cand;
                            path->score += dataSource->getTransitionScore(preCand, c);

                            // path accumulative score
                            path->score += prev_point->state_paths[i]->score;
                        }

                        // set the new paths in the decoder table
                        addPaths(point, path);
                    }
                }
            }
        }

        void CViterbiDecoder::addPaths(CVTPoint *point, CVTPath *newpaths)
        {
            CVTPath *pnext, *pcur;
            for (pcur = newpaths; pcur != NULL; pcur = pnext)
            {
                // need to save "pnext" as "pcur" could be deleted in "addPath"
                pnext = pcur->next;
                addPath(point, pcur);
            }
        }

        void CViterbiDecoder::addPath(CVTPoint *point, CVTPath *newpath)
        {
            unsigned int newstate = newpath->state;
            if (newstate > point->num_states)
            {
                // invalid state number, raise exception and return
                throw;
            }
            else if (point->state_paths[newstate] == NULL || betterThan(newpath->score, point->state_paths[newstate]->score))
            {
                // the "new path" has "better" score, replace the original path
                delete point->state_paths[newstate];
                point->state_paths[newstate] = newpath;
            }
            else
            {
                // the "new path" has "worse" score, delete new path, keep original path
                delete newpath;
            }
        }

        unsigned int CViterbiDecoder::getResult(CVTList *results, double *scores, unsigned int n) const
        {
            if (timeline == NULL || timeline->next == NULL || results == NULL || scores == NULL || n == 0)
            {
                // it is an empty list, no data will be returned
                return 0;
            }

            // get the best path
            CVTPath **paths = new CVTPath*[n];
            n = findBestEnd(paths, n);
            if (n == 0)
            {
                // there is not any answer, error
                return 0;
            }

            // return the result
            for (unsigned int i = 0; i < n; ++i)
            {
                for (CVTPath *p = paths[i]; p != NULL; p = p->from)
                {
                    if (p->cand != NULL)
                    {
                        results[i].prepend()->data = p->cand->name;
                    }
                }
                scores[i] = paths[i]->score;
            }

            // free memory
            delete []paths;
            return n;
        }

        unsigned int CViterbiDecoder::findBestEnd(CViterbiDecoder::CVTPath **paths, unsigned int n) const
        {
            // no data to be returned
            if (n == 0)
                return 0;

            // seek to the last node of the decoder table
            CVTPoint *t = NULL;
            for (t = timeline; t->next != NULL; t = t->next)
            {
                if( t->num_states <= 0 )
                {
                    // no paths at specific frame
                    return 0;
                }
            }

            // get the maximum number
            if (n > t->num_states)
                n = t->num_states;

            // find N best paths
            paths[0] = NULL;
            for (unsigned int i = 0; i < t->num_states; ++i)
            {
                if (t->state_paths[i] == NULL)
                    continue;
                // find the index to insert the path
                unsigned int j = 0;
                for (j = 0; j < n; ++j)
                {
                    if (paths[j] == NULL || betterThan(t->state_paths[i]->score, paths[j]->score))
                        break;
                }
                // move afterward to reserve space
                for (unsigned int k = n-1; k > j; --k)
                {
                    paths[k] = paths[k-1];
                }
                // insert path
                if (j < n)
                    paths[j] = t->state_paths[i];
            }

            return n;
        }
    }
}
