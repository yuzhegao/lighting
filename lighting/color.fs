#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
vec3 lightPos=vec3(2.0f, 2.0f, 2.0f); 
vec3 lightColor=vec3(1.0f, 1.0f, 1.0f);
vec3 objectColor=vec3(0.0f, 1.0f, 1.0f);


void main()
{
    
    FragColor = vec4(1.0f-2*FragPos, 1.0);
}