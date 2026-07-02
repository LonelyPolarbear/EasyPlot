#version 450 core
layout(local_size_x = 256) in;

layout(std430, binding = 1) buffer InputBuffer {
    float data[];
};
void main() { 
    // Compute shader code goes here}
    //测试，将data中的每个元素乘以2.0
    uint idx = gl_GlobalInvocationID.x;
    data[idx] *= 2.0;
}