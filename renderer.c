#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* ------------- DECLERATIONS ------------- */

#define WIDTH 80
#define HEIGHT 25
char screen[HEIGHT][WIDTH];

#define SCALE 20

typedef struct {
  float x;
  float y;
  float z;
} Vec3;

/* ---------------- RAW MODE ---------------- */

struct termios original;

void disableRawMode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

void enableRawMode(void) {
  tcgetattr(STDIN_FILENO, &original);
  atexit(disableRawMode);
  struct termios raw = original;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* ---------------- BUFFER ---------------- */

void clearBuffer(void) {
  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      screen[y][x] = ' ';
}

void drawPoint(int x, int y, char c) {
  if (x < 0 || x >= WIDTH)
    return;
  if (y < 0 || y >= HEIGHT)
    return;

  screen[y][x] = c;
}

void render(void) {
  write(STDOUT_FILENO, "\x1b[H", 3);

  for (int y = 0; y < HEIGHT; y++) {
    write(STDOUT_FILENO, screen[y], WIDTH);
    write(STDOUT_FILENO, "\r\n", 2);
  }
}

/* ---------------- 3D ---------------- */

Vec3 rotateY(Vec3 v, float a) {
  Vec3 r;

  r.x = v.x * cosf(a) - v.z * sinf(a);
  r.z = v.x * sinf(a) + v.z * cosf(a);
  r.y = v.y;

  return r;
}

void project(Vec3 v, int *x, int *y) {
  float z = v.z + 5.0f; // push the vec forward so its not behind the camera

  *x = (int)(WIDTH / 2 + (v.x / z) * SCALE);
  *y = (int)(HEIGHT / 2 - (v.y / z) * SCALE);
} 

/* --------------- MAIN --------------- */

// int main(void) {
//   while (1) {
//     clearBuffer();
//     drawPoint(40, 12, 'H');
//     render();
//     usleep(16000);
//   }
// }
