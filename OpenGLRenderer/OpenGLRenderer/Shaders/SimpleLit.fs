#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;

uniform sampler2D albedo;
uniform vec3 _LightPos;
uniform vec3 _LightColor;
uniform vec3 _ObjectColor;
uniform vec3 _ViewPos;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = _LightColor * ambientStrength;

    //diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(_LightPos - WorldPos);

    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = _LightColor * diff;

    //Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(_ViewPos - WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * _LightColor;

    vec3 result = (ambient + diffuse + specular) * _ObjectColor;
    FragColor = vec4(result, 1.0f);
}
