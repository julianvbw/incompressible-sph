#version 330 core
uniform vec4 color;
uniform vec3 cam;
uniform mat4 proj;
uniform mat4 view;

out vec4 pixColor;

in vec3 vpos;

void main() {
    float r2 = length(vpos - vec3(0.5, 0.5, 0));
    if (r2 < 0.25)
        pixColor = vec4((1-r2)*color.xyz, color.w);
    else discard;
}
