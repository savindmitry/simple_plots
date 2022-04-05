#include <windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

#include "parser.h"

using namespace std;

int main() {
  HWND hwnd = GetConsoleWindow();
  HDC hdc = GetDC(hwnd);

  RECT rc;
  GetClientRect(hwnd, &rc);
  int w = rc.right; // ширина рабочей области
  int h = rc.bottom;// высота рабочей области

  int size = 20;

  double x = -(w * 1.0 / 2) / size;

  double y;

  vector<shared_ptr<Node>> expressions;

  while (cin.peek() != '|') {
    string expression;
    getline(cin, expression);

    auto e = Parse(expression.begin(), expression.end(), x);
    expressions.push_back(e);
  }

  for (int he = 0; he < h; ++he) {
    SetPixel(hdc, w * 1.0 / 2, he, RGB(255, 255, 255));
  }
  for (int wi = 0; wi < w; ++wi) {
    SetPixel(hdc, wi, h * 1.0 / 2, RGB(255, 255, 255));
  }

  for (; x < (w * 1.0 / 2) / size; x += 0.01 / size) {
    for (const auto& e: expressions) {
      y = e->Evaluate();
      SetPixel(hdc,
               (w * 1.0 / 2) + size * x,
               (h * 1.0 / 2) - size * y,
               RGB(int(122 + 122 * cos(x + M_PI * 5 / 2)),
                   int(122 + 122 * cos(x + M_PI / 7)),
                   int(122 + 122 * sin(x))));
    }
  }
  ReleaseDC(hwnd, hdc);
  std::cin.ignore();

  char stars;
  /*cout << "How many stars can you give to my program?)";*/
  cin >> stars;
  return 0;
}