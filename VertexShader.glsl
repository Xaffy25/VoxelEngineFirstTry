#version 460

layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vPos;

void main()
{
	vec4 worldPos = uModel * vec4 (aPos,1.0);
	vPos = worldPos.xyz;
	gl_Position = uProjection * uView * worldPos;
}