#include "BitmapUtils.h"

void InsertBitmapToBitmap(unsigned char *data, unsigned int w, unsigned int h,
						  unsigned char* destination, unsigned int dest_w, unsigned int dest_h,
						  unsigned int dest_x, unsigned int dest_y, unsigned char bpp = 1)
{
	if(dest_w < w || dest_h < h)
		return;

	int rowLen = w;
	int rowOffset = (dest_w-w);

	unsigned int dest_i = dest_x + dest_y*dest_w;

	for(int i = 0; i < w*h; i++)
	{
		int x = i - rowLen*(i/rowLen);
		int row = i/rowLen;
		
		for(int j = 0; j < bpp; j++)
		{
			destination[(dest_i+i+rowOffset*row)*bpp+j] = data[i*bpp+j];
		}
	}
}