#include <iostream>
#include <list>
#include <cmath>
#include <stdint.h>

class Vector {
  private:
    float x;
    float y;
    float z;
  public:
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector(const Vector & v) : x(v.x), y(v.y), z(v.z) {}
    float length() {
      return sqrt(x * x + y * y + z * z);
    }
    float dot(const Vector& v) {
      return (x * v.x) + (y * v.y) + (z * v.z);
    }
    Vector normalized() {
      return (*this) * (1.0f / length());
    }
    Vector operator - (const Vector& v) {
      return Vector(
          x - v.x,
          y - v.y,
          z - v.z);
    }
    Vector operator + (const Vector& v) {
      return Vector(
          x + v.x,
          y + v.y,
          z + v.z);
    }
    Vector operator * (float scale) {
      return Vector(
          x * scale,
          y * scale,
          z * scale);
    }
};

float pixelCoordinateToWorldCoordinate(int coordinate) {
  return ((coordinate / 32.0f) - 0.5f) * 2.0f;
}

Vector spherePoint(Vector rayOrigin, Vector rayDirection, float t) {
  return rayOrigin + (rayDirection * t);
}

std::list<Vector> calculateSphereIntersections(Vector rayOrigin, Vector rayDirection) {
  std::list<Vector> ret;
  Vector sphereCenter(0.0f, 0.0f, -1.0f);
  float sphereRadius = 0.5f;
  float b = rayDirection.dot(rayOrigin - sphereCenter);
  float c = ((rayOrigin - sphereCenter).dot(rayOrigin - sphereCenter)) - (sphereRadius * sphereRadius);
  float discriminant = ((b * b) - c);
  if (discriminant >= 0) {
    float t0 = -b - (sqrt(discriminant));
    float t1 = -b + (sqrt(discriminant));
    ret.push_back(spherePoint(rayOrigin, rayDirection, std::min(t0, t1)));
    ret.push_back(spherePoint(rayOrigin, rayDirection, std::max(t0, t1)));
  }
  return ret;
}

float calculateLambert(Vector intersection) {
  Vector lightPosition(0.5f, 0.5f, 0.0f);
  Vector sphereCenter(0.0f, 0.0f, -1.0f);
  Vector lightDirection = (lightPosition - intersection).normalized();
  Vector sphereNormal = (intersection - sphereCenter).normalized();
  return std::max(0.0f, lightDirection.dot(sphereNormal));
}

void renderImage(uint8_t* pixels) {
  uint8_t* p = pixels;
  for(int i = 0; i < 32; ++i) {
    for(int j = 0; j < 32; ++j) {
      Vector rayOrigin(
          pixelCoordinateToWorldCoordinate(j),
          pixelCoordinateToWorldCoordinate(i),
          0.0f);
      Vector rayDirection(0.0f, 0.0f, -1.0f);
      std::list<Vector> sphereIntersections = calculateSphereIntersections(
          rayOrigin,
          rayDirection);
      if(!sphereIntersections.empty()) {
        uint8_t red = calculateLambert(sphereIntersections.front()) * 0xFF;
        *p = 0x0 & 0xFF; p++;
        *p = 0x0 & 0xFF; p++;
        *p = red & 0xFF; p++;
      } else {
        p += 3;
      }
    }
  }
}

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
  
  renderImage(pixels);

  fwrite(tgaHeader, sizeof(uint8_t), 18, outputFile);
  fwrite(pixels, sizeof(uint8_t), 32 * 32 * 3, outputFile);
  fclose(outputFile);

  free(pixels);

  return 0;
}
