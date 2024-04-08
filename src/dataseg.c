//--------------------------------------------------------------------------------------------------
// System Programming                       Memory Lab                                   Spring 2024
//
/// @file
/// @brief simulated data segment
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

//
// Simulated data segment
// ======================
// This module implements a simulated data segment to be used by our dynamic memory allocator.
//
// Design:
// -------
// The data segment encloses the heap area that is presented to the user. One memory page at the
// beginning and another one at the end are always marked PROT_NONE to catch accidential accesses
// outside the assigned heap area.
//
// In particular, only the area between ds_heap_start and ds_heap_brk is marked READ/WRITE, every-
// thing else is marked PROT_NONE. This helps catching accesses beyond the brk pointer.
//
//
// ds_start     ds_heap_start       ds_heap_brk             ds_heap_end      ds_end
//    |              |                   |                       |              |
//    v              v                   v                       v              v
//    +--------------+===================+--------------------------------------+
//    |  no access   | read/write access |        no access      |  no access   |
//    +--------------+===================+--------------------------------------+
//     <- one page -> <---          max_heap_size            ---> <- one page ->
//
//
// Operation:
// ----------
// The data segment is allocated and initalized by calling ds_allocate(). Initially, the
// start of the heap and the brk pointer both point to the beginning of the heap
// (i.e., to ds_start + PAGESIZE).
//
// The heap size can be adjusted by calling ds_sbrk(). The memory protection flags are set 
// automatically whenever the ds_heap_brk pointer is adjusted.
//
// ds_heap_stat() can be used to retrieve information about the heap area.
//
// ds_release() releases all memory and resets all internal variables. A subsequent call to
// ds_allocate() is supported and initializes a 'fresh' heap.
//

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dataseg.h"


static void *ds_start = NULL;       ///< start of the data segment
static void *ds_end   = NULL;       ///< end of the data segment
static void *ds_heap_start = NULL;  ///< start of the user space heap
static void *ds_heap_brk   = NULL;  ///< current logical end of the user space heap
static void *ds_heap_end   = NULL;  ///< end of the user space heap
static int  PAGESIZE  = 0;          ///< (system) page size
static int  ds_initialized = 0;     ///< initialized flag (yes: 1, otherwise 0)
static int  ds_loglevel    = 0;     ///< log level (0: off; 1: info; 2: verbose)
static int  ds_domprotect  = 1;     ///< mprotect() heap areas (0: off, 1: on)
static ssize_t ds_num_sbrk = 0;     ///< number of times ds_sbrk() was called with a non-zero 
                                    ///< argument


/// @brief print a log message if level <= ds_loglevel. The variadic argument is a printf format
///        string followed by its parametrs
#ifdef DEBUG
  #define LOG(level, ...) ds_log(level, __VA_ARGS__)

/// @brief print a log message. Do not call directly; use LOG() instead
/// @param level log level of message.
/// @param ... variadic parameters for vprintf function (format string with optional parameters)
static void ds_log(int level, ...)
{
  if (level > ds_loglevel) return;

  va_list va;
  va_start(va, level);
  const char *fmt = va_arg(va, const char*);

  if (fmt != NULL) vfprintf(stdout, fmt, va);

  va_end(va);

  fprintf(stdout, "\n");
}

#else
  #define LOG(level, ...)
#endif

void ds_allocate(size_t max_heap_size)
{
  LOG(1, "ds_allocate(%lx)", max_heap_size);

  if (ds_start != NULL) ds_release();

  PAGESIZE = getpagesize();
  size_t ds_size = max_heap_size + 2*PAGESIZE;

  // allocate memory for the data segment
  LOG(2, "  allocating %lx bytes of memory", ds_size);
  ds_start = mmap(NULL, ds_size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
  if (ds_start == (void*)-1) {
    fprintf(stderr, "ERROR: cannot map memory in %s: %s.\n",
                    __func__, strerror(errno));
    exit(EXIT_FAILURE);
  }

  // try to lock the memory in RAM. Print only a warning if we don't succeed.
  /* don't do this for now. Requires changing resource limits in VM.
  LOG(2, "  locking memory in DRAM...", ds_size);
  if (mlock(ds_start, ds_size) < 0) {
    fprintf(stderr, "WARNING: cannot lock memory in %s: %s.\n",
                    __func__, strerror(errno));
  }
  */

  // initalize pointers
  ds_end         = ds_start + ds_size;
  ds_heap_start  = ds_start + PAGESIZE;
  ds_heap_brk    = ds_heap_start;
  ds_heap_end    = ds_end - PAGESIZE;
  ds_initialized = 1;
  ds_num_sbrk    = 0;

  LOG(2, "  ds_start:           %p\n"
         "  ds_heap_start:      %p\n"
         "  ds_heap_brk:        %p\n"
         "  ds_heap_end:        %p\n"
         "  ds_end:             %p\n"
         "  PAGESIZE:           %d\n",
         ds_start, ds_heap_start, ds_heap_brk, ds_heap_end, ds_end, PAGESIZE);
}


void ds_release(void)
{
  LOG(1, "ds_release()");

  if (ds_start != NULL) {
    // unlock & release memory. Ignore error message here.
    //munlock(ds_start, ds_end-ds_start);
    munmap(ds_start, ds_end-ds_start);
  }

  ds_start = ds_end = ds_heap_start = ds_heap_brk = ds_heap_end = NULL;
  PAGESIZE = 0;
  ds_initialized = 0;
}


void* ds_sbrk(intptr_t increment)
{
  LOG(1, "ds_sbrk(%c0x%lx)", increment < 0 ? '-' : '+', labs(increment));
  assert(ds_initialized);

  void *old_heap_brk = ds_heap_brk;

  if (increment != 0) {
    ds_heap_brk += increment;
    ds_num_sbrk++;

    if ((ds_heap_start <= ds_heap_brk) && (ds_heap_brk < ds_heap_end)) {
      if (ds_domprotect) {
        // adjust memory access permissions
        // since we are not forcing alignment of brk at PAGESIZE, we need to mark the invalid part
        // before allowing access to the permissible area because permissions are set on a page-level
        // basis
        LOG(2, "  setting memory protection:\n"
            "    READ/WRITE from %p to %p\n"
            "    NO ACCESS  from %p to %p\n",
            ds_heap_start, ds_heap_brk, ds_heap_brk, ds_end);

        void *aligned_brk = (void*)(((unsigned long)ds_heap_brk) / PAGESIZE * PAGESIZE); // round down

        if ((mprotect(aligned_brk, ds_end-aligned_brk, PROT_NONE) != 0) ||
            (mprotect(ds_heap_start, ds_heap_brk-ds_heap_start, PROT_READ|PROT_WRITE) != 0))
        {
          fprintf(stderr, "ERROR: cannot set memory protection flags in %s: %s.\n", 
              __func__, strerror(errno));
          exit(EXIT_FAILURE);
        }
      }
    } else {
      // ignore increment and signal an error if we ended up outside the simulated data segment
      LOG(1, "  invalid increment (ended up outside valid data segment)");
      errno = ENOMEM;
      ds_heap_brk = old_heap_brk;
      old_heap_brk = (void*)-1;
    }
  }

  return old_heap_brk;
}


int ds_getpagesize(void)
{
  assert(ds_initialized);

  return PAGESIZE;
}


void ds_heap_stat(void **start, void **brk, void **end)
{
  if (start) *start = ds_heap_start;
  if (brk)   *brk   = ds_heap_brk;
  if (end)   *end   = ds_heap_end;
}


ssize_t ds_getnsbrk(void)
{
  return ds_num_sbrk;
}

void ds_setloglevel(int level)
{
  ds_loglevel = level;
}


void ds_setmprotect(int active)
{
  ds_domprotect = (active > 0);
}


