#include <chrono>

#ifdef __APPLE__
#include <cli/cli.hpp>
#else
#include <CLI/CLI.hpp>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl.h"
#include "globals.hpp"
#include "icosasphere.hpp"
#include "input.hpp"
#include "log.hpp"
#include "shader.hpp"
#include "stb_image_write.h"
#include "ui.hpp"

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
    ret = (stbi_write_png(file_path.c_str(), globals::width, globals::height, 3,
                          pixmap,
                          sizeof(std::uint8_t) * globals::width * 3) != 0);
  } else if (ends_with(file_path, ".bmp")) {
    ret = (stbi_write_bmp(file_path.c_str(), globals::width, globals::height, 3,
                          pixmap) != 0);
  } else if (ends_with(file_path, ".tga")) {
    ret = (stbi_write_tga(file_path.c_str(), globals::width, globals::height, 3,
                          pixmap) != 0);
  } else if (ends_with(file_path, ".jpg")) {
    ret = (stbi_write_jpg(file_path.c_str(), globals::width, globals::height, 3,
                          pixmap, 75) != 0);
  }
  return ret;
}

void glfw_error_callback(int error_code, const char* description) {
  error("GLFW [{}]: {}", error_code, description);
}
void glfw_framebuffer_size_callback(GLFWwindow*, int w, int h) {
  globals::width = w;
  globals::height = h;
  glViewport(0, 0, globals::width, globals::height);
}
static bool mouse_first = true;
void glfw_mouse_callback(GLFWwindow*, double xpos, double ypos) {
  if (mouse_first == true) {
    globals::last_x = xpos;
    globals::last_y = ypos;
    mouse_first = false;
  }

  float xoffset = xpos - globals::last_x;
  float yoffset = globals::last_y - ypos;
  globals::last_x = xpos;
  globals::last_y = ypos;

  float sensitivity = 0.05;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  globals::yaw += xoffset;
  globals::pitch += yoffset;

  if (globals::pitch > 89.0f) globals::pitch = 89.0f;
  if (globals::pitch < -89.0f) globals::pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(globals::yaw)) * cos(glm::radians(globals::pitch));
  front.y = sin(glm::radians(globals::pitch));
  front.z = sin(glm::radians(globals::yaw)) * cos(glm::radians(globals::pitch));
  view_dir = glm::normalize(front);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  if (globals::fov >= 1.0f && globals::fov <= 45.0f) globals::fov -= yoffset;
  if (globals::fov <= 1.0f) globals::fov = 1.0f;
  if (globals::fov >= 45.0f) globals::fov = 45.0f;
}

int main(int argc, char* argv[]) {
  bool fullscreen = false, floating = false;
  CLI::App app{"Astri"};
  app.add_flag("--fullscreen", fullscreen, "Enables fullscreen window");
  app.add_flag("--floating", floating, "Enables floating window");
  app.add_option("--width", globals::width, "Width of window in pixels");
  app.add_option("--height", globals::height, "Height of window in pixels");

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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  if (fullscreen) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    globals::window = glfwCreateWindow(mode->width, mode->height, "Astri",
                                       glfwGetPrimaryMonitor(), NULL);
    globals::width = mode->width;
    globals::height = mode->height;
  } else if (floating) {
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    globals::window =
        glfwCreateWindow(globals::width, globals::height, "Astri", NULL, NULL);
  } else {
    globals::window =
        glfwCreateWindow(globals::width, globals::height, "Astri", NULL, NULL);
  }
  if (globals::window == NULL) {
    glfwTerminate();
    LCRITICAL("Failed to create GLFW window");
    return -1;
  }
  glfwMakeContextCurrent(globals::window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LCRITICAL("Failed to load OpenGL");
    glfwDestroyWindow(globals::window);
    glfwTerminate();
    return -1;
  }
  glViewport(0, 0, globals::width, globals::height);
  glfwSetFramebufferSizeCallback(globals::window,
                                 glfw_framebuffer_size_callback);
  glfwSetCursorPosCallback(globals::window, glfw_mouse_callback);
  glEnable(GL_DEPTH_TEST);
  ui::init();
  srand(time(NULL));

  colors["background"] = {0.149f, 0.196f, 0.219f};
  colors["light"] = {1.0f, 1.0f, 1.0f};

  shaders["object"] =
      Shader("resources/vertex.glsl", "resources/fragment.glsl");
  shaders["object"].use();
  shaders["object"].set("uLightPos", glm::vec3(10.0, 0.0, 0.0));
  shaders["object"].set("uLightColor", colors["light"]);
  shaders["object"].set("uModel", glm::mat4(1.0f));
  shaders["object"].set("uColor", glm::vec3(1.0, 1.0, 1.0));
  shaders["object"].set("uViewPos", view_pos);

  IcosaSphere sun(1.0, 3);
  sun.add_instance();
  sun.set_color(255, 255, 255);
  globals::plot_update("FPS", globals::fps_history_length /
                                  (globals::fps_history_rate / 1000.0f));

  glClearColor(colors["background"].r, colors["background"].g,
               colors["background"].b, 1.0f);
  while (!glfwWindowShouldClose(globals::window)) {
    input::process();
    glm::mat4 view = glm::lookAt(view_pos, view_pos + view_dir, view_up);
    glm::mat4 projection = glm::perspective(
        glm::radians(globals::fov),
        (float)globals::width / (float)globals::height, 0.1f, 100.0f);
    shaders["object"].set("uView", view);
    shaders["object"].set("uProjection", projection);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ui::draw();

    // RENDERING GOES HERE
    sun.draw();

    ui::render();
    if (globals::capture == 1) {
      glfwGetFramebufferSize(globals::window, &globals::width,
                             &globals::height);
      GLubyte* framebuffer = (GLubyte*)(malloc(
          sizeof(GLubyte) * globals::width * globals::height * 3));
      glReadPixels(0, 0, globals::width, globals::height, GL_RGB,
                   GL_UNSIGNED_BYTE, framebuffer);
      char file_buffer[255] = "";
      snprintf(file_buffer, 255, globals::file_fmt, rand() % 1000);
      write_image(std::string(file_buffer) +
                      globals::file_ext[globals::file_ext_choice],
                  framebuffer);
      free(framebuffer);
      globals::capture = 0;
    } else if (globals::capture > 1) {
      globals::capture--;
    }
    glfwSwapBuffers(globals::window);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - globals::start_time)
            .count() >= globals::fps_history_rate) {
      globals::plot_push(
          "FPS", globals::frame_count * 1000.0f /
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::high_resolution_clock::now() -
                         globals::start_time)
                         .count());

      globals::start_time = std::chrono::high_resolution_clock::now();
      globals::frame_count = 0;
    }
    globals::frame_count++;
  }

  glfwDestroyWindow(globals::window);
  glfwTerminate();
  return 0;
}
