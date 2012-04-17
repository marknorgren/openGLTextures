#version 150
//You need the texture coordinates to be passed in from the vertex shader (interpolated)
in vec2 fTexCoord;
//You also need your uniform sampler so you can use those coordinates to look up a texel

out vec4  fColor;

uniform sampler2D texture;

void main()
{
	

	//you need to look up the appropriate color for this fragment based on the texture map and the texture cordinates

	fColor = texture2D(texture, fTexCoord);

}