#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios original;

void disableRawMode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &original);
  atexit(disableRawMode());
  struct termios raw = original;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#define WIDTH 80
#define HEIGHT 25

char screen[HEIGHT][WIDTH];

void clearBuffer(void) {
  for (int y=0; y < HEIGHT; y++)
    for (int x=0; x < WIDTH; x++)
      screen[y][x] = ' ';
}

void drawPoint(int x, int y, char c) {
  if (x < 0 || x >= WIDTH) return;
  if (y < 0 || y >= HEIGHT) return;

  screen[y][x] = c; 
}
