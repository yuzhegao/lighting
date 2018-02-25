#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
vec3 lightPos=vec3(1.0f, 2.0f, 0.5f); 
vec3 lightPos2=vec3(-1.0f, -2.0f, -0.5f); 
vec3 lightColor=vec3(1.0f, 1.0f, 1.0f);
vec3 objectColor=vec3(0.0f, 1.0f, 1.0f);


void main()
{
    //FragColor = vec4(0.0f, 0.2f, 1.0f, 1.0f);
    // ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // diffuse 
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    diffuse += diff2 * lightColor;
            
    vec3 result = (ambient + diffuse*0.5) * objectColor;
    FragColor = vec4(result, 1.0);
}