#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushConsts
{
	mat4 transform;
	uint cascadeIndex;
} pushConsts;

layout(set = 0,binding = 0) uniform UniformBufferObject
{
    mat4 projView[16];
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout (location = 0) in vec3 position;

void main()
{
    mat4 proj;
    switch(pushConsts.cascadeIndex)
    {
        case 0 : 
            proj = ubo.projView[0];
            break;
        case 1 : 
            proj = ubo.projView[1];
            break;
        case 2 : 
            proj = ubo.projView[2];
            break;
        default : 
            proj = ubo.projView[3];
            break;
    }
    gl_Position = vec4(position, 1.0) * pushConsts.transform * proj; 
}