#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

static void on_render(GLFWwindow *window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1.0);

    glfwSwapBuffers(window);
}

static void glfw_error_callback(int error_code, const char *description) {
    (void)error_code;
    fprintf(stderr, "%s\n", description);
}

static void glfw_refresh_callback(GLFWwindow *window) {
    on_render(window);
}

int main(void) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "glfwInit failed\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(800, 450, "Wraithpad", NULL, NULL);
    if (!window) {
        fprintf(stderr, "glfwInit failed\n");
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "gladLoadGL failed\n");
        return EXIT_FAILURE;
    }

    glfwSetWindowRefreshCallback(window, glfw_refresh_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
        on_render(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
