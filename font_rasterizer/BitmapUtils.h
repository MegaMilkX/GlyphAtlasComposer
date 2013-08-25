#ifndef _BITMAPUTILS_H_
#define _BITMAPUTILS_H_

void InsertBitmapToBitmap(unsigned char *data, unsigned int w, unsigned int h,
						  unsigned char* destination, unsigned int dest_w, unsigned int dest_h,
						  unsigned int dest_x, unsigned int dest_y, unsigned char bpp);

#endif