/*******************************************************************
    Implements the Perlin noise algorithm to display a kind of lava
    lamp effect on a 64x32 LED panel

    Written by Maximilian Laurenz
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP8266 set up
// ----------------------------

#include <Ticker.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <PxMatrix.h>
// The library for controlling the LED Matrix
// Needs to be manually downloaded and installed
// https://github.com/2dom/PxMatrix

Ticker display_ticker;

// Pins for LED MATRIX
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0

// Choose your board configuration:
// PxMATRIX display(32, 16, P_LAT, P_OE, P_A, P_B, P_C);
   PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);
// PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Perlin noise initializer HASH:
int p[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69
, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219
, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175
, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230
, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209
, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198
, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212
, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2
, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110
, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144
, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106
, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67
, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

// === Perlin noise adapted from http://www.rosettacode.org/
double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
  int h = hash & 15;
  double u = h<8 ? x : y,
    v = h<4 ? y : h==12||h==14 ? x : z;
  return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double noise(double x, double y, double z) {
  int X = (int)floor(x) & 255,
  Y = (int)floor(y) & 255,
  Z = (int)floor(z) & 255;
  x -= floor(x);
  y -= floor(y);
  z -= floor(z);
  double u = fade(x),
  v = fade(y),
  w = fade(z);
  int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,
  B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;

  return
    lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                grad(p[BA  ], x-1, y  , z   )),
            lerp(u, grad(p[AB  ], x  , y-1, z   ),
                grad(p[BB  ], x-1, y-1, z   ))),
    lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                grad(p[BA+1], x-1, y  , z-1 )),
            lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                grad(p[BB+1], x-1, y-1, z-1 ))));
}
// ===

double mapRange(double a1, double a2, double b1, double b2, double s) {
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

// ISR for display refresh
void display_updater() {
  display.display(70);
}

void setup() {
  for (int i = 0; i < 256; i++) p[256+i] = p[i]; // initialize the second half of the array
  display.begin(16);
  display.display(0);
  display_ticker.attach(0.002, display_updater);
  yield();
  int c = 0;
  uint16_t cc;
  
  for (float z = 0; z < 2; z = z + 0.02) { // 100 frames
    // display.clearDisplay(); // doesn't help

    for (float y = 0; y < 3.2; y = y + 0.1) {
      for (float x = 0; x < 6.4; x = x + 0.1) {
        c = mapRange(-1, 1, 0, 128, noise(x, y, z));
        if (c < 64) c = 0; // erase lower color values
        cc = display.color565(c, 0, 0);
        display.drawPixel(x * 10, y * 10, cc);
      }
      delay(1);
    }

    delay(50);
  }
}

void loop() {
}
