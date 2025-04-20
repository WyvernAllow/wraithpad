#version 330
layout(location = 0) out vec4 v_frag;

in vec2 v_texcoord;

void main() {
    v_frag = vec4(1.0, 0.5, 0.5, 1.0);
}