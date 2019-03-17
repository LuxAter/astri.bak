#include <iostream>

#include <stdbool.h>
#include <string.h>

#include <cmath>

#include "colors.hpp"
#include "data.hpp"
#include "glad/glad.h"
#include "logger.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "url.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GLFW/glfw3.h>

#define COUNT 1000

astri::Object* earth = NULL;
std::vector<astri::Object*> satellites;

static int width_ = 500, height_ = 500;
static double scaled_sec = 1.0;

float frand() { return ((double)rand() / (RAND_MAX)); }

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  width_ = width;
  height_ = height;
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }else if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS){
    scaled_sec = 1.0;
  }
}

glm::vec3 cartesian(const glm::vec3& polar) {
  return glm::vec3(polar.x * std::sin(std::fmod(polar.y, 2.0 * M_PI)) *
                       std::cos(std::fmod(-polar.z, 2.0 * M_PI)),
                   polar.x * std::cos(std::fmod(polar.y, 2.0 * M_PI)),
                   polar.x * std::sin(std::fmod(polar.y, 2.0 * M_PI)) *
                       std::sin(std::fmod(-polar.z, 2.0 * M_PI)));
}

int main(int argc, char* argv[]) {
  // INIT
  // --------------------------------------------------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  astri::data::validate_data();

  // return 0;

  GLFWwindow* window = NULL;

  if (argc > 1 && !strncmp(argv[1], "--full", 6)) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    window = glfwCreateWindow(mode->width, mode->height, "Astri",
                              glfwGetPrimaryMonitor(), NULL);
    width_ = mode->width;
    height_ = mode->height;
  } else if (argc > 1 && !strncmp(argv[1], "--float", 7)) {
    // FLOATING IS NOT WORKING :(
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    window = glfwCreateWindow(500, 500, "Astri", NULL, NULL);
  } else {
    window = glfwCreateWindow(500, 500, "Astri", NULL, NULL);
  }
  if (window == NULL) {
    glfwTerminate();
    astri::Fatal("Failed to open window \"%s\"", "Astri");
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    astri::Fatal("Failed to load OpenGL functions");
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  astri::Shader shader("shaders/vs.glsl", "shaders/fs.glsl");

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  srand(time(NULL));

  glEnable(GL_DEPTH_TEST);

  glm::vec3 view_pos = glm::vec3(0.0, 0.0, 3.0f);
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, -view_pos);

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f),
                                (float)width_ / (float)height_, 0.1f, 100.0f);

  earth = new astri::Object(&shader, 1.0, 2);
  earth->add_instance();
  earth->set_color(144, 164, 174);
  // earth->back() = glm::rotate(earth->back(), (float)(M_PI/2.0),
  // glm::vec3(1.0, 0.0, 0.0));

  satellites.push_back(new astri::Object(&shader, 0.01, 0));
  satellites.back()->set_color(76, 175, 80);
  satellites.push_back(new astri::Object(&shader, 0.01, 0));
  satellites.back()->set_color(33, 150, 243);
  satellites.push_back(new astri::Object(&shader, 0.01, 0));
  satellites.back()->set_color(244, 67, 54);
  satellites.push_back(new astri::Object(&shader, 0.01, 0));
  satellites.back()->set_color(255, 193, 7);

  std::vector<std::vector<std::array<double, 3>>> factors(satellites.size());

  for (int i = 0; i < COUNT; ++i) {
    for (int j = 0; j < satellites.size(); ++j) {
      satellites[j]->add_instance();
      factors[j].push_back(
          {{frand() + 1.0, (frand() + 1.0) * 1e-2, (frand() + 1.0) * 1e-2}});
    }
  }

  double g_time = 0.0;

  glm::vec3 sph(1.2, M_PI / 2.0, 0.0);

  shader.use();
  shader.set("uLightPos", glm::vec3(-100.0, 0.0, 50.0));
  shader.set("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
  shader.set("uView", view);
  shader.set("uProjection", projection);
  shader.set("uViewPos", view_pos);

  while (!glfwWindowShouldClose(window)) {
    // input
    process_input(window);
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
      scaled_sec +=
          std::max(5.0,
                   std::pow(3.0, std::floor(std::log(std::fabs(scaled_sec))))) /
          60.0;
      astri::Info("dT=%lf", scaled_sec);
    } else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
      scaled_sec -=
          std::max(5.0,
                   std::pow(3.0, std::floor(std::log(std::fabs(scaled_sec))))) /
          60.0;
    }

    // rendering
    glClearColor(0.149f, 0.196f, 0.219f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // shader.set("uLightPos", glm::vec3(100.0 * std::sin(1e-4 * g_time),
    //                                   100.0 * std::cos(1e-4 * g_time), 0.0));
    earth->back() = glm::rotate(glm::mat4(1.0f), (float)(7.29211e-5 * g_time),
                                glm::vec3(0.0, 1.0, 0.0));
    // earth->back() = glm::translate(glm::mat4(1.0f), glm::vec3(std::sin(1e-5*g_time), 0.0, std::cos(1e-5*g_time)));
    earth->draw();

    for (int i = 0; i < COUNT; ++i) {
      for (int j = 0; j < satellites.size(); ++j) {
        sph.z = (float)(factors[j][i][0]);
        sph.y = (float)(factors[j][i][1] * g_time);
        sph.z = (float)(factors[j][i][2] * g_time);
        satellites[j]->at(i) = glm::translate(glm::mat4(1.0),
        cartesian(sph));
      }
    }
    for (auto& sat : satellites) {
      sat->draw();
    }

    // swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
    g_time += (scaled_sec / 60.0);
  }

  free(earth);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
