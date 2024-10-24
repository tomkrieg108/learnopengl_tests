#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    //vec3 TangentLightPos;
    //vec3 TangentViewPos;
    //vec3 TangentFragPos;

    //I added
    vec3 TangentLightDir;
    vec3 TangentViewDir;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model))); //matrix to transform a normal, retaining a dot product of 0
    vec3 T = normalize(normalMatrix * aTangent);  //transform T to world space using same method as for normal
    vec3 N = normalize(normalMatrix * aNormal); //Transform normal to world space
    T = normalize(T - dot(T, N) * N);  //Gram-schmitt 
    vec3 B = cross(N, T);
    
    //Note that [TBN] is a 3x3 matrix.  Each column vector is in world space It can only be used to transform tangent space vectors (i.e. the sampled normal) to a world space normal vector.  It does't transform positions as the model matrix can

    //transpose = inverse since [T.B,N] is orthonormal.
    //Resulting matrix transforms from world space to tangent space
    //lightPos & viewPos uniforms already in world space

    //The following direction vectors can be transformed to tangent space here in the vertex shader.//i.e. independent of position.  That's whay the 
    mat3 invTBN = transpose(mat3(T, B, N));    
    //vs_out.TangentLightPos = TBN * lightPos;
    //vs_out.TangentViewPos  = TBN * viewPos;
    //vs_out.TangentFragPos  = TBN * vs_out.FragPos;

    vs_out.TangentLightDir = invTBN * (lightPos - vs_out.FragPos);
    vs_out.TangentViewDir = invTBN * (viewPos - vs_out.FragPos);
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}