#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec3 fragPos;

layout(set=1,binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D armChair;

layout(set=0,binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightDir[4];
	vec3 lightPos[4];
	vec4 lightColor[4];
	vec3 eyePos;
	vec4 lightOn;
	} gubo;

vec3 point_light_dir(vec3 pos, int i) {
 // Point light - direction vector
 // Position of the light in <gubo.lightPos[i]>
 return normalize(gubo.lightPos[i] - pos);
}

vec3 point_light_color(vec3 pos, int i) {
 // Point light - color
 // Color of the light in <gubo.lightColor[i].rgb>
 // Scaling factor g in <gubo.lightColor[i].a>
 // Decay power beta: constant and fixed to 2.0
 // Position of the light in <gubo.lightPos[i]>
 return pow(gubo.lightColor[i].a / (length(gubo.lightPos[i] - pos)),2.0f)*gubo.lightColor[i].rgb;
 
}
vec3 BRDF(vec3 md, vec3 Norm, vec3 EyeDir, vec3 LD) {
// Just the diffuse part of the BRDF because parts of the bed does not have specular reflection--> they don't reflect light
	vec3 Diffuse;
	vec3 Specular;
	Diffuse = md * max(dot(Norm, LD),0.0f);
	return Diffuse ;
}
void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 md = texture(armChair, fragTexCoord).rgb;
	vec3 LD;	// light direction
	vec3 LC;	// light color

	vec3 RendEqSol = vec3(0);

	// First light
	LD = point_light_dir(fragPos, 0);
	LC = point_light_color(fragPos, 0);
	RendEqSol += BRDF(md, Norm, EyeDir, LD) * LC * gubo.lightOn.x;// point light
	// second light
	LD = point_light_dir(fragPos, 1);
	LC = point_light_color(fragPos, 1);
	RendEqSol += BRDF(md, Norm, EyeDir, LD) * LC * gubo.lightOn.y;// point light
	// thirt light
	LD = point_light_dir(fragPos, 2);
	LC = point_light_color(fragPos, 2);
	RendEqSol += BRDF(md, Norm, EyeDir, LD) * LC * gubo.lightOn.z;// point light
	// Fourth light
	LD = point_light_dir(fragPos, 3);
	LC = point_light_color(fragPos, 3);
	RendEqSol += BRDF(md, Norm, EyeDir, LD) * LC * gubo.lightOn.w;// point light
	// Output color
	outColor = vec4(RendEqSol, 1.0f);
	
}