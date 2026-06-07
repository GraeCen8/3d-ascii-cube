#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* ------------- DECLERATIONS ------------- */

#define WIDTH 80
#define HEIGHT 25
char screen[HEIGHT][WIDTH];

#define CAMERA_DISTANCE 5.0f
#define X_SCALE 60
#define Y_SCALE 24

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
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
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

Vec3 rotateX(Vec3 v, float a) {
  Vec3 r;

  r.y = v.y * cosf(a) - v.z * sinf(a);
  r.z = v.y * sinf(a) + v.z * cosf(a);
  r.x = v.x;

  return r;
}

void project(Vec3 v, int *x, int *y) {
  float z = CAMERA_DISTANCE - v.z;

  *x = (int)(WIDTH / 2 + (v.x / z) * X_SCALE);
  *y = (int)(HEIGHT / 2 - (v.y / z) * Y_SCALE);
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

int main(void) {
  enableRawMode();

  Vec3 cube[8] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                  {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

  float angle = 0.0f;

  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  while (1) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
      if (c == 'q') break;

    clearBuffer();

    Vec3 transformed[8];

    // rotate the cube
    for (int i = 0; i < 8; i++)
      transformed[i] = rotateX(rotateY(cube[i], angle), 0.45f);

    // draw vertices
    for (int i = 0; i < 8; i++) {
      int x, y;
      project(transformed[i], &x, &y);
      drawPoint(x, y, '#');
    }

    render();
    angle += 0.03f;
    usleep(16000);
  }

  disableRawMode();
  return 0;
}
