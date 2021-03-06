#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;

const int MAX_LIGHTS=14;
struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
	int LightCount;
	Light lights[MAX_LIGHTS];
};

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
   // vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    //vec3 R = reflect(-L,N);
	//vec3 H = normalize(E + L);
	
    //vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    //vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
	
    vec3 Reflexion = vec3(0,0,0);
	for(int i = 0; i < LightCount;i++){
		vec3 L ;
		float Intensity = 1.0;
		float Dist = length(lights[i].Position - Position);		
		if(lights[i].Type == 0){
			L = normalize(lights[i].Position - Position); 
			Intensity /= (lights[i].Attenuation.x + lights[i].Attenuation.y * Dist + lights[i].Attenuation.z * Dist * Dist);
		}
		if(lights[i].Type == 1){
			L = normalize(-lights[i].Direction);
		}
		if(lights[i].Type == 2){
			L = normalize(lights[i].Position - Position);
			Intensity /= (lights[i].Attenuation.x + lights[i].Attenuation.y * Dist + lights[i].Attenuation.z * Dist * Dist);
			Intensity *= 1-sat((acos(dot(normalize(L),normalize(-lights[i].Direction)))-lights[i].SpotRadius.x)/(lights[i].SpotRadius.y - lights[i].SpotRadius.x));

		}
		vec3 lichtColor = lights[i].Color * Intensity;
		vec3 DiffuseComponent  = lichtColor * DiffuseColor * sat(dot(N,L));
		vec3 H = normalize(E + L);
		vec3 SpecularComponent = lichtColor * SpecularColor * pow(sat(dot(N,H)), SpecularExp);
		Reflexion+= DiffuseComponent * DiffTex.rgb + SpecularComponent;
	}
	
    FragColor = vec4(Reflexion + AmbientColor*DiffTex.rgb +  Reflexion ,DiffTex.a);
}