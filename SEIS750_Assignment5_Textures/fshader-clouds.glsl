#version 150
in vec2 fTexCoord;
in vec3 position;
in vec3 vN;

uniform vec4 light_position;
uniform vec4 light_color;

uniform sampler2D cloudsTexture;

out vec4  fColor;

void main()
{
	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 N = normalize(vN);

	vec4 diff = max(dot(L,N), 0.0) * texture2D(cloudsTexture, fTexCoord) * light_color;

	fColor = diff;
	fColor.w = (texture2D(cloudsTexture, fTexCoord).w);
}