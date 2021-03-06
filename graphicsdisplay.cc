#include <string>
#include "graphicsdisplay.h"
#include "window.h"
using namespace std;

GraphicsDisplay::GraphicsDisplay(bool hold): xw{Xwindow()}, hold{hold} {
  // Game border:
  xw.fillRectangle(130, 10, 5, 480, Xwindow::Black);
  xw.fillRectangle(130, 10, 355, 5, Xwindow::Black);
  xw.fillRectangle(130, 485, 355, 5, Xwindow::Black);
  xw.fillRectangle(485, 10, 5, 480, Xwindow::Black);
  // Title:
  xw.drawString(40, 20, "QUADRIS");
  // Credits: :-)
  xw.drawString(10, 455, "Made by:");
  xw.drawString(10, 470, "Ryan Quanz &");
  xw.drawString(10, 485, "Jonathan Donas");
}
    
void GraphicsDisplay::notify(Info info) {
  if(info.x < 0 || info.x > 10 || info.y < 0 || info.y > 14)
    return;
  if (info.type == BlockType::Empty)
    drawCell(info.x, info.y, Xwindow::White);
  else if (info.type == BlockType::IBlock)
    drawCell(info.x, info.y, Xwindow::Cyan);
  else if (info.type == BlockType::JBlock)
    drawCell(info.x, info.y, Xwindow::Blue);
  else if (info.type == BlockType::LBlock)
    drawCell(info.x, info.y, Xwindow::Orange);
  else if (info.type == BlockType::OBlock)
    drawCell(info.x, info.y, Xwindow::Yellow);
  else if (info.type == BlockType::SBlock)
    drawCell(info.x, info.y, Xwindow::Green);
  else if (info.type == BlockType::ZBlock)
    drawCell(info.x, info.y, Xwindow::Red);
  else if (info.type == BlockType::TBlock)
    drawCell(info.x, info.y, Xwindow::Purple);
  else if (info.type == BlockType::SingleBlock)
    drawCell(info.x, info.y, Xwindow::Brown);
  else if (info.type == BlockType::HintBlock)
    drawCell(info.x, info.y, Xwindow::Hint);
}

void GraphicsDisplay::drawCell(int x, int y, int colour) {
  int y_margin = 25;
  int x_margin = 145;

  if (colour == Xwindow::Hint)
    xw.drawString(x_margin + 30*x + 12, y_margin + 30*y + 18, "?");
  else
    xw.fillRectangle(x_margin + 30*x, y_margin + 30*y, 29, 29, colour);
}

void GraphicsDisplay::drawLegend(int level, int score, int high_score, vector<vector<int>> next_cells, int next_colour, vector<vector<int>> hold_cells, int hold_colour) {
  int y_margin = 60;
  int x_margin = 10;
  // clear legend
  xw.fillRectangle(x_margin, 40, 115, 280, Xwindow::White);

  // draw legend, right padded
  string lvl = "Level:";
  xw.drawString(x_margin, y_margin, lvl.append(10 - to_string(level).length(), ' ').append(to_string(level)));
  string scr = "Score:";
  xw.drawString(x_margin, y_margin + 30, scr.append(10 - to_string(score).length(), ' ').append(to_string(score)));
  string hscr = "Hi Score:";
  xw.drawString(x_margin, y_margin + 60, hscr.append(7 - to_string(high_score).length(), ' ').append(to_string(high_score)));
  xw.drawString(x_margin, y_margin + 90, "Next:");

  // clear and draw next block;
  y_margin = 170;
  for (auto cell: next_cells) {
    xw.fillRectangle(x_margin + cell[0]*20, y_margin + cell[1]*20, 19, 19, next_colour);
  }
  if (hold) {
    xw.drawString(x_margin, y_margin + 70, "Hold:");
    // clear and draw hold block
    y_margin = 260;
    for (auto cell: hold_cells) {
      xw.fillRectangle(x_margin + cell[0]*20, y_margin + cell[1]*20, 19, 19, hold_colour);
    }
  }
}

GraphicsDisplay::~GraphicsDisplay() {}
