#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "wp_version.h"

static void on_init(void);
static void on_quit(void);
static void on_render(void);
static void error_callback(int error_code, const char *description);
static void refresh_callback(GLFWwindow *window);
static void resize_callback(GLFWwindow *window, int new_w, int new_h);

#ifndef NDEBUG
#define WINDOW_TITLE ("Wraithpad " WP_VERSION " (debug)")
#else
#define WINDOW_TITLE ("Wraithpad " WP_VERSION)
#endif

#define INIT_WINDOW_W (800)
#define INIT_WINDOW_H (450)

struct {
    size_t window_w;
    size_t window_h;
    GLFWwindow *window;
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

    glfwShowWindow(state.window);
}

void on_quit(void) {
    glfwDestroyWindow(state.window);
    glfwTerminate();
}

void on_render(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1.0);

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
