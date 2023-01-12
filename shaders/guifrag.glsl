#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform bool crosshair; // if crosshair then draw crosshair, else draw selected block
uniform sampler2D ourTexture;

void main() {
   if (crosshair) {
      FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   } else {
      FragColor = texture(ourTexture, TexCoord);
   }
   
}