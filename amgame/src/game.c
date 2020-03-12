#include <game.h>

#define SIDE 16

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
  int w = screen_width();
  int h = screen_height();
  int x = 2;
  int y = 2;
  printf("w:%d,h:%d\n",w,h);
  //puts("Press any key to see its key code...\n");
  while (1) {
	  while (receive == _KEY_NONE)
	      receive = read_key();
	  if(receive == 1 || ((receive | 0x8000) == 0x8001)) 
	      _halt(0);
	  if (receive != 1 && (receive | 0x8000) != 0x8001) {
		  if (x < w / SIDE - 2 && y < h / SIDE - 2) {
			  pacman_go(x, y);
			  x ++;
		  }
		  else if(x == w / SIDE - 2 && y < h / SIDE - 2) {
              x = 2;
			  y ++;
			  continue;
		  } 
		  else if( y == h / SIDE - 2) {
		      blacksh(); 
			  x = 2;
			  y = 2;
			  continue;
		  }
		  receive = _KEY_NONE;
	     /// update_game_bg();
	  } 
	  
  }
    //_halt(0);
		//if(receive != _KEY_NONE && ((receive | 0x8000)!= receive) ) 
  return 0;
}
