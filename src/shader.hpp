#ifndef ASTRI_SHADER_HPP_
#define ASTRI_SHADER_HPP_

#include <fstream>
#include <string>

#include "glad/glad.h"

#include <GLFW/glfw3.h>

namespace astri {
class Shader {
 public:
  Shader(const std::string& vertex_path, const std::string& fragment_path);

  void use();

  void set(const std::string& name, bool value) const;
  void set(const std::string& name, int value) const;
  void set(const std::string& name, float value) const;

  unsigned int ID;
};
}  // namespace astri

#endif  // ASTRI_SHADER_HPP_
