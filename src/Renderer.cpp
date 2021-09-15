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

Renderer::Renderer()
    :m_drawing_mode(GL_TRIANGLES)
{
}

void Renderer::clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::setDrawingMode(int mode)
{
    m_drawing_mode = mode;
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.bind();
    // bind vertex array:
    va.bind();
    // bind the index buffer:
    ib.bind();
    int num_drawing = ib.getCount();
    //std::cout << "Drawing " << num_drawing << " vertices." << std::endl;


    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glEnable(GL_DEPTH_TEST));

    GLCall(glPointSize(10));
    //GLCall(glDisable(GL_CULL_FACE));
    GLCall(glShadeModel(GL_FLAT));

    GLCall(glDrawElements(/* mode                        */ m_drawing_mode,//GL_TRIANGLES,
                          /* number of indices to render */ ib.getCount(),
                          /* type                        */ GL_UNSIGNED_INT,
                          /* pointer to the index buffer */ nullptr));
}
