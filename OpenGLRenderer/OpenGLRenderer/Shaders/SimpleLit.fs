#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiness;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;

uniform sampler2D albedo;
uniform Light _Light;
uniform Material _Material;
uniform vec3 _ViewPos;

void main()
{
    vec3 ambient = _Light.ambient * _Material.ambient;

    //diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(_Light.position - WorldPos);

    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = _Light.diffuse * (diff * _Material.diffuse);

    //Specular
    vec3 viewDir = normalize(_ViewPos - WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shiness);
    vec3 specular = (spec * _Material.specular) * _Light.specular;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}
