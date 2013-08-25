#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "BinPacker2D.h"
#include "BitmapUtils.h"

#pragma comment(lib, "freetype250MT.lib")

struct Rect
{
	unsigned int x, y;
	unsigned int w, h;
};

struct GlyphInfo
{
	float hBearingX, hBearingY;
	float vBearingX, vBearingY;
	float advanceX, advanceY;
	//Bitmap coords
	Rect bmpRect;
};

struct Bitmap
{
	void* data;
	unsigned int w, h;
	unsigned char bytePerPixel;
};

class FontRasterizer
{
public:
	FontRasterizer(void);
	FontRasterizer(std::string fontFileName);
	~FontRasterizer(void);

	bool ReadFontFile(std::string fontFileName);

	void AddGlyph(unsigned long charCode);
	void AddGlyphRange(unsigned long charCodeFirst, unsigned long charCodeLast);
	void AddGlyphs(char* bytes);
	void SetSize(unsigned int s);
	void Rasterize();
	
	Bitmap GetBitmapData();
	GlyphInfo* GetGlyph(unsigned long charCode);
private:
	FT_Library ftLib;
	FT_Face face;
	unsigned int faceSize;
	void* fontFileData;
	unsigned int fontFileSize;
	std::vector<unsigned long> charCodes; //UTF-32

	std::map<unsigned long, unsigned long> codeToIndex; //first is unicode value, second is the glyph index in the font
	std::map<unsigned long, Bitmap> glyphBitmaps; //Key is a glyph code used in font file, not a unicode value

	Bitmap atlasBitmap;
	std::map<unsigned long, GlyphInfo> glyphs; //Struct representing glyph's bitmap location in atlas bitmap and other stuff like bearing
};
