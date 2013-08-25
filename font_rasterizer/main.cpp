#include "lodepng.h"
#include <string>
#include "FontRasterizer.h"

int main()
{
	FontRasterizer* fr = new FontRasterizer();
	fr->ReadFontFile("arial.ttf");

	fr->AddGlyphRange(0, 128);
	fr->AddGlyphRange(0x410, 0x44f);
	fr->SetSize(64);
	fr->Rasterize();

	Bitmap bmp = fr->GetBitmapData();
	lodepng::encode("test.png", (unsigned char*)bmp.data, bmp.w, bmp.h, ::LCT_GREY);

	delete fr;
	//scanf("ass");
	return 0;
}