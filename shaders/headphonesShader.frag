#version 450

layout(location = 0) in vec3 fragPos;

layout(location = 0) out vec4 outColor;
	vec3 lightDir[4];
	vec3 lightPos[4];
	vec4 lightColor[4];
	vec3 eyePos;
	vec4 lightOn;

	float Pow;

	vec3 Result = Diffuse + Specular;
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);
	brdf = BRDF(Norm, EyeDir, lightDir, lightColor, Pow, md);
	outColor = vec4(col, 1.0f);