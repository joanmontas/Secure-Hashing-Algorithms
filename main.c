// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#include <stdio.h>
#include <stdint.h>

#define BLOCK_BUFFER_SIZE 16u // 32 * 16 = 512 bits

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
	uint32_t f = 0u;
	uint32_t k = 0u;
	uint32_t temp = 0u;

	// intermediate processing

	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;
	uint32_t h4 = 0xC3D2E1F0;

	// read input
	while ((bytes_read = fread(block_buffer, 1, sizeof(block_buffer),
				   file0)) > 0) {
		// NOTE() Zero out unread state
		for (size_t i = bytes_read + 1u / sizeof(uint32_t);
		     i < BLOCK_BUFFER_SIZE; i++) {
			block_buffer[i] = 0u;
		};

		// print the 512 bits
		for (size_t i = 0u; i < BLOCK_BUFFER_SIZE; i++) {
			printf("%u ", block_buffer[i]);
		};
		printf("\n");
	}

	printf("result = %u %u %u %u %u\n", h0, h1, h2, h3, h4);

	fclose(file0);
	return 0;
}
