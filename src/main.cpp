#include <iostream>

#include <stdbool.h>
#include <string.h>

#include "glad/glad.h"
#include "logger.hpp"
#include "shader.hpp"

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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

  GLFWwindow* window = NULL;

  if (argc > 1 && !strncmp(argv[1], "--full", 6)) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    window = glfwCreateWindow(mode->width, mode->height, "Astri",
                              glfwGetPrimaryMonitor(), NULL);
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

  float vertices[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // top
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3,  // first triangle
  };

  astri::Shader shader("shaders/vs.glsl", "shaders/fs.glsl");

  // VBO
  // --------------------------------------------------------------------------
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // --------------------------------------------------------------------------

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    // input
    process_input(window);

    // rendering
    glClearColor(0.149f, 0.196f, 0.219f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
