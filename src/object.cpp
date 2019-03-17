#include "object.hpp"

#include <cmath>
#include <string>
#include <vector>

#include <iostream>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

astri::Object::Object(Shader* shader, float radius, unsigned sub)
    : radius_(radius), sub_(sub), shader_(shader) {
  gen_vertices();
}

void astri::Object::draw() const {
  glBindVertexArray(VAO);
  shader_->set("uColor", glm::vec3(red_, green_, blue_));
  for(std::size_t i = 0; i < instances.size(); ++i){
    shader_->set("uModel", instances[i]);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
}

void astri::Object::add_instance(){
  instances.push_back(glm::mat4(1.0));
}
void astri::Object::set_color(int r, int g, int b){
  red_ = r / 255.0;
  green_ = g / 255.0;
  blue_ = b / 255.0;
}
void astri::Object::set_color(float r, float g, float b){
  red_ = r;
  green_ = g;
  blue_ = b;
}

std::vector<float> astri::Object::compute_vertices() {
  const float h_angle = M_PI / 180.0 * 72.0;
  const float v_angle = std::atan(1.0f / 2.0);

  std::vector<float> vertices(12 * 3);
  int i1, i2;
  float z, xy;
  float h_angle_1 = -M_PI / 2.0 - h_angle / 2.0;
  float h_angle_2 = -M_PI / 2.0;

  vertices[0] = 0;
  vertices[1] = 0;
  vertices[2] = radius_;

  for (int i = 1; i <= 5; ++i) {
    i1 = i * 3;
    i2 = (i + 5) * 3;
    z = radius_ * std::sin(v_angle);
    xy = radius_ * std::cos(v_angle);

    vertices[i1] = xy * std::cos(h_angle_1);
    vertices[i2] = xy * std::cos(h_angle_2);
    vertices[i1 + 1] = xy * std::sin(h_angle_1);
    vertices[i2 + 1] = xy * std::sin(h_angle_2);
    vertices[i1 + 2] = z;
    vertices[i2 + 2] = -z;
    h_angle_1 += h_angle;
    h_angle_2 += h_angle;
  }
  i1 = 11 * 3;
  vertices[i1] = 0;
  vertices[i1 + 1] = 0;
  vertices[i1 + 2] = -radius_;
  return vertices;
}

void astri::Object::gen_vertices() {
  std::vector<float> tmp_vert = compute_vertices();

  vertices.clear();
  normals.clear();
  indices.clear();

  const float *v0, *v1, *v2, *v3, *v4, *v11;
  float n[3];
  unsigned int index = 0;

  v0 = &tmp_vert[0];
  v11 = &tmp_vert[11 * 3];
  for (int i = 1; i <= 5; ++i) {
    v1 = &tmp_vert[i * 3];
    if (i < 5) {
      v2 = &tmp_vert[(i + 1) * 3];
    } else {
      v2 = &tmp_vert[3];
    }

    v3 = &tmp_vert[(i + 5) * 3];
    if ((i + 5) < 10) {
      v4 = &tmp_vert[(i + 6) * 3];
    } else {
      v4 = &tmp_vert[6 * 3];
    }

    compute_face_normal(v0, v1, v2, n);
    add_vertices(v0, v1, v2);
    add_normals(n, n, n);
    add_indices(index, index + 1, index + 2);

    compute_face_normal(v1, v3, v2, n);
    add_vertices(v1, v3, v2);
    add_normals(n, n, n);
    add_indices(index + 3, index + 4, index + 5);

    compute_face_normal(v2, v3, v4, n);
    add_vertices(v2, v3, v4);
    add_normals(n, n, n);
    add_indices(index + 6, index + 7, index + 8);

    compute_face_normal(v3, v11, v4, n);
    add_vertices(v3, v11, v4);
    add_normals(n, n, n);
    add_indices(index + 9, index + 10, index + 11);

    index += 12;
  }

  subdivide_vertices();
  build_interleaved();
}

void astri::Object::subdivide_vertices() {
  std::vector<float> tmp_vert;
  std::vector<unsigned> tmp_ind;
  int index_count;
  const float *v1, *v2, *v3;
  float new_v1[3], new_v2[3], new_v3[3];
  float normal[3];
  unsigned index = 0;
  int i, j;
  for (i = 1; i <= sub_; ++i) {
    tmp_vert = vertices;
    tmp_ind = indices;

    vertices.clear();
    normals.clear();
    indices.clear();

    index = 0;
    index_count = (int)tmp_ind.size();
    for (j = 0; j < index_count; j += 3) {
      v1 = &tmp_vert[tmp_ind[j] * 3];
      v2 = &tmp_vert[tmp_ind[j + 1] * 3];
      v3 = &tmp_vert[tmp_ind[j + 2] * 3];

      compute_half_vertex(v1, v2, radius_, new_v1);
      compute_half_vertex(v2, v3, radius_, new_v2);
      compute_half_vertex(v1, v3, radius_, new_v3);

      add_vertices(v1, new_v1, new_v3);
      compute_face_normal(v1, new_v1, new_v3, normal);
      add_normals(normal, normal, normal);
      add_indices(index, index + 1, index + 2);

      add_vertices(new_v1, v2, new_v2);
      compute_face_normal(new_v1, v2, new_v2, normal);
      add_normals(normal, normal, normal);
      add_indices(index + 3, index + 4, index + 5);

      add_vertices(new_v1, new_v2, new_v3);
      compute_face_normal(new_v1, new_v2, new_v3, normal);
      add_normals(normal, normal, normal);
      add_indices(index + 6, index + 7, index + 8);

      add_vertices(new_v3, new_v2, v3);
      compute_face_normal(new_v3, new_v2, v3, normal);
      add_normals(normal, normal, normal);
      add_indices(index + 9, index + 10, index + 11);

      index += 12;
    }
  }
}

void astri::Object::build_interleaved() {
  interleaved.clear();

  std::size_t i, j;
  std::size_t count = vertices.size();
  glm::mat4 rot = glm::rotate(glm::mat4(1.0), (float)(M_PI/2.0), glm::vec3(1.0, 0.0, 0.0));
  for (i = 0, j = 0; i < count; i += 3, j += 2) {
    // glm::vec3 v = rot * glm::vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0);
    glm::vec3 v = glm::vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0);
    interleaved.push_back(v.x);
    interleaved.push_back(v.y);
    interleaved.push_back(v.z);
    interleaved.push_back(normals[i]);
    interleaved.push_back(normals[i + 1]);
    interleaved.push_back(normals[i + 2]);
  }
  if (VAO != 0) glDeleteBuffers(1, &VAO);
  if (VBO != 0) glDeleteBuffers(1, &VBO);
  if (EBO != 0) glDeleteBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float),
               interleaved.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void astri::Object::add_vertices(float x, float y, float z) {
  vertices.push_back(x);
  vertices.push_back(y);
  vertices.push_back(z);
}
void astri::Object::add_vertices(const float v1[3], const float v2[3],
                                 const float v3[3]) {
  vertices.push_back(v1[0]);  // x
  vertices.push_back(v1[1]);  // y
  vertices.push_back(v1[2]);  // z
  vertices.push_back(v2[0]);
  vertices.push_back(v2[1]);
  vertices.push_back(v2[2]);
  vertices.push_back(v3[0]);
  vertices.push_back(v3[1]);
  vertices.push_back(v3[2]);
}
void astri::Object::add_normals(float nx, float ny, float nz) {
  normals.push_back(nx);
  normals.push_back(ny);
  normals.push_back(nz);
}
void astri::Object::add_normals(const float n1[3], const float n2[3],
                                const float n3[3]) {
  normals.push_back(n1[0]);  // nx
  normals.push_back(n1[1]);  // ny
  normals.push_back(n1[2]);  // nz
  normals.push_back(n2[0]);
  normals.push_back(n2[1]);
  normals.push_back(n2[2]);
  normals.push_back(n3[0]);
  normals.push_back(n3[1]);
  normals.push_back(n3[2]);
}
void astri::Object::add_indices(unsigned i1, unsigned i2, unsigned i3) {
  indices.push_back(i1);
  indices.push_back(i2);
  indices.push_back(i3);
}

void astri::Object::compute_face_normal(const float v1[3], const float v2[3],
                                        const float v3[3], float n[3]) {
  const float ep = 0.000001f;

  n[0] = n[1] = n[2] = 0;

  float ex1 = v2[0] - v1[0];
  float ey1 = v2[1] - v1[1];
  float ez1 = v2[2] - v1[2];
  float ex2 = v3[0] - v1[0];
  float ey2 = v3[1] - v1[1];
  float ez2 = v3[2] - v1[2];

  float nx, ny, nz;
  nx = ey1 * ez2 - ez1 * ey2;
  ny = ez1 * ex2 - ex1 * ez2;
  nz = ex1 * ey2 - ey1 * ex2;

  float length = std::sqrt(nx * nx + ny * ny + nz * nz);
  if (length > ep) {
    float length_inv = 1.0 / length;
    n[0] = nx * length_inv;
    n[1] = ny * length_inv;
    n[2] = nz * length_inv;
  }
}

void astri::Object::compute_half_vertex(const float v1[3], const float v2[3],
                                        float length, float new_v[3]) {
  new_v[0] = v1[0] + v2[0];
  new_v[1] = v1[1] + v2[1];
  new_v[2] = v1[2] + v2[2];
  float scale = compute_scale_for_length(new_v, length);
  new_v[0] *= scale;
  new_v[1] *= scale;
  new_v[2] *= scale;
}

float astri::Object::compute_scale_for_length(const float v[3], float length) {
  return length / std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
