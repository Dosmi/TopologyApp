#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLFunctionLog(#x, __FILE__, __LINE__))

void GLClearError();
bool GLFunctionLog(const char* function, const char* file, int line);

class Renderer
{
public:
    Renderer();
    void clear() const;
    void setDrawingMode(int mode);
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

private:
    int m_drawing_mode; // GL_POLYGON, GL_POINTS, GL_TRIANGLEs ... 
};

