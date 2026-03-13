#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform float TIME;
uniform sampler2D MAIN_TEXTURE;
uniform sampler2D NOISE_TEXTURE;
uniform vec4 COLOR;

void main()
{
   vec2 scaleUV = vec2(TexCoord.x, TexCoord.y) * sin(TIME* 0.5f);
   vec2 offest = texture(NOISE_TEXTURE, TexCoord + vec2(TIME * 0.1)).gb;
   FragColor = texture(MAIN_TEXTURE, scaleUV + (offest * 1)) * COLOR;// * vec4(ourColor, 1.0);

}