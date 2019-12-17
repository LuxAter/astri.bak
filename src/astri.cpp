#include <chrono>

#ifdef __APPLE__
#include <cli/cli.hpp>
#else
#include <CLI/CLI.hpp>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl.h"
#include "imgui/imgui.h"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "log.hpp"
#include "shader.hpp"
#include "stb_image_write.h"
#include "icosasphere.hpp"

static int width_ = 500, height_ = 500;

bool ends_with(std::string const& fullString, std::string const& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

bool write_image(const std::string& file_path, std::uint8_t* pixmap) {
  LINFO("Writing image to \"{}\"", file_path);
  bool ret = false;
  stbi_flip_vertically_on_write(true);
  if (ends_with(file_path, ".png")) {
    ret = (stbi_write_png(file_path.c_str(), width_, height_, 3, pixmap,
                          sizeof(std::uint8_t) * width_ * 3) != 0);
  } else if (ends_with(file_path, ".bmp")) {
    ret = (stbi_write_bmp(file_path.c_str(), width_, height_, 3, pixmap) != 0);
  } else if (ends_with(file_path, ".tga")) {
    ret = (stbi_write_tga(file_path.c_str(), width_, height_, 3, pixmap) != 0);
  } else if (ends_with(file_path, ".jpg")) {
    ret = (stbi_write_jpg(file_path.c_str(), width_, height_, 3, pixmap, 75) !=
           0);
  }
  return ret;
}

void glfw_error_callback(int error_code, const char* description) {
  error("GLFW [{}]: {}", error_code, description);
}
void glfw_framebuffer_size_callback(GLFWwindow*, int w, int h) {
  width_ = w;
  height_ = h;
  glViewport(0, 0, width_, height_);
}

int main(int argc, char* argv[]) {
  bool fullscreen = false, floating = false;
  glm::vec3 background(0.149f, 0.196f, 0.219f);

  CLI::App app{"Astri"};
  app.add_flag("--fullscreen", fullscreen, "Enables fullscreen window");
  app.add_flag("--floating", floating, "Enables floating window");
  app.add_option("--width", width_, "Width of window in pixels");
  app.add_option("--height", height_, "Height of window in pixels");

  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp& e) {
    app.exit(e);
    return 0;
  } catch (const CLI::ParseError& e) {
    app.exit(e);
    return -1;
  }

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    const char* glfw_err;
    glfwGetError(&glfw_err);
    LCRITICAL("Failed to initalize GLFW: {}", glfw_err);
    return -1;
  }
  GLFWwindow* window = NULL;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  if (fullscreen) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width, mode->height, "Astri",
                              glfwGetPrimaryMonitor(), NULL);
    width_ = mode->width;
    height_ = mode->height;
  } else if (floating) {
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width_, height_, "Astri", NULL, NULL);
  } else {
    window = glfwCreateWindow(width_, height_, "Astri", NULL, NULL);
  }
  if (window == NULL) {
    glfwTerminate();
    LCRITICAL("Failed to create GLFW window");
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LCRITICAL("Failed to load OpenGL");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  glViewport(0, 0, width_, height_);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
  glClearColor(background.r, background.g, background.b, 1.0f);
  glEnable(GL_DEPTH_TEST);

  srand(time(NULL));

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags &= ImGuiConfigFlags_NavEnableKeyboard;
  io.Fonts->AddFontFromFileTTF("resources/Roboto-Regular.ttf", 20.0f);
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  Shader object_shader("resources/vertex.glsl", "resources/fragment.glsl");
  glm::vec3 light_color(1.0);
  glm::vec3 view_pos(0.0, 0.0, 10.0);
  glm::vec3 view_dir(0.0, 0.0, -1.0);
  glm::vec3 view_up(0.0, 1.0, 0.0);
  glm::mat4 view = glm::lookAt(view_pos, view_pos + view_dir, view_up);
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)width_ / (float)height_, 0.1f, 100.0f);
  object_shader.use();
  object_shader.set("uLightPos", glm::vec3(10.0, 0.0, 0.0));
  object_shader.set("uLightColor", light_color);
  object_shader.set("uView", view);
  object_shader.set("uProjection", projection);
  object_shader.set("uModel", glm::mat4(1.0f));
  object_shader.set("uColor", glm::vec3(1.0, 1.0, 1.0));
  object_shader.set("uViewPos", view_pos);

  IcosaSphere sun(1.0, 3);
  sun.add_instance();
  sun.set_color(255, 255, 255);


  bool hide_menu = false;
  bool config_window = false;
  bool debug_window = false;

  char file_fmt[255] = "%04d";
  const char* file_ext[] = {".png", ".bmp", "tga", "jpg"};
  static int file_ext_current = 0;

  std::uint8_t capture = 0;

  auto start_time = std::chrono::high_resolution_clock::now();
  std::size_t frame_count = 0;
  std::vector<float> fps_history;
  float fps_sample_rate = 100.0f;
  float fps_history_length = 10.0;

  int key_delay = 0;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (key_delay <= 0 && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      debug_window = !debug_window;
      key_delay = 10;
    }
    if (key_delay <= 0 && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      config_window = !config_window;
      key_delay = 10;
    }
    if (key_delay <= 0 && glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      hide_menu = !hide_menu;
      key_delay = 10;
    }
    if (key_delay <= 0 && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      capture = 2;
      key_delay = 10;
    }
    if (key_delay <= 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      key_delay = 10;
    }
    if (key_delay > 0) {
      key_delay--;
    }


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (!hide_menu && capture == 0) {
      if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Open")) {
          if (ImGui::MenuItem("Hide", "Ctrl+H")) {
            hide_menu = !hide_menu;
          }
          if (ImGui::MenuItem("Config", "Ctrl+C")) {
            config_window = !config_window;
          }
          if (ImGui::MenuItem("Debug", "Ctrl+D")) {
            debug_window = !debug_window;
          }
          if (ImGui::MenuItem("ScreenShot", "Ctrl+S")) {
            capture = 2;
          }
          ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Quit", "Esc")) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        ImGui::EndMainMenuBar();
      }
    }
    if (config_window && capture == 0) {
      if (ImGui::Begin("Config", &config_window, 0)) {
        if (ImGui::CollapsingHeader("Colors")) {
          if (ImGui::ColorEdit3("Background", glm::value_ptr(background))) {
            glClearColor(background.r, background.g, background.b, 1.0f);
          }
          if (ImGui::ColorEdit4("Text", (float*)&style.Colors[0])) {
          }
          if (ImGui::ColorEdit3("Light", glm::value_ptr(light_color))) {
            object_shader.set("uLightColor", light_color);
          }
        }
        if (ImGui::CollapsingHeader("Screenshot")) {
          ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5);
          ImGui::InputTextWithHint("", "File format", file_fmt,
                                   IM_ARRAYSIZE(file_fmt));
          ImGui::SameLine();
          ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2);
          ImGui::Combo("", &file_ext_current, file_ext, IM_ARRAYSIZE(file_ext));
        }
      }
      ImGui::End();
    }
    if (debug_window && capture == 0) {
      if (ImGui::Begin("Debug", &debug_window, 0)) {
        ImGui::PlotLines("FPS", fps_history.data(), fps_history.size(), 0, NULL,
                         FLT_MAX, FLT_MAX, ImVec2(0, 50));
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3);
        ImGui::SliderFloat("FPS Rate", &fps_sample_rate, 1000.0f, 10.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3);
        ImGui::SliderFloat("FPS History", &fps_history_length, 1.0f, 120.0f);
      }
      ImGui::End();
    }

    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING GOES HERE
    sun.draw();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (capture == 1) {
      glfwGetFramebufferSize(window, &width_, &height_);
      GLubyte* framebuffer =
          (GLubyte*)(malloc(sizeof(GLubyte) * width_ * height_ * 3));
      glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE,
                   framebuffer);
      char file_buffer[255] = "";
      snprintf(file_buffer, 255, file_fmt, rand() % 1000);
      write_image(std::string(file_buffer) + file_ext[file_ext_current],
                  framebuffer);
      free(framebuffer);
      capture = 0;
    } else if (capture > 1) {
      capture--;
    }
    glfwSwapBuffers(window);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time)
            .count() >= fps_sample_rate) {
      fps_history.push_back(
          frame_count * 1000.0f /
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::high_resolution_clock::now() - start_time)
              .count());
      while (fps_history.size() >=
             fps_history_length / (fps_sample_rate / 1000.0f)) {
        fps_history.erase(fps_history.begin());
      }
      start_time = std::chrono::high_resolution_clock::now();
      frame_count = 0;
    }
    frame_count++;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
