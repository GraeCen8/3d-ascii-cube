#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios original;

void disableRawMode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

void enableRawMode(void) {
  tcgetattr(STDIN_FILENO, &original);
  atexit(disableRawMode);
  struct termios raw = original;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#define WIDTH 80
#define HEIGHT 25

char screen[HEIGHT][WIDTH];

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

int main(void) {
  while (1) {
    clearBuffer();
    drawPoint(40, 12, 'H');
    render();
    usleep(16000);
  }
}
