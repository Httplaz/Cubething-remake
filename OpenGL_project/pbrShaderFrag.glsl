#version 430 core
layout(early_fragment_tests) in;
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;
in flat int side1;
in flat int texID;
in float blockAO;

vec3 tangent = TBN[0], bitangent = TBN[1], normal = TBN[2];

// параметры материалов

//uniform sampler2D albedoMap;
//uniform sampler2D normalMap;
//uniform sampler2D metallicMap;
//uniform sampler2D roughnessMap;
//uniform sampler2D AOMap;
//uniform sampler2D heightMap;
uniform sampler2D albedoMetallicMap;
uniform sampler2D normalRoughnessMap;
uniform sampler2D heightAmbientOcclusionMap;
uniform vec3 camPos;
uniform float tileAtlasSize;

uniform bool p;


vec3 albedo;
float metallic;
float roughness;
float ao;
vec3 Normal;

float test = 0.1;


float viewDistance = distance(camPos, WorldPos);
float ndotv;

const float parallaxPower = .04;

// источники света
vec3 lightPositions[3] = {camPos, WorldPos+normalize(vec3(5., 1., 1.)), WorldPos+normalize(vec3(-4., 2., -2.)) };
vec3 lightColors[3] = {vec3(0.,.0,0.), vec3(1.,1.,0.9), vec3 (1., 1., 0.9)};
const float PI = 3.14159265359;
  


//vec4 ntexture(sampler2D sampler, vec2 uv)
//{
//}

float parallaxShadow(vec2 uv, vec3 dir, vec3 normal)
{
    float numLayers = 8;  
    int rep = side1%3;
    vec3 ray = rep==0? dir.zyx : rep==1?dir.xzy : dir.xyz;
    //ray.xy/abs(ray.z);
    //if(side1>2)
        //ray*=-1;
    vec2 startingUV = uv;
    //float currentHeight = texture(heightMap, uv).r;
    float currentHeight = texture(heightAmbientOcclusionMap, uv).r;
    float currentLayerHeight = currentHeight;
    float deltaLayerHeight = (1.-currentLayerHeight)/numLayers;
    ray*=(1.-currentHeight)/abs(ray.z);
    float c = 0.;
    for (int i=0; i<numLayers; i++)
    {
        if(currentLayerHeight<currentHeight)
            c = max(c, (currentHeight-currentLayerHeight) * (1. - (i+1.)/numLayers)*4.4);
        uv+=ray.xy;
        currentLayerHeight+=deltaLayerHeight;
        //currentHeight = texture(heightMap, uv).r;
        currentHeight = texture(heightAmbientOcclusionMap, uv).r;

    }
    return 1.-c;
}

vec2 parallax(vec2 uv, vec3 dir, vec3 normal)
{
    const float minLayers = 4.0;
    const float maxLayers = 34.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(normal, dir)));   
    int rep = side1%3;
    vec3 ray = rep==0? dir.zyx : rep==1?dir.xzy : dir.xyz;
    //ray.xy/abs(ray.z);
    //if(side1>2)
        //ray*=-1;
    float startingDepth = 0.;
    vec2 startingUV = uv;
    float currentDepth = pow((1.-texture(heightAmbientOcclusionMap, uv/tileAtlasSize).r), 5.);
    float currentLayerDepth = startingDepth;
    float deltaLayerDepth = (1.-currentLayerDepth)/numLayers;
    ray*=abs(((1.-currentLayerDepth)/ray.z)/numLayers*parallaxPower);
    //ray.xy/abs(ray.z);
    float prevDepth = currentDepth;
    vec2 prevUV = uv;
    while(currentLayerDepth < currentDepth)
    //for(int i=0; i<20; i++)
    {
        prevDepth = currentDepth;
        prevUV = uv;

        //if(distance(ivec2(uv), ivec2(uv+ray.xy))==0)
            uv+=ray.xy;
        //else
        {
            //uv = startingUV;
            //break;
        }
        currentLayerDepth+=deltaLayerDepth;
        currentDepth = pow((1.-texture(heightAmbientOcclusionMap, uv/tileAtlasSize).r), 5.);
    }

    float frontDepth = abs(currentLayerDepth - currentDepth);
    float backDepth = abs(currentLayerDepth-deltaLayerDepth - prevDepth);

    float weight = frontDepth/(frontDepth+backDepth);


    //uv = mix(clamp(uv, 0., 1.), clamp(prevUV, 0., 1.), weight);
    uv = mix(uv, prevUV, weight);
    return uv;
}


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

vec4 ntexture(sampler2D tex, vec2 uv)
{
    //return textureLod(tex, uv, viewDistance);
    //float l = log2(viewDistance);
    return textureLod(tex, uv, (log2(viewDistance)+ndotv*5.));
}


float lodAngleCoeff(vec3 wc, vec3 n)
{

    return 1.-abs(dot(wc, n));


}


void main()
{	
    vec3 MV = camPos - WorldPos;
    viewDistance = length(MV);
    vec3 V = normalize(MV);
    ndotv = lodAngleCoeff(V, TBN[2]);
    vec2 atlasOffset = vec2(texID%int(tileAtlasSize), texID/int(tileAtlasSize));
    vec2 texCoords = fract(TexCoords)+atlasOffset;//vec2(fract(TexCoords.x), TexCoords.y);
    //texCoords = (length(MV)<4.) ? fract(parallax((texCoords), -V, TBN[2]))+atlasOffset : texCoords;
    //texCoords = fract((parallax(texCoords, -V, TBN[2])));
    //while(texCoords.x>1.)
        //texCoords.y = clamp(texCoords.y, 0., 1.);
    //while(texCoords.y>1.)
      //  texCoords.y-=1.;
    float blockAO12 = 1.-blockAO/5.;
    albedo = ntexture(albedoMetallicMap, texCoords/tileAtlasSize).rgb;
    metallic = ntexture(albedoMetallicMap, texCoords/tileAtlasSize).a;
    roughness = ntexture(normalRoughnessMap, texCoords/tileAtlasSize).a;
    ao = ntexture(heightAmbientOcclusionMap, texCoords/tileAtlasSize).g;
    //metallic = 0.;
    Normal = (ntexture(normalRoughnessMap, texCoords/tileAtlasSize).xyz-vec3(0.5))*2.*TBN;
    vec2 local = (abs(texCoords)-vec2(.5))*2./sqrt(2.);
    float up = sqrt(1-local.x*local.x-local.y*local.y);
    //Normal = vec3(local, up)*TBN;
    //lightPosition = camPos + vec3(0, 0, 0);


    vec3 N = normalize(Normal);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    // выражение отражающей способности
    vec3 Lo = vec3(0.0);
    //расчет энергетической
    for(int i=1; i<3; i++)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float dist    = max(pow((length(lightPositions[i] - WorldPos)), 0.2f), 0.1f);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance     = lightColors[i] * attenuation;
        
        // Co-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness); 
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), test), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 1.0 * max(dot(N, V), 0) * max(dot(N, L), test);
        vec3 specular     = numerator / max(denominator, 0.001);
        float NdotL = max(dot(N, L), 0.);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// * parallaxShadow(texCoords, L, Normal); 
    }
    //ao = 0.0;
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color*=(1.-blockAO);
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    //color = vec3(1./4.*blockAO);
    //color.r = ndotv;
    //color.xyz = color.rrr;
    //color.xyz = (Normal + vec3(1.))/2;
    //color = vec3(texture(heightAmbientOcclusionMap, texCoords).xxx);
    //color = vec3(texture(albedoMetallicMap, texCoords).yyy);
    FragColor = vec4(color, 1.0);
}   