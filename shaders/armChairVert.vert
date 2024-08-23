#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set=1, binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;
void main()
{
  	// Clipping coordinates must be returned in global variable gl_Posision
	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
	// Here the value of the out variables passed to the Fragment shader are computed
	fragUV = inUV;
}  