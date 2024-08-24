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

layout(set = 1, binding = 1) uniform sampler2D room;

layout(set=0,binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightDir[2];
	vec3 lightPos[2];
	vec4 lightColor[2];
	vec3 eyePos;
	vec4 lightOn;
	} gubo;

vec3 direct_light_dir(vec3 pos, int i) {
 // Direct light - direction vector
 // Direction of the light in <gubo.lightDir[i]>
 return gubo.lightDir[i];
}

vec3 direct_light_color(vec3 pos, int i) {
 // Direct light - color
 // Color of the light in <gubo.lightColor[i].rgb>
 return gubo.lightColor[i].rgb;
}

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
vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
// Compute the BRDF, with a given color <Albedo>, in a given position characterized bu a given normal vector <Norm>,
// for a light direct according to <LD>, and viewed from a direction <EyeDir>
	vec3 Diffuse;
	vec3 Specular;
	Diffuse = Albedo * max(dot(Norm, LD),0.0f);
	Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)),0.0f), 160.0f));
	
	return Diffuse + Specular;
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(room, fragTexCoord).rgb;
	vec3 LD;	// light direction
	vec3 LC;	// light color

	vec3 RendEqSol = vec3(0);

	// First light
	LD = point_light_dir(fragPos, 0);
	LC = point_light_color(fragPos, 0);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn.x;
	// Fourth light
	LD = direct_light_dir(fragPos, 3);
	LC = direct_light_color(fragPos, 3);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.y;

	// Indirect illumination simulation
	// A special type of non-uniform ambient color, invented for this course
	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.2;
	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.2;
	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.2;
	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.2;
	const vec3 czp = vec3(0.8,0.2,0.4) * 0.2;
	const vec3 czn = vec3(0.3,0.6,0.7) * 0.2;
	
	vec3 Ambient =((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +
				   (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +
				   (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) * Albedo;
	RendEqSol += Ambient         * gubo.lightOn.w;
	
	// Output color
	outColor = vec4(RendEqSol, 1.0f);
	//vec4 Tx = texture(room, fragTexCoord);
	//outColor = Tx;
	
}
