#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord0;
layout(location=3) in vec2 VertexTexcoord1;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord0;
out vec2 Texcoord1;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;

void main()
{
	vec4 scaledPos = VertexPos;
    scaledPos.x *= Scaling.x;
    scaledPos.y *= Scaling.y;
    scaledPos.z *= Scaling.z;
	vec4 scaledNormal = VertexNormal;
	scaledNormal.x /= Scaling.x;
	scaledNormal.y /= Scaling.y;
	scaledNormal.z /= Scaling.z;
	scaledNormal = normalize(scaledNormal);
	
	
	
    Position = (ModelMat * scaledPos).xyz;
    Normal = (ModelMat * vec4(scaledNormal.xyz,0)).xyz;
    Texcoord0 = VertexTexcoord0;
	Texcoord1 = VertexTexcoord1;
    gl_Position = ModelViewProjMat * scaledPos;
}
