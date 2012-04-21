#version 150
//You need the texture coordinates to be passed in from the vertex shader (interpolated)
in vec2 fTexCoord;
//You also need your uniform sampler so you can use those coordinates to look up a texel

in vec3 position;
in vec3 vN;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

uniform sampler2D texture;
uniform sampler2D specMapTexture;
uniform sampler2D nightMapTexture;
uniform sampler2D cloudsTexture;

out vec4  fColor;

void main()
{
	vec4 AmbientDiffuseColor;
	vec4 SpecularColor;
	float SpecularExponent;

	//you need to look up the appropriate color for this fragment based on the texture map and the texture cordinates


	SpecularColor = texture2D(specMapTexture, fTexCoord);
	SpecularExponent = SpecularColor.w*900;

	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);

	vec3 H = normalize(L+E);
	
	float dayNightDiff = dot(L,N);
	if(dayNightDiff < 0){
			AmbientDiffuseColor = texture2D(nightMapTexture, fTexCoord);
			AmbientDiffuseColor.w = 1.0;
			//amb = AmbientDiffuseColor * ambient_light;
		}
	else if(dayNightDiff > 0) {
			AmbientDiffuseColor = texture2D(texture, fTexCoord);
			AmbientDiffuseColor.w = 1.0;
		}
	if ( (dayNightDiff >= -1) && ( dayNightDiff <= 1) ){
		vec4 night;
		vec4 day;
		float nightAmount = 0.0;
		float dayAmount = 0.0;
		
		dayAmount = 1.25*(dayNightDiff+.4);
		nightAmount = 1-dayAmount;
		//nightAmount = 1.25*(dayNightDiff+.4);
		//dayAmount = 1-nightAmount;
		
		night = vec4((nightAmount * (texture2D(nightMapTexture, fTexCoord)).xyz), 1.0);
		day = vec4((dayAmount * (texture2D(texture, fTexCoord)).xyz), 1.0);
		
		AmbientDiffuseColor = ((night+day)/2);
		AmbientDiffuseColor.w = 1.0;
	}
		
	vec4 amb = AmbientDiffuseColor * ambient_light;	
	vec4 diff = max(dot(L,N), 0.0) * AmbientDiffuseColor * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), SpecularExponent) *  SpecularColor * light_color;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	}
	fColor = amb + diff + spec;
	fColor.w = 1.0;

	fColor = texture2D(cloudsTexture, fTexCoord);
	//fColor.w = 0.0;
	//fColor = vec4( vN, 1);
		
	//fColor = AmbientDiffuseColor;
	//fColor = spec;

}