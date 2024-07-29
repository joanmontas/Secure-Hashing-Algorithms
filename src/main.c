// Copyright Joan Montas
// All rights reserved.
// License under GNU General Public License v3.0

#include <stdio.h>
#include "../include/sha.h"

int main()
{
	uint32_t h0 = 0u;
	uint32_t h1 = 0u;
	uint32_t h2 = 0u;
	uint32_t h3 = 0u;
	uint32_t h4 = 0u;

	sha0("./data/test.txt", &h0, &h1, &h2, &h3, &h4);
	printf("%08x%08x%08x%08x%08x\n", h0, h1, h2, h3, h4);

	sha1("./data/test.txt", &h0, &h1, &h2, &h3, &h4);
	printf("%08x%08x%08x%08x%08x\n", h0, h1, h2, h3, h4);
	return 0;
}
