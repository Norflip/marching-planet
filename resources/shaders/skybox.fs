#version 430
smooth in vec3 eyeDirection;
out vec4 frag_color;


#define DAY_IN_SECONDS (5*60)
float smooth_ping_pong (float t)
{
    t = clamp(t - floor(t / 2.0) * 2.0, 0.0, 2.0);
    t = 1.0 - abs(t - 1.0);
    
    // hermit polynom
    t = -2.0 * pow(t, 3.0) + (3.0 * t * t);
    return clamp(t,0.0, 1.0);
}

void main ()
{
    const vec4 topcolor = vec4(0.43, 0.72, 0.79, 1.0);
    const vec4 bottomcolor = vec4(0.09, 0.23, 0.3, 1);

   // const vec4 groundColor = vec4(0.35, 0.29, 0.22, 1);
    
    float t = (dot(vec3(0,1,0), eyeDirection) + 1.0) * 0.5;
    frag_color = mix(bottomcolor, topcolor, t);
}