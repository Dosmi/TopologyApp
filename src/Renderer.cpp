#include "Renderer.h"
#include <iostream>


void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLFunctionLog(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error in " << file << ": " << function << "-ln" << line << "]\n" <<
            "(dec:" << error << ";hex:" << std::hex << error << ")" << std::endl;
        return false;
    }
    return true;
}

void Renderer::clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.bind();
    // bind vertex array:
    va.bind();
    // bind the index buffer:
    ib.bind();
    int num_drawing = ib.getCount();
    std::cout << "Drawing " << num_drawing << " vertices." << std::endl;

    GLCall(glPointSize(10));
    GLCall(glDrawElements(/* mode                        */ GL_POINTS,
                          /* number of indices to render */ ib.getCount(),
                          /* type                        */ GL_UNSIGNED_INT,
                          /* pointer to the index buffer */ nullptr));
}
