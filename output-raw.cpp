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
    Vector() : x(0), y(0), z(0) {}
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector(const Vector & v) : x(v.x), y(v.y), z(v.z) {}
    float length() const {
      return sqrt(x * x + y * y + z * z);
    }
    float dot(const Vector& v) const {
      return (x * v.x) + (y * v.y) + (z * v.z);
    }
    Vector normalized() const {
      return (*this) * (1.0f / length());
    }
    Vector operator - (const Vector& v) const {
      return Vector(
          x - v.x,
          y - v.y,
          z - v.z);
    }
    Vector operator + (const Vector& v) const {
      return Vector(
          x + v.x,
          y + v.y,
          z + v.z);
    }
    Vector operator * (float scale) const {
      return Vector(
          x * scale,
          y * scale,
          z * scale);
    }
    std::string toString() const {
      char buffer[100];
      snprintf(buffer, 100, "x: %.3f y: %.3f z: %.3f", x, y, z);
      return buffer;
    }
};

std::list<Vector> spheres;

float pixelCoordinateToWorldCoordinate(int coordinate) {
  return ((coordinate / 32.0f) - 0.5f) * 2.0f;
}

Vector spherePoint(Vector rayOrigin, Vector rayDirection, float t) {
  return rayOrigin + (rayDirection * t);
}

std::pair<bool, Vector> calculateSphereIntersection(
    Vector sphereCenter,
    Vector rayOrigin,
    Vector rayDirection) {
  float sphereRadius = 0.5f;
  Vector l = sphereCenter - rayOrigin;
  float s = l.dot(rayDirection);
  float lSquared = l.dot(l);
  float sphereRadiusSquared = sphereRadius * sphereRadius;
  if (s < 0 && lSquared > sphereRadiusSquared) return std::make_pair(false, Vector());
  float mSquared = lSquared - (s * s);
  if (mSquared > sphereRadiusSquared) return std::make_pair(false, Vector());
  float q = sqrt(sphereRadiusSquared - mSquared);
  float t = 0.0;
  if (lSquared > sphereRadiusSquared) t = s - q;
  else t = s + q;
  return std::make_pair(true, spherePoint(rayOrigin, rayDirection, t));
}

float calculateLambert(Vector sphereCenter, Vector intersection) {
  Vector lightPosition(0.5f, 0.5f, 0.0f);
  Vector lightDirection = (lightPosition - intersection).normalized();
  Vector sphereNormal = (intersection - sphereCenter).normalized();
  return std::max(0.0f, lightDirection.dot(sphereNormal));
}

void renderImage(uint8_t* pixels) {
  spheres.push_back(Vector(0.0f, 0.0f, -1.0f));
  uint8_t* p = pixels;
  for(int i = 0; i < 32; ++i) {
    for(int j = 0; j < 32; ++j) {
      Vector rayOrigin(
          pixelCoordinateToWorldCoordinate(j),
          pixelCoordinateToWorldCoordinate(i),
          0.0f);
      Vector rayDirection(0.0f, 0.0f, -1.0f);
      std::pair<bool, Vector> sphereIntersection = calculateSphereIntersection(
          spheres.front(),
          rayOrigin,
          rayDirection);
      if(sphereIntersection.first) {
        uint8_t red = calculateLambert(spheres.front(), sphereIntersection.second) * 0xFF;
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
