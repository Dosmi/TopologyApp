#shader vertex
#version 330 core

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_normal;

out vec3 vertout_normal;
out vec3 vertout_fragment_position;

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

    vertout_fragment_position = vec3(u_model * in_position);
    vertout_normal = mat3(transpose(inverse(u_model))) * vec3(in_normal.x, in_normal.y, in_normal.z);
    //vertout_normal = vec3(in_normal.x, in_normal.y, in_normal.z);

};




















#shader fragment
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vertout_normal;
in vec3 vertout_fragment_position;

uniform vec3 u_light_position;
uniform vec3 u_camera_position;

uniform vec4 u_color;

float diffuse_clamp(vec3 vector1, vec3 vector2)
{
    float dot = dot(vector1, vector2);
    if (dot > 0.0)
    {
        return dot;
    }
    else
    {
        return 0.0;
    }
}

vec3 add_local_illumination(vec3 diffuse_col, float opacity, int shine,
                            vec3 normal, vec3 light_dir, vec3 camera_dir,
                            float specular_on, float diffuse_on, float ambient_on)
{
    vec3 sum = vec3(0.0, 0.0, 0.0);
    vec3 ambient = vec3(0.0, 0.0, 0.0);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    vec3 light_colour = vec3(1.0, 1.0, 1.0);
    vec3 ambient_colour = vec3(0.5, 0.5, 0.5);
    vec3 specularColor = vec3(1.0);

    float directionality = 0.f;

    // AMBIENT LIGHT:
    // (constant colour)
    ambient += light_colour * ambient_colour;

    // DIFFUSE LIGHT:
    int num_lights = 1;

    for (int i = 0; i < num_lights; i++)
    {
        directionality = diffuse_clamp(light_dir, normal);
        diffuse += directionality * light_colour;
    }

    // SPECULAR LIGHT:
    vec3 direction_to_reflected_ray = 2 * dot(light_dir, normal) * normal - light_dir;

    vec3 h_vec = camera_dir + 1 / length(camera_dir + 1);
    float dot_prod = 0.0;
    for (int i = 0; i < shine; i++)
    {
        dot_prod += dot(direction_to_reflected_ray, camera_dir);
    }
    // specular lighting - if camera direction and reflected ray direction match, ...
    // ... exponentiate by power of 'shine', else 0.0:
    float spec = pow(max(dot(camera_dir, direction_to_reflected_ray), 0.0), shine);
    // (multiply by 10.f to make the specular highlights more visible)
    specular = 10.f * spec * light_colour;
    // it turned out to be faster to not have branches when computing the lighting ...
    // ... therefore each term is computed anyway and multiplied by '_on':
    return vec3(ambient_on * ambient + diffuse_on * diffuse + specular_on * specular) * diffuse_col;
}

void main()
{
	//out_color = vec4(1.0, 0.5, 0.2, 1.0);
	out_color = u_color; 
	//out_color = vec4(vertout_normal, 1.f);
    //out_color = vec4(vertout_fragment_position, 1.f);

    u_light_position;
    u_camera_position;


    // Normal vector:
    //grad = texture(gradient, sample_index).rgba;
    vec3 N = normalize(vertout_normal.rgb);// / grad.a;
    // Light direction (from current ray position)
    vec3 L = normalize(u_light_position - vertout_fragment_position);
    // Camera direction (from current ray position)
    vec3 C = normalize(u_camera_position - vertout_fragment_position);
    // add illumination to the colour:
    vec3 illuminated_color = add_local_illumination(/* diffuse color */ u_color.rgb, //vertout_normal, //u_color.rgb,
                                                    /* opacity       */ 1.f,
                                                    /* shine         */ 1000,
                                                    /* normal, light direction, camera direction */ N, L, C,
                                                    /* turn individual lights on                 */ 1.f, 1.f, 1.f);

    out_color.r = illuminated_color.r;
    out_color.g = illuminated_color.g;
    out_color.b = illuminated_color.b;
    out_color.a = 1.f;
                    
    //out_color = vec4(vertout_normal.xyz, 1.f);

};
