#include "BinPacker2D.h"

namespace bp2D
{

bool BinRectCompareMaxSide(BinRect r0, BinRect r1)
{return r0.GetMaxSide()>r1.GetMaxSide();}
bool BinRectCompareWidth(BinRect r0, BinRect r1)
{return r0.w>r1.w;}
bool BinRectCompareHeight(BinRect r0, BinRect r1)
{return r0.h>r1.h;}
bool BinRectCompareSquare(BinRect r0, BinRect r1)
{return r0.GetSquare()>r1.GetSquare();}

}