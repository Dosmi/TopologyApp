#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
    
#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	: m_Filepath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = parseShader(filepath);
    m_RendererID = createShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::setUniform1f(const std::string& name, float v0)
{
    GLCall(glUniform1f(getUniformLocation(name), v0));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& mat4x4)
{
    GLCall(glUniformMatrix4fv(getUniformLocation(name), 
                              /* count of matrices     */ 1, 
                              /* transpose?            */ GL_FALSE,
                              /* pointer to first lmnt */ &mat4x4[0][0]));
}






ShaderProgramSource Shader::parseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    // go through the file line by line to see ...
    // ... where shaders are split up
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };
    ShaderType type = ShaderType::NONE;
    std::string line;
    std::stringstream ss[2];

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            // has found '#shader' in the current line
            // ... find which type of shader it is:
            if (line.find("vertex") != std::string::npos)
            {
                // set type to 'vertex'
                type = ShaderType::VERTEX;

            }
            else if (line.find("fragment") != std::string::npos)
            {
                // set mode to 'fragment'
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    // return vertex and fragment shaders as strings:
    return { ss[0].str(), ss[1].str() };
}


unsigned int Shader::compileShader(unsigned int type,
                                   const std::string& source)
{
    unsigned int shaderID = glCreateShader(type);
    const char* src = source.c_str();

    // Set the source code in 'shader' ...
    // ... to the source code in the array of strings.
    // The number of strings in the array is specified in 'count', ...
    // ... while the 'length' = NULL assumes the entire string ...
    // ... is the source.
    glShaderSource(/* shader ID          */ shaderID,
        /* count of src codes */ 1,
        /* ptr to ptr of src  */ &src,
        /* src length         */ NULL);
    glCompileShader(shaderID);

    // Query the result of the compilation:
    int compilation_result;
    // iv - types - i = int, v = vector (array/pointer)
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilation_result);
    if (compilation_result == GL_FALSE)
    {
        // Shader did not compile successfully
        int length;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        char* error_message = (char*)alloca(length * sizeof(char));

        glGetShaderInfoLog(shaderID, length, &length, error_message);

        std::cout << "ERROR: Failed to compile" <<
            (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "Shader:"
            << std::endl;
        std::cout << error_message << std::endl;

        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

unsigned int Shader::getUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "WARNING: uniform '" << name << "' does not exist." << std::endl;
    }
    // caching the location
    m_UniformLocationCache[name] = location;
    return location;
}

/* Provide OpenGL with our Shader Sourcecode,
   ... and OpenGL to compile it and link it, ...
   ... giving an ID back so that we can bind it. */
unsigned int Shader::createShader(const std::string& vertexShader,
                                  const std::string& fragmentShader)
{
    unsigned int programID = glCreateProgram();

    unsigned int vsID = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fsID = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachObjectARB(programID, vsID);
    glAttachObjectARB(programID, fsID);

    glLinkProgram(programID);
    glValidateProgram(programID);

    // Once the shaders have been linked to the program, ...
    // ... it is safe to delete their intermediates:
    glDeleteShader(vsID);
    glDeleteShader(fsID);

    return programID;
}