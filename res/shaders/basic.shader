#shader vertex
#version 330 core

layout(location = 0) in vec4 in_position;
//uniform mat4 u_modelviewprojection; // MVP
uniform mat4 u_model;		// M
uniform mat4 u_view;		// V
uniform mat4 u_projection;	// P


void main()
{

	//vec4 eye_position = u_modelview * vec4(in_position);
	/*gl_Position = u_projection * eye_position;*/
	gl_Position = u_projection * u_view * u_model * vec4(in_position);
	//gl_Position = MVP * vec4(in_position);
	//gl_Position = u_projection * u_modelview * in_position;

	gl_PointSize = 10.0;

};




















#shader fragment
#version 330 core

layout(location = 0) out vec4 out_color;
uniform vec4 u_color;

void main()
{
	//out_color = vec4(1.0, 0.5, 0.2, 1.0);
	out_color = u_color; 
};
