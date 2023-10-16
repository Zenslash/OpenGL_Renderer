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
in VS_OUT
{
    vec3 Normal;
    vec3 WorldPos;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} fs_in;

uniform DirLight _DirLight;
uniform Material _Material;
uniform vec3 _ViewPos;
uniform sampler2D shadowMap;

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

float PCF(vec3 projCoords, float bias, float currentDepth)
{
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    //Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //Tranform to [0, 1] range
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.005);
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = PCF(projCoords, bias, currentDepth);
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}


void main()
{
    vec3 albedo = vec3(texture(_Material.texture_diffuse1, fs_in.TexCoord));
    vec3 viewDir = normalize(_ViewPos - fs_in.WorldPos);
    vec3 normal = normalize(fs_in.Normal);
    
    //Directional Light
    vec3 result = CalcDirLight(_DirLight, normal, viewDir, albedo);
    vec3 ambient = _DirLight.ambient * albedo;

    //Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        //result += CalcPointLight(_PointLights[i], normal, fs_in.WorldPos, viewDir, albedo);
    }

    //Spot Light
    //result += CalcSpotLight(_SpotLight, normal, albedo, viewDir);

    //Shadow calculation
    vec3 lightDir = normalize(-_DirLight.direction);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    result = (ambient + (1.0 - shadow) * result);
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);

    //diffuse
    float diff = max(dot(normal, -lightDir), 0.0f);
    //Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), _Material.shiness);
    //Combine result
    //vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = (spec * vec3(texture(_Material.texture_specular1, fs_in.TexCoord))) * light.specular;
    //return (ambient + diffuse + specular);
    return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(fs_in.WorldPos - light.position);

    //diffuse
    float diff = max(dot(normal, -lightDir), 0.0f);
    //specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), _Material.shiness);
    //attenuation
    float distance = length(fs_in.WorldPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                                    light.quadratic * (distance * distance));
    //combine results
    vec3 diffuse = light.diffuse * diff * albedo * attenuation * light.intensity;
    vec3 specular = (spec * vec3(texture(_Material.texture_specular1, fs_in.TexCoord))) * light.specular * attenuation * light.intensity;
    //return (ambient + diffuse + specular);
    return (diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 albedo, vec3 viewDir)
{
    vec3 lightDir = normalize(fs_in.WorldPos - light.position);
    float theta = dot(light.direction, normalize(lightDir));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * albedo;

    if(theta > light.outerCutOff)
    {
        //diffuse
        float diff = max(dot(normal, -lightDir), 0.0f);
        //specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), _Material.shiness);

        vec3 diffuse = light.diffuse * diff * albedo * intensity;
        vec3 specular = (spec * vec3(texture(_Material.texture_specular1, fs_in.TexCoord))) * light.specular * intensity;

        return (ambient + diffuse + specular);
    }
    else
    {
        return ambient;
    }
}
