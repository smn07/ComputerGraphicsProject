#version 450
#extension GL_ARB_separate_shader_objects : enable

//Vertex shader is applied for each vertex of the object

// set=1 means that we are using the descriptor set related to the object and not the Global one
// binding=0 means that we are using the first buffer of the descriptor set
layout(set=1, binding = 0) uniform RoomUniformBufferObject {
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

// these are the locations related to the binding 0.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 norm;


// variables which will be passed to the Fragment shader
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec3 fragPos;

void main()
{
   	// Clipping coordinates must be returned in global variable gl_Posision
	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
	// Here the value of the out variables passed to the Fragment shader are computed
	fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;
	fragNorm = mat3(ubo.nMat) * norm;
	fragTexCoord = inUV;
}  