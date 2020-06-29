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
/// @brief  Definition for traversal operation on SSML document.
///
/// @version    0.1.0
/// @date       2008/05/28
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/05/28
///   Changed:  Created
///


#ifndef _CST_TTS_BASE_SSML_TRAVERSAL_H_
#define _CST_TTS_BASE_SSML_TRAVERSAL_H_

namespace cst
{
    namespace xml
    {
        ///
        /// @brief  The class for traversing the SSML document
        ///
        class CSSMLTraversal
        {
        public:
            /// Constructor
            CSSMLTraversal() {}

            /// Destructor
            virtual ~CSSMLTraversal() {}

        public:
            ///
            /// @brief  The procedure for traversing the SSML document
            ///
            /// The procedure walks through all the nodes of the SSML document by depth first iteration method.
            ///
            /// When the node is first occurred, the procedure will call 
            /// "processNode" to perform specific processing to the node.\n
            /// When all the children of the node are processed, the procedure will call
            /// "postProcessNode" to perform the post-processing to the node.
            ///
            /// @see    processNode, postProcessNode
            ///
            /// @param  [in] pSSMLDocument  The SSML document to be traversed
            ///
            /// @return Whether traversing is successful for the SSML document
            ///
            virtual int traverse(xml::CSSMLDocument *pSSMLDocument);

        protected:
            ///
            /// @brief  Process a specific SSML document node.
            ///
            /// It is just an empty implementation here in the base class.
            /// Child-class should overload this function to perform its specific work.
            ///
            /// If the children of current node is not processed (childProcessed = false), the main 
            /// process procedure will perform the subtree (children of this node) traveling further.
            /// Otherwise (childProcessed = true), the subtree will not be traveled.
            ///
            /// @param  [in]  pNode             The input SSML node to be processed
            /// @param  [out] childProcessed    Return whether children of pNode have been processed
            ///
            /// @return Whether operation is successful
            /// @retval ERROR_SUCCESS The operation is successful
            ///
            virtual int processNode(xml::CXMLNode *pNode, bool &childProcessed);

            ///
            /// @brief  Perform the post-processing of a specific SSML document node
            ///
            /// This procedure is called after all the children nodes are processed.
            /// It is just an empty implementation here in the base class.
            ///
            /// @param  [in]  pNode     The SSML node to be post-processed
            ///
            /// @return Whether operation is successful
            /// @retval ERROR_SUCCESS The operation is successful
            ///
            virtual int postProcessNode(xml::CXMLNode *pNode);

        };//CSSMLTraversal
    }
}

#endif//_CST_TTS_BASE_SSML_TRAVERSAL_H_
