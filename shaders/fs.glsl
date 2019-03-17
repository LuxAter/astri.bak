#version 450 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uColor;

void main(){
  float amb_int = 0.2;
  vec3 ambient = amb_int * uLightColor;

  vec3 norm = normalize(Normal);
  vec3 light_dir = normalize(uLightPos - FragPos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffuse = diff * uLightColor;

  float spec_int = 0.5;
  vec3 view_dir = normalize(uViewPos - FragPos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = spec_int * spec * uLightColor;

  vec3 result = (ambient + diffuse + specular) * uColor;
  FragColor = vec4(result, 1.0);
}
