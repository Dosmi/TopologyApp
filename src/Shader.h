#pragma once
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_Filepath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	// caching for uniforms:
public:
	Shader(const std::string& filename);
	~Shader();

	void bind() const;
	void unbind() const;

	// set uniforms:
	void setUniform1f(const std::string& name, float v0);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniformMat4f(const std::string& name, const glm::mat4& mat4x4);

private:
	ShaderProgramSource parseShader(const std::string& filepath);
	
	unsigned int compileShader(unsigned int type,
							   const std::string& source);

	unsigned int createShader(const std::string& vertexShader,
							  const std::string& fragmentShader);

	unsigned int getUniformLocation(const std::string& name);

};