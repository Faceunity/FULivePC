#ifndef __STB_IMAGE_H
#define __STB_IMAGE_H
unsigned char *stbi_load_from_memory(const void*buffer, int len, int *x, int *y, int *comp, int req_comp);
float *stbi_loadf_from_memory(const void*buffer, int len, int *x, int *y, int *comp, int req_comp);
char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
#endif
