#include <game.h>

// Operating system is a C program!
int main(const char *args) {
  _ioe_init();

  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");

//  splash();

  for(int x = 2; x <= 4; x ++) {
      for(int y = 2; y <= 4; y ++) {
      game_bg();
      pacman_go(x,y);	  
	  }
  }

  puts("Press any key to see its key code...\n");
  while (1) {
    //_halt(0);
    print_key();
  }
  return 0;
}
