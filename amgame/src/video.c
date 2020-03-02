#include <game.h>

#define SIDE 16
static int w, h;

static void init() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // careful! stack is limited!
  _DEV_VIDEO_FBCTRL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTRL, &event, sizeof(event));
}

void splash() {
  init();
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
	printf("H:%d W:%d x:%d y:%d\n",h,w,x,y);
    }
  }
}

void game_bg() {
  init();
  for(int x = 0; x < 2; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
    }
  }

  for(int x = w / SIDE - 2; x < w / SIDE; x ++) {
	for (int y = 0; y * SIDE <= h; y++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
	}
  }

  for(int y = 0; y < 2; y ++) {
    for (int x = 0; x * SIDE <= w; x++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
    }
  }

  for(int y = h / SIDE - 2; y < h / SIDE; y ++) {
    for (int x = 0; x * SIDE <= w; x++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
    }
  }
}

void blacksh() {
  init();
  for(int x = 2; x < x / SIDE - 2; x ++) {
    for (int y = 2; y < y / SIDE -2; y++) {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x272727); // white
    }
  }
    
}

void pacman_go(int x, int y) {
	init();
    draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xf9f900);
    
}
