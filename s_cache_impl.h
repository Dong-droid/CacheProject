#pragma once

/* DO NOT CHANGE THE FOLLOWING DEFINITIONS EXCEPT 'DEFAULT_CACHE_ASSOC */

#ifndef _CACHE_IMPL_H_
#define _CACHE_IMPL_H_

#define WORD_SIZE_BYTE 4 // 1 word = 4 bytes
#define DEFAULT_CACHE_SIZE_BYTE 32 // cache size = 32 bytes
#define DEFAULT_CACHE_BLOCK_SIZE_BYTE 8 // block size = 8 bytes
#define DEFAULT_CACHE_ASSOC 4// association = 2
// direct 1, 2-way 2, fully 4
#define DEFAULT_MEMORY_SIZE_WORD 128 // main memory size = 128 words
// = 128 * 4 bytes
#define CACHE_ACCESS_CYCLE 2 // cache access time = 1 cycle
#define MEMORY_ACCESS_CYCLE 100 // main memory access time
// = 100 cycles
#define CACHE_SET_SIZE ((DEFAULT_CACHE_SIZE_BYTE) / (DEFAULT_CACHE_BLOCK_SIZE_BYTE * DEFAULT_CACHE_ASSOC))
// set size=[direct]32/(8*1)=4, [2-way]32/(8*2)=2, [full] 32/(8*4)=1

/* Function Prototypes */
void init_memory_content();
void init_cache_content();
void print_cache_entries();
void func(void* x, char type);
int check_cache_data_hit(void* addr, char type);
int access_memory(void* addr, char type);
int retrieve_data(void* addr, char data_type);

/* Cache Entry Structure */
typedef struct cache_entry {
    int valid; // present = 1, not present = 0
    int tag; // tag of the stored data
    int timestamp; // most recent access time
    char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE]; // data from memory[address]
} cache_entry_t; // define type as cache_entry_t

#endif
