#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(){
	bmp in = readbmp("colortest.bmp");

	printf("\nRead a bitmap with:\n");
	printf("height: %d\n", in.h);
	printf("width: %d\n", in.w);
	printf("bytes per pixel: %d\n", in.bpp);

	writebmp(in, "out.bmp");

	return 0;
}
