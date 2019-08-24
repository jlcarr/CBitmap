#include <stdio.h>
#include <stdlib.h>


// BMP struct with minimal data required
struct bmp{
	unsigned int w; // image width
	unsigned int h; // image height
	unsigned int bpp; // bytes per pixel
	unsigned char * px;
};
typedef struct bmp bmp;




bmp readbmp(const char* impath){
	// Init the struct returned
	bmp output;
	output.px = NULL;

	
	// Open the file
	FILE* fp = fopen(impath,"rb");
	if (!fp){
		printf("Image could not be opened\n");
		return output; //will be empty
	}
	
	
	// Process bitmap file header
	
	// Begin with opening the file
	unsigned char fheader[14]; // Each BMP file begins by a 14-byte file header
	if (fread(fheader, sizeof(unsigned char), 14, fp) != 14 ){ // If not 14 bytes read : problem
		printf("File too small hold a BMP header\n");
		return output; //will be empty
	}
	
	// 0: Verify begin: 2
	if (fheader[0]!='B' || fheader[1]!='M'){
		printf("Unable to parse non 'BM' BMP file header\n");
		return output; //will be empty
	}
	
	// 2: Skip filesize: 4
	// 6: Skip unused: 4
	
	// 10: Offset to pixel array: 4
	unsigned int dOffset = 0; // Position in the file where the actual data begins
	for(int i=0; i<4; i++) dOffset += (unsigned int)fheader[10+i] << 8*i; // bitshift each byte into correct uint position
	
	
	// Process DIB header
	int DIBhsize = dOffset-14;
	unsigned char* DIBheader = malloc(sizeof(unsigned char)*DIBhsize);; // Each BMP file begins by a variable sized DIB header
	if (fread(DIBheader, sizeof(unsigned char), DIBhsize, fp) != DIBhsize ){ // If not DIBhsize bytes read : problem
		printf("Too small to for specified DIB header size\n");
		return output; //will be empty
	}
	
	// 0: Skip DIB header size (redundant): 4
	
	// 4: Width in pixels: 4
	output.w = 0;
	for(int i=0; i<4; i++) output.w += DIBheader[4+i] << 8*i; //see dOffset comment
	
	// 8: Height in pixels: 4
	output.h = 0;
	for(int i=0; i<4; i++) output.h += DIBheader[8+i] << 8*i; //see dOffset comment
	
	// 12: Skip color planes (TODO: what is this?): 2
	
	// 14: Bits per pixel: 2
	output.bpp = 0;
	for(int i=0; i<2; i++) output.bpp += DIBheader[14+i] << 8*i; //see dOffset comment
	output.bpp /= 8; //convert from bits per pixel to bytes per pixel
	
	// 16: Compression/Format (TODO: use different formats): 4
	unsigned int compression = 0;
	for(int i=0; i<4; i++) compression += DIBheader[16+i] << 8*i; //see dOffset comment
	
	// 20: Skip image size (redundant =width*height*ppx/8) (TODO: padding?): 4
	// 24: Skip horizontal resolution (TODO: better rendering): 4
	// 28: Skip vertical resolution (TODO: better rendering): 4
	// 32: Skip colors in palette (TODO: this): 4
	// 36: Skip important colors (TODO: this): 4
	// 40: Bitmasks (TODO: what is the exact specification? Is this even right?): 16
	unsigned char r_mask[4], g_mask[4], b_mask[4], a_mask[4];
	if (compression == 3){
		for(int i=0; i<4; i++) r_mask[i] = DIBheader[40+i];
		for(int i=0; i<4; i++) g_mask[i] = DIBheader[44+i];
		for(int i=0; i<4; i++) b_mask[i] = DIBheader[48+i];
		for(int i=0; i<4; i++) a_mask[i] = DIBheader[52+i];
	}
	// 56: Skip rest of the header (TODO: what else can there be?): ...

	// Finish by freeing the no longer needed DIBheader memory
	free(DIBheader);
	
	
	// Read the pixel data
	
	// Create the memory allocation
	int imSize = output.w * output.h * output.bpp;
	output.px = malloc(sizeof(unsigned char) * imSize);
	// Read the actual data from the file (TODO: padding??)
	fread(output.px, sizeof(unsigned char), imSize, fp);
	
	//Everything is in memory now, the file can be closed
	fclose(fp);
	
	
	// Convert to RGBA using bitmasks (TODO: other formats and bpp?)
	if (compression == 3){
		// Loop over pixels (TODO: assuming output.bpp = 4)
		for(int i = 0; i < output.w * output.h; i++){
			// Extract channels using bitmasks (TODO: is this correct?)
			unsigned char r = 0;
			for(int j=0; j<output.bpp; j++) r |= r_mask[j] & output.px[i*output.bpp+j];
			unsigned char g = 0;
			for(int j=0; j<output.bpp; j++) g |= g_mask[j] & output.px[i*output.bpp+j];
			unsigned char b = 0;
			for(int j=0; j<output.bpp; j++) b |= b_mask[j] & output.px[i*output.bpp+j];
			unsigned char a = 0;
			for(int j=0; j<output.bpp; j++) a |= a_mask[j] & output.px[i*output.bpp+j];
			// Resent pixels with extracted values in RGBA order
			output.px[i*output.bpp+0] = r;
			output.px[i*output.bpp+1] = g;
			output.px[i*output.bpp+2] = b;
			output.px[i*output.bpp+3] = a;
		}
	}

	return output;
}






int writebmp(bmp img, const char* impath){
	// Write a bmp struct with RGBA format to file
	

	// Prepare the bitmap file header
	
	// Start with initial values
	unsigned char fheader[14] = {
		// 0: Bitmap ID: 2
		'B','M',
		// 2: filesize: 4
		0,0,0,0,
		// 6: unused: 4
		0,0,0,0,
		// 10: pixel data offset: 4
		70,0,0,0
	};
	
	// Fill in variable values
	// 2: filesize: 4
	unsigned int filesize = 70 + img.w * img.h * img.bpp;
	for(int i=0; i<4; i++) fheader[2+i] = filesize >> 8*i; //see dOffset comment


	// Prepare the DIB header
	
	// Start with initial values
	unsigned char DIBheader[56] = {
		// 0: DIB size: 4
		56,0,0,0,
		// 4: width: 4
		0,0,0,0,
		// 8: height: 4
		0,0,0,0,
		// 12: plane: 2
		1,0,
		// 14: bits per pixel: 2
		8*img.bpp,0,
		// 16: compression (default as bitfield masks): 4
		3,0,0,0,
		// 20: total size (including padding, but none cuz bitfield masks): 4
		0,0,0,0,
		// 24:horizontal resolution: 4
		0,0,0,0,
		// 28: vertical resolution: 4
		0,0,0,0,
		// 32: color palette: 4
		0,0,0,0,
		// 36: important colors: 4
		0,0,0,0,
		//normal BMP ends here: add RGBA bitmask
		// 40: Red channel bitmask: 4
		0xff,0,0,0,
		// 44: Green channel bitmask: 4
		0,0xff,0,0,
		// 48: Blue channel bitmask: 4
		0,0,0xff,0,
		// 52: Alpha channel bitmask: 4
		0,0,0,0xff
	};
	
	// Fill in variable values
	// 4: width: 4
	for(int i=0; i<4; i++) DIBheader[4+i] = img.w >> 8*i; //see dOffset comment
	// 8: height: 4
	for(int i=0; i<4; i++) DIBheader[8+i] = img.h >> 8*i; //see dOffset comment
	// 20: total size (including padding, but none cuz bitfield masks): 4
	for(int i=0; i<4; i++) DIBheader[20+i] = img.w * img.h * img.bpp >> 8*i; //see dOffset comment


	//Write the file

	//Open the file
	FILE *fp = fopen(impath,"wb");
	if (!fp){
		printf("Image could not be opened\n");
		return 1;
	}
	
	// Write all the prepared data
	fwrite(fheader, sizeof(unsigned char), 14, fp);
	fwrite(DIBheader, sizeof(unsigned char), 56, fp);
	fwrite(img.px, sizeof(unsigned char), img.w * img.h * img.bpp, fp);

	// Close the file
	fclose(fp);
	
	
	//return succcessfully
	return 0;
}



