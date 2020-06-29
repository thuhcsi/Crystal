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
/// @brief  Definition of encapsulated hash map manipulation.
///
/// @version    0.1.0
/// @date       2008/02/14
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/02/14
///   Changed:  Created
///

#ifndef _CST_TTS_BASE_CMN_HASH_MAP_H_
#define _CST_TTS_BASE_CMN_HASH_MAP_H_

#include <hash_map>

namespace cst
{
    namespace cmn
    {
        ///
        /// @brief  The template class to be used to order and hash the elements
        ///
        template<class key>
        class CHashCompare : public stdext::hash_compare<key>
        {
        };

        ///
        /// @brief  The specialization of the template class to be used to order and hash (wchar_t*) elements
        ///
        template<>
        class CHashCompare<wchar_t*> : public stdext::hash_compare<wchar_t*>
        {
        public:
            size_t operator()(wchar_t* const &key) const
            {
                //return stdext::hash_compare<wchar_t*>::operator ()((wchar_t* const)key);
                //printf("%ls %d\n", key, (*key));
                //std::cout << cmn::wstring(key).c_str() << L" " << (*key) << std::endl;
                return (*key);
            }

            bool operator()(wchar_t* const &key1, wchar_t* const &key2) const
            {
                if (wcscmp(key1, key2) > 0)
                    return true;
                return false;
            }
        };

        ///
        /// @brief  The specialization of the template class to be used to order and hash (char*) elements
        ///
        template<>
        class CHashCompare<char*> : public stdext::hash_compare<char*>
        {
        public:
            size_t operator()(char* const &key) const
            {
                return (*key);
            }

            bool operator()(char* const &key1, char* const &key2) const
            {
                if (strcmp(key1, key2) > 0)
                    return true;
                return false;
            }
        };

        ///
        /// @brief  The class which stores and retrieves data quickly from a map collection (implemented as hash table).
        ///
        /// In the class, each element is a pair that has a sort key whose value is unique and an associated data value.
        ///
        template<class Key, class Type, class Traits = CHashCompare<Key> >
        class CHashMap : public stdext::hash_map<Key, Type, Traits>
        {
        public:
            ///
            /// @brief  Get the hash value of the key. The returned value is wrapped to hash table size (bucket_count)
            ///
            size_type hash_value(const key_type& keyVal) const
            {
                return _Hashval(keyVal);
            }

            ///
            /// @brief  Get the hash table size (the count of the buckets)
            ///
            size_type bucket_count() const
            {
                return _Maxidx;
            }

            ///
            /// @brief  Make the pair for the map
            ///
            static std::pair<Key, Type> make_pair(const Key& key, const Type& val)
            {
                return std::pair<Key, Type>(key, val);
            }
        };

        ///
        /// @brief  The virtual hash map class which is used for retrieving the hash value of the key
        ///
        /// The virtual hash map is provided only for easy retrieving the hash value of a specific key,
        /// which does not create and have real internal buffer to store data.
        ///
        template<class Key, class Type, class Traits = CHashCompare<Key> >
        class CHashKey : protected stdext::hash_map<Key, Type, Traits>
        {
        public:
            ///
            /// @brief  Constructing the hash key with specified size for getting hash value
            ///
            /// The hash key does not create and have real buffer to store data.
            ///
            explicit CHashKey(size_type Count)
            {
                _Maxidx = Count;
                _Mask = 0;
                while (Count != 0)
                {
                    _Mask = (_Mask << 1) | 1;
                    Count = Count >> 1;
                }
            }

            ///
            /// @brief  Set the bucket count of the hash table
            ///
            void resize(size_type Count)
            {
                _Maxidx = Count;
                _Mask = 0;
                while (Count != 0)
                {
                    _Mask = (_Mask << 1) | 1;
                    Count = Count >> 1;
                }
            }

            ///
            /// @brief  Get the hash value of the key. The returned value is wrapped to hash table size (bucket_count)
            ///
            size_type hash_value(const key_type& keyVal) const
            {
                return _Hashval(keyVal);
            }
        };
    }
}

#endif//_CST_TTS_BASE_CMN_HASH_MAP_H_
