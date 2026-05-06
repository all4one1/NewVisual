#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


std::string _pathGL(std::string fname)
{
#ifdef __linux__ 
    return fname + "/";
#endif

#ifdef _WIN32
    return fname + "//";
#endif
}
std::string _strGLmask(size_t number, int width) {
    std::stringstream ss;
    ss << std::setw(width) << std::setfill('0') << number;
    return ss.str();
}


void saveFrame(std::string filename = "image", int width = 0, int height = 0)
{
    filename += ".png";
    //if (width == 0 || height == 0) glfwGetFramebufferSize(window, &width, &height); causes crash

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (width == 0)  width = viewport[2];  // Ширина
    if (height == 0) height = viewport[3]; // Высота

    width = width - (width % 4);
    height = height - (height % 4);

    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // OpenGL сохраняет пиксели снизу вверх, нужно перевернуть
    std::vector<unsigned char> flippedPixels(width * height * 3);
    for (int y = 0; y < height; ++y) {
        memcpy(
            &flippedPixels[y * width * 3],
            &pixels[(height - 1 - y) * width * 3],
            width * 3
        );
    }

    stbi_write_png(filename.c_str(), width, height, 3, flippedPixels.data(), width * 3);
}