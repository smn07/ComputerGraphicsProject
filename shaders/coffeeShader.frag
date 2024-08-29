#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec3 fragPos;

layout(set=1,binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
	int selected;
} ubo;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D room;

layout(set=0,binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightDir[4];
	vec3 lightPos[4];
	vec4 lightColor[4];
	vec3 eyePos;
	vec4 lightOn;
	//float cosIn;
	//float cosOut;
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
/*
vec3 spot_light_dir(vec3 pos, int i)
{
    // Spot light - direction vector
    // Direction of the light in <gubo.lightDir[i]>
    // Position of the light in <gubo.lightPos[i]>
   // gubo.lightDir[i] = normalize(gubo.lightDir[i]);
    return normalize(gubo.lightPos[i] - pos);
    // return vec3(1,0,0);
}

vec3 spot_light_color(vec3 pos, int i)
{

    // Spot light - color
    // Color of the light in <gubo.lightColor[i].rgb>
    // Scaling factor g in <gubo.lightColor[i].a>
    // Decay power beta: constant and fixed to 2.0
    // Position of the light in <gubo.lightPos[i]>
    // Direction of the light in <gubo.lightDir[i]>
    // Cosine of half of the inner angle in <gubo.cosIn>
    // Cosine of half of the outer angle in <gubo.cosOut>
    return point_light_color(pos, i) * clamp((dot(normalize(gubo.lightPos[i] - pos), gubo.lightDir[i]) - gubo.cosOut) / (gubo.cosIn - gubo.cosOut), 0.0f, 1.0f);
    // return vec3(1,0,0);
}
*/
float D_cookTorrance(vec3 halfVector, vec3 normalVector, float roughness) {
	float numerator=exp(-(1 - (pow(dot(halfVector, normalVector), 2))) / (pow(dot(halfVector, normalVector), 2) * pow(roughness, 2)));
	float denominator= 3.14159*pow(roughness,2)*pow(dot(halfVector,normalVector),4);
	return numerator/denominator;
}
float G_cookTorrance(vec3 halfVector, vec3 normalVector, vec3 lightVector, vec3 viewVector) {
	float firstTerm = 2*dot(halfVector,normalVector)*dot(halfVector,viewVector)/dot(viewVector,halfVector);
	float secondTerm = 2*dot(halfVector,normalVector)*dot(halfVector,lightVector)/dot(viewVector,halfVector);

	return min(1,min(firstTerm,secondTerm));
}
float F_cookTorrance(vec3 halfVector, vec3 viewVector,float F0) {

	float F= F0+ (1-F0)*pow((1-clamp(dot(viewVector,halfVector),0.0001,1)),5);
	return F;
}
vec3 BRDF(vec3 objectColor, vec3 Norm, vec3 EyeDir, vec3 LD,vec3 halfVector) {
	vec3 Diffuse;
	vec3 Specular;
	float D= D_cookTorrance(halfVector, Norm, 0.2f);
	float G= G_cookTorrance(halfVector, Norm, LD, EyeDir);
	float F= F_cookTorrance(halfVector, EyeDir, 0.05);
	Specular =  vec3(1,1,1)*(D * G * F / (4 * clamp(dot(EyeDir,Norm),0.000001,1)));
	Diffuse = objectColor * max(dot(Norm, LD),0.0f);

	return Diffuse + Specular ;
}


void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 md = texture(room, fragTexCoord).rgb;
	
	vec4 Tx = texture(room, fragTexCoord);
	vec3 LD;	// light direction
	vec3 LC;	// light color
	vec3 RendEqSol = vec3(0);
	vec3 halfVec=vec3(0);
	if (ubo.selected==0){
	// First light
	LD = point_light_dir(fragPos, 0);
	LC = point_light_color(fragPos, 0);
	halfVec= normalize(LD + EyeDir);
	RendEqSol += BRDF(md, Norm, EyeDir, LD,halfVec) * LC* gubo.lightOn.x;
	// second light
	LD = point_light_dir(fragPos, 1);
	LC = point_light_color(fragPos, 1);
	halfVec= normalize(LD + EyeDir);
	RendEqSol += BRDF(md, Norm, EyeDir, LD,halfVec) * LC * gubo.lightOn.y;// point light
	// thirt light
	LD = point_light_dir(fragPos,2);
	LC = point_light_color(fragPos,2);
	halfVec= normalize(LD + EyeDir);
	RendEqSol += BRDF(md, Norm, EyeDir, LD,halfVec) * LC * gubo.lightOn.z;// point light
	// Fourth light
	LD = point_light_dir(fragPos, 3);
	LC = point_light_color(fragPos, 3);
	halfVec= normalize(LD + EyeDir);
	RendEqSol += BRDF(md, Norm, EyeDir, LD,halfVec) * LC * gubo.lightOn.w;// point light
	}
	else {
	//LD = spot_light_dir(fragPos, 4);
	//LC = spot_light_color(fragPos, 4);
	//halfVec= normalize(LD + EyeDir);
	//RendEqSol += BRDF(md, Norm, EyeDir, LD,halfVec) * LC;

	}
	//output color
	outColor = vec4(RendEqSol,1);
	
}