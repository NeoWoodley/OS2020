#include <am.h>
#include <amdev.h>
#include <klib.h> //modified

void splash();
void game_bg();
void pacman_go(int x, int y);
void print_key();
static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}
