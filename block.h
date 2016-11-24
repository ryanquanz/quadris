#ifndef _BLOCK_H_
#define _BLOCK_H_
#include <vector>
#include "block_type.h"
#include "cell.h"
#include "textdisplay.h"
using namespace std;

class Block {
public:
  Block(BlockType type, TextDisplay *);
  std::vector<Cell> positions();
  BlockType getType() const;
  void down();
  void left();
  void right();
  void clockwise();
  void cclockwise();
  int colsOccupied(int);
  void remove(int);
  void dropAbove(int); 
  void draw() const;
  vector<int> maxMin() const;

private:
  BlockType type;
  std::vector<Cell> cells;
  TextDisplay *td;
  void init(vector<vector<int>>);
  void clear();
  void shift(int, int);
};

#endif
