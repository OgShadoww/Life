#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/select.h>

typedef struct {
  int width;
  int height;
  int size;
  int population;
  char *board;
} Board;

typedef struct {
  int x;
  int y;
} Cursor;

struct termios OriginalTermios;

char dead;
char live;

int clamp(int v, int lo, int hi) {
  if(hi < lo) return lo;
  if(v > hi) return hi;
  if(v < lo) return lo;
  return v;
}

void move_cursor(int x, int y) {
  dprintf(STDOUT_FILENO, "\033[%d;%dH", y, x);
}

void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &OriginalTermios);
}

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &OriginalTermios);
  atexit(disable_raw_mode);
  struct termios raw = OriginalTermios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


void timeout(long long time) {
  usleep(time);
}

void clear() {
  printf("\033[2J\033[H");
}

void print_board(Board *board, Cursor *cursor) {
  for(int i = 0; i < board->size; i++) {
    printf("%c", board->board[i]);
    if((i + 1) % board->width == 0 && i != 0) {
      printf("\n");
    }
  }

  move_cursor(cursor->x, cursor->y);
}

Board *init_board() {
  Board *b = malloc(sizeof(*b));
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
  b->width = w.ws_col;
  b->height = w.ws_row;
  b->board = calloc(b->height * b->width, sizeof(char));
  b->size = b->width * b->height;
  memset(b->board, (int)dead, b->size);

  return b;
}

void handle_user_input(char c, Cursor *cursor, Board *board) {
  switch (c) {
    case 'j':
      cursor->y = clamp(cursor->y+1, 1, board->height);
      move_cursor(cursor->x, cursor->y);
      break;
    case 'k':
      cursor->y = clamp(cursor->y-1, 1, board->height);
      move_cursor(cursor->x, cursor->y);
      break;
    case 'h':
      cursor->x = clamp(cursor->x-1, 1, board->width);
      move_cursor(cursor->x, cursor->y);
      break;
    case 'l':
      cursor->x = clamp(cursor->x+1, 1, board->width);
      move_cursor(cursor->x, cursor->y);
      break;
    case 'i':
      board->board[(board->width * cursor->y) + cursor->x-1] = live;
      move_cursor(cursor->x, cursor->y);
      break;
    case 'p':
      board->board[(board->width * cursor->y) + cursor->x-1] = dead;
      move_cursor(cursor->x, cursor->y);
      break;
    case '\n':
      return;
  }
}

void setup_board(Board *board) {
  Cursor cursor = {
    .x = 1, 
    .y = 1
  };
  char c;

  while((c = getchar()) != '\n') {
    handle_user_input(c, &cursor, board);
    print_board(board, &cursor);
  }
}

void game_step(Board *board) {
  char next[board->size];

  for (int i = 0; i < board->size; i++) {
    int x = i % board->width;
    int y = i / board->width;

    int neighbors = 0;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;

        int nx = x + dx;
        int ny = y + dy;

        if (nx < 0 || ny < 0 || nx >= board->width || ny >= board->height)
          continue;

        if (board->board[ny * board->width + nx] == live)
          neighbors++;
      }
    }

    if (board->board[i] == live) {
      if (neighbors == 2 || neighbors == 3)
        next[i] = live;
      else
        next[i] = dead;
    } else {
      if (neighbors == 3)
        next[i] = live;
      else
        next[i] = dead;
    }
  }

  for (int i = 0; i < board->size; i++) {
    board->board[i] = next[i];
  }
}

int main(int argc, char **argv) {
  if(argc < 3) {
    dead = '.';
    live = '#';
  }
  else {
    dead = argv[1][0];
    live = argv[2][0];
  }
  enable_raw_mode();
  clear();
  Board *board = init_board();
  print_board(board, &(Cursor){.x=1, .y=1});
  setup_board(board);
  long long basic_time = 500000;

  int running = 1;

  while (running) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = basic_time;

    int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    if (ret == -1) break;

    if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
      char c = getchar();
      if (c == '+') {
        basic_time -= 10000;
        if (basic_time < 10000) basic_time = 10000;
      }
      else if (c == '-') {
        basic_time += 10000;
      }
      else if(c == 'q') {
        clear();
        free(board->board);
        free(board);
        return 0;
      }
    }

    clear();
    game_step(board);
    print_board(board, &(Cursor){.x=1, .y=1});
  }

  disable_raw_mode();
  free(board->board);
  free(board);

  return 0;
}
