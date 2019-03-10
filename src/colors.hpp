#ifndef ASTRI_COLORS_HPP_
#define ASTRI_COLORS_HPP_

#include <map>
#include <string>
#include <array>

#include "glm/glm.hpp"

namespace astri {
  extern std::map<std::string, std::array<float, 3>> colors;
  glm::vec3 get_color(std::string c);
} // namespace astri

#endif  // ASTRI_COLORS_HPP_
