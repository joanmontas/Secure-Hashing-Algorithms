// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#include <stdio.h>
#include <string.h>
#include "../include/sha.h"

int main(int argc, char *argv[])
{
	uint32_t h0 = 0u;
	uint32_t h1 = 0u;
	uint32_t h2 = 0u;
	uint32_t h3 = 0u;
	uint32_t h4 = 0u;

	if (argc != 3) {
		printf("\nPlease provide exactly a two arguments\n");
		printf("Example-0: %s sha0 test.txt \n", argv[0]);
		printf("Example-1: %s sha1 test.txt \n", argv[0]);
		return 1;
	}
	if ((strcmp(argv[1], "sha0")) == 0) {
		sha0(argv[2], &h0, &h1, &h2, &h3, &h4);
	} else if ((strcmp(argv[1], "sha1")) == 0) {
		sha1(argv[2], &h0, &h1, &h2, &h3, &h4);
	} else {
		printf("\nError: Unknown Algorithm: %s\n", argv[1]);
		return 1;
	}

	printf("%08x%08x%08x%08x%08x\n", h0, h1, h2, h3, h4);
	return 0;
}
