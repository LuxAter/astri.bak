#include "colors.hpp"

#include <array>
#include <map>
#include <string>

namespace astri {
std::map<std::string, std::array<float, 3>> colors = {
    {"sun", {{1.0, 1.0, 1.0}}},
    {"jupiter", {{0.690, 0.647, 0.588}}},
    {"uranus", {{0.686, 0.831, 0.847}}},
    {"saturn", {{0.666, 0.600, 0.451}}},
    {"venus", {{0.713, 0.702, 0.670}}},
    {"mars", {{0.768, 0.474, 0.364}}},
    {"earth", {{0.690, 0.698, 0.772}}},
    {"mecury", {{0.623, 0.611, 0.619}}},
    {"neptune", {{0.223, 0.324, 0.788}}},
    {"red", {{0.956, 0.262, 0.211}}},
    {"pink", {{0.913, 0.117, 0.388}}},
    {"purple", {{0.611, 0.152, 0.690}}},
    {"deep_purple", {{0.403, 0.227, 0.717}}},
    {"indigo", {{0.247, 0.317, 0.709}}},
    {"blue", {{0.129, 0.588, 0.952}}},
    {"pale_blue", {{0.011, 0.662, 0.956}}},
    {"cyan", {{0.0, 0.737, 0.831}}},
    {"teal", {{0.341, 0.780, 0.721}}},
    {"green", {{0.298, 0.686, 0.313}}},
    {"pale_green", {{0.545, 0.764, 0.290}}},
    {"lime", {{0.803, 0.862, 0.223}}},
    {"yellow", {{1.0, 0.921, 0.231}}},
    {"amber", {{1.0, 0.756, 0.027}}},
    {"orange", {{1.0, 0.596, 0.0}}},
    {"deep_orange", {{1.0, 0.341, 0.133}}},
    {"brown", {{0.474, 0.333, 0.282}}}};
}  // namespace astri

glm::vec3 astri::get_color(std::string c) {
  return glm::vec3(colors[c][0], colors[c][1], colors[c][2]);
}
