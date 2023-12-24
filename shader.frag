#version 330 core

in vec2 textureCoord;

out vec4 fragColor;

uniform sampler2D textures0;
uniform sampler2D textures1;
uniform sampler2D textures2;
uniform sampler2D textures3;
uniform sampler2D textures4;
uniform sampler2D textures5;
uniform sampler2D textures6;
uniform sampler2D textures7;

uniform int numTextures;

void main() {
	vec4 finalColor = vec4(1.0);
			
	if (numTextures > 0){
		finalColor *= texture(textures0, textureCoord);
	}
	if (numTextures > 1){
		finalColor *= texture(textures1, textureCoord);
	}
	if (numTextures > 2){
		finalColor *= texture(textures2, textureCoord);
	}
	if (numTextures > 3){
		finalColor *= texture(textures3, textureCoord);
	}
	if (numTextures > 4){
		finalColor *= texture(textures4, textureCoord);
	}
	if (numTextures > 5){
		finalColor *= texture(textures5, textureCoord);
	}
	if (numTextures > 6){
		finalColor *= texture(textures6, textureCoord);
	}
	if (numTextures > 7){
		finalColor *= texture(textures7, textureCoord);
	}

	fragColor = finalColor;
}