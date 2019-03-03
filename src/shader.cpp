#include "shader.hpp"

#include <fstream>
#include <string>

#include "glad/glad.h"
#include "logger.hpp"

#include <GLFW/glfw3.h>

astri::Shader::Shader(const std::string& vertex_path,
                      const std::string& fragment_path) {
  std::string vertex_source, fragment_source;
  std::ifstream v_shader_file(vertex_path.c_str());
  if (v_shader_file.is_open()) {
    std::stringstream ss;
    ss << v_shader_file.rdbuf();
    v_shader_file.close();
    vertex_source = ss.str();
  } else {
    Error("Failed to load vertex shader file \"%s\"", vertex_path.c_str());
  }
  std::ifstream f_shader_file(fragment_path.c_str());
  if (f_shader_file.is_open()) {
    std::stringstream ss;
    ss << f_shader_file.rdbuf();
    f_shader_file.close();
    fragment_source = ss.str();
  } else {
    Error("Failed to load fragment shader file \"%s\"", fragment_path.c_str());
  }

  const char* vertex_code = vertex_source.c_str();
  const char* fragment_code = fragment_source.c_str();
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertex_code, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    Error("Failed to compile vertex shader \"%s\"", infoLog);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragment_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    Error("Failed to compile fragment shader \"%s\"", infoLog);
  }

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    Error("Failed to link shader program \"%s\"", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void astri::Shader::use() { glUseProgram(ID); }

void astri::Shader::set(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void astri::Shader::set(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void astri::Shader::set(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
