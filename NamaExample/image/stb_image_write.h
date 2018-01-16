#ifndef __STB_IMAEG_WRITE_H
#define __STB_IMAEG_WRITE_H
void stbi_free_image_write(void*p);
unsigned char *stbi_write_png_to_mem(unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len);
unsigned char * stbi_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality);
#endif
