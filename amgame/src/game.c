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
  //puts("Press any key to see its key code...\n");
  while (1) {
	  receive = read_key();
	  while(receive != 1 && ((receive | 0x8000) != 0x8001)) {
	     /// update_game_bg();
	  } 
	  _halt(0);
    //_halt(0);
		//if(receive != _KEY_NONE && ((receive | 0x8000)!= receive) ) 
  }
  return 0;
}
