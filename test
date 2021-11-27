/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implentation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2021
 *
 */

#include <stdio.h>
#include "cache_impl.h"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */

    /* Invoke check_cache_data_hit() */


    
    /* In case of the cache miss event, access the main memory by invoking access_memory() */



    return value_returned;    
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();
    
    ifp = fopen("access_input.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Fill out here by invoking retrieve_data() */





    fclose(ifp);
    fclose(ofp);
    
    print_cache_entries();
    return 0;
}
