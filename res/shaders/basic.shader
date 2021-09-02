#shader vertex
#version 330 core

layout(location = 0) in vec4 in_position;
uniform mat4 u_modelviewprojection; // MVP

void main()
{
	gl_Position = u_modelviewprojection * in_position;
};




#shader fragment
#version 330 core

layout(location = 0) out vec4 out_color;
uniform vec4 u_color;

void main()
{
	out_color = u_color; //vec4(1.0, 0.5, 0.2, 1.0);
};
