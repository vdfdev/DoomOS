const int SCREEN_COLS = 80;
const int SCREEN_ROWS = 25;

void write_char(int x, int y, char c, char attrib) {
  *((char*)0xB8000 + (y * SCREEN_COLS + x) * 2) = c;
  *((char*)0xB8000 + (y * SCREEN_COLS + x) * 2 + 1) = attrib;
}

void clear_screen() {
  for (int i = 0; i < SCREEN_COLS; i++) {
    for (int j = 0; j < SCREEN_ROWS; j++) {
      write_char(i, j, ' ', 0);
    }
  }
}

void write_cute_str(int x, int y, char* s) {
  int j = 0;
  for (int i = 0; *(s+i) != 0; i++) {
    char c = *(s+i);
    write_char(x + i, y, c, 2 + (j%5));
    j++;
    if (c == ' ') {
      j = 0;
    }
  }
}

void write_str(int x, int y, char* s) {
  for (int i = 0; *(s+i) != 0; i++) {
    write_char(x + i, y, *(s+i), 7);
  }
}

void screen_start() {
  clear_screen();
  write_cute_str(30, 1, "Hello World, QuakeOS!");
  write_str(30, 2, "A Didatic OS.");
  write_str(0, 4, "Learning about Operating Systems and computers by running Quake.");
}