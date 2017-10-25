#include <ncurses.h>

struct Ball
{
  double rx;
  double ry;
  double vx;
  double vy;
};

struct Paddle
{
  int upper;
  int lower;
};

// Width of the board.
int width_;

// Height of the board.
int height_;

// Current position of the ball.
Ball ball_;

// Edges of the paddle.
Paddle paddle_;

// Direction of the paddle.
int dir_;

bool
legalArrowKey(int c)
{
  return c == KEY_UP || c == KEY_DOWN;
}

void
readArrowKeys()
{
  int c = getch();
  if (legalArrowKey(c)) {
    dir_ = c;
  } else {
    // Sentinel for "not up or down".
    dir_ = 0;
  }
}

void
render()
{
  clear();

  mvaddch(ball_.ry, ball_.rx, ACS_DIAMOND);

  for (int y = paddle_.upper; y <= paddle_.lower; ++y)
    mvaddch(y, width_ - 1, ACS_BLOCK);
}

void
movePaddle()
{
  if (dir_ == KEY_UP && paddle_.upper != 0) {
    paddle_.lower--;
    paddle_.upper--;
  } else if (dir_ == KEY_DOWN && paddle_.lower != height_ - 1) {
    paddle_.lower++;
    paddle_.upper++;
  }
}

bool
hitsPaddle(const Ball& ball, const Paddle& paddle)
{
  return ball.rx >= width_ - 1 && ball.ry >= paddle.upper &&
         ball.ry <= paddle.lower;
}

bool
update()
{
  movePaddle();

  double& rx = ball_.rx;
  double& ry = ball_.ry;
  double& vx = ball_.vx;
  double& vy = ball_.vy;

  // Detect collision with borders and paddle.
  //
  // (0,0)  +----------+ (xmax, 0)
  //        |
  //        |    *     |
  //        |
  //        +----------+
  //
  if (rx <= 0)
    vx = -vx;
  else if (hitsPaddle(ball_, paddle_))
    vx = -vx;
  else if (rx >= width_)
    return false;
  else if (ry <= 0)
    vy = -vy;
  else if (ry >= height_)
    vy = -vy;

  // Calculate new postion, based on speed.
  rx += vx;
  ry += vy;

  return true;
}

void
gameLoop()
{
  bool alive = true;
  while (alive) {
    render();
    readArrowKeys();
    alive = update();
  }
}

void
initBoard()
{
  getmaxyx(stdscr, height_, width_);

  double xmid = width_ / 2.0;
  double ymid = height_ / 2.0;
  double vx = -4.5;
  double vy = -0.5;

  paddle_ = { static_cast<int>(ymid - 10), static_cast<int>(ymid + 10) };

  ball_ = { xmid, ymid, vx, vy };
}

int
main()
{
  enableRawMode();
  initBoard();
  gameLoop();

  // Restore terminal state.
  endwin();
}