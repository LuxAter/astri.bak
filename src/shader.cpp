#include "shader.hpp"

#include <fstream>
#include <string>
#include <sstream>

#include "gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#include "log.hpp"

Shader::Shader(const std::string& vertex_path,
                      const std::string& fragment_path) {
  std::string vertex_source, fragment_source;
  std::ifstream v_shader_file(vertex_path.c_str());
  if (v_shader_file.is_open()) {
    std::stringstream ss;
    ss << v_shader_file.rdbuf();
    v_shader_file.close();
    vertex_source = ss.str();
  } else {
    LERROR("Failed to load vertex shader file \"{}\"", vertex_path);
  }
  std::ifstream f_shader_file(fragment_path.c_str());
  if (f_shader_file.is_open()) {
    std::stringstream ss;
    ss << f_shader_file.rdbuf();
    f_shader_file.close();
    fragment_source = ss.str();
  } else {
    LERROR("Failed to load fragment shader file \"{}\"", fragment_path);
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
    LERROR("Failed to compile vertex shader \"{}\"", infoLog);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragment_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    LERROR("Failed to compile fragment shader \"{}\"", infoLog);
  }

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    LERROR("Failed to link shader program \"{}\"", infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::set(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::set(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::set(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::set(const std::string& name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}
void Shader::set(const std::string& name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
               glm::value_ptr(value));
}
