#version 450#extension GL_ARB_separate_shader_objects : enable
// this defines the variable received from the Vertex Shader// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;
// This defines the color computed by this shader. Generally is always location 0.
layout(location = 0) out vec4 outColor;// Here the Uniform buffers are defined. In this case, the Global Uniforms of Set 0// The texture of Set 1 (binding 1), and the Material parameters (Set 1, binding 2)// are used. Note that each definition must match the one used in the CPP codelayout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightDir[4];
	vec3 lightPos[4];
	vec4 lightColor[4];
	vec3 eyePos;
	vec4 lightOn;} gubo;
layout(set = 1, binding = 2) uniform HeadphonesParUniformBufferObject {
	float Pow;} mubo;
layout(set = 1, binding = 1) uniform sampler2D tex;// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model// The scene is lit by a single Spot Lightvec3 BRDF(vec3 Norm, vec3 EyeDir, vec3 lightDir, vec3 lightColor, float Pow, vec3 md){	vec3 Diffuse = texture(tex, fragUV).rgb * 0.975f * max(dot(Norm, lightDir),0.0);	vec3 Specular = md * vec3(pow(max(dot(Norm, normalize(lightDir + EyeDir)),0.0), Pow));
	vec3 Result = Diffuse + Specular;	return Result;}void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);	vec3 md = texture(tex, fragUV).rgb;	vec3 col = vec3(0.0f,0.0f,0.0f);	float Pow = mubo.Pow;		vec3 lightDir = normalize(gubo.lightDir[0]);	vec3 lightColor = gubo.lightColor[0].rgb;
	vec3 brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);	col  += brdf * lightColor * gubo.lightOn.x;		lightDir = normalize(gubo.lightDir[1]);	lightColor = gubo.lightColor[1].rgb;
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);	col  += brdf * lightColor * gubo.lightOn.y;		lightDir = normalize(gubo.lightDir[2]);	lightColor = gubo.lightColor[2].rgb;
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);	col  += brdf * lightColor * gubo.lightOn.z;		lightDir = normalize(gubo.lightDir[3]);	lightColor = gubo.lightColor[3].rgb;
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);	col  += brdf * lightColor * gubo.lightOn.w;	
	outColor = vec4(col, 1.0f);	//outColor = vec4(md, 1.0f);//	outColor = vec4(gubo.eyePos/5.0+vec3(0.5),1.0);//	outColor = vec4(0.5*Norm+vec3(0.5),1.0);//	outColor = vec4(fragPos/5.0+vec3(0.5),1.0);}