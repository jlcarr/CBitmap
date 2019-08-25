#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(){
	bmp in = bmpread("colortest.bmp");

	printf("\nRead a bitmap with:\n");
	printf("height: %d\n", in.h);
	printf("width: %d\n", in.w);
	printf("bytes per pixel: %d\n", in.bpp);
	
	bmp out = bmpblur(in, 4);

	bmpwrite(out, "out.bmp");

	return 0;
}
