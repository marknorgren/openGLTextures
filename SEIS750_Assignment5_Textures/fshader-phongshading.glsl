#version 150
in vec3 position;
in vec3 vN;

in vec4 AmbientDiffuseColor;
in vec4 SpecularColor;
in float SpecularExponent;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

out vec4  fColor;

void main()
{
	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);

	vec3 H = normalize(L+E);
	vec4 amb = AmbientDiffuseColor * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) * AmbientDiffuseColor * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), SpecularExponent) *  SpecularColor * light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}
	fColor = amb + diff + spec;
	//fColor = vec4( vN, 1);
}