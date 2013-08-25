#include "FontRasterizer.h"

FontRasterizer::FontRasterizer(void)
{
	fontFileData=0;
	ftLib=0;
	face=0;
	atlasBitmap.data = 0;
	faceSize = 16;
	FT_Init_FreeType(&ftLib);
}

FontRasterizer::FontRasterizer(std::string fontFileName)
{
	FontRasterizer();
	this->ReadFontFile(fontFileName);
}

FontRasterizer::~FontRasterizer(void)
{
	if(fontFileData)
		free(fontFileData);

	std::map<unsigned long, Bitmap>::iterator it;
	for(it = glyphBitmaps.begin(); it != glyphBitmaps.end(); it++)
	{
		if(it->second.data)
			free(it->second.data);
	}

	if(atlasBitmap.data)
		free(atlasBitmap.data);

	//Freetype
	if(face)
		FT_Done_Face(face);
	if(ftLib)
		FT_Done_FreeType(ftLib);
}

bool FontRasterizer::ReadFontFile(std::string fontFileName)
{
	std::fstream file;
	file.open(fontFileName.c_str(), std::ios_base::in | std::ios::binary);
	if(file)
	{
		file.seekg(0, file.end);
		unsigned int fileLength = file.tellg();
		file.seekg(0, file.beg);
		fontFileSize = fileLength;

		fontFileData = malloc(fileLength);
		if(!file.read((char*)fontFileData, fontFileSize))
		{
			//An error occured while reading the file
			free(fontFileData);
			fontFileData = 0;
			file.close();
			return false;
		}

		file.close();

		if(FT_New_Memory_Face(ftLib, (FT_Byte*)fontFileData, fontFileSize, 0, &face))
		{
			//An error occured while loading a font face
			printf("Face could not be loaded.");
		}
		
		//All is good
		return true;
	}
	else
		return false;
}

void FontRasterizer::AddGlyph(unsigned long charCode)
{
	charCodes.insert(charCodes.end(), charCode);
}

void FontRasterizer::AddGlyphRange(unsigned long charCodeFirst, unsigned long charCodeLast)
{
	for(unsigned long i = charCodeFirst; i <= charCodeLast; i++)
	{
		charCodes.insert(charCodes.end(), i);
	}
}

void FontRasterizer::AddGlyphs(char* bytes)
{
}

void FontRasterizer::SetSize(unsigned int s)
{
	faceSize = s;
}

void FontRasterizer::Rasterize()
{
	if(!ftLib)
		return;
	if(!face)
		return;

	FT_Error error;

	FT_Set_Char_Size(face, 0, faceSize*64, 72, 72);

	//Converting all char codes into bitmaps
	for(int i = 0; i < charCodes.size(); i++)
	{
		unsigned int glyphIndex = FT_Get_Char_Index(face, charCodes[i]);

		//Skip this iteration if that char code is already exists
		if(codeToIndex.find(charCodes[i])!=codeToIndex.end())
			continue;
		//Write down charCode -> glyphIndex
		codeToIndex.insert(std::make_pair(glyphIndex, glyphIndex));
		
		//If the glyph already exists skip this iteration
		if(glyphs.find(glyphIndex)!=glyphs.end())
			continue;

		if(error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL))
		{
			//Can't load glyph
		}

		//Add new glyph info
		GlyphInfo glyph;
		glyph.advanceX = face->glyph->advance.x;
		glyph.advanceY = face->glyph->advance.y;
		glyph.hBearingX = face->glyph->metrics.horiBearingX;
		glyph.hBearingY = face->glyph->metrics.horiBearingY;
		glyph.vBearingX = face->glyph->metrics.vertBearingX;
		glyph.vBearingY = face->glyph->metrics.vertBearingY;
		glyphs.insert(std::make_pair(charCodes[i], glyph));

		if(face->glyph->format!=FT_GLYPH_FORMAT_BITMAP)
		{
			//There is no bitmap for that glyph. Rendering.
			if(error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
			{
				//Glyph could not be rendered
			}
		}

		Bitmap glyphBitmap;
		glyphBitmap.w = face->glyph->bitmap.width;
		glyphBitmap.h = face->glyph->bitmap.rows;

		switch(face->glyph->bitmap.pixel_mode)
		{
		case FT_PIXEL_MODE_NONE:
			//There is no bitmap
			//Somethig went wrong
			break;
		case FT_PIXEL_MODE_MONO:
			
			break;
		case FT_PIXEL_MODE_GRAY:
			glyphBitmap.data = malloc(face->glyph->bitmap.width*face->glyph->bitmap.rows*1);
			memcpy(glyphBitmap.data, face->glyph->bitmap.buffer, face->glyph->bitmap.width*face->glyph->bitmap.rows*1);
			break;
		case FT_PIXEL_MODE_GRAY2:
			
			break;
		case FT_PIXEL_MODE_GRAY4:
			
			break;
		case FT_PIXEL_MODE_LCD:
			
			break;
		case FT_PIXEL_MODE_LCD_V:
			
			break;
		case FT_PIXEL_MODE_BGRA:
			
			break;
		}

		glyphBitmaps.insert(std::make_pair(glyphIndex, glyphBitmap));
	}

	//Make texture atlas
	bp2D::BinPacker2D *packer2D = new bp2D::BinPacker2D();

	std::map<unsigned long, Bitmap>::iterator it;
	for(it = glyphBitmaps.begin(); it != glyphBitmaps.end(); it++)
	{packer2D->AddRect(bp2D::BinRect(it->first, it->second.w, it->second.h));}

	packer2D->Pack();
	bp2D::BinRect rootRect = packer2D->GetRootRect(); //bitmap size
	std::vector<bp2D::BinRect> rects = packer2D->GetVolumes(); //every glyph position and size
	delete packer2D;

	std::map<unsigned long, GlyphInfo>::iterator giIt;
	for(int i = 0; i < rects.size(); i++) //Save all rectangle info into appropriate glyphInfos
	{
		if((giIt = glyphs.find(rects[i].id))!=glyphs.end())
		{
			giIt->second.bmpRect.x = rects[i].x;
			giIt->second.bmpRect.y = rects[i].y;
			giIt->second.bmpRect.w = rects[i].w;
			giIt->second.bmpRect.h = rects[i].h;
		}
	}

	if(atlasBitmap.data)
		free(atlasBitmap.data);
	
	atlasBitmap.data = malloc(rootRect.w * rootRect.h);
	memset(atlasBitmap.data, 0, rootRect.w * rootRect.h);
	atlasBitmap.bytePerPixel = 1;
	atlasBitmap.w = rootRect.w; atlasBitmap.h = rootRect.h;

	for(int i = 0; i < rects.size(); i++)
	{
		InsertBitmapToBitmap((unsigned char*)glyphBitmaps.find(rects[i].id)->second.data, rects[i].w, rects[i].h,
			(unsigned char*)atlasBitmap.data, atlasBitmap.w, atlasBitmap.h, rects[i].x, rects[i].y, 1);
	}

	//dunno yet if I want to delete separate bitmaps
	//as they are needed to rebuild texture atlas
	//if these are deleted we will need to re-render them, which is not desirable
	/*
	std::map<unsigned long, Bitmap>::iterator it;
	for(it = glyphBitmaps.begin(); it != glyphBitmaps.end(); it++)
	{
		if(it->second.data)
			free(it->second.data);
	}
	glyphBitmaps.clear();*/
}

Bitmap FontRasterizer::GetBitmapData()
{
	return atlasBitmap;
}
GlyphInfo* FontRasterizer::GetGlyph(unsigned long charCode)
{
	std::map<unsigned long, unsigned long>::iterator ctiIt = codeToIndex.find(charCode);
	std::map<unsigned long, GlyphInfo>::iterator giIt;
	if(ctiIt!=codeToIndex.end())
	{
		giIt = glyphs.find(ctiIt->second);
		if(giIt!=glyphs.end())
			return &giIt->second;
		else
			return 0;
	}
	else
		return 0;
}