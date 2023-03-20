#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shiness;
};

struct Light
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform Light _Light;
uniform Material _Material;
uniform vec3 _ViewPos;

void main()
{
    vec3 albedo = vec3(texture(_Material.diffuse, TexCoord));
    vec3 ambient = _Light.ambient * _Material.ambient * albedo;

    //diffuse lighting
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(-_Light.direction);
    vec3 lightDir = normalize(WorldPos - _Light.position);
    float distance = length(WorldPos - _Light.position);
    float attenuation = 1.0 / (_Light.constant + _Light.linear * distance +
                                _Light.quadratic * (distance * distance));

    float diff = max(dot(normal, -lightDir), 0.0f);
    vec3 diffuse = _Light.diffuse * (diff * albedo);

    //Specular
    vec3 viewDir = normalize(_ViewPos - WorldPos);
    vec3 reflectDir = reflect(lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shiness);
    vec3 specular = (spec * vec3(texture(_Material.specular, TexCoord))) * _Light.specular;

    vec3 result = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(result, 1.0f);
}
