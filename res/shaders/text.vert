#version 330
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform vec2 u_window_size;

out vec2 v_texcoord;

vec2 camera_project(vec2 pixel_coords) {
    return vec2((a_position.x / u_window_size.x) * 2.0 - 1.0,
                1.0 - (a_position.y / u_window_size.y) * 2.0);
}

void main() {
    gl_Position = vec4(camera_project(a_position), 0.0, 1.0);
    v_texcoord = a_texcoord;
}
