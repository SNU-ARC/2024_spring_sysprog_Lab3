//--------------------------------------------------------------------------------------------------
// System Programming                       Memory Lab                                   Spring 2024
//
/// @file
/// @brief dynamic memory manager
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/09/27 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2020-2023, Computer Systems and Platforms Laboratory, SNU
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice, this list of condi-
///   tions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
///   tions and the following disclaimer in the documentation and/or other materials provided with
///   the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
/// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
/// CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)   HOWEVER CAUSED AND ON ANY THEORY OF
/// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//--------------------------------------------------------------------------------------------------

#ifndef __MEMMGR_H__
#define __MEMMGR_H__

#include <stddef.h>

// !! Remove allocation policy !!

/// @brief supported free list managing policies
typedef enum {
  fp_Implicit,                    ///< Implicit list management
  fp_Explicit,                    ///< Explicit list management
} FreelistPolicy;

/// @brief initialize heap. Must be called before any of the other functions can be used.
void mm_init(FreelistPolicy ap);

/// @brief allocate a block of memory of @a size bytes
/// @param size requested size in bytes
/// @retval void* pointer to first byte of memory on success
/// @retval NULL if memory allocation failed
void* mm_malloc(size_t size);

/// @brief allocate a block of memory of @a nelem * @a size bytes initialized with zeroes.
/// @param nelem number of elements
/// @param size size of one element in bytes
/// @retval void* pointer to first byte of zeroed memory on success
/// @retval NULL if memory allocation failed
void* mm_calloc(size_t nelem, size_t size);

/// @brief re-allocate a block of memory to change its size to @a size bytes.
/// @param ptr previously allocated block or NULL
/// @param size requested new size in bytes
/// @retval void* pointer to first byte of re-allocated memory on success
/// @retval NULL if memory allocation failed
void* mm_realloc(void *ptr, size_t size);

/// @brief free a previously allocated block of memory
/// @param ptr pointer to allocated memory obtained by calling mm_malloc, mm_calloc, or mm_realloc
void mm_free(void *ptr);

/// @brief set log level
/// @brief level log level (0: no logging, 1: info; 2: verbose)
void mm_setloglevel(int level);

/// @brief dump heap and perform some sanity checks
void mm_check(void);

#endif // __MEMMGR_H__
