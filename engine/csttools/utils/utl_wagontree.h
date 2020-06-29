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
/// @brief  Declaration of Wagon Tree - a kind of CART (Classification And Regression Tree)
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


#ifndef _CST_TOOLS_UTL_WAGONTREE_H_
#define _CST_TOOLS_UTL_WAGONTREE_H_

#include <map>
#include <vector>

namespace cst
{
    namespace Wagon
    {
        ///
        /// @brief  Class of the properties (feature, value) to be evaluated against the question
        ///
        class TestProperty
        {
        public:
            ///
            /// @brief  Constructor for (feature, string value)
            ///
            TestProperty(const std::wstring feat, const std::wstring val) : feature(feat), sValue(val), fValue(0.0f) {}

            ///
            /// @brief  Constructor for (feature, float value)
            ///
            TestProperty(const std::wstring feat, float val) : feature(feat), fValue(val), sValue(L"") {}

        public:
            std::wstring feature;   ///< Feature
            std::wstring sValue;    ///< String value
            float        fValue;    ///< Float value
        };


        ///
        /// @brief  Class for question evaluation
        ///
        /// @verbatim
        ///     QUESTION ::= "(" FEATURENAME "is" VALUE ")" |
        ///                  "(" FEATURENAME "=" FLOAT ")" |
        ///                  "(" FEATURENAME "<" FLOAT ")" |
        ///                  "(" FEATURENAME ">" FLOAT ")"
        /// @endverbatim
        ///
        class QuestionEvaluator
        {
        public:
            ///
            /// @brief  Operator of the question
            ///
            enum EWagonOp
            {
                OP_IS_NOT,      ///< " is not "
                OP_IS,          ///< " is "
                OP_EQ,          ///< " = "
                OP_GT,          ///< " > "
                OP_LT,          ///< " < "
                OP_MAX
            };

        public:
            ///
            /// @brief  Default constructor
            ///
            QuestionEvaluator():m_op(OP_IS),m_fValue(0) {};

            ///
            /// @brief  Construct a question evaluator from the string
            ///
            /// @param  [in] question   Question string
            ///
            QuestionEvaluator(const std::wstring &question);

            ///
            /// @brief  Return the feature of the question to be evaluated
            ///
            const std::wstring &getFeature()const {return m_feature;}

            ///
            /// @brief  Return the value of the question
            ///
            const std::wstring &getValue()const {return m_sValue;}

            ///
            /// @brief  Get the operator of the question
            ///
            EWagonOp getOP()const {return m_op;}

            ///
            /// @brief  Evaluate the question against the property
            ///
            /// @param  [in] test   The properties to be evaluated against current question
            ///
            /// @retval true    One of the properties satisfies this question evaluator (Question returns 'YES')
            /// @retval false   None of the properties satisfies this question (Question returns 'NO')
            ///
            bool ask(const std::map<std::wstring, TestProperty> &test);

        private:
            std::wstring m_feature;                 ///< Feature of the question
            std::wstring m_sValue;                  ///< String value of the question
            float        m_fValue;                  ///< Float value of the question
            EWagonOp     m_op;                      ///< Operator of the question
            static std::wstring m_strOP[OP_MAX];    ///< The supported Operator string
        };


        ///
        /// @brief  Base class for the node of Wagon Tree
        ///
        class CWagonNode
        {
        public:
            ///
            /// @brief  Virtual destructor
            ///
            virtual ~CWagonNode() {}

            ///
            /// @brief  Leaf pair values (Value, Probability) of the leaf node
            ///
            typedef std::vector<std::pair<std::wstring, float> > typeLeafPairs;

            ///
            /// @brief  Get the predicted value according to the properties
            ///
            /// @param  [in]  test          The properties to be evaluated
            /// @param  [out] mostProbValue Return the value with the biggest probability (most probable value)
            ///
            /// @return Leaf pair values (Value, Probability) of the leaf node that matches the input properties
            ///
            virtual const typeLeafPairs &predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue) = 0;
        };


        ///
        /// @brief  Class for defining the question node of Wagon Tree
        ///
        /// @verbatim
        ///     QUESTION-NODE ::= "(" QUESTION YES-NODE NO-NODE ")"
        /// @endverbatim
        ///
        class CWagonNodeQuestion : public CWagonNode
        {
        public:
            ///
            /// @brief  Constructor of the question node
            ///
            CWagonNodeQuestion(const std::wstring &Question);

            ///
            /// @brief  Destructor, clear all child-nodes of this node
            ///
            virtual ~CWagonNodeQuestion() {delete m_pChildYes; delete m_pChildNo; m_pChildYes = m_pChildNo = NULL;}

            ///
            /// @brief  Set the child node according to the index (0: yes_child, 1: no_child, 0ther: error)
            ///
            /// @param  [in] pChild     Pointer to the child node
            /// @param  [in] idx        Index of the child node (0: yes_child, 1: no_child, Other: error)
            ///
            /// @return Whether operation is successful
            ///
            bool setChild(CWagonNode *pChild, int idx);

            ///
            /// @brief  Get the predicted value according to the properties
            ///
            /// @param  [in]  test          The properties to be evaluated
            /// @param  [out] mostProbValue Return the value with the biggest probability (most probable value)
            ///
            /// @return Leaf pair values (Value, Probability) of the leaf node that matches the input properties
            ///
            virtual const CWagonNode::typeLeafPairs &predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue);

        private:
            CWagonNode       *m_pChildYes;  ///< Link to child node (yes node)
            CWagonNode       *m_pChildNo;   ///< Link to child node (no node)
            QuestionEvaluator m_question;   ///< Question eavaluator
        };


        ///
        /// @brief  Class for defining the leaf node of Wagon Tree
        ///
        /// @verbatim
        ///     LEAF-NODE ::= "(" LEAF ")"
        ///     LEAF      ::= "(" "(" VALUE0 PROB0 ")" "(" VALUE1 PROB1 ")" ... MOSTPROBVAL ")"
        /// @endverbatim
        ///
        class CWagonNodeLeaf : public CWagonNode
        {
        public:
            ///
            /// @brief  Constructor of the leaf node
            ///
            /// @param  [in] vecPairs       Pair values (Value, Probability) of the leaf node
            /// @param  [in] mostProbValue  Value in the pair values which has the biggest probability
            ///
            CWagonNodeLeaf(const std::vector<std::pair<std::wstring, std::wstring> > &vecPairs, const std::wstring &mostProbValue);

            ///
            /// @brief  Get the predicted value according to the properties
            ///
            /// @param  [in]  test          The properties to be evaluated
            /// @param  [out] mostProbValue Directly return the most probable value of the current leaf node
            ///
            /// @return Directly return leaf pair values of the current leaf node
            ///
            virtual const CWagonNode::typeLeafPairs &predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue);

        private:
            typeLeafPairs m_leafPairs;      ///< Pair values (Value, Probability) of the leaf node
            std::wstring  m_mostProbValue;  ///< Value in the pair values which has the biggest probability

        };

        ///
        /// @brief  Class for defining a Wagon Tree (a kind of CART - Classification And Regression Tree)
        ///
        /// @verbatim
        ///     TREE ::= LEAF-NODE | QUESTION-NODE
        ///
        ///     QUESTION-NODE ::= "(" QUESTION YES-NODE NO-NODE ")"
        ///
        ///     YES-NODE := TREE
        ///
        ///     NO-NODE := TREE
        ///
        ///     QUESTION ::= "(" FEATURENAME "is" VALUE ")" |
        ///                  "(" FEATURENAME "=" FLOAT ")" |
        ///                  "(" FEATURENAME "<" FLOAT ")" |
        ///                  "(" FEATURENAME ">" FLOAT ")"
        ///
        ///     LEAF-NODE ::= "(" LEAF ")"
        ///
        ///     LEAF ::= "(" "(" VALUE0 PROB0 ")" "(" VALUE1 PROB1 ")" ... MOSTPROBVAL ")"
        /// @endverbatim
        ///
        class CWagonTree
        {
        public:
            ///
            /// @brief  Constructor
            ///
            CWagonTree() : m_pRoot(NULL) {};

            ///
            /// @brief  Destructor
            ///
            ~CWagonTree() { delete m_pRoot; m_pRoot = NULL; }

        public:
            ///
            /// @brief  Load the Wagon Tree from a text stream
            ///
            /// @param  [in] iStream  Stream to load Wagon Tree from
            ///
            /// @return Whether Wagon Tree is loaded successfully
            ///
            bool loadFromText(std::wistream &iStream);

            ///
            /// @brief  Get the predicted value according to the properties
            ///
            /// @param  [in]  test          The properties to be evaluated
            /// @param  [out] mostProbValue Return the value with the biggest probability (most probable value)
            ///
            /// @return Leaf pair values (Value, Probability) of the leaf node that matches the input properties
            ///
            const CWagonNode::typeLeafPairs &predict(const std::map<std::wstring, TestProperty> &test, std::wstring &mostProbValue);

        private:
            ///
            /// @brief  State for parsing the input text of Wagon Tree
            ///
            enum EParseState
            {
                PS_START,               ///< Start of parsing
                PS_TreeNodeS,           ///< Start of tree node
                PS_TreeNodeE,           ///< End of the tree node
                PS_TreeNodeContentS,    ///< Start of the content of the tree node (Question or Leaf)
                PS_TreeNodeContentE,    ///< End of the content of the tree node (Question or Leaf)
                PS_LeafPairFst,         ///< The first element of the leaf pair
                PS_LeafPairSnd,         ///< The second element of the leaf pair
                PS_LeafContent,         ///< The content of the tree node for Leaf
                PS_LeafOutput,          ///< The most probable output of the tree node for Leaf
                PS_TreeQuestion,        ///< The content of the tree node for Question
            };

            ///
            /// @brief  Root node of the Wagon Tree
            ///
            CWagonNode *m_pRoot;

        private:
            ///
            /// @brief  Get the next character from the stream by skipping the white space or not
            ///
            /// @param  [in] iStream            Stream from which to read the character
            /// @param  [in] ignoreWhiteSpace   Whether white space is skipped while retrieving the next character
            ///
            /// @return The next character
            ///
            static wchar_t nextCh(std::wistream &iStream, bool ignoreWhiteSpace);
        };

    }//namespace Wagon
}

#endif//_CST_TOOLS_UTL_WAGONTREE_H_
