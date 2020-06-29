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
/// @brief  Implementation of Wagon Tree - a kind of CART (Classification And Regression Tree)
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Yongxin (fefe.wyx@gmail.com)
///   Date:     2011/05/10
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2011/06/09
///   Changed:  Modified by re-organizing the classes and adding comments
///


#include <string>
#include <stack>
#include <iostream>
#include "utl_wagontree.h"

#ifdef _DEBUG
#define _DEBUG_WAGON_TREE_
#endif

namespace cst
{
    namespace Wagon
    {

        //////////////////////////////////////////////////////////////////////////
        //
        //  QuestionEvaluator
        //
        //////////////////////////////////////////////////////////////////////////

        std::wstring QuestionEvaluator::m_strOP[OP_MAX] =
        {
            L" is not ",
            L" is ",
            L" = ",
            L" > ",
            L" < "
        };

        QuestionEvaluator::QuestionEvaluator(const std::wstring &question)
        {
            size_t pos, i;
            for (i = 0; i < OP_MAX; i++)
            {
                if ((pos = question.find(m_strOP[i])) != question.npos)
                {
                    m_feature = question.substr(0, pos);
                    m_sValue  = question.substr(pos + m_strOP[i].length(), question.npos);
                    m_fValue  = (float)std::wcstod(m_sValue.c_str(), NULL);
                    m_op      = (EWagonOp)i;
                    break;
                }
            }
#ifdef _DEBUG_WAGON_TREE_
            if (i >= OP_MAX)
            {
                std::wcerr << L" ERROR ! Q : " << question << std::endl;
            }
#endif
        }

        bool QuestionEvaluator::ask(const std::map<std::wstring, TestProperty> &test)
        {
            // find the property of the feature name
            std::map<std::wstring, TestProperty>::const_iterator it = test.find(m_feature);

            if (it != test.end())
            {
                // feature found
                const TestProperty &feat = it->second;
#ifdef _DEBUG_WAGON_TREE_
                std::wcout << L"TESTING  : " << feat.feature << L"," << feat.sValue << L"," << feat.fValue << std::endl;
                std::wcout << L" AGAINST : " << m_strOP[m_op] << L"," << m_sValue << L"," << m_fValue << std::endl;
#endif
                // return test value
                switch (m_op)
                {
                case OP_IS_NOT:
                    return feat.sValue != m_sValue;
                case OP_IS:
                    return feat.sValue == m_sValue;
                case OP_EQ:
                    return feat.fValue == m_fValue;
                case OP_GT:
                    return feat.fValue > m_fValue;
                case OP_LT:
                    return feat.fValue < m_fValue;
                default:
                    return false;
                }
            }
            else
            {
                // feature not found
#ifdef _DEBUG_WAGON_TREE_
                std::wcerr << L" UNKNOWN TEST : " << m_feature << std::endl;
#endif
                return false;
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  WagonNodeLeaf
        //
        //////////////////////////////////////////////////////////////////////////

        CWagonNodeLeaf::CWagonNodeLeaf(const std::vector<std::pair<std::wstring, std::wstring> > &vecPairs, const std::wstring &mostProbValue)
        {
            m_mostProbValue = mostProbValue;
            m_leafPairs.resize(vecPairs.size());
            for (size_t i = 0; i< vecPairs.size(); ++i)
            {
                m_leafPairs[i].first  = vecPairs[i].first;
                m_leafPairs[i].second = (float)wcstod(vecPairs[i].second.c_str(), NULL);
            }
        }

        const CWagonNode::typeLeafPairs &CWagonNodeLeaf::predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue)
        {
            mostProbValue = m_mostProbValue;
            return m_leafPairs;
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  WagonNodeQuestion
        //
        //////////////////////////////////////////////////////////////////////////

        CWagonNodeQuestion::CWagonNodeQuestion(const std::wstring &Question)
            : m_question(Question), m_pChildYes(NULL), m_pChildNo(NULL)
        {
        }

        bool CWagonNodeQuestion::setChild(CWagonNode *pChild, int idx)
        {
            // (0: yes_child, 1: no_child, Other: error)
            if (idx == 0)
                m_pChildYes = pChild;
            else if (idx == 1)
                m_pChildNo = pChild;
            else
                return false;
            return true;
        };

        const CWagonNode::typeLeafPairs &CWagonNodeQuestion::predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue)
        {
            if (m_question.ask(test))
            {
                // question matched -> YES-NODE
                return m_pChildYes->predict(test, mostProbValue);
            }
            else
            {
                // question mismatched -> NO-NODE
                return m_pChildNo->predict(test, mostProbValue);
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //
        //  WagonTree
        //
        //////////////////////////////////////////////////////////////////////////

        const CWagonNode::typeLeafPairs &CWagonTree::predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue)
        {
            return m_pRoot->predict(test, mostProbValue);
        }

        wchar_t CWagonTree::nextCh(std::wistream &iStream, bool ignoreWhiteSpace)
        {
            wchar_t ch;
            bool found = false;
            static const std::wstring ignoreChars = L" \t\r\n";
            while (iStream.get(ch))
            {
                if (!ignoreWhiteSpace || ignoreChars.find(ch) == std::wstring::npos)
                {
                    found = true;
                    break;
                }
            }
            return (found ? ch : 0);
        }

        bool CWagonTree::loadFromText(std::wistream &iStream)
        {
            // Clear original Wagon Tree
            delete m_pRoot;

            wchar_t ch;
            CWagonNode *node;
            std::stack<std::pair<CWagonNode *, int> > node_stack;

            std::wstring Question = L"";    // QUESTION string
            std::wstring LeafOutput;        // Most probable output of the LEAF
            std::pair<std::wstring, std::wstring> LeafPair;                  // One LEAF pair (Value, Probability)
            std::vector<std::pair<std::wstring, std::wstring> > vecLeafPair; // LEAF pairs (Value, Probability)

            // parsing the input stream
            EParseState ps_state = PS_START;
            bool error = false;

            while (!error)
            {
                switch (ps_state)
                {
                case PS_START:
                    // tree node must start with '('
                    ch = nextCh(iStream, true);
                    if (ch != L'(')
                    {
                        error = true;
                    }
                    ps_state = PS_TreeNodeS;
                    break;

                case PS_TreeNodeS:
                    // node content must start with '(('
                    ch = nextCh(iStream, true);
                    if (ch != L'(')
                    {
                        error = true;
                    }
                    ps_state = PS_TreeNodeContentS;
                    break;

                case PS_TreeNodeContentS:
                    ch = nextCh(iStream, true);
                    if (ch == L'(')
                    {
                        // leaf must start with '(((' or '(((('
                        ps_state = PS_LeafPairFst;
                    }
                    else
                    {
                        // question should only start with '(('
                        ps_state = PS_TreeQuestion;
                    }
                    break;

                case PS_TreeQuestion:
                    // get the question
                    // white space should be kept for the question
                    // read all characters including white space until ')'
                    Question.clear();
                    while (ch != ')')
                    {
                        Question += ch;
                        ch = nextCh(iStream, false);
                    }
                    // create new question node
                    node = new CWagonNodeQuestion(Question);
                    // indicate the end of the current node content
                    ps_state = PS_TreeNodeContentE;
                    break;

                case PS_LeafPairFst:
                    // get the first element of the leaf pair
                    while (ch == L'(')
                    {
                        // skip leading '('
                        ch = nextCh(iStream, true);
                    }
                    // read until white space ' '
                    while (ch != L' ')
                    {
                        LeafPair.first += ch;
                        ch = nextCh(iStream, false);
                    }
                    ps_state = PS_LeafPairSnd;
                    break;

                case PS_LeafPairSnd:
                    // get the second element of the leaf pair
                    // skip remaining ' '
                    ch = nextCh(iStream, true);
                    // read until ')'
                    while (ch != L')')
                    {
                        LeafPair.second += ch;
                        ch = nextCh(iStream, false);
                    }
                    // save current leaf pair
                    vecLeafPair.push_back(LeafPair);
                    // read for another leaf pair
                    LeafPair = std::make_pair(L"", L"");
                    ps_state = PS_LeafContent;
                    break;

                case PS_LeafContent:
                    // the content part of the tree leaf
                    // skip remaining ')'
                    while (ch == L')')
                    {
                        ch = nextCh(iStream, true);
                    }
                    // another leaf pair or output
                    if (ch == L'(')
                    {
                        // start of another leaf pair
                        ps_state = PS_LeafPairFst;
                    }
                    else
                    {
                        // the most probable output of the leaf
                        ps_state = PS_LeafOutput;
                    }
                    break;

                case PS_LeafOutput:
                    // get the most probable output of the leaf
                    // read until ')'
                    LeafOutput.clear();
                    while (ch != ')')
                    {
                        LeafOutput += ch;
                        ch = nextCh(iStream, true);
                    }
                    // create new leaf node
                    node = new CWagonNodeLeaf(vecLeafPair, LeafOutput);
                    // read for another leaf node
                    vecLeafPair.clear();
                    // indicate the end of the current node content
                    ps_state = PS_TreeNodeContentE;
                    break;

                case PS_TreeNodeContentE:
                case PS_TreeNodeE:
                    if (ps_state == PS_TreeNodeContentE && node)
                    {
                        // a new tree node is ready, save it
                        if (!node_stack.empty())
                        {
                            // set the child of existing node
                            std::pair<CWagonNode *, int> &nodep = node_stack.top();
                            CWagonNodeQuestion *pnode = dynamic_cast<CWagonNodeQuestion*>(nodep.first);
                            error = (pnode) && !pnode->setChild(node, nodep.second);
                            //error = !nodep.first->setChild(node, nodep.second);
                            nodep.second++;
                        }
                        else
                        {
                            // set the root node
                            m_pRoot = node;
                        }
                        // save current tree node
                        node_stack.push(std::make_pair(node, 0));
                        node = NULL;
                    }

                    if (ps_state == PS_TreeNodeE)
                    {
                        // end of a tree node, pop it out
                        node_stack.pop();
                        if (node_stack.empty())
                        {
                            // all nodes have been parsed,
                            // parsing is successful
                            return m_pRoot != NULL;
                        }
                    }

                    // must be ')'
                    if (ch != L')')
                    {
                        error = true;
                        break;
                    }

                    // get next character
                    ch = nextCh(iStream, true);
                    if (ch == '(')
                    {
                        // start of another tree node
                        ps_state = PS_TreeNodeS;
                    }
                    else if (ch == ')')
                    {
                        // end of a tree node
                        ps_state = PS_TreeNodeE;
                    }
                    else
                    {
                        // not valid format
                        error = true;
                    }
                    break;
                }
            }
            return (!error && m_pRoot != NULL);
        }

    }
}
