#ifndef SHADER_HPP_80PHQYK7
#define SHADER_HPP_80PHQYK7

#include <fstream>
#include <glm/glm.hpp>
#include <string>

#include "gl.h"

class Shader {
 public:
  Shader() {}
  Shader(const std::string& vertex_path, const std::string& fragment_path);

  void use();

  void set(const std::string& name, bool value) const;
  void set(const std::string& name, int value) const;
  void set(const std::string& name, float value) const;
  void set(const std::string& name, glm::mat4 trans) const;
  void set(const std::string& name, glm::vec3 trans) const;

  unsigned int ID;
};

#endif /* end of include guard: SHADER_HPP_80PHQYK7 */
