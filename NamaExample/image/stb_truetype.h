#ifndef __STB_TRUETYPE_H
#define __STB_TRUETYPE_H

void* FTW_LoadFont(char* a,int sz);
int FTW_UnloadFont(void* ft);
int FTW_GetGlyphId(void* ft,int ch);
unsigned char* FTW_GetGlyphEx(void* ft,float h,
	int x_aa,int y_aa, 
	float* x_filter,float* y_filter,int x_htaps,int y_htaps,
	float embolden, int ch,float* ret);
float FTW_GetAscent(void* ft,int h);
float FTW_GetGlyphAdvance(void* ft,float h,int id);
float FTW_GetKerning(void* ft,float h,int ch1, int ch2);
void FTW_SetRetardedWindingOrder(void* ft);
#endif
