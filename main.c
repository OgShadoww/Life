#include<stdio.h>
#include<unistd.h>
#include<sys/ioctl.h>

void clear() {
  printf("\033[2J\033[H");
}

int main() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
  const int WIDTH = w.ws_col;
  const int HEIGHT = w.ws_row;

  //printf("%d %d", WIDTH, HEIGHT);

  char th[HEIGHT][WIDTH];
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      th[i][j] = '.';
    }
  }

  clear();
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
        printf("%c ", th[i][j]);
      }
    printf("\n");
  }

  while(1) {

  }

  return 0;
}
