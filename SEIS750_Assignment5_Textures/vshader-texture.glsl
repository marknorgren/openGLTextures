#version 150

in vec4 vPosition; 
in vec2 texCoord;
in vec3 vNormal;

out vec2 fTexCoord;
out vec3 position;
out vec3 vN;

uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;

void main()
{
	vec4 vNormal = vec4(vNormal, 0.0);

	vec4 veyepos = model_view*vPosition;
	
	vN = normalize(model_view * vNormal).xyz;
	position = veyepos.xyz;

	fTexCoord = texCoord;
	gl_Position = projection * veyepos;
}
