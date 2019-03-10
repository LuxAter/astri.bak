#include <iostream>

#include <stdbool.h>
#include <string.h>

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

astri::Object* obj = NULL;

static int width_ = 500, height_ = 500;

float frand() { return ((double)rand() / (RAND_MAX)); }

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  width_ = width;
  height_ = height;
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
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
  // --------------------------------------------------------------------------

  astri::Shader shader("shaders/vs.glsl", "shaders/fs.glsl");

  // VBO
  // --------------------------------------------------------------------------
  // unsigned int VBO;
  // unsigned int VAO;
  // unsigned int EBO;
  // glGenVertexArrays(1, &VAO);
  // glGenBuffers(1, &VBO);
  // glGenBuffers(1, &EBO);
  //
  // glBindVertexArray(VAO);
  //
  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  //
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);
  //
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
  // (void*)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 3,
  // GL_FLOAT, GL_FALSE, 9 * sizeof(float),
  //                       (void*)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
  //                       (void*)(6 * sizeof(float)));
  // glEnableVertexAttribArray(2);
  // --------------------------------------------------------------------------

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  srand(time(NULL));

  obj = new astri::Object(&shader);

  glEnable(GL_DEPTH_TEST);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f),
                                (float)width_ / (float)height_, 0.1f, 100.0f);

  for (int i = 0; i < 10; ++i) {
    obj->add_instance();
    glm::vec3 pos(2 * (frand() - 0.5), 2 * (frand() - 0.5),
                  2 * (frand() - 0.5));
    obj->back().first = glm::translate(obj->back().first, pos);
    float v = frand();
    obj->back().first = glm::scale(obj->back().first, glm::vec3(v, v, v));
    obj->back().second = glm::vec3(frand(), frand(), frand());
  }
  // std::vector<glm::vec3> colors;
  // for (int i = 0; i < 10; ++i) {
  //   colors.push_back(glm::vec3(frand(), frand(), frand()));
  // }

  while (!glfwWindowShouldClose(window)) {
    // input
    process_input(window);

    // rendering
    glClearColor(0.149f, 0.196f, 0.219f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.set("view", view);
    shader.set("projection", projection);
    obj->draw();
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(obj);

  // glDeleteVertexArrays(1, &VAO);
  // glDeleteBuffers(1, &VBO);
  //
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
