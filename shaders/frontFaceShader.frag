#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set=1,binding = 1) uniform sampler2D room;

// Colori definiti
const vec4 FGcolor = vec4(1.0f);
const vec4 BGcolor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 SHcolor = vec4(0.0f, 0.0f, 0.0f, 0.5f);

// Colore da usare come sovrapposizione
const vec4 overlayColor = vec4(0.6f, 0.3f, 0.1f, 1.0f); // Marrone

void main() {
    // Prendi il colore dalla texture
    vec4 Tx = texture(room, fragTexCoord);
    
    // Applica la sovrapposizione di colore
    vec4 modifiedColor = mix(Tx, overlayColor, 1.8); // 0.5 determina la quantit� di sovrapposizione
    
    // Imposta il colore di output
    outColor = Tx;
}