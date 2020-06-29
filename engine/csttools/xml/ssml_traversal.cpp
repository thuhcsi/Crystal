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
/// @brief  Implementation for traversal operation on SSML document.
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


#include "ssml_document.h"
#include "ssml_traversal.h"
#include "cmn/cmn_error.h"

namespace cst
{
    namespace xml
    {
        int CSSMLTraversal::traverse(xml::CSSMLDocument *pSSMLDocument)
        {
            ///@todo Error processing of processNode and postProcessNode

            if (pSSMLDocument == NULL)
            {
                return ERROR_SUCCESS;
            }
            int retval = ERROR_SUCCESS;

            // use iteration (loop) instead of recursion,
            // because in embedded system, recursion might cause crash for using up "system stack"

            // traverse all the nodes using depth first method.
            xml::CXMLNode *pCurrentNode = pSSMLDocument->firstChild();
            while (pCurrentNode != NULL)
            {
                bool childProcessed = false;
                retval = processNode(pCurrentNode, childProcessed);
                if (retval != ERROR_SUCCESS)
                {
                    // error occurred
                    return retval;
                }
                if (pCurrentNode->firstChild() != NULL && !childProcessed)
                {
                    // start the children processing
                    pCurrentNode = pCurrentNode->firstChild();
                    continue;
                }

                while (pCurrentNode != NULL)
                {
                    // current-node processing is over, do post-processing
                    retval = postProcessNode(pCurrentNode);
                    if (retval != ERROR_SUCCESS)
                    {
                        // error occurred
                        return retval;
                    }

                    if (pCurrentNode->nextSibling() != NULL)
                    {
                        // next sibling first
                        pCurrentNode = pCurrentNode->nextSibling();
                        break;
                    }
                    else 
                    {
                        // back to parent
                        pCurrentNode = pCurrentNode->parent();
                    }
                }
            }

            return ERROR_SUCCESS;
        }

        int CSSMLTraversal::processNode(xml::CXMLNode *pNode, bool &childProcessed)
        {
            childProcessed = false;
            return ERROR_SUCCESS;
        }

        int CSSMLTraversal::postProcessNode(xml::CXMLNode *pNode)
        {
            return ERROR_SUCCESS;
        }
    }
}
