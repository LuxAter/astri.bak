#include "input.hpp"

#include "gl.h"
#include "globals.hpp"

void input::process() {
  float current_frame = glfwGetTime();
  globals::delta_time = current_frame - globals::last_time;
  globals::last_time = current_frame;
  float camera_speed = 2.5f * globals::delta_time;
  glfwPollEvents();

  if (globals::key_delay <= 0 &&
      glfwGetKey(globals::window, GLFW_KEY_D) == GLFW_PRESS &&
      glfwGetKey(globals::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    globals::debug_window = !globals::debug_window;
    globals::key_delay = 10;
  }
  if (globals::key_delay <= 0 &&
      glfwGetKey(globals::window, GLFW_KEY_C) == GLFW_PRESS &&
      glfwGetKey(globals::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    globals::config_window = !globals::config_window;
    globals::key_delay = 10;
  }
  if (globals::key_delay <= 0 &&
      glfwGetKey(globals::window, GLFW_KEY_H) == GLFW_PRESS &&
      glfwGetKey(globals::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    globals::hide_menu = !globals::hide_menu;
    globals::key_delay = 10;
  }
  if (globals::key_delay <= 0 &&
      glfwGetKey(globals::window, GLFW_KEY_S) == GLFW_PRESS &&
      glfwGetKey(globals::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    globals::capture = 2;
    globals::key_delay = 10;
  }
  if (globals::key_delay <= 0 &&
      glfwGetKey(globals::window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(globals::window, GLFW_TRUE);
    globals::key_delay = 10;
  }
  if (glfwGetKey(globals::window, GLFW_KEY_W) == GLFW_PRESS)
    view_pos += camera_speed * view_dir;
  if (glfwGetKey(globals::window, GLFW_KEY_S) == GLFW_PRESS)
    view_pos -= camera_speed * view_dir;
  if (glfwGetKey(globals::window, GLFW_KEY_A) == GLFW_PRESS)
    view_pos -= glm::normalize(glm::cross(view_dir, view_up)) * camera_speed;
  if (glfwGetKey(globals::window, GLFW_KEY_D) == GLFW_PRESS)
    view_pos += glm::normalize(glm::cross(view_dir, view_up)) * camera_speed;
  if (globals::key_delay > 0) {
    globals::key_delay--;
  }
}
