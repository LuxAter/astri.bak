#include "globals.hpp"

#include "log.hpp"

namespace globals {
int width = 500, height = 500;
GLFWwindow* window = NULL;

bool hide_menu = false, config_window = false, debug_window = false;
char file_fmt[255] = "%04d";
const char* file_ext[4] = {".png", ".bmp", ".tga", ".jpg"};
int file_ext_choice = 0;

float delta_time = 0.0f, last_time = 0.0f;
std::chrono::high_resolution_clock::time_point start_time;
std::size_t frame_count = 0;
float last_x = -1.0, last_y = -1.0;

std::map<std::string, std::pair<std::size_t, std::vector<float>>> plot_data;
float fps_history_length = 10.0f, fps_history_rate = 100.0f;

std::map<std::string, glm::vec3> colors;

int key_delay = 0;
int capture = 0;
float yaw = -90.0f, pitch = 0.0f, fov = 45.0f;
}  // namespace globals
std::map<std::string, Shader> shaders;
std::map<std::string, glm::vec3> colors;

glm::vec3 view_pos(0.0, 0.0, 10.0);
glm::vec3 view_dir(0.0, 0.0, -1.0);
glm::vec3 view_up(0.0, 1.0, 0.0);

void globals::plot_update(const std::string& key, std::size_t size) {
  if (plot_data.find(key) != plot_data.end()) {
    plot_data.at(key).first = size;
  } else {
    int size_v = size;
    plot_data.insert({key, std::make_pair<std::size_t, std::vector<float>>(
                               size_v, std::vector<float>(0, 0.0f))});
  }
}
void globals::plot_push(const std::string& key, const float& v) {
  if (plot_data.find(key) != plot_data.end()) {
    auto& it = plot_data.at(key);
    it.second.push_back(v);
    while (it.second.size() > it.first) {
      it.second.erase(it.second.begin());
    }
  } else {
    plot_data.insert(
        {key, std::make_pair<std::size_t, std::vector<float>>(100, {v})});
  }
}
const float* globals::get_data(const std::string& key) {
  if (plot_data.find(key) != plot_data.end()) {
    return plot_data.at(key).second.data();
  } else {
    return NULL;
  }
}
std::size_t globals::get_size(const std::string& key) {
  if (plot_data.find(key) != plot_data.end()) {
    return plot_data.at(key).second.size();
  } else {
    return 0;
  }
}
