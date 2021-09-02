#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count)
{ 
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    unsigned int buffer_id;
    GLCall(glGenBuffers(/* how many */ 1,
                        /* ID       */ &m_RendererID));
    // bind (select) it:
    GLCall(glBindBuffer(/* type     */ GL_ELEMENT_ARRAY_BUFFER,
                        /* ID       */ m_RendererID));
    // put data to the buffer:
    GLCall(glBufferData(/* type          */ GL_ELEMENT_ARRAY_BUFFER,
                        /* size in bytes */ count * sizeof(unsigned int) ,
                        /* data          */ data,
                        /* usage-pattern */ GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID))
}

void IndexBuffer::bind() const 
{
    GLCall(glBindBuffer(/* type     */ GL_ELEMENT_ARRAY_BUFFER,
                        /* ID       */ m_RendererID));
}

void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(/* type     */ GL_ELEMENT_ARRAY_BUFFER,
                        /* ID       */ 0));
}