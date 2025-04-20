#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "utils.h"
#include "wp_version.h"

#ifndef NDEBUG
#define WINDOW_TITLE ("Wraithpad " WP_VERSION " (debug)")
#else
#define WINDOW_TITLE ("Wraithpad " WP_VERSION)
#endif

#define INIT_WINDOW_W (800)
#define INIT_WINDOW_H (450)

static GLuint compile_shader(const char *source, GLenum type);
static GLuint compile_program(GLuint vert, GLuint frag);
static GLuint compile_program_from_files(const char *vert_filename,
                                         const char *frag_filename);
static void create_vertex_buffer(GLuint *vao, GLuint *vbo, GLuint *ebo);
static void on_init(void);
static void on_quit(void);
static void on_render(void);
static void error_callback(int error_code, const char *description);
static void refresh_callback(GLFWwindow *window);
static void resize_callback(GLFWwindow *window, int new_w, int new_h);

GLuint compile_shader(const char *source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint did_compile;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &did_compile);
    if (!did_compile) {
        char info_log[256];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint compile_program(GLuint vert, GLuint frag) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    GLint did_link;
    glGetProgramiv(program, GL_LINK_STATUS, &did_link);
    if (!did_link) {
        char info_log[256];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Shader linking failed: %s\n", info_log);

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint compile_program_from_files(const char *vert_filename,
                                  const char *frag_filename) {
    const char *vert_source = slurp_file_str(vert_filename);
    if (!vert_source) {
        fprintf(stderr, "Failed to load shader source: %s\n", vert_filename);
        return 0;
    }

    const char *frag_source = slurp_file_str(frag_filename);
    if (!frag_source) {
        fprintf(stderr, "Failed to load shader source: %s\n", frag_filename);
        return 0;
    }

    GLuint vert = compile_shader(vert_source, GL_VERTEX_SHADER);
    if (!vert) {
        fprintf(stderr, "Failed to compile shader: %s\n", vert_filename);
        return 0;
    }

    GLuint frag = compile_shader(frag_source, GL_FRAGMENT_SHADER);
    if (!frag) {
        fprintf(stderr, "Failed to compile shader: %s\n", frag_filename);
        return 0;
    }

    free((void *)vert_source);
    free((void *)frag_source);
    return compile_program(vert, frag);
}

struct vertex {
    float position[2];
    float texcoord[2];
};

#define MAX_QUADS (512)
#define MAX_VERTS (MAX_QUADS * 4)
#define MAX_INDICES (MAX_QUADS * 6)

void create_vertex_buffer(GLuint *vao, GLuint *vbo, GLuint *ebo) {
    float x = 32.0f;
    float y = 32.0f;
    float w = 16.0f;
    float h = 16.0f;

    /* clang-format off */
    struct vertex vertices[4] = {
        {{x + w, y + h}, {1.0f, 1.0f}},
        {{x + w, y    }, {1.0f, 0.0f}},
        {{x,     y    }, {0.0f, 0.0f}},
        {{x,     y + h}, {0.0f, 1.0f}},
    };
    /* clang-format on */

    uint32_t *indices = malloc(sizeof(uint32_t) * MAX_INDICES);
    for (size_t i = 0; i < MAX_QUADS; i++) {
        indices[i * 6 + 0] = 0 + i * 4;
        indices[i * 6 + 1] = 1 + i * 4;
        indices[i * 6 + 2] = 3 + i * 4;
        indices[i * 6 + 3] = 1 + i * 4;
        indices[i * 6 + 4] = 2 + i * 4;
        indices[i * 6 + 5] = 3 + i * 4;
    }

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct vertex) * 4, vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * MAX_INDICES,
                 indices, GL_STATIC_DRAW);

    /* Position attribute */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
                          (const void *)(offsetof(struct vertex, position)));

    /* Texcoord attribute */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
                          (const void *)(offsetof(struct vertex, texcoord)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(indices);
}

struct {
    size_t window_w;
    size_t window_h;
    GLFWwindow *window;

    GLuint program;
    GLuint u_window_size;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} state;

void on_init(void) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "glfwInit failed\n");
        exit(EXIT_FAILURE);
    }

    state.window_w = INIT_WINDOW_W;
    state.window_h = INIT_WINDOW_H;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    state.window = glfwCreateWindow(state.window_w, state.window_h,
                                    WINDOW_TITLE, NULL, NULL);
    if (!state.window) {
        fprintf(stderr, "glfwCreateWindow failed\n");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(state.window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "gladLoadGL failed\n");
        exit(EXIT_FAILURE);
    }

    glfwSetWindowRefreshCallback(state.window, refresh_callback);
    glfwSetWindowSizeCallback(state.window, resize_callback);

    state.program = compile_program_from_files("res/shaders/text.vert",
                                               "res/shaders/text.frag");
    if (!state.program) {
        fprintf(stderr, "compile_program_from_files failed\n");
        exit(EXIT_FAILURE);
    }

    state.u_window_size = glGetUniformLocation(state.program, "u_window_size");

    create_vertex_buffer(&state.vao, &state.vbo, &state.ebo);

    glfwShowWindow(state.window);
}

void on_quit(void) {
    glfwDestroyWindow(state.window);
    glfwTerminate();
}

void on_render(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1.0);

    glUseProgram(state.program);
    glUniform2f(state.u_window_size, state.window_w, state.window_h);

    glBindVertexArray(state.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glUseProgram(0);

    glfwSwapBuffers(state.window);
}

void error_callback(int error_code, const char *description) {
    (void)error_code;
    fprintf(stderr, "%s\n", description);
}

void refresh_callback(GLFWwindow *window) {
    assert(window == state.window);
    on_render();
}

void resize_callback(GLFWwindow *window, int new_w, int new_h) {
    assert(window == state.window);
    state.window_w = new_w;
    state.window_h = new_h;
    glViewport(0, 0, state.window_w, state.window_h);
}

int main(void) {
    on_init();

    while (!glfwWindowShouldClose(state.window)) {
        glfwWaitEvents();
        on_render();
    }

    on_quit();
    return EXIT_SUCCESS;
}
