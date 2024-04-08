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

#include <assert.h>
#include "blocklist.h"

Block *head = NULL;
Block *tail = NULL;

void init_blocklist(void)
{
  if (head != NULL) free_blocklist();

  //
  // create head & tail sentinels
  //
  head = calloc(1, sizeof(Block));
  tail = calloc(1, sizeof(Block));

  head->next = tail;
  tail->prev = head;

  // We set
  //   head->ptr = NULL
  //   tail->ptr = (void*)-1
  //
  // This simplifies the search for a (valid) ptr since the invariant
  //   head->ptr < ptr < tail->ptr
  // always holds.
  head->ptr  = NULL;
  tail->ptr  = (void*)-1;
}

void free_blocklist(void)
{
  Block *b = head;
  while (b != NULL) {
    Block *next = b->next;
    free(b);
    b = next;
  }
  head = tail = NULL;
}

Block* insert_block(void *ptr, size_t size, int flags)
{
  assert(head != NULL);
  assert((ptr != NULL) && (ptr != (void*)-1));

  Block *b = calloc(1, sizeof(Block));
  if (b != NULL) {
    b->ptr = ptr;
    b->size = size;
    b->flags = flags;

    Block *s = head;
    while (s->ptr <= ptr) s = s->next;

    b->next = s;
    b->prev = s->prev;
    s->prev = b;
    b->prev->next = b;
  }

  return b;
}

Block* find_block(void *ptr)
{
  assert(head != NULL);
  assert((ptr != NULL) && (ptr != (void*)-1));

  Block *b = head;
  while (b->ptr < ptr) b = b->next;

  return b->ptr == ptr ? b : NULL;
}

Block* find_block_by_index(size_t idx)
{
  assert(head != NULL);

  Block *b = head->next;
  while ((b != tail) && (idx > 0)) {
    b = b->next;
    idx--;
  }

  return b != tail ? b : NULL;
}

int delete_block(void *ptr)
{
  assert(head != NULL);
  assert((ptr != NULL) && (ptr != (void*)-1));

  Block *b = find_block(ptr);
  if (b != NULL) {
    b->prev->next = b->next;
    b->next->prev = b->prev;
    free(b);
  }

  return b != NULL;
}

const Block* first_block(void)
{
  return find_block_by_index(0);
}

const Block* next_block(const Block *b)
{
  assert(head != NULL);
  assert(b != NULL);

  b = b->next;

  return b != tail ? b : NULL;
}

int iterate_blocks(int (*callback)(const Block *, size_t, void*), void *ptr)
{
  assert(head != NULL);
  assert(callback != NULL);

  int res = 0;
  size_t idx = 0;
  Block *b = head->next;
  while ((b != tail) && (res == 0)) {
    res = callback(b, idx, ptr);
    b = b->next;
    idx++;
  }

  return res;
}

size_t num_blocks(void)
{
  assert(head != NULL);

  size_t res = 0;
  Block *b = head->next;

  while (b != tail) {
    b = b->next;
    res++;
  }

  return res;
}

Block** get_block_array(void)
{
  assert(head != NULL);

  size_t nblocks = num_blocks();
  Block **res = (Block**)calloc(nblocks+1, sizeof(Block*));

  if (res != NULL) {
    size_t idx = 0;
    Block *b = head->next;
    while ((b != tail) && (idx < nblocks)) {
      res[idx] = b;
      idx++;
      b = b->next;
    }
    assert(idx == nblocks);
  }

  return res;
}
