#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include "cache.h"
#include <math.h>

/* hit ratio = (num_cache_hits / (num_cache_hits + num_cache_misses)) */
int num_cache_hits = 0;   // # of hits
int num_cache_misses = 0; // # of misses

/* bandwidth = (num_bytes / num_acess_cycles) */
int num_bytes = 0;		   // # of accessed bytes
int num_access_cycles = 0; // # of clock cycles
int global_timestamp = 0;  // # of data access trial

FILE *ifp = NULL, *ofp = NULL; // file pointers

unsigned long int access_addr; // byte address (located at 1st column)
char access_type;  // 'b'(byte), 'h'(halfword), or 'w'(word) // (located at 2nd
				   // column)
int accessed_data; // data to retrive

/* initialize memory and cache by invoking init_memory_content() and
 * init_cache_content() */
cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];

int main() {
	/*initialize */
	init_memory_content();
	init_cache_content();

	/* open input file as reading mode */
	ifp = fopen("input.txt", "r");
	if (ifp == NULL) {
		printf("Can't open input file\n");
		return -1;
	}
	/*open output file as writing mode */
	ofp = fopen("output.txt", "w");
	if (ofp == NULL) {
		printf("Can't open output file\n");
		fclose(ifp);
		return -1;
	}

	/* read each line and get the data in given (address, type) by invoking
	 * retrieve_data() */
	while (EOF != fscanf(ifp, "%lu %c", &access_addr, &access_type)) {
		fprintf(
			ofp, "===== addr %lu type %c =====\n", access_addr, access_type);
		printf("===== addr %lu type %c =====\n", access_addr, access_type);
		accessed_data =
			retrieve_data(&access_addr, access_type); // invoke retrieve_data()
		fprintf(ofp, "0x%x\n", accessed_data);		  // print accessed data
		// make sum of num_bytes to calculate bandwidth
		if (access_type == 'b')
			num_bytes++;
		else if (access_type == 'h')
			num_bytes += 2; // half word = 2 bytes
		else if (access_type == 'w')
			num_bytes += 4; // word = 4 bytes
		/* print hit ratio and bandwidth for each cache mechanism as regards to
		 * cache association size */
		global_timestamp++; // increasae timestamp

		/* print the final cache entries by invoking print_cache_entries() */
		print_cache_entries();
		printf("\n");
	}
	/* caculate hit ratio & bandwidth */
	float hit_ratio =
		(float)num_cache_hits / (num_cache_hits + num_cache_misses);
	float bandwidth = (float)num_bytes / num_access_cycles;
	fprintf(
		ofp,
		"hit ratio = %.2f (%d / %d)\nbandwidth = %.2f (%d / %d)\n",
		/* print hit ratio and bandwidth */
		hit_ratio,
		num_cache_hits,
		(num_cache_hits + num_cache_misses),
		bandwidth,
		num_bytes,
		num_access_cycles);
	/* print hit ratio and bandwidth */
	printf(
		"hit ratio = %.2f (%d / %d)\nbandwidth = %.2f (%d / %d)\n",
		hit_ratio,
		num_cache_hits,
		(num_cache_hits + num_cache_misses),
		bandwidth,
		num_bytes,
		num_access_cycles);

	/* close files */
	fclose(ifp);
	fclose(ofp);

	/* print the final cache entries by invoking print_cache_entries() */
	print_cache_entries();
	printf("\n");

	return 0;
}

void init_memory_content() {
	unsigned char sample_upward[16] = {0x001,
									   0x012,
									   0x023,
									   0x034,
									   0x045,
									   0x056,
									   0x067,
									   0x078,
									   0x089,
									   0x09a,
									   0x0ab,
									   0x0bc,
									   0x0cd,
									   0x0de,
									   0x0ef};
	unsigned char sample_downward[16] = {0x0fe,
										 0x0ed,
										 0x0dc,
										 0x0cb,
										 0x0ba,
										 0x0a9,
										 0x098,
										 0x087,
										 0x076,
										 0x065,
										 0x054,
										 0x043,
										 0x032,
										 0x021,
										 0x010};
	int index, i = 0, j = 1, gap = 1;
	printf("DATA >>\n");

	for (index = 0; index < DEFAULT_MEMORY_SIZE_WORD; index++) { // 128
		printf(" [i = %d, j = %d, gap = %d] ", i, j, gap);
		memory_array[index] = (sample_upward[i] << 24) |
			(sample_upward[j] << 16) // 1word = 4 bytes = 4 *8bit= 32bits
			| (sample_downward[i] << 8) | (sample_downward[j]);
		if (++i >= 16)
			i = 0; // cycle
		if (++j >= 16)
			j = 0;					// cycle
		if (i == 0 && j == i + gap) // difference of i and j == gap
			j = i + (++gap);		// increases 1 gap and new j for each cycle
		printf("\tmem[%d] = %#x\n", index, memory_array[index]);
	}
	printf("\n\n");
}
void init_cache_content() {
	int i, j;
	/* initialize cache data */
	for (i = 0; i < CACHE_SET_SIZE; i++) {
		for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
			cache_entry_t *pEntry = &cache_array[i][j];
			pEntry->valid = 0;	 // invalid
			pEntry->tag = -1;	  // no tag
			pEntry->timestamp = 0; // no access trial
		}
	}
}
void print_cache_entries() {
	int i, j, k;
	printf("ENTRY >>\n");
	// for each set
	for (i = 0; i < CACHE_SET_SIZE; i++) {
		printf(" [Set %d] ", i);
		// for each entry in a set
		for (j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
			cache_entry_t *pEntry = &cache_array[i][j];
			printf(
				"\n Valid: %d Tag: %#x Time: %d Data: ",
				pEntry->valid,
				pEntry->tag,
				pEntry->timestamp);
			// for each block in a entry
			for (k = 0; k < DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
				printf("(%d)%#x ", k, pEntry->data[k]);
			}
			printf("\t");
		}
		printf("\n");
	}
}

int check_cache_data_hit(void *addr, char type) {
	int *n = addr;
	int tag = floor(*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE) /
		CACHE_SET_SIZE; // calculate tag
	int index = (int)floor(*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE) %
		CACHE_SET_SIZE; // calculate index
	// print block address, byte offset, cache inde, and tag
	printf(
		"CACHE >> block_addr = %d, byte_offset = %d, cache_index = %d, tag = "
		"%d\n",
		*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE, // block_addr
		*n % DEFAULT_CACHE_BLOCK_SIZE_BYTE, // byte_offset
		index,								// cache_index
		tag									// tag
	);
	/* add this cache access cycle to global access cycle */
	num_access_cycles += CACHE_ACCESS_CYCLE;

	/* check all entries in a set */
	int byte_offset = *n % DEFAULT_CACHE_BLOCK_SIZE_BYTE;
	for (int j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
		cache_entry_t *pEntry = &cache_array[index][j];
		if (pEntry->valid == 1 &&
			tag == pEntry->tag) { // if valid ==1 and those tags are same, then
								  // hit or miss
			printf("=> Hit!\n");
			num_cache_hits++; // increase #cache hit
			pEntry->timestamp =
				global_timestamp; // update timestamp for hit case
			int return_data = 0;
			switch (type) {
			case 'b':
				return pEntry->data[byte_offset]; // return data
				break;
			case 'h':
				for (int i = 0; i <= 1; i++) {
					return_data |= (pEntry->data[i + byte_offset] & 255)
						<< 8 * i;
				}
				return return_data;
				break;
			case 'w':
				for (int i = 0; i < 4; i++) {
					return_data |= (pEntry->data[i + byte_offset] & 255)
						<< 8 * i;
				}
				return return_data;
				break;
			}
		}
	}
	printf("=> Miss!\n");
	num_cache_misses++; // increase #cache miss
	// return -1 for missing
	return -1;
}
int find_entry_index_in_set(int cache_index) {
	int entry_index;

	/* If the set has only 1 entry, return index 0 */
	if (DEFAULT_CACHE_ASSOC == 1)
		return 0;
	/* Check if there exists any empty cache space by checking 'valid' */
	for (int j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
		cache_entry_t *pEntry = &cache_array[cache_index][j];
		if (pEntry->valid == 1)
			continue;
		return j;
	}
	/* Otherwise, search over all entries to find the least recently used entry
	 * by checking 'timestamp' */
	int temp = 1000000000; // assume that temp has a very large number
	for (int j = 0; j < DEFAULT_CACHE_ASSOC; j++) {
		cache_entry_t *pEntry = &cache_array[cache_index][j];
		if (pEntry->timestamp < temp) { // find out LRU data
			temp = pEntry->timestamp;   // update temp
			entry_index = j;
		}
	}
	return entry_index;
}

int access_memory(void *addr, char type) { // miss -> copy
	/* get the entry index by invoking find_entry_index_in_set() for copying to
	 * the cache */
	int data = 0;
	int *n = addr;
	int right_shift = 8; // 1byte = 8bits

	int cache_index = (int)floor(*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE) %
		CACHE_SET_SIZE; // caculate cache index
	int entry_index =
		find_entry_index_in_set(cache_index); // calculate entry index

	/* add this main memory access cycle to global access cycle */
	num_access_cycles += MEMORY_ACCESS_CYCLE;

	/* Fetch the data from the main memory and copy them to the cache */
	cache_entry_t *pEntry = &cache_array[cache_index][entry_index];
	pEntry->valid = 1; // update valid to 1
	pEntry->tag = floor(*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE) /
		CACHE_SET_SIZE;					  // caculate tag
	pEntry->timestamp = global_timestamp; // update timestamp for miss case
	int word_index = floor(*n / DEFAULT_CACHE_BLOCK_SIZE_BYTE) *
		DEFAULT_CACHE_BLOCK_SIZE_BYTE /
		WORD_SIZE_BYTE; // calculate word index to copy
	int byte_offset =
		*n % DEFAULT_CACHE_BLOCK_SIZE_BYTE;			   // calculate byte offset
	printf("MEMORY >> word index = %d\n", word_index); // print word index
	int val = memory_array[word_index];

	// copy  the first word from memory to cache
	for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE - 4; i++) {
		pEntry->data[i] = val;	// copy data
		val = val >> right_shift; // shift right1 byte
	}
	// copy the second word from memory to cache
	val = memory_array[word_index + 1];
	for (int i = 4; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
		pEntry->data[i] = val;	// copy data
		val = val >> right_shift; // shift right 1 byte
	}
	// copy done

	val = memory_array[word_index + 2]; // use one more word from memory for
										// corner cases
	// set (accessed)data to return according to the char type
	switch (type) {
	case 'b':
		data = pEntry->data[byte_offset];
		break;
	case 'h':
		if (byte_offset == 7)
			data = ((val & 255) << 8) |
				(pEntry->data[byte_offset] &
				 255); // when byte offset is 7, use one more word for corner
					   // case
		else
			data = (pEntry->data[byte_offset + 1] << 8) |
				(pEntry->data[byte_offset] &
				 255); // use bit & operation to make all bits 0 except [8-0]
					   // bits
		break;
	case 'w':
		// when byte offset is bigger than 4, use one more word
		if (byte_offset > 4) {
			int shift_amount = 8 - byte_offset; // cacluate shift amount
			for (int i = 0; i <= 7 - byte_offset;
				 i++) { // until byte offset reaches the end of the word
				data |=
					(pEntry->data[i + byte_offset] &
					 255); // use bit & operation to make all bits 0 except
						   // [8-0] bits
			}
			// copy one more word for corner case
			for (int i = 0; i < byte_offset - 4; i++) {
				data |= (val & 255)
					<< 8 * (shift_amount++); // shift left to locate its correct
											 // location
				val = val >> right_shift;	// shift right to get next byte
			}
		}
		// when byte offset if not bigger than 4, don't have to use one more
		// word
		else {
			data = (pEntry->data[byte_offset] &
					255) // use bit & operation to make all bits 0 except [8-0]
						 // bits
				| ((pEntry->data[byte_offset + 1] & 255)
				   << 8) // shift left to locate its correct location
				| ((pEntry->data[byte_offset + 2] & 255)
				   << 16) // shift left to locate its correct location
				| (pEntry->data[byte_offset + 3]
				   << 24); // shift left to locate its correct location
		}
		break;

	default:
		return -1; // return -1 for unknown type
	}
	return data;
}

int retrieve_data(void *addr, char data_type) {
	int value_returned = -1; /* accessed data */
	/* Check data by invoking check_cache_data_hit() */
	int check = check_cache_data_hit(addr, data_type);

	if (check == -1) { // cache miss
		/* In case of the cache miss event, retrieve data from the main memory
		by invoking access_memory() */
		value_returned = access_memory(addr, data_type);
		if (value_returned == -1) {
			printf("UNKNOWN TYPE\n");
			return -1;
		}
	} else
		value_returned = check; // cache hit

	return value_returned; // return data
}
