// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#ifndef SHA_H
#define SHA_H
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h> // get file size
#define BLOCK_BUFFER_SIZE 64u

// sha-0
int sha0(char *input_file, uint32_t *h0, uint32_t *h1, uint32_t *h2,
	 uint32_t *h3, uint32_t *h4);
void process_chunk_sha0(uint8_t chunk[BLOCK_BUFFER_SIZE], uint32_t *h0,
			uint32_t *h1, uint32_t *h2, uint32_t *h3, uint32_t *h4);
// sha-1
int sha1(char *input_file, uint32_t *h0, uint32_t *h1, uint32_t *h2,
	 uint32_t *h3, uint32_t *h4);
void process_chunk_sha1(uint8_t chunk[BLOCK_BUFFER_SIZE], uint32_t *h0,
			uint32_t *h1, uint32_t *h2, uint32_t *h3, uint32_t *h4);
// helper
uint32_t left_rotate(uint32_t val, size_t rotate_by);
int pre_processing(uint8_t block[BLOCK_BUFFER_SIZE], size_t bytes_read);
void clear_hash(uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3,
		uint32_t *h4);

#endif
