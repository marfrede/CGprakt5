#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;
// uniform mat4 NormalMat; needed??

void main()
{
    // gegeben:
    // Position = (ModelMat * VertexPos).xyz;
    // Normal = (ModelMat * vec4(VertexNormal.xyz,0)).xyz;
    // Texcoord = VertexTexcoord;
    // gl_Position = ModelViewProjMat * VertexPos;

    // Exercise 1
    // TODO: apply scaling on terrain model..
    vec4 ScaledPos = VertexPos * vec4(Scaling.xyz, 1.0f);
    Position = (ModelMat * ScaledPos).xyz;
    Normal = (ModelMat * vec4(VertexNormal.xyz,0) /* * vec4(Scaling.xyz, 1.0f)*/ ).xyz;
    Texcoord = VertexTexcoord;
    gl_Position = ModelViewProjMat * ScaledPos;
}
