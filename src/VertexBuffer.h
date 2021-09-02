#pragma once

class VertexBuffer 
{
private:
	unsigned m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;
};
