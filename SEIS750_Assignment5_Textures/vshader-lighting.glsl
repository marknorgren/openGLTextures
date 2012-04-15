#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec3 vNormal;
in vec4 vSpecularColor;
in float vSpecularExponent;


out vec4 color;

uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;


void
main()
{
	vec4 vNormal = vec4(vNormal, 0.0);	
    vec4 veyepos = model_view*vPosition;
	vec3 L = normalize( light_position.xyz - veyepos.xyz);
	vec3 E = normalize(-veyepos.xyz);
	vec3 H = normalize(L+E);

	vec3 N = normalize(model_view * vNormal).xyz;
	vec4 amb = vAmbientDiffuseColor * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) * vAmbientDiffuseColor * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), vSpecularExponent) *  vSpecularColor * light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}
	//spec = vec4(1,1,1,1);
	gl_Position = projection * veyepos;
	color = amb + diff + spec;
}