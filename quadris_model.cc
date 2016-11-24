#include "quadris_model.h"
#include "cell.h"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <iomanip>
using namespace std;

int QuadrisModel::high_score = 0;

QuadrisModel::QuadrisModel(): td{TextDisplay()}, 
current_block{Block(BlockType::Empty, &td, false)}, next_block{BlockType::Empty} {
  score = 0;
  seed = 0;
  level = 0;
  sequence_file = "sequence.txt";
  file_in = ifstream(sequence_file);
  // load two random blocks: one for current and one for next block
  nextBlock();
  nextBlock();
}

void QuadrisModel::down(int m) {
  for (int i = 0; i < m; ++i) {
    if(canDown()) {
      current_block.down();
    }
  }
}

void QuadrisModel::right(int m) {
  for (int i = 0; i < m; ++i) {
    if (canRight()) {
      current_block.right();
    }
  }
  if (current_block.isHeavy())
    down(1);
}

void QuadrisModel::left(int m) {
  for (int i = 0; i < m; ++i) {
    if (canLeft()) {
      current_block.left();
    }
  }
  if (current_block.isHeavy())
    down(1);
}

void QuadrisModel::drop(int m) {
  for (int i = 0; i < m; ++i) {
    while(canDown()) {
      current_block.down();
    }
    blocks.push_back(current_block);
    clearRows();
    nextBlock();
  }
}

void QuadrisModel::clockwise(int m) {
  for (int i = 0; i < m; ++i) {
    if (canClock()) {
      current_block.clockwise();
    }
  }
  if (current_block.isHeavy())
    down(1);
}

void QuadrisModel::cclockwise(int m) {
  for (int i = 0; i < m; ++i) {
    if (canCclock()) {
      current_block.cclockwise();
    }
  }
  if (current_block.isHeavy())
    down(1);
}

void QuadrisModel::levelUp(int m) {
  for (int i = 0; i < m; ++i) {
    if (level != 4)
      ++level;
  }
}

void QuadrisModel::levelDown(int m) {
  for (int i = 0; i < m; ++i) {
    if (level != 0)
      --level;
  }
}

bool QuadrisModel::isOver() {
  return false;
}

void QuadrisModel::clearRows() {
  int rows_cleared = 0;
  for(int r = 14; r >= 0; r--) {
    int cols_occupied = 0;
    for(auto &block : blocks) {
      cols_occupied += block.colsOccupied(r);
    }
    if (cols_occupied == 11) {
      for(auto &block : blocks) {
        block.remove(r);
        block.dropAbove(r);
      }
      rows_cleared++;
      //so we recheck the current row
      r++;
    }
  }
  updatePositions();
  rows_cleared += level;
  updateScore(rows_cleared * rows_cleared);
}

void QuadrisModel::updateScore(int update) {
  score += update;
  if (score > high_score) {
    high_score = score;
  }
}

bool QuadrisModel::canDown() {
  return canMove(0, 1);
}

bool QuadrisModel::canLeft() {
  return canMove(-1, 0);
}

bool QuadrisModel::canRight() {
  return canMove(1, 0);
}

bool QuadrisModel::canClock() {
  vector<int> vals = current_block.maxMin();
  for (auto cell: current_block.positions()) {
    Info info = cell.getInfo();
    int x = info.x - (info.x - vals[3]) + (vals[2] - info.y);
    int y = info.y - (vals[1] - info.x) + (vals[2] - info.y);
    if (!isOpen(x, y)) {
      return false;
    }
  }
  return true;
}

bool QuadrisModel::canCclock() {
  vector<int> vals = current_block.maxMin();
  for (auto cell: current_block.positions()) {
    Info info = cell.getInfo();
    int x = info.x - (info.x - vals[3]) + (info.y - vals[0]);
    int y = info.y - (info.x - vals[3]) + (vals[2] - info.y);
    if (!isOpen(x, y)) {
      return false;
    }
  }
  return true;
}

// checks absolute position and not relative like canMove.
bool QuadrisModel::isOpen(int x, int y) {
  if (x < 0 || x >= 11 || y < -3 || y >= 15) {
      return false;
  }
  for (auto position: positions) {
    if (x == position.x && y == position.y) {
        return false;
    }
  }
  return true;
}

bool QuadrisModel::canMove(int x, int y) {
  vector<Info> current_positions;
  for (auto cell : current_block.positions()) {
    Info info = cell.getInfo();
    info.x += x;
    info.y += y;
    // check that the new position is within the grid
    if (info.x < 0 || info.x >= 11 || info.y < -3 || info.y >= 15) {
      return false;
    }
    current_positions.push_back(info);
  }
  //check that the current spot is not taken up by the currently occupied positions
  for(auto position : positions) {
    for(auto updated_position : current_positions) {
      if (updated_position.x == position.x && updated_position.y == position.y) {
        return false;
      } 
    }
  }
  return true;
}

void QuadrisModel::nextBlock() {
  updatePositions();
  // distrubtion where all reals between 0 and 1 have equal probability
  uniform_real_distribution<double> distribution(0.0, 1.0);
  // create a seed based on current time so that we don't always have the same blocks
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);
  double random = distribution(generator);

  // make the next block the current block
  if (level >= 3)
    current_block = Block(next_block, &td, true);
  else
    current_block = Block(next_block, &td, false);
  
  current_block.draw();

  if (level == 0) {
    string type;
    file_in >> type;
    if (type == "Z") {
      next_block = BlockType::ZBlock;
    } else if (type == "S") {
      next_block = BlockType::SBlock;
    } else if (type == "L") {
      next_block = BlockType::LBlock;
    } else if (type == "J") {
      next_block = BlockType::JBlock;
    } else if (type == "I") {
      next_block = BlockType::IBlock;
    } else if (type == "O") {
      next_block = BlockType::OBlock;
    } else if (type == "T") {
      next_block = BlockType::TBlock;
    } 
  } else if (level == 1) {
    if (random <= 1.0/12.0) {
      next_block = BlockType::ZBlock;
    } else if (random <= 1.0/6.0) {
      next_block = BlockType::SBlock;
    } else if (random <= 2.0/6.0) {
      next_block = BlockType::LBlock;
    } else if (random <= 3.0/6.0) {
      next_block = BlockType::JBlock;
    } else if (random <= 4.0/6.0) {
      next_block = BlockType::IBlock;
    } else if (random <= 5.0/6.0) {
      next_block = BlockType::OBlock;
    } else {
      next_block = BlockType::TBlock;
    }
  } else if (level == 2) {
    if (random <= 1.0/7.0) {
      next_block = BlockType::ZBlock;
    } else if (random <= 2.0/7.0) {
      next_block = BlockType::SBlock;
    } else if (random <= 3.0/7.0) {
      next_block = BlockType::LBlock;
    } else if (random <= 4.0/7.0) {
      next_block = BlockType::JBlock;
    } else if (random <= 5.0/7.0) {
      next_block = BlockType::IBlock;
    } else if (random <= 6.0/7.0) {
      next_block = BlockType::OBlock;
    } else {
      next_block = BlockType::TBlock;
    }
  } else {
    if (random <= 2.0/9.0) {
      next_block = BlockType::ZBlock;
    } else if (random <= 4.0/9.0) {
      next_block = BlockType::SBlock;
    } else if (random <= 5.0/9.0) {
      next_block = BlockType::LBlock;
    } else if (random <= 6.0/9.0) {
      next_block = BlockType::JBlock;
    } else if (random <= 7.0/9.0) {
      next_block = BlockType::IBlock;
    } else if (random <= 8.0/9.0) {
      next_block = BlockType::OBlock;
    } else {
      next_block = BlockType::TBlock;
    }
  }
}

void QuadrisModel::updatePositions() {
  positions.clear();
  for(auto block : blocks) {
    for(auto position : block.positions()) {
      positions.push_back(position.getInfo());
    }
  }
}

std::ostream &operator<<(std::ostream &out, const QuadrisModel &model) {
  cout << endl << "Level:" << setw(9) << model.level << endl;
  cout << "Score:" << setw(9) << model.score << endl;
  cout << "Hi Score:" << setw(6) << model.high_score << endl;
  cout << "  -----------" << endl;
  cout << model.td;
  cout << "  -----------" << endl;
  cout << "Next:" << endl;

  if (model.next_block == BlockType::ZBlock)
    cout << "ZZ\n ZZ" << endl;
  else if (model.next_block == BlockType::SBlock)
    cout << " SS\nSS" << endl;
  else if (model.next_block == BlockType::LBlock)
    cout << "  L\nLLL" << endl;
  else if (model.next_block == BlockType::JBlock)
    cout << "J  \nJJJ" << endl;
  else if (model.next_block == BlockType::IBlock)
    cout << "IIII" << endl;
  else if (model.next_block == BlockType::OBlock)
    cout << "OO\nOO" << endl;
  else if (model.next_block == BlockType::TBlock)
    cout << "TTT\n T" << endl;

  return out;
}
