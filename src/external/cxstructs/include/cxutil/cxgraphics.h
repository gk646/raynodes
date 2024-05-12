// Copyright (c) 2023 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define CX_FINISHED
#ifndef CXSTRUCTS_SRC_CXUTIL_CXGRAPHICS_H_
#  define CXSTRUCTS_SRC_CXUTIL_CXGRAPHICS_H_

#  include "../cxconfig.h"
#  include "../cxstructs/Geometry.h"
#  include "../cxstructs/QuadTree.h"

#  ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    include <functional>
#    include <utility>

namespace cxstructs {
class GraphicsWindow {
  using RenderCallback = std::function<void(GraphicsWindow&)>;
  HWND hwnd_;
  HBITMAP bitmap_, old_bitmap_;
  int width_, height_;
  bool running_;
  RenderCallback callback_;
  HFONT hFont;
  static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
      PostQuitMessage(0);
    } else {
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
  }

 public:
  GraphicsWindow(int width, int height, RenderCallback callback)
      : width_(width), height_(height), running_(true), callback_(std::move(callback)) {
    WNDCLASS wc = {};
    hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");

    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "GraphicsWindow";

    RegisterClass(&wc);

    RECT rect = {0, 0, width_, height_};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd_ =
        CreateWindowEx(0, "GraphicsWindow", "GraphicsWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                       CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr,
                       nullptr, GetModuleHandle(nullptr), nullptr);

    // Create bitmap
    HDC hdc = GetDC(hwnd_);
    SelectObject(hdc, hFont);
    bitmap_ = CreateCompatibleBitmap(hdc, width_, height_);
    ReleaseDC(hwnd_, hdc);
    old_bitmap_ = nullptr;

    ShowWindow(hwnd_, SW_SHOW);

    // Start the render loop
    renderLoop();
  }

  void renderLoop() {
    MSG msg;
    while (running_) {
      // Process any messages in the queue.
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          running_ = false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      if (callback_) {
        callback_(*this);
      }
      // Redraw window
      InvalidateRect(hwnd_, nullptr, TRUE);
      UpdateWindow(hwnd_);
      Sleep(1000 / 60);
    }
  }

  void drawLine(int x1, int y1, int x2, int y2, COLORREF color) {
    HDC hdc = GetDC(hwnd_);
    HDC memDC = CreateCompatibleDC(hdc);
    old_bitmap_ = (HBITMAP)SelectObject(memDC, bitmap_);

    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    SelectObject(memDC, hPen);

    MoveToEx(memDC, x1, y1, nullptr);
    LineTo(memDC, x2, y2);

    BitBlt(hdc, 0, 0, width_, height_, memDC, 0, 0, SRCCOPY);

    DeleteObject(hPen);
    SelectObject(memDC, old_bitmap_);
    DeleteDC(memDC);
    ReleaseDC(hwnd_, hdc);
  }

  void fillRect(Rect rect, COLORREF color) {
    HDC hdc = GetDC(hwnd_);
    HDC memDC = CreateCompatibleDC(hdc);
    old_bitmap_ = (HBITMAP)SelectObject(memDC, bitmap_);

    RECT rect_1;
    rect_1.left = rect.x();
    rect_1.top = rect.x();
    rect_1.right = rect.x() + rect.width();
    rect_1.bottom = rect.y() + rect.height();

    HBRUSH hBrush = CreateSolidBrush(color);
    FillRect(memDC, &rect_1, hBrush);

    BitBlt(hdc, 0, 0, width_, height_, memDC, 0, 0, SRCCOPY);

    DeleteObject(hBrush);
    SelectObject(memDC, old_bitmap_);
    DeleteDC(memDC);
    ReleaseDC(hwnd_, hdc);
  }

  void clear(COLORREF color = RGB(255, 255, 255)) {
    fillRect({0, 0, (float)width_, (float)height_}, color);
  }

  void fill(COLORREF color) {
    HDC hdc = GetDC(hwnd_);
    HDC memDC = CreateCompatibleDC(hdc);
    old_bitmap_ = (HBITMAP)SelectObject(memDC, bitmap_);

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width_;
    rect.bottom = height_;
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(memDC, &rect, brush);
    DeleteObject(brush);

    BitBlt(hdc, 0, 0, width_, height_, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, old_bitmap_);
    DeleteDC(memDC);
    ReleaseDC(hwnd_, hdc);
  }
  template <typename T>
  void drawQuadTree(QuadTree<T> tree) {}

  template <typename PointType>
  void drawPointsWithAxis(std::vector<PointType> points) {
    HDC hdc = GetDC(hwnd_);

    int startX = width_ * 0.1;
    int startY = height_ * 0.1;

    int axisWidth = width_ - 2 * startX;
    int axisHeight = height_ - 2 * startY;

    float minX = points[0].x(), maxX = minX;
    float minY = points[0].y(), maxY = minY;

    for (const auto& point : points) {
      minX = minX < point.x() ? minX : point.x();
      maxX = maxX > point.x() ? maxX : point.x();
      minY = minY < point.y() ? minY : point.y();
      maxY = point.y() > maxY ? point.y() : maxY;
    }

    minY -= 1;
    minX -= 1;

    MoveToEx(hdc, startX, height_ - startY, nullptr);
    LineTo(hdc, startX, startY);
    MoveToEx(hdc, startX, height_ - startY, nullptr);
    LineTo(hdc, startX + axisWidth, height_ - startY);

    float scaleX = maxX - minX / (float)axisWidth;
    float scaleY = maxY - minY / (float)axisHeight;

    for (const auto& point : points) {
      int x = startX + ((point.x() - minX) / (maxX - minX)) * axisWidth;
      int y = startY + (1 - (point.y() - minY) / (maxY - minY)) * axisHeight;

      SetPixel(hdc, x, y, RGB(255, 0, 0));
      SetPixel(hdc, x + 1, y + 1, RGB(255, 0, 0));
      SetPixel(hdc, x + 1, y, RGB(255, 0, 0));
      SetPixel(hdc, x, y + 1, RGB(255, 0, 0));
    }

    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    std::string minXStr = std::to_string((int)minX);
    std::string minYStr = std::to_string((int)minY);
    std::string maxXStr = std::to_string((int)maxX);
    std::string maxYStr = std::to_string((int)maxY);

    TextOut(hdc, startX, height_ - startY + startY / 2, minXStr.c_str(), minXStr.size());
    TextOut(hdc, startX - startX / 2, height_ - startY, minYStr.c_str(), minYStr.size());

    TextOut(hdc, startX + axisWidth - 50, startY + axisHeight + 10, maxXStr.c_str(),
            maxXStr.size());
    TextOut(hdc, startX - 50, startY, maxYStr.c_str(), maxYStr.size());

    ReleaseDC(hwnd_, hdc);
  }
};
#    undef max
#    undef min
#  else
#    pragma("Graphics support only for Windows")
//#include <X11/Xlib.h>
#  endif

#  ifdef CX_INCLUDE_TESTS
static void TEST() {
  std::vector<Point> p{{2, 2}, {40, 40}, {2, 3}, {5, 6}, {10, 15}, {25, 25}};
  GraphicsWindow win(800, 800, [&p](GraphicsWindow& win) {
    win.clear();
    win.drawPointsWithAxis(p);
  });
}
#  endif
}
#endif  //CXSTRUCTS_SRC_CXUTIL_CXGRAPHICS_H_
