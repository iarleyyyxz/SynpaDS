#pragma once
#include "../gpu_renderer.h"
#include <glad/glad.h>

struct OpenGLRenderer : GPURenderer {
    GLuint tex = 0;
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLuint shaderProgram = 0;

    OpenGLRenderer();
    ~OpenGLRenderer();

    void renderFrame(const uint8_t* vram) override;
    void clear() override;

private:
    void setupTexture();
    void setupQuad();
    void setupShader();
    GLuint compileShader(GLenum type, const char* src);
};
