#version 400
uniform vec3 EyePos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D MixTex; // for exercise 3
uniform sampler2D DetailTex[2]; // for exercise 3
uniform vec3 Scaling;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord0;
in vec2 Texcoord1;
out vec4 FragColor;

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}


void main()
{
    vec3 N      = normalize(Normal);
    vec3 L      = normalize(LightPos); // light is treated as directional source
    vec3 D      = EyePos-Position;
    float Dist  = length(D);
    vec3 E      = D/Dist;
    vec3 R      = reflect(-L,N);
    
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
    
    // Exercise 3
    // TODO: Add texture blending code here..
    //int k = 100;
	vec4 grass = texture(DetailTex[0], Texcoord1);
	vec4 stone = texture(DetailTex[1], Texcoord1);
	vec4 texelMixeture = texture(MixTex, Texcoord0);
	vec4 reflectColor = vec4(((DiffuseComponent + AmbientColor) + SpecularComponent),1);
	vec4 texFinal = mix(grass, stone , texelMixeture);
    FragColor = texFinal * reflectColor;
}
