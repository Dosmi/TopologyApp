#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{ 
    unsigned int buffer_id;
    GLCall(glGenBuffers(/* how many */ 1,
                        /* ID       */ &m_RendererID));
    // bind (select) it:
    GLCall(glBindBuffer(/* type     */ GL_ARRAY_BUFFER,
                        /* ID       */ m_RendererID));
    // put data to the buffer:
    GLCall(glBufferData(/* type          */ GL_ARRAY_BUFFER,
                        /* size in bytes */ size,
                        /* data          */ data,
                        /* usage-pattern */ GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID))
}

void VertexBuffer::bind() const
{
    GLCall(glBindBuffer(/* type     */ GL_ARRAY_BUFFER,
                        /* ID       */ m_RendererID));
}

void VertexBuffer::unbind() const
{
    GLCall(glBindBuffer(/* type     */ GL_ARRAY_BUFFER,
                        /* ID       */ 0));
}