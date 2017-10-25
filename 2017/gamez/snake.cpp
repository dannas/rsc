// NAME
//      snake - the classical growing snake game
//
// SYNOPSIS
//      ./snake
//
// DESCRIPTION
//      A simple ncurses based snakes game. The speed of the game is constant.
//      There are no score keeping or levels - the only way to win is not to
//      play.
//      Use the arrow keys for controlling the snake.
//      The snake will travel faster while an arrow key is continously pressed.
//
// AUTHOR
//      Daniel Naslund <dannas@dannas.name>

#include <algorithm>
#include <cstdlib>
#include <deque>

#include <ncurses.h>

using namespace std;

struct Point
{
  int x;
  int y;
};

bool operator==(const Point& lhs, const Point& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

// The direction of the snake.
int dir_;

// Width of the board.
int width_;

// Height of the board.
int height_;

// Positions occupied by the snake.
deque<Point> snake_;

// Next food to eat.
Point food_;

bool
legalArrowKey(int c)
{
  return (c == KEY_UP && dir_ != KEY_DOWN) ||
         (c == KEY_DOWN && dir_ != KEY_UP) ||
         (c == KEY_RIGHT && dir_ != KEY_LEFT) ||
         (c == KEY_LEFT && dir_ != KEY_RIGHT);
}

void
readArrowKeys()
{
  int c = getch();
  if (legalArrowKey(c))
    dir_ = c;
}

bool
collides(const deque<Point>& points, const Point& p)
{
  return count(begin(points), end(points), p);
}

bool
outsideBoard(const Point& p)
{
  return p.x < 0 || p.x >= width_ || p.y < 0 || p.y >= height_;
}

// Generate a random integer in range [lo, hi]
int
randInt(int lo, int hi)
{
  return lo + rand() / (RAND_MAX / (hi - lo + 1) + 1);
}

// Generate coordinates for a new food item that don't collide with existing
// items or borders.
Point
newFood()
{
  // Border width.
  int bw = 10;

  while (true) {
    int x = randInt(bw, width_ - bw);
    int y = randInt(bw, height_ - bw);
    Point f{ x, y };
    if (!collides(snake_, f))
      return f;
  }
}

bool
moveOneStep()
{
  // Copy current head.
  Point head = snake_.front();

  // Adjust position
  if (dir_ == KEY_UP)
    head.y--;
  else if (dir_ == KEY_DOWN)
    head.y++;
  else if (dir_ == KEY_RIGHT)
    head.x++;
  else if (dir_ == KEY_LEFT)
    head.x--;

  // Game over if we collide with ourself or the borders.
  if (collides(snake_, head))
    return false;
  if (outsideBoard(head))
    return false;

  // Do the move.
  snake_.push_front(head);

  // Keep the tail if we've found food.
  if (head == food_)
    food_ = newFood();
  else
    snake_.pop_back();

  return true;
}

void
render()
{
  clear();
  for (auto p : snake_) {
    mvaddch(p.y, p.x, ACS_BLOCK);
  }
  mvaddch(food_.y, food_.x, ACS_DIAMOND);
}

void
gameLoop()
{
  bool alive = true;

  while (alive) {
    render();
    readArrowKeys();
    alive = moveOneStep();
  }
}

void
initBoard()
{
  dir_ = KEY_UP;
  getmaxyx(stdscr, height_, width_);
  int xmid = width_ / 2;
  int ymid = height_ / 2;

  for (int i = 0; i < 10; i++)
    snake_.push_back({ xmid, ymid + i });

  food_ = newFood();
}

void
enableRawMode()
{
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

int
main()
{
  enableRawMode();
  initBoard();
  gameLoop();

  endwin(); // Restore terminal state.
}
