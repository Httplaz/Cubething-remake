#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D metallic;
uniform sampler2D roughness;
uniform sampler2D AO;

void main()
{
    FragColor = texture(albedo, TexCoords);
    //FragColor = texelFetch(tex, ivec2(texCoords*1600.), 0);
} 