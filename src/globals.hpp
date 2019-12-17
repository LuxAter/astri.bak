#ifndef GLOBALS_HPP_KMONEUID
#define GLOBALS_HPP_KMONEUID

#include <chrono>
#include <glm/glm.hpp>
#include <map>
#include <tuple>
#include <vector>

#include "gl.h"
#include "shader.hpp"

namespace globals {
extern int width, height;
extern GLFWwindow* window;

extern bool hide_menu, config_window, debug_window;
extern char file_fmt[255];
extern const char* file_ext[4];
extern int file_ext_choice;

extern float delta_time, last_time;
extern std::chrono::high_resolution_clock::time_point start_time;
extern std::size_t frame_count;

extern std::map<std::string, std::pair<std::size_t, std::vector<float>>>
    plot_data;
extern float fps_history_length, fps_history_rate;

extern float last_x, last_y;

extern int key_delay;
extern int capture;
extern float yaw, pitch, fov;

void plot_update(const std::string& key, std::size_t size);
void plot_push(const std::string& key, const float& v);
const float* get_data(const std::string& key);
std::size_t get_size(const std::string& key);

}  // namespace globals
extern std::map<std::string, Shader> shaders;
extern std::map<std::string, glm::vec3> colors;
extern glm::vec3 view_pos;
extern glm::vec3 view_dir;
extern glm::vec3 view_up;

#endif /* end of include guard: GLOBALS_HPP_KMONEUID */
