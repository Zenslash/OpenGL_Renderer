#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float shiness;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    float intensity;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float intensity;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform PointLight _PointLights[NR_POINT_LIGHTS];

uniform SpotLight _SpotLight;

out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform DirLight _DirLight;
uniform Material _Material;
uniform vec3 _ViewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 albedo, vec3 viewDir);

float near = 0.1; 
float far  = 100.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec3 albedo = vec3(texture(_Material.texture_diffuse1, TexCoord));
    vec3 viewDir = normalize(_ViewPos - WorldPos);
    vec3 normal = normalize(Normal);
    
    //Directional Light
    vec3 result = CalcDirLight(_DirLight, normal, viewDir, albedo);

    //Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        //result += CalcPointLight(_PointLights[i], normal, WorldPos, viewDir, albedo);
    }

    //Spot Light
    result += CalcSpotLight(_SpotLight, normal, albedo, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);

    //diffuse
    float diff = max(dot(normal, -lightDir), 0.0f);
    //Specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shiness);
    //Combine result
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = (spec * vec3(texture(_Material.texture_specular1, TexCoord))) * light.specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(WorldPos - light.position);

    //diffuse
    float diff = max(dot(normal, -lightDir), 0.0f);
    //specular
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shiness);
    //attenuation
    float distance = length(WorldPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                                    light.quadratic * (distance * distance));
    //combine results
    vec3 ambient = light.ambient * albedo * attenuation;
    vec3 diffuse = light.diffuse * diff * albedo * attenuation * light.intensity;
    vec3 specular = (spec * vec3(texture(_Material.texture_specular1, TexCoord))) * light.specular * attenuation * light.intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 albedo, vec3 viewDir)
{
    vec3 lightDir = normalize(WorldPos - light.position);
    float theta = dot(light.direction, normalize(lightDir));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * albedo;

    if(theta > light.outerCutOff)
    {
        //diffuse
        float diff = max(dot(normal, -lightDir), 0.0f);
        //specular
        vec3 reflectDir = reflect(lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shiness);

        vec3 diffuse = light.diffuse * diff * albedo * intensity;
        vec3 specular = (spec * vec3(texture(_Material.texture_specular1, TexCoord))) * light.specular * intensity;

        return (ambient + diffuse + specular);
    }
    else
    {
        return ambient;
    }
}
