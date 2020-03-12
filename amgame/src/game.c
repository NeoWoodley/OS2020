#include <game.h>

// Operating system is a C program!
int main(const char *args) {
  _ioe_init();

  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");

//  splash();
  game_bg();

  puts("Start your game!\n");
  int receive = 0;
  int x = 5;
  int y = 5;
  //puts("Press any key to see its key code...\n");
  while (1) {
	  while (receive == _KEY_NONE)
	      receive = read_key();
	  if (receive != 1 && (receive | 0x8000) != 0x8001) {
		  if (x < 25) {
		      x ++;
		  }
		  else {
		      x = 5;
		      y ++;
		  }
	      pacman_go(x, y);
		  receive = _KEY_NONE;
	     /// update_game_bg();
	  } 
	  
	  else if (receive == 1 || ((receive | 0x8000) != 0x8001)) 
          _halt(1);
  }
    //_halt(0);
		//if(receive != _KEY_NONE && ((receive | 0x8000)!= receive) ) 
  return 0;
}
