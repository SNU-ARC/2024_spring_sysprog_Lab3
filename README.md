# Lab 3: Memory Lab

In this lab, we implement our own dynamic memory manager.

You will learn
   * how a dynamic memory manager works
   * how to implement a dynamic memory manager
   * how to work with macros in C
   * how to work with function pointers in C
   * how to debug code
   * that writing a dynamic memory manager is simple in theory and difficult in practice.


## Important Dates

| Date | Description |
|:---  |:--- |
| Tuesday, April 9, 18:30 | Memory Lab hand-out |
| Tuesday, April 16, 18:30 | Memory Lab session 1 |
| Tuesday, April 23, 18:30 | Memory Lab session 2 |
| Monday, April 29, 23:59 | Submission deadline |

There are no separate deadlines for phases 1 and 2. At the end of the lab, both phases should be implemented.

## Logistics

### Hand-out

You can **clone this repository** directly on your VM instance or local computer and get to work. If you want to keep your own repository, you should **keep the lab's visibility to private.** Otherwise, others would see your work. Read the instructions here carefully. Then clone the lab to your local computer and get to work. 

**Changing visibility**

After cloning the repository, you should change the push remote URL to your own repository.

1. Create an empty repository that you're going to manage **(again, keep it private)**
2. Copy the url of that repository
3. On your terminal in the cloned directory, type `git remote set-url --push origin <repo_url>`
4. Check with `git remote -v` if the push URL has changed to yours while the fetch URL remains the same (this repo)

### Submission

You should upload your archive file(.tar) containing code (memmgr.c) and report (2024-12345.pdf) via eTL. To make an archive file, follow the example below on your own VM.
```
$ ls
2024-12345.pdf  doc  Makefile  README.md  driver  src  tests
$ tar -cvf 2024-12345.tar src/memmrg.c 2024-12345.pdf
src/memmrg.c
2024-12345.pdf
$ file 2024-12345.tar
2024-12345.tar: POSIX tar archive (GNU)
```

* With VirtualBox :
```
scp -P 8888 sysprog@localhost:<target_path> <download_path>
# example: scp -P 8888 sysprog@localhost:/home/sysporg/2024_spring_sysprog_Lab3/2024-12345.tar .
```

* With UTM :
```
scp sysprog@<hostname>:<target_path> <download_path>
# example: scp sysprog@192.168.4.4:/home/sysprog/2024_spring_sysprog_Lab3/2024-12345.tar .
```

## Dynamic Memory Manager

### Overview

The dynamic memory manager implemented in this lab provides the same API as the C standard library's memory manager: `malloc()`, `free()`, `calloc()`, and `realloc()`. In addition to these four functions, additional methods for initialization, logging, and debugging need to be implemented.
The following table shows the API of our dynamic memory manager:

| API function | libc | Description |
|:---          |:---  |:---         |
| `void* mm_malloc(size_t size)`| `malloc`  | allocate a block of memory with a payload size of (at least) _size_ bytes |
| `void mm_free(void *ptr)` | `free` | free a previously allocated block of memory |
| `void* mm_calloc(size_t nelem, size_t size)` | `calloc` | allocate a block of memory with a payload size of (at least) _size_ bytes and initialize with zeroes |
| `void* mm_realloc(void *ptr, size_t size)` | `realloc` | change the size of a previously allocated block _ptr_ to a new _size_. This operation may need to move the memory block to a different location. The original payload is preserved up to _min(old size, new size)_ |
| `void mm_init(void)`  | n/a  | initialize dynamic memory manager |
| `void mm_setloglevel(int level)` | similar to `mtrace()` | set the logging level of the allocator |
| `void mm_check(void)` | simiar to `mcheck()` | check and dump the status of the heap |


### Operation

Since libc's memory allocator is built into the standard C runtime, our memory manager cannot directly manipulate the heap of the process. Instead, we operate on a simulated heap. The simulated heap is controlled with the same two functions the kernel offers for that purpose: `sbrk()` and `getpagesize()`.

The C runtime initializes the heap and the dynamic memory manager automatically whenever a process is started. For our memory manager, we have to do that manually, hence the `mm_init()` function. Similarly, the simulated heap has to be initialized before it can be used by calling `ds_allocate()`. 

The following diagram shows the organization and operation of our allocator:

```
                                      File: mm_test.c
  +-------------------------------------------------+
  | user-level process. After initializing the data |
  | segment and the heap, mm_malloc/free/calloc/    |
  | realloc can be used as in libc.                 |
  +-------------------------------------------------+
     |            |                    |
1. ds_allocate()  |       3. sequence of mm_malloc(),
     |            |           mm_free(), mm_calloc(),
     |       2. mm_init()        and mm_realloc()
     |            |                    |
     |            v                    v
     |    +-----------------------------------------+
     |    | custom memory manager. Manages the heap |
     |    +-----------------------------------------+
     |    File: memmgr.c        |
     |                       ds_sbrk()
     |                          |
     v                          v
  +-------------------------------------------------+
  | custom data segment implementation. Manages the |
  | data segment to be used by our allocator.       |
  +-------------------------------------------------+
  File: dataseg.c
 ```                                        

### Heap Organization

Our memory manager is a 64-bit operator, i.e., one word in the allocator is 8 bytes long. The heap is conceptually organized into blocks. The minimal block size is 32 bytes. Each block must have a boundary tag (header/footer). In this lab, we plan to implement two approaches to free list management: implicit and explicit free list.

The boundary tags comprise of the size of the block and an allocated bit. Since block sizes are a muliple of 32, the low 4 bits of the size are always 0. We use bit 0 to indicate the status of the block (1: allocated, 0: free).

You are free to add special sentinel blocks at the start and end of the heap to simplify the operation of the allocator.


### API Specification

### mm_init()

You can assume that `mm_init()` is called before any other operations on the heap are performed. The function needs to initialize the heap to its initial state.

### mm_malloc()

The `void* mm_malloc(size_t size)` routine returns a pointer to an allocated payload block of at least
_size_ bytes. The entire allocated block must lie within the heap region and must not overlap with
any other block.

### mm_calloc()

`void* mm_calloc(size_t nmemb, size_t size)` returns a pointer to an allocated payload block of at least
_nmemb*size_ bytes that is initialized to zero. The same constraints as for `mm_malloc()` apply.

### mm_realloc()

The `void* mm_realloc(void *ptr, size_t size)` routine returns a pointer to an allocated region of at least size bytes with the following constraints.

* if `ptr` is NULL, the call is equivalent to `mm_malloc(size)`.
* if `size` is equal to zero, the call is equivalent to `mm_free(ptr)`.
* if `ptr` is not NULL, it must point to a valid allocated block. The call to `mm_realloc` changes
the size of the memory block pointed to by `ptr` (the old block) to `size` bytes and returns the
address of the new block. Notice that the address of the new block might be the same as the
old block, or it might be different, depending on your implementation, the amount of internal
fragmentation in the old block, and the size of the `realloc` request.
The contents of the new block are the same as those of the old `ptr` block, up to the minimum of
the old and new sizes. Everything else is uninitialized. For example, if the old block is 8 bytes
and the new block is 12 bytes, then the first 8 bytes of the new block are identical to the first 8
bytes of the old block and the last 4 bytes are uninitialized. 
* if `size` is smaller than the size of the memory block pointed to by `ptr`, the block should be split into a smaller allocated block and a new free block.
* if `size` is larger than the size of the memory block pointed to by `ptr`, the following should be performed: First, check if the successor block is free and large enough when merged. If so, the memory manager should increase the block size and fix the remaining free block. If the successor block is neither free nor large enough to place the requested new size when merged, it should assign a new block by the allocation policy(best-fit) and copy the original contents. Afterwards, the original block must be freed.

### mm_free()

The `void mm_free(void *ptr)` routine frees the block pointed to by `ptr` that was returned by an earlier call to
`mm_malloc()`, `mm_calloc()`, or `mm_realloc()` and has not yet been freed. When when the callee tries to free a freed
memory block, an error is printed.


### Free block management and policies

In this lab, we will be implementing the allocation of requested blocks based on the best fit policy, utilizing both implicit and explicit free list management methods. Although multiple options are available when freeing allocated blocks using the explicit free list method, we will implement the LIFO policy.

**Best fit** allocates memory by selecting the smallest available block that is large enough to fulfill the request, aiming to reduce fragmentation and optimize space utilization. 

**Implicit free list** utilizes the spaces within the memory itself to track free blocks, integrating allocation metadata directly within the memory being managed.

**Explicit free list** manages free memory by creating a linked list of free blocks, where each block points to the next and previous block, facilitating precise and flexible memory allocation.


## Handout Overview

The handout contains the following files and directories

| File/Directory | Description |
|:---  |:--- |
| `doc/` | Doxygen instructions, configuration file, and auto-generated documentation. Run `make doc` to generate. |
| `driver/` | Pre-compiled modules required to link your implementation to the `mm_driver` test program |
| `reference/` | Reference implementation |
| `src/` | Source code of the lab. You will modify memmgr.c/h, mm_test.c |
| `tests/` | Test allocation/deallocation sequences to test your allocator |
| `README.md` | this file |
| `Makefile` | Makefile driver program |

The files you will work on or modify are all located in the `src/` directory

| File | Description |
|:---  |:--- |
| `blocklist.c/h` | Implementation of a list to manage allocated blocks for debugging/verification purposes. **Do not modify!** |
| `datasec.c/h` | Implementation of the data segment. **Do not modify!** |
| `nulldriver.c/h` | Implementation of an empty allocator that does nothing. Useful to measure overhead. **Do not modify!** |
| `memmgr.c/h` | The dynamic memory manager. A skeleton is provided. Implement your solution by editing the C file. |
| `mm_test.c`  | A simple test program to test your implementation step-by-step. |

### Reference implementation

The directory `reference` contains a simple test driver program. You can use it to understand how our allocator works but should not take the output literally.


## Phase 1

Your task in phase 1 is to implement the basic functionalities of the dynamic memory allocator(`mm_malloc()`, `mm_free()`, and `mm_realloc()`) by using implicit free list. `mm_calloc()` will work without any modification if you implemented `mm_malloc()` and `mm_free()` correctly.


### Implementation

The skeleton provides some global variables and macros that we think will be helpful, logging and panic functions, and skeletons for the individual `mm_X()` functions that are more or less complete.

Start by working on `mm_init()`. Use the `mm_check()` function to inspect your heap. Once the initial heap looks good, proceed with the implementation of `mm_malloc()`, followed by `mm_free()`.

Have a look at `mm_test.c` and modify the code in there to test different cases. Have a look at the documentation of `mm_setloglevel()`, `ds_setloglevel()`, and `mm_check()`; these functions will be very handy to understand what's going on and debug your code.


## Phase 2

In phase 2, implement the functions to also support the explicit free list. Have a close look at your implementations of phase 1, and identify the points where the next and previous block pointers should be managed. As in phase 1, utilizing global variables and macros will ease the job.

`mm_test` will show the next and previous block pointers of each free block when explicit free list mode is selected. Test your implementation, and make sure every free block is connected after any operation.

You can now test your allocator with the test driver (`mm_driver`, see below.)


## Evaluation
For the evaluation, we use a driver program that issues a series of memory allocation and deallocation requests to your implementation. The grading focuses on implementation completeness and correctness over performance.

### Grading
We expect you to implement `mm_malloc()`, `mm_free()`, `mm_calloc()`, and `mm_realloc()`.
We will test the correctness of your implementation and measure the heap utilization. Performance is only considered when testing your explicit free list implementation. Even then, the relative performance against your implicit free list implementation is considered, not the absolute performance.

### mm_driver
The handout includes a test version of `mm_driver` in pre-compiled form in the `driver/` directory. You can build the driver and link it to your memory manager by running
```bash
$ make mm_driver
gcc -Wall -Wno-stringop-truncation -O2 -g -MMD -MP -o memmgr.o -c memmgr.c
gcc -Wall -Wno-stringop-truncation -O2 -g -o mm_driver memmgr.o dataseg.o driver/blocklist.o driver/mm_driver.o
```

The `mm_driver` program takes as an input a script file that describes the test. We provide some simple tests in the directory `tests/`. The scripts should be self-explanatory.
Use as follows:
```bash
$ ./mm_driver tests/demo.dmas
Processing 'tests/demo.dmas'...
------------------------------------------------------------
Configuration:
  script file:           tests/demo.dmas

  configuration:
    implementation:      memory manager
    mode:                correctness
    freelist policy:     implicit
    data segment size:   0x2000000 (33554432)

Action: m 1 16
Action: m 2 50
...
Action: stop
------------------------------------------------------------
Statistics:
  Actions:              17
    malloc:              9
    calloc:              0
    realloc:             0
    free:                8

  Utilization:
    payload:            30 (      48) bytes
    heap size:       10000 (   65536) bytes
    utilization:         0.1%
    #sbrk():             1 times

  Performance:
    total time:          0.018493  sec
    throughput:          0.92 kops/sec
------------------------------------------------------------
```

You can test with other options. Use the following command to see the options:
```bash
$ ./mm_driver --help
```

## Hints

### Skeleton code
The skeleton code is meant to help you get started. You can modify it in any way you see fit - or implement this lab completely from scratch.

### Allocator in the textbook
The textbook contains an implementation of a simple allocator with a free list in chapter 9.9.12. 
It is quite similar to what you need to do here. Try by yourself first. If you are stuck, study the allocator from the book and apply your gained knowledge to your code.

### Final words

Implementing a dynamic memory allocator is easy in theory and very difficult in practice. A tiny mistake may destroy your heap, and to make matters worse, you may not notice that error until a few `mm_malloc()` and `mm_free()` operations later. At the beginning, you may feel overwhelmed and have no idea how to approach this task. 

Do not despair - we will give detailed instructions during the lab sessions and provide individual help so that each of you can finish this lab. After completing this lab, you will understand dynamic memory allocation from both the user and the system perspective.

<div align="center" style="font-size: 1.75em;">

**Happy coding!**
</div>
