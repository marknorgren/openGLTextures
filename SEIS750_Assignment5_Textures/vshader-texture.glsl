#version 150

in vec4 vPosition; 
in vec2 texCoord;
in vec3 vNormal;
in vec4 vAmbientDiffuseColor;
in vec4 vSpecularColor;
in float vSpecularExponent;

out vec2 ftexCoord;
out vec3 L;
out vec3 H;
out vec4 AmbientDiffuseColor;
out vec4 SpecularColor;
out float SpecularExponent;
//you'll need to pass the texture coordinates over to the fragment shader, so you'll need an out
out vec2 fTexCoord;
out vec3 position;
out vec3 vN;


uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;

void main()
{
	vec4 vNormal = vec4(vNormal, 0.0);
	//don't forget to pass your texture coordinate through!

	SpecularColor = vSpecularColor;
	SpecularExponent = vSpecularExponent;

	vec4 veyepos = model_view*vPosition;
	
	vN = normalize(model_view * vNormal).xyz;
	position = veyepos.xyz;

	fTexCoord = texCoord;
	//gl_Position = projection * model_view*vPosition;
	gl_Position = projection * veyepos;


}
