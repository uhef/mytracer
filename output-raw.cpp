#include <iostream>
#include <stdint.h>

int main() {
  FILE* outputFile = fopen("output.tga", "wb");

  uint8_t* pixels = (uint8_t*)malloc(32 * 32 * 3);
  uint8_t* p = pixels;
  uint8_t blue = 0;
  for(int i = 0; i < 32; ++i) {
    uint8_t green = 0;
    for(int j = 0; j < 32; ++j) {
      *p = blue & 0xFF; p++;
      *p = green & 0xFF; p++;
      *p = 0x0; p++;
      green += 7;
    }
    blue += 7;
  }

  uint8_t tgaHeader[18] = {0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  tgaHeader[12] = 32 & 0xFF;
  tgaHeader[13] = (32 >> 8) & 0xFF;
  tgaHeader[14] = (32) & 0xFF; 
  tgaHeader[15] = (32 >> 8) & 0xFF;
  tgaHeader[16] = 24; 
  
  fwrite(tgaHeader, sizeof(uint8_t), 18, outputFile);
  fwrite(pixels, sizeof(uint8_t), 32 * 32 * 3, outputFile);
  fclose(outputFile);

  free(pixels);

  return 0;
}
