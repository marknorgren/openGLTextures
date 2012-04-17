#version 150

in vec4 vPosition; 
in vec2 texCoord;

//you'll need to pass the texture coordinates over to the fragment shader, so you'll need an out
out vec2 fTexCoord;

uniform mat4 model_view;
uniform mat4 projection;

void main()
{

	//don't forget to pass your texture coordinate through!
	fTexCoord = texCoord;
	gl_Position = projection * model_view*vPosition;


}
