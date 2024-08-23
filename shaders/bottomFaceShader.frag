#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


layout(set = 1, binding = 1) uniform sampler2D room;
const vec4 FGcolor = vec4(1.0f);
const vec4 BGcolor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 SHcolor = vec4(0.0f, 0.0f, 0.0f, 0.5f);

void main() {
	vec4 Tx = texture(room, fragTexCoord);
	outColor = Tx;
	
}