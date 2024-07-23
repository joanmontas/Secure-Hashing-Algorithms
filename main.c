// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#include <stdio.h>
#include <stdint.h>

#define BLOCK_BUFFER_SIZE 16u // 32 * 16 = 512 bits

uint32_t left_rotate(uint32_t val, size_t rotate_by) {
    return (val << rotate_by) | (val >> (32 - rotate_by));
}

void process_chunk(uint32_t chunk[BLOCK_BUFFER_SIZE], uint32_t *h0,
		   uint32_t *h1, uint32_t *h2, uint32_t *h3, uint32_t *h4)
{
        uint32_t f = 0u;
	uint32_t k = 0u;
        uint32_t temp = 0u;
        uint32_t w[80];
        for (size_t i = 0u; i < 16u; i++) {
                w[i] = chunk[i];
        }

        for (size_t i = 16; i < 80u; i++) {
                w[i] = left_rotate(w[i - 3u] ^ w[i - 8u] ^ w[i - 14u] ^ w[i - 16u], 1u);
        }

        uint32_t a = *h0;
        uint32_t b = *h1;
        uint32_t c = *h2;
        uint32_t d = *h3;
        uint32_t e = *h4;

        for (int i = 0; i < 80; i++) {

                if (i < 20) {
                        f = (b & c) | ((~b) & d);
                        k = 0x5A827999;
                } else if (i < 40) {
                        f = b ^ c ^ d;
                        k = 0x6ED9EBA1;
                } else if (i < 60) {
                        f = (b & c) | (b & d) | (c & d);
                        k = 0x8F1BBCDC;
                } else {
                        f = b ^ c ^ d;
                        k = 0xCA62C1D6;
                }

                temp = left_rotate(a, 5u) + f + e + k + w[i]; // left rotate
                e = d;
                d = c;
                c = left_rotate(b, 30u);  // left rotate
                b = a;
                a = temp;
        }

        *h0 = *h0 + a;
        *h1 = *h1 + b;
        *h2 = *h2 + c;
        *h3 = *h3 + d;
        *h4 = *h4 + e;
}

int main()
{
	// input
	char *input_file = "./test.txt";
	FILE *file0 = fopen(input_file, "rb");
	if (file0 == NULL) {
		perror("Error: Could not open input_file");
		return 1;
	}

	// input processing
	uint32_t block_buffer[BLOCK_BUFFER_SIZE];
	size_t bytes_read = 0u;


	// intermediate processing
	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;
	uint32_t h4 = 0xC3D2E1F0;

	// read input
	while ((bytes_read = fread(block_buffer, 1, sizeof(block_buffer),
				   file0)) > 0) {

		for (size_t i = bytes_read;
		     i < BLOCK_BUFFER_SIZE; i++) {
			block_buffer[i] = 0u;
		};

		process_chunk(block_buffer, &h0, &h1, &h2, &h3, &h4);
	}

	printf("result = %u %u %u %u %u\n", h0, h1, h2, h3, h4);

	fclose(file0);
	return 0;
}
