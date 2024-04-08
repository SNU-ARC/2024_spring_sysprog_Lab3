//--------------------------------------------------------------------------------------------------
// System Programming                       Memory Lab                                   Spring 2024
//
/// @file
/// @brief block list management for dynamic memory manager test program
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/10/04 Bernhard Egger created
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

#ifndef __BLOCKLIST_H__
#define __BLOCKLIST_H__

#include <stdlib.h>

/// @brief Block structure in doubly-linked list to hold information about a memory block
typedef struct __block {
  struct __block  *prev, *next;   ///< prev/next pointers in linked list
  void            *ptr;           ///< pointer to memory block (payload) obtained by malloc() et al
  size_t          size;           ///< size of payload
  int             flags;          ///< flags
} Block;

/// @brief initialize blocklist
void init_blocklist(void);

/// @brief free entrie blocklist
void free_blocklist(void);

/// @brief insert a block into the blocklist
/// @param ptr pointer to memory block obtained by malloc() and variants
/// @param size block (payload) size
/// @param flags block flags
/// @retval Block* pointer to inserted Block structure
/// @retval NULL on failure
Block* insert_block(void *ptr, size_t size, int flags);

/// @brief find a block in the blocklist by its payload pointer @a ptr
/// @param ptr pointer to memory block
/// @retval Block* pointer to Block structure containin @a ptr
/// @retval NULL if no such Block exists
Block* find_block(void *ptr);

/// @brief find a block in the blocklist by its index in the list
/// @param idx index
/// @retval Block* pointer to @a idx-th Block structure
/// @retval NULL if no such Block exists
Block* find_block_by_index(size_t idx);

/// @brief delete a block from the blockslist
/// @param ptr pointer to memory block
/// @retval 1 on success
/// @retval 0 if no such Block exists
int delete_block(void *ptr);

/// @brief get the first block in the list
/// @retval Block* pointer to first block
/// @retval NULL if list is empty
const Block* first_block(void);

/// @brief get the next block after @a b
/// @param b current block
/// @retval Block* pointer to block following @a b
/// @retval NULL if @a b is the last block in the list
const Block* next_block(const Block *b);

/// @brief iterate through blocklist and call @a callback for each element
/// @param callback callback function. Iteration continues as long as @a callback returns 0.
///                 The iterator function receives a pointer to the block, the index of the
///                 block in the list, and the pointer @a ptr provided to iterate_blocks().
/// @param ptr pointer passed to callback
/// @retval 0 if entire list has been iterated
/// @retval otherwise: return value of last callback
int iterate_blocks(int (*callback)(const Block *, size_t, void*), void *ptr);

/// @brief get the number of blocks
/// @retval size_t number of blocks in list
size_t num_blocks(void);

/// @brief obtain a copy of the list as an array
/// @retval array of Blocks (ordered by ptr), terminated by a NULL entry
Block** get_block_array(void);

#endif // __BLOCKLIST_H__



