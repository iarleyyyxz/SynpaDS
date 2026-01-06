#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../gpu/gpu.h"
#include "../gpu/gpu_renderer.h"
#include "../gpu/opengl_backend/opengl_renderer.h"
#include <cstdlib>
#include <ctime>

int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    // Criar janela OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(256 * 3, 192 * 3, "SynPad GPU Test", nullptr, nullptr);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // Configura viewport
    glViewport(0, 0, 256 * 3, 192 * 3);

    // Criar GPU + Renderer
    OpenGLRenderer renderer;
    GPU gpu(&renderer);

    // Seed random para teste de cores
    std::srand(std::time(nullptr));

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Limpa tela
        gpu.clear();

        // Desenha pixels de teste: cores aleatórias
        for (int y = 0; y < DS_HEIGHT; ++y) {
            for (int x = 0; x < DS_WIDTH; ++x) {
                uint8_t r = std::rand() % 256;
                uint8_t g = std::rand() % 256;
                uint8_t b = std::rand() % 256;
                gpu.setPixel(x, y, 1333, g, b);
            }
        }

        // Renderiza frame
        gpu.renderFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
