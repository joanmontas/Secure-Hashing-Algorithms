// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#include "../include/sha.h"

/**
 * @brief Performs a left rotation of an uint32_t
 * @param val Value to be rotated.
 * @param rotate_by Amount of rotation.
 * @return returns left rotated value
 */
inline uint32_t left_rotate(uint32_t val, size_t rotate_by)
{
	return (val << rotate_by) | (val >> (32 - rotate_by));
}

/**
 * @brief Zero-fill non-used bytes in the block
 * @param block 512-bits block/chunk from given file
 * @param bytes_read Number of bytes currently present in the block
 * @return returns left rotated value
 */
int pre_processing(uint8_t block[BLOCK_BUFFER_SIZE], size_t bytes_read)
{
	if (bytes_read > 0 && bytes_read > BLOCK_BUFFER_SIZE) {
		return -1;
	}

	// zero out remaining block
	for (size_t i = bytes_read; i < BLOCK_BUFFER_SIZE; i++) {
		block[i] = 0u;
	}

	return 0;
}

void process_chunk_sha0(uint8_t chunk[BLOCK_BUFFER_SIZE], uint32_t *h0,
			uint32_t *h1, uint32_t *h2, uint32_t *h3, uint32_t *h4)
{
	// // break 512 bits into 16 32-bits and zero it out at first
	uint32_t w[80]; // first 16 hold the 512-bit block
	for (size_t i = 0; i < 80; i++) {
		w[i] = 0u;
	}

	// break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15
	for (size_t i = 0u; i < 16; i++) {
		w[i] = (((uint32_t)chunk[4u * i + 0u]) << 24) |
		       (((uint32_t)chunk[4u * i + 1u]) << 16) |
		       (((uint32_t)chunk[4u * i + 2u]) << 8u) |
		       (((uint32_t)chunk[4u * i + 3u]) << 0);
	}

	// extend the sixteen 32-bit words into eighty 32-bit words
	for (size_t i = 16; i < 80u; i++) {
		w[i] = w[i - 3u] ^ w[i - 8u] ^ w[i - 14u] ^ w[i - 16u];
	}

	uint32_t a = *h0;
	uint32_t b = *h1;
	uint32_t c = *h2;
	uint32_t d = *h3;
	uint32_t e = *h4;

	uint32_t f = 0u;
	uint32_t k = 0u;
	uint32_t temp = 0u;

	// Main loop
	for (int i = 0; i < 80; i++) {
		if (0 <= i && i < 20) {
			f = (b & c) | ((~b) & d);
			k = 0x5A827999;
		} else if (20 <= i && i <= 39) {
			f = b ^ c ^ d;
			k = 0x6ED9EBA1;
		} else if (40 <= i && i <= 59) {
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		} else if (60 <= i && i <= 79) {
			f = b ^ c ^ d;
			k = 0xCA62C1D6;
		}

		temp = left_rotate(a, 5u) + f + e + k + w[i];
		e = d;
		d = c;
		c = left_rotate(b, 30u);
		b = a;
		a = temp;
	}

	*h0 += a;
	*h1 += b;
	*h2 += c;
	*h3 += d;
	*h4 += e;
}

int sha0(char *input_file, uint32_t *h0, uint32_t *h1, uint32_t *h2,
	 uint32_t *h3, uint32_t *h4)
{
	// input
	FILE *file0 = fopen(input_file, "rb");
	if (file0 == NULL) {
		perror("Error: Could not open input_file");
		return 1;
	}

	// pre-processing, utilizing linux file system to optimize a streamlining
	struct stat file0_stats;
	if (stat(input_file, &file0_stats) != 0) {
		fclose(file0);
		return 1;
	}

	size_t original_message_length_bytes = (size_t)(file0_stats.st_size);
	size_t original_message_length_bits =
		original_message_length_bytes * 8u;
	size_t number_of_512bit_blocks =
		((original_message_length_bits + 8u + 64u) / 512u) + 1u;

	// input processing
	uint8_t block_buffer[BLOCK_BUFFER_SIZE];
	size_t bytes_read = 0u;

	*h0 = 0x67452301;
	*h1 = 0xEFCDAB89;
	*h2 = 0x98BADCFE;
	*h3 = 0x10325476;
	*h4 = 0xC3D2E1F0;

	// read input 512-bits at a time
	while (1) {
		// once last 512-bits, is congruent to −64 ≡ 448 (mod 512)
		if (number_of_512bit_blocks == 0) {
			// mark end of input
			block_buffer[bytes_read] = 0x80;

			// insert last 64 bits containing length (in bits) of input
			block_buffer[56] =
				((uint8_t)(original_message_length_bits >> 56) &
				 0xFF);
			block_buffer[57] =
				((uint8_t)(original_message_length_bits >> 48) &
				 0xFF);
			block_buffer[58] =
				((uint8_t)(original_message_length_bits >> 40) &
				 0xFF);
			block_buffer[59] =
				((uint8_t)(original_message_length_bits >> 32) &
				 0xFF);
			block_buffer[60] =
				((uint8_t)(original_message_length_bits >> 24) &
				 0xFF);
			block_buffer[61] =
				((uint8_t)(original_message_length_bits >> 16) &
				 0xFF);
			block_buffer[62] =
				((uint8_t)(original_message_length_bits >> 8) &
				 0xFF);
			block_buffer[63] =
				((uint8_t)(original_message_length_bits >> 0) &
				 0xFF);

			// perform final processing
			process_chunk_sha0(block_buffer, h0, h1, h2, h3, h4);

			break;
		}

		bytes_read =
			fread(block_buffer, 1, sizeof(block_buffer), file0);

		pre_processing(block_buffer, bytes_read);

		number_of_512bit_blocks = number_of_512bit_blocks - 1u;

		if (number_of_512bit_blocks != 0) {
			process_chunk_sha0(block_buffer, h0, h1, h2, h3, h4);
		}
	}

	// Produce the final hash value (big-endian) as a 160-bit number:
	fclose(file0);
	return 0;
}

void process_chunk_sha1(uint8_t chunk[BLOCK_BUFFER_SIZE], uint32_t *h0,
			uint32_t *h1, uint32_t *h2, uint32_t *h3, uint32_t *h4)
{
	// // break 512 bits into 16 32-bits and zero it out at first
	uint32_t w[80]; // first 16 hold the 512-bit block
	for (size_t i = 0; i < 80; i++) {
		w[i] = 0u;
	}

	// break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15
	for (size_t i = 0u; i < 16; i++) {
		w[i] = (((uint32_t)chunk[4u * i + 0u]) << 24) |
		       (((uint32_t)chunk[4u * i + 1u]) << 16) |
		       (((uint32_t)chunk[4u * i + 2u]) << 8u) |
		       (((uint32_t)chunk[4u * i + 3u]) << 0);
	}

	// extend the sixteen 32-bit words into eighty 32-bit words
	// NOTE() The only difference between sha0 and sha1 is that sha1 rotates by 1 here
	for (size_t i = 16; i < 80u; i++) {
		w[i] = left_rotate(
			w[i - 3u] ^ w[i - 8u] ^ w[i - 14u] ^ w[i - 16u], 1u);
	}

	uint32_t a = *h0;
	uint32_t b = *h1;
	uint32_t c = *h2;
	uint32_t d = *h3;
	uint32_t e = *h4;

	uint32_t f = 0u;
	uint32_t k = 0u;
	uint32_t temp = 0u;

	// Main loop
	for (int i = 0; i < 80; i++) {
		if (0 <= i && i < 20) {
			f = (b & c) | ((~b) & d);
			k = 0x5A827999;
		} else if (20 <= i && i <= 39) {
			f = b ^ c ^ d;
			k = 0x6ED9EBA1;
		} else if (40 <= i && i <= 59) {
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		} else if (60 <= i && i <= 79) {
			f = b ^ c ^ d;
			k = 0xCA62C1D6;
		}

		temp = left_rotate(a, 5u) + f + e + k + w[i];
		e = d;
		d = c;
		c = left_rotate(b, 30u);
		b = a;
		a = temp;
	}

	*h0 += a;
	*h1 += b;
	*h2 += c;
	*h3 += d;
	*h4 += e;
}

int sha1(char *input_file, uint32_t *h0, uint32_t *h1, uint32_t *h2,
	 uint32_t *h3, uint32_t *h4)
{
	// input
	FILE *file0 = fopen(input_file, "rb");
	if (file0 == NULL) {
		perror("Error: Could not open input_file");
		return 1;
	}

	// pre-processing, utilizing linux file system to optimize a streamlining
	struct stat file0_stats;
	if (stat(input_file, &file0_stats) != 0) {
		fclose(file0);
		return 1;
	}

	size_t original_message_length_bytes = (size_t)(file0_stats.st_size);
	size_t original_message_length_bits =
		original_message_length_bytes * 8u;
	size_t number_of_512bit_blocks =
		((original_message_length_bits + 8u + 64u) / 512u) + 1u;

	// input processing
	uint8_t block_buffer[BLOCK_BUFFER_SIZE];
	size_t bytes_read = 0u;

	*h0 = 0x67452301;
	*h1 = 0xEFCDAB89;
	*h2 = 0x98BADCFE;
	*h3 = 0x10325476;
	*h4 = 0xC3D2E1F0;

	// read input 512-bits at a time
	while (1) {
		// once last 512-bits, is congruent to −64 ≡ 448 (mod 512)
		if (number_of_512bit_blocks == 0) {
			// mark end of input
			block_buffer[bytes_read] = 0x80;

			// insert last 64 bits containing length (in bits) of input
			block_buffer[56] =
				((uint8_t)(original_message_length_bits >> 56) &
				 0xFF);
			block_buffer[57] =
				((uint8_t)(original_message_length_bits >> 48) &
				 0xFF);
			block_buffer[58] =
				((uint8_t)(original_message_length_bits >> 40) &
				 0xFF);
			block_buffer[59] =
				((uint8_t)(original_message_length_bits >> 32) &
				 0xFF);
			block_buffer[60] =
				((uint8_t)(original_message_length_bits >> 24) &
				 0xFF);
			block_buffer[61] =
				((uint8_t)(original_message_length_bits >> 16) &
				 0xFF);
			block_buffer[62] =
				((uint8_t)(original_message_length_bits >> 8) &
				 0xFF);
			block_buffer[63] =
				((uint8_t)(original_message_length_bits >> 0) &
				 0xFF);

			// perform final processing
			process_chunk_sha1(block_buffer, h0, h1, h2, h3, h4);

			break;
		}

		bytes_read =
			fread(block_buffer, 1, sizeof(block_buffer), file0);

		pre_processing(block_buffer, bytes_read);

		number_of_512bit_blocks = number_of_512bit_blocks - 1u;

		if (number_of_512bit_blocks != 0) {
			process_chunk_sha1(block_buffer, h0, h1, h2, h3, h4);
		}
	}

	// Produce the final hash value (big-endian) as a 160-bit number:
	fclose(file0);
	return 0;
}

/**
 * @brief Zero-fill uint32_t (120-bits)
 * @param h0 1th 32 bits of 120-bit hash
 * @param h1 2th 32 bits of 120-bit hash
 * @param h2 3th 32 bits of 120-bit hash
 * @param h3 4th 32 bits of 120-bit hash
 * @param h4 5th 32 bits of 120-bit hash
 */
void clear_hash(uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3,
		uint32_t *h4)
{
	*h0 = 0x0;
	*h1 = 0x0;
	*h2 = 0x0;
	*h3 = 0x0;
	*h4 = 0x0;
}