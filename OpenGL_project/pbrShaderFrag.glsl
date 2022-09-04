#version 430 core
layout(early_fragment_tests) in;
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;

// параметры материалов

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D AOMap;
uniform vec3 camPos;


vec3 albedo;
float metallic;
float roughness;
float ao;
vec3 Normal;

float test = 0.1;

// источники света
vec3 lightPositions[2] = {camPos, WorldPos+vec3(1.)};
vec3 lightColors[2] = {vec3(0.,1,0.), vec3(1.,0.,0.0)};
const float PI = 3.14159265359;
  
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), test);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), test);
    float NdotL = max(dot(N, L), test);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    albedo = texture(albedoMap, TexCoords).rgb;
    metallic = 0;//texture(metallicMap, TexCoords).r;
    roughness = texture(roughnessMap, TexCoords).r;
    ao = texture(AOMap, TexCoords).r;
    Normal = (texture(normalMap, TexCoords).xyz-vec3(0.5))*2.*TBN;
    //lightPosition = camPos + vec3(0, 0, 0);


    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    // выражение отражающей способности
    vec3 Lo = vec3(0.0);
    //расчет энергетической
    for(int i=0; i<2; i++)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float dist    = max(pow((length(lightPositions[i] - WorldPos)), 0.2f), 0.1f);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance     = lightColors[i] * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness); 
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), test), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), test) * max(dot(N, L), test);
        vec3 specular     = numerator / max(denominator, 0.001);
        float NdotL = max(dot(N, L), 0.8);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }
    //ao = 0.0;
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo/2;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
    //color = vec3(texture(normalMap, TexCoords).xyz);
    FragColor = vec4(color, 1.0);
}   