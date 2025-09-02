#version 330 core
layout (location = 0) in vec3 pos;

out vec3 vpos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    vpos = pos;
    gl_Position = proj * view * model * vec4(pos, 1.0);
}
