#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec3 FragPos;
out vec3 Normal;


out vec4 Color;

void main()
{
    gl_Position = perspective * view * model * vec4(aPos, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal; 
}