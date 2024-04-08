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

#ifndef __DATASEG_H__
#define __DATASEG_H__

#include <unistd.h>

/// @brief initialize simulated data segment. Allocates & locks memory pages in RAM to minimize
///        performance variance.
/// @param max_heap_size maximum possible size of heap data segment
void ds_allocate(size_t max_heap_size);

/// @brief release simulated data segment
void ds_release(void);

/// @brief sbrk() implementation on our simulated data segment. Operates exactly as the kernel's
///        sbrk() function (see man sbrk)
/// @param increment offset by which to increase/decrease current brk.
/// @retval old brk on success.
/// @retval (void*)-1 on error. errno is set to ENOMEM
void* ds_sbrk(intptr_t increment);

/// @brief retrieve pagesize of data segment
/// @retval page size
/// @retval 0 if not data segment not initialized)
int ds_getpagesize(void);

/// @brief retrieve statistics about our data segment. Returns start, end, and current brk address.
/// @param[out] start starting address of user-space heap
/// @param[out] brk   current break pointer of user-space heap
/// @param[out] end   largest possible address of user-space heap
/// @param[out] nsbrk number of times sbrk() was called with a non-zero argument
void ds_heap_stat(void **start, void **brk, void **end);

/// @brief retrieve the number of sbrk() was called with a non-zero argument
/// @retval ssize_t number of sbrk() calls
ssize_t ds_getnsbrk(void);

/// @brief set log level
/// @brief level log level (0: no logging, 1: info; 2: verbose)
void ds_setloglevel(int level);

/// @brief turn mprotect() on/off
/// @brief active (1: mprotect() activated, 0: mprotect() not executed)
void ds_setmprotect(int active);

#endif // __DATSEG_H__
