#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

typedef struct {
  int width;
  int height;
  int size;
  int population;
  char *board;
} Board;

void timeout() {
  sleep(1000);
}

void clear() {
  printf("\033[2J\033[H");
}

Board *init_board() {
  Board *b = malloc(sizeof(*b));
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
  b->width = w.ws_col;
  b->height = w.ws_row;
  b->board = calloc(b->height * b->width, sizeof(char));
  b->size = b->width * b->height;
  memset(b->board, (int)' ', b->size);

  return b;
}

void game_step(Board *board) {
  
}

void print_board(Board *board) {
  for(int i = 0; i < board->size; i++) {
    printf("%c", board->board[i]);
    if((i + 1) % board->width == 0 && i != 0) {
      printf("\n");
    }
  }
}

int main() {
  clear();
  Board *board = init_board();
  print_board(board);
  while(1) {

  }

  free(board->board);
  free(board);

  return 0;
}
