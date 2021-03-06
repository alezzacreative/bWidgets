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

#include "DefaultStage.h"
#include "EventManager.h"
extern "C" {
#include "gawain/gwn_context.h"
#include "gawain/gwn_immediate.h"
}
#include "GPU.h"
#include "Layout.h"

#include "Window.h"

using namespace bWidgets;

namespace bWidgetsDemo {

Window::Window(const std::string& name, unsigned int size_x, unsigned int size_y)
    : width(size_x), height(size_y)
{
  //	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  //	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  //	glfwWindowHint(GLFW_SAMPLES, 4);  // antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // For MacOS
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  if (width == 0) {
    //		width = 0.8f * mode->width;
  }
  if (height == 0) {
    //		height = 0.8 * mode->height;
  }

  glfw_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(glfw_window);

  GPU_init();
  gwn_context = GWN_context_create();

  glEnable(GL_SCISSOR_TEST);

  EventManager::setupWindowHandlers(*this);

  float px_scale_x, px_scale_y;
  glfwGetWindowContentScale(glfw_window, &px_scale_x, &px_scale_y);

  stage = std::make_unique<DefaultStage>(getWidth(), getHeight());
  stage->setContentScale(px_scale_x, px_scale_y);
}

Window::~Window()
{
  /* Let stage destruct shaders first. */
  stage = nullptr;

  GWN_context_active_set(gwn_context);
  GPU_exit();
  GWN_context_discard(gwn_context);

  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(glfw_window);
}

void Window::draw()
{
  GWN_context_active_set(gwn_context);
  immActivate();

  stage->draw();

  immDeactivate();
  GWN_context_active_set(nullptr);

  glfwSwapBuffers(glfw_window);
}

auto Window::processEvents() -> Window::WindowAction
{
  if (glfwWindowShouldClose(glfw_window)) {
    return WINDOW_ACTION_CLOSE;
  }

  return WINDOW_ACTION_CONTINUE;
}

auto Window::getCursorPosition() const -> bWidgets::bwPoint
{
  int win_size_y;
  double x, y;

  glfwGetCursorPos(glfw_window, &x, &y);
  glfwGetWindowSize(glfw_window, nullptr, &win_size_y);

  /* Invert vertically. */
  bwPoint position{float(x), float(win_size_y - y)};

#ifndef __APPLE__
  /* We need unscaled coordinates, which only Apple gives by default. */
  float px_scale_x, px_scale_y;
  glfwGetWindowContentScale(glfw_window, &px_scale_x, &px_scale_y);
  position.x /= px_scale_x;
  position.y /= px_scale_y;
#endif

  return position;
}

void Window::handleResizeEvent(const int new_win_x, const int new_win_y)
{
  width = new_win_x;
  height = new_win_y;
  stage->handleWindowResizeEvent(*this);
}

void Window::handleContentScaleEvent(const float new_scale_x, const float new_scale_y)
{
  stage->setContentScale(new_scale_x, new_scale_y);
}

auto Window::getGlfwWindow() const -> GLFWwindow&
{
  return *glfw_window;
}

auto Window::getWidth() const -> int
{
  return width;
}

auto Window::getHeight() const -> int
{
  return height;
}

}  // namespace bWidgetsDemo
