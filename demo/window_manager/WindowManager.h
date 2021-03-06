/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Original work Copyright (c) 2018 Julian Eisel
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#pragma once

#include <list>
#include <memory>
#include <optional>

#include "bwUtil.h"

#include "Window.h"

namespace bWidgetsDemo {

class WindowManager {
 public:
  // Constructor is private (singleton!)
  static auto getWindowManager() -> WindowManager&;
  ~WindowManager();

  void mainLoop();
  auto addWindow(std::string name) -> Window&;
  auto isMainWindow(const Window& win) const -> bool;

  void removeWindow(Window& win);

  using WindowList = std::list<Window>;

 private:
  WindowManager();
  WindowManager(WindowManager const&) = delete;
  void operator=(WindowManager const&) = delete;

  enum WindowManagerAction {
    WM_ACTION_CONTINUE,
    WM_ACTION_CLOSE,
  };
  auto processEvents() -> WindowManagerAction;
  void drawWindows();

  class EventManager& event_manager;
  WindowList windows;
  Window* main_win;
};

}  // namespace bWidgetsDemo
