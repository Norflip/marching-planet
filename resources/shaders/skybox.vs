#version 430
uniform mat4 invProj;
uniform mat4 view;

in vec4 aPosition;

smooth out vec3 eyeDirection;

void main ()
{
    mat3 invModel = transpose(mat3(view));
    vec3 unprojected = (invProj * aPosition).xyz;
    eyeDirection = normalize(invModel * unprojected);
    gl_Position = aPosition;
}