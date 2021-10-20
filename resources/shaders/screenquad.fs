#version 430
out vec4 frag_colour;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main () {

    frag_colour = texture(screenTexture, TexCoords);

    //frag_colour = vec4(TexCoords, 0, 1);
    
    //frag_colour = texture(screenTexture, TexCoords);

    /*
    if(clamp(uv, 0, 1) == uv)
    {
        frag_colour = color;    
    }
    else
    {
        frag_colour = vec4(0,0,0,1);
    }
    */
}