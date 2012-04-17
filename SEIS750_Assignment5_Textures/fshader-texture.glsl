#version 150
//You need the texture coordinates to be passed in from the vertex shader (interpolated)
in vec2 fTexCoord;
//You also need your uniform sampler so you can use those coordinates to look up a texel

out vec4  fColor;

in vec3 position;
in vec3 vN;



uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

uniform sampler2D texture;

void main()
{
	vec4 AmbientDiffuseColor;
	vec4 SpecularColor;
	float SpecularExponent;

	//you need to look up the appropriate color for this fragment based on the texture map and the texture cordinates

	AmbientDiffuseColor = texture2D(texture, fTexCoord);
	AmbientDiffuseColor.w = 1.0;
	fColor = AmbientDiffuseColor;

}