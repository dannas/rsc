#include <ncurses.h>
#include <cstdlib>

// TODO(dannas): How handle float vs int for coordinates?
// TODO(dannas): How cleanup the references to width-1 and height-1?
// TODO(dannas): How take radius into account for collision detection?
// TODO(dannas): How make a proper rendering loop that uses fixed fps?

using Dir = int;

struct Ball {
  float x, y;   // coordinates
  float dx, dy; // next change in coordinates
};

struct Paddle {
  int hi;        // upper y coordinate
  int lo;        // lower y coordinate
  int x;         // x coordinate
  Dir dir;       // direction of the paddle
};

void readArrowKeys(Dir& dir) {
  int c = getch();

  if (c == KEY_UP || c == KEY_DOWN) {
    dir = c;
  } else {
    // Sentinel for "not up or down".
    dir = 0;
  }
}

void render(const Ball& b, const Paddle& p) {
  clear();

  mvaddch(b.y, b.x, ACS_DIAMOND);

  for (int y = p.hi; y <= p.lo; ++y)
    mvaddch(y, p.x, ACS_BLOCK);
}

void move(Paddle& p, int h) {
  if (p.dir == KEY_UP && p.hi != 0) {
    p.lo--;
    p.hi--;
  } else if (p.dir == KEY_DOWN && p.lo != h - 1) {
    p.lo++;
    p.hi++;
  }
}

bool hits(const Ball& b, const Paddle& p) {
  return b.x >= p.x && b.y >= p.hi &&
         b.y <= p.lo;
}

bool update(Ball& b, Paddle& p, int h, int w) {
  move(p, h);

  // Change direction if ball hits left border or paddle.
  if (b.x <= 0 || hits(b, p)) {
    b.dx = -b.dx;

  // Change direction if ball hits top or bottom.
  } else if (b.y <= 0 || b.y >= h) {
    b.dy = -b.dy;

  // Game Over if ball hits right border outside the paddle.
  } else if (b.x >= w) {
    return false;
  }

  b.x += b.dx;
  b.y += b.dy;

  return true;
}

void gameLoop(Ball& b, Paddle& p, int h, int w) {
  bool alive = true;

  while (alive) {
    render(b, p);
    readArrowKeys(p.dir);
    alive = update(b, p, h, w);
  }
}

void initBoard(Ball& b, Paddle& p, int& h, int& w) {
  getmaxyx(stdscr, h, w);

  float xmid = w / 2.0;
  float ymid = h / 2.0;

  // TODO(dannas): How avoid hardcoding the size of the paddle?
  int hi = ymid - 10;
  int lo = ymid + 10;
  p = {hi, lo, w-1, Dir(0)};

  // TODO(dannas): How describe how these constants were choosen?
  float dx = -4.5;
  float dy = -0.5;
  b = {xmid, ymid, dx, dy};
}

void disableRawMode() {
  // Restore terminal state.
  endwin();
}

void enableRawMode() {
  atexit(disableRawMode);

  // Create stdscr.
  initscr();

  // Don't wait for newlines.
  cbreak();

  // Don't echo chars written.
  noecho();

  // Enable fn and arrow keys.
  keypad(stdscr, TRUE);

  // Hide the cursor.
  curs_set(0);

  // Let getch() block for 100ms.
  timeout(100);
}

int main() {
  Ball b;
  Paddle p;
  int h, w;

  enableRawMode();

  initBoard(b, p, h, w);
  gameLoop(b, p, h, w);
}
