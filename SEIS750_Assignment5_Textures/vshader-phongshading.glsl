#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec3 vNormal;
in vec4 vSpecularColor;
in float vSpecularExponent;

out vec4 color;
out vec3 position;
out vec3 vN;

out vec4 AmbientDiffuseColor;
out vec4 SpecularColor;
out float SpecularExponent;

uniform mat4 model_view;
uniform mat4 projection;


void
main()
{
	vec4 vNormal = vec4(vNormal, 0.0);
    AmbientDiffuseColor = vAmbientDiffuseColor;
	SpecularColor = vSpecularColor;
	SpecularExponent = vSpecularExponent;
	
	vec4 veyepos = model_view*vPosition;
	


	vN = normalize(model_view * vNormal).xyz;
	
	//spec = vec4(1,1,1,1);
	position = veyepos.xyz;
	gl_Position = projection * veyepos;
	

}