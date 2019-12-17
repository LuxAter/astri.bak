#include "ui.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl.h"
#include "globals.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "log.hpp"

void ui::init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags &= ImGuiConfigFlags_NavEnableKeyboard;
  io.Fonts->AddFontFromFileTTF("resources/Roboto-Regular.ttf", 20.0f);
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(globals::window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}
void ui::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if (!globals::hide_menu && globals::capture == 0) {
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Open")) {
        if (ImGui::MenuItem("Hide", "Ctrl+H")) {
          globals::hide_menu = !globals::hide_menu;
        }
        if (ImGui::MenuItem("Config", "Ctrl+C")) {
          globals::config_window = !globals::config_window;
        }
        if (ImGui::MenuItem("Debug", "Ctrl+D")) {
          globals::debug_window = !globals::debug_window;
        }
        if (ImGui::MenuItem("ScreenShot", "Ctrl+S")) {
          globals::capture = 2;
        }
        ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Quit", "Esc")) {
        glfwSetWindowShouldClose(globals::window, GLFW_TRUE);
      }
      ImGui::EndMainMenuBar();
    }
  }
  if (globals::config_window && globals::capture == 0) {
    if (ImGui::Begin("Config", &globals::config_window, 0)) {
      if (ImGui::CollapsingHeader("Colors")) {
        ImGuiStyle& style = ImGui::GetStyle();
        if (ImGui::ColorEdit3("Background",
                              glm::value_ptr(colors["background"]))) {
          glClearColor(colors["background"].r, colors["background"].g,
                       colors["background"].b, 1.0f);
        }
        if (ImGui::ColorEdit4("Text", (float*)&style.Colors[0])) {}
        if (ImGui::ColorEdit3("Light", glm::value_ptr(colors["light"]))) {
          shaders["object"].set("uLightColor", colors["light"]);
        }
      }
      if (ImGui::CollapsingHeader("Screenshot")) {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5);
        ImGui::InputTextWithHint("", "File format", globals::file_fmt,
                                 IM_ARRAYSIZE(globals::file_fmt));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2);
        ImGui::Combo("", &globals::file_ext_choice, globals::file_ext,
                     IM_ARRAYSIZE(globals::file_ext));
      }
    }
    ImGui::End();
  }
  if (globals::debug_window && globals::capture == 0) {
    if (ImGui::Begin("Debug", &globals::debug_window, 0)) {
      ImGui::PlotLines("FPS", globals::get_data("FPS"),
                       globals::get_size("FPS"), 0, NULL, FLT_MAX, FLT_MAX,
                       ImVec2(0, 50));
      ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3);
      if (ImGui::SliderFloat("FPS Rate", &globals::fps_history_rate, 1000.0f,
                             10.0f)) {
        globals::plot_update("FPS", globals::fps_history_length /
                                        (globals::fps_history_rate / 1000.0f));
      }
      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3);
      if (ImGui::SliderFloat("FPS History", &globals::fps_history_length, 1.0f,
                             120.0f)) {
        globals::plot_update("FPS", globals::fps_history_length /
                                        (globals::fps_history_rate / 1000.0f));
      }
    }
    ImGui::End();
  }
  ImGui::Render();
}

void ui::render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
