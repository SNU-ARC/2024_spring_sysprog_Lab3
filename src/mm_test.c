//--------------------------------------------------------------------------------------------------
// System Programming                       Memory Lab                                   Spring 2024
//
/// @file
/// @brief dynamic memory manager test program
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dataseg.h"
#include "memmgr.h"
#include "blocklist.h"

#define ALLOC 1
#define FREE  0

void enter(void)
{
  printf("Press enter to continue.\n");
  getchar();
}

void do_malloc(void)
{
  printf("(1) enter payload size(s) to malloc: ");

  char *str = NULL;
  size_t lstr = 0;

  if (getline(&str, &lstr, stdin) > 0) {
    int pos = 0, bread;
    size_t bytes;
    while (sscanf(&str[pos], "%lu%n", &bytes, &bread) == 1) {
      printf("  allocating %lu bytes\n", bytes);
      void *ptr = mm_malloc(bytes);
      if (ptr != NULL) insert_block(ptr, bytes, 1);
      pos += bread;
    }
  }

  if (str != NULL) free(str);
}

void do_free(void)
{
  printf("(2) Block list\n");

  Block **block = get_block_array();
  size_t nblocks = 0;
  while (block[nblocks] != NULL) {
    Block *b = block[nblocks];
    printf("  [%3lu] %p: size: %6lx (%7ld), status: %s\n",
           nblocks, b->ptr, b->size, b->size,
           b->flags == ALLOC ? "allocated" : "free");
    nblocks++;
  }
  printf("\nEnter index/indices of blocks to free: ");

  char *str = NULL;
  size_t lstr = 0;

  if (getline(&str, &lstr, stdin) > 0) {
    int pos = 0, bread;
    size_t index;
    while (sscanf(&str[pos], "%lu%n", &index, &bread) == 1) {
      printf("  freeing %lu-th block\n", index);
      if (index < nblocks) {
        Block *b = block[index];
        mm_free(b->ptr);
        delete_block(b->ptr);
      } else {
        printf("    --> no such block\n");
      }
      pos += bread;
    }
  }

  free(block);
}

void do_setloglevel(void)
{
}

int main(int argc, char *argv[])
{
  void *ptr[100];
  unsigned int idx = 0;
  unsigned debug = 0;

  init_blocklist();

  ds_setloglevel(0);
  mm_setloglevel(0);

  // !! Add freelist policy selection !!
  char *line = NULL;
  size_t llen = 0;
  char c = '\0';

  FreelistPolicy fp = fp_Implicit;

  do {
    printf("-----------------------------------\n"
           "  Select freelist policy.\n"
           "(i) implicit list\n"
           "(e) explicit list\n"
           "(q) quit\n"
           "Your selection: ");
    fflush(stdout);

    if (getline(&line, &llen, stdin) > 0) {
      c = line[0];
      switch (c) {
        case 'i': fp = fp_Implicit; break;
        case 'e': fp = fp_Explicit; break;
        case 'q': return EXIT_SUCCESS;
        default:  if (c > ' ') printf("Invalid selection.\n");
      }
    } else {
      printf("Error reading character.\n");
    }
  } while (c != 'i' && c != 'e');

  printf("\n\n\n----------------------------------------\n"
         "  Initializing heap...\n"
         "\n\n");
  ds_allocate(32*1024*1024);

  // !! replace to freelist policy !!
  mm_init(fp);
  mm_check();

  printf("\n\n\n----------------------------------------\n"
         "  Testing mm_malloc()...\n"
         "\n\n");

  mm_setloglevel(2);

  do {
    printf("-----------------------------------\n"
           "(m) malloc\n"
           "(f) free\n"
           "(c) check heap\n"
           "(l) set log level\n"
           "(q) quit\n"
           "Your selection: ");
    fflush(stdout);

    if (getline(&line, &llen, stdin) > 0) {
      c = line[0];
      switch (c) {
        case 'm': do_malloc(); break;
        case 'f': do_free(); break;
        case 'c': mm_check(); break;
        case 'l': do_setloglevel(); break;
        case 'q': break;
        default:  if (c > ' ') printf("Invalid selection.\n");
      }
    } else {
      printf("Error reading character.\n");
    }
  } while (c != 'q');

  if (line != NULL) free(line);

  return EXIT_SUCCESS;



  debug = 1;
  ds_setloglevel(0);
  mm_setloglevel(0);

  enter(); ptr[idx++] = mm_malloc(1);       if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(15);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(16);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(17);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(31);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(32);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(33);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(47);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(48);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(49);      if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(100);     if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(200);     if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(400);     if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(1024);    if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(0x1000);  if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(0x2000);  if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(0x4000);  if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(0x8000);  if (debug) mm_check();
  enter(); ptr[idx++] = mm_malloc(0x10000); if (debug) mm_check();

  printf("\n\n\n----------------------------------------\n"
         "  Testing mm_free()...\n"
         "\n\n");
  debug = 1;
  ds_setloglevel(0);
  mm_setloglevel(0);

  enter(); mm_free(ptr[0]);  if (debug) mm_check();
  enter(); mm_free(ptr[1]);  if (debug) mm_check();
  enter(); mm_free(ptr[3]);  if (debug) mm_check();
  enter(); mm_free(ptr[2]);  if (debug) mm_check();
  enter(); mm_free(ptr[7]);  if (debug) mm_check();
  enter(); mm_free(ptr[6]);  if (debug) mm_check();
  enter(); mm_free(ptr[5]);  if (debug) mm_check();
  enter(); mm_free(ptr[4]);  if (debug) mm_check();
  enter(); mm_free(ptr[10]); if (debug) mm_check();


  
  printf("\n\n\n----------------------------------------\n"
         "  Testing mm_realloc()...\n"
         "\n\n");
  debug = 1;
  ds_setloglevel(0);
  mm_setloglevel(2);

  enter(); mm_realloc(ptr[9], 50);  if (debug) mm_check();
  enter(); mm_realloc(ptr[9], 60);  if (debug) mm_check();
  enter(); mm_realloc(ptr[9], 48);  if (debug) mm_check();
  enter(); mm_realloc(ptr[9], 220); if (debug) mm_check();
  

  return EXIT_SUCCESS;
}


