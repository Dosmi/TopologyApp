#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"


VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}
	
void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    bind();     // bind the vertex array
	vb.bind();  // bind the buffer that we want to deal with
    const auto& elements = layout.getElements();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        // enable the vertex attribute:
        //glEnableVertexArrayAttrib()
        GLCall(glEnableVertexAttribArray(i));

        // tell the layout of our buffer (after binding):
        // (here we link buffer with the VAO - say: index 0 of the vertex array ...
        //  ... going to be bound to currently bound array buffer)
        GLCall(glVertexAttribPointer(// this is the first (0th) atribute - location = 0:
            /* index            */ i, 
            // position - x, y - 2 components (vec2):
            /* count            */ element.count,
            /* type             */ element.type,
            /* normalised?      */ element.normalized,
            // amount of bytes between each VERTEX:
            /* stride           */ layout.getStride(),
            // bytes offset to the next ATTRIBUTE:
            /* pointer (offset) */ (const void*)offset));

        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }


}

void VertexArray::bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}
void VertexArray::unbind() const
{
    GLCall(glBindVertexArray(0));
}



