#ifndef ASTRI_OBJECT_HPP_
#define ASTRI_OBJECT_HPP_

#include <map>
#include <vector>

#include "glm/glm.hpp"

#include "shader.hpp"

namespace astri {
class Object {
 public:
  Object(Shader* shader, float radius = 1.0, unsigned sub = 1);

  void draw() const;

  void add_instance();

  inline std::pair<glm::mat4, glm::vec3>& front() { return instances.front(); }
  inline std::pair<glm::mat4, glm::vec3>& back() { return instances.back(); }
  inline std::pair<glm::mat4, glm::vec3>& operator[](std::size_t i) {
    return instances[i];
  }

 private:
  std::vector<float> compute_vertices();
  void gen_vertices();

  void subdivide_vertices();
  void build_interleaved();

  void add_vertices(float x, float y, float z);
  void add_vertices(const float v1[3], const float v2[3], const float v3[3]);
  void add_normals(float nx, float ny, float nz);
  void add_normals(const float n1[3], const float n2[3], const float n3[3]);
  void add_indices(unsigned i1, unsigned i2, unsigned i3);

  void compute_face_normal(const float v1[3], const float v2[3],
                           const float v3[3], float n[3]);
  void compute_half_vertex(const float v1[3], const float v2[3], float length,
                           float new_v[3]);
  float compute_scale_for_length(const float v[3], float length);

  float radius_;
  unsigned sub_;

  unsigned VBO, VAO, EBO;

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<unsigned> indices;

  std::vector<float> interleaved;
  std::vector<std::pair<glm::mat4, glm::vec3>> instances;

  const Shader* shader_;
};
}  // namespace astri

#endif  // ASTRI_OBJECT_HPP_