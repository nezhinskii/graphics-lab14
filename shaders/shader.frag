#version 330 core

in vec3 positionOut;
in vec3 colorOut;
in vec2 texCoordOut;
in vec3 normalOut;

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
uniform vec4 lightColor;
uniform vec3 camPos;

struct PointSource {
	float intensity;
	vec3 pos;
};

struct SpotlightSource {
	float intensity;
	vec3 pos;
	vec3 direction;
	float cone;
};

struct DirectionalSource {
	float intensity;
	vec3 direction;
};

uniform PointSource pSource;
uniform SpotlightSource sSource;
uniform DirectionalSource dSource;

void pointLight(inout float diffSum, inout float specSum)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = pSource.pos - positionOut;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = pSource.intensity / (a * dist * dist + b * dist + 1.0f);

	// diffuse lighting
	vec3 normal = normalize(normalOut);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - positionOut);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	diffSum += diffuse * inten;
	specSum += specular * inten;
}

void spotLight(inout float diffSum, inout float specSum)
{
	// controls how big the area that is lit up is
	float outerCone = sSource.cone;
	float innerCone = sSource.cone * 0.95f;

	// diffuse lighting
	vec3 normal = normalize(normalOut);
	vec3 lightDirection = normalize(sSource.pos - positionOut);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - positionOut);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the positionOut based on its angle to the center of the light cone
	float angle = dot(sSource.direction, -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	diffSum += diffuse * inten * sSource.intensity;
	specSum += specular * inten * sSource.intensity;
}

void direcLight(inout float diffSum, inout float specSum)
{
    // diffuse lighting
    vec3 normal = normalize(normalOut);
    vec3 lightDirection = normalize(dSource.direction);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - positionOut);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

	diffSum += diffuse * dSource.intensity;
    specSum += specular * dSource.intensity;
}

vec4 comm()
{
	vec4 finalColor = vec4(1.0);
			
	if (numTextures > 0){
		finalColor *= texture(textures0, texCoordOut);
	}
	if (numTextures > 1){
		finalColor *= texture(textures1, texCoordOut);
	}
	if (numTextures > 2){
		finalColor *= texture(textures2, texCoordOut);
	}
	if (numTextures > 3){
		finalColor *= texture(textures3, texCoordOut);
	}
	if (numTextures > 4){
		finalColor *= texture(textures4, texCoordOut);
	}
	if (numTextures > 5){
		finalColor *= texture(textures5, texCoordOut);
	}
	if (numTextures > 6){
		finalColor *= texture(textures6, texCoordOut);
	}
	if (numTextures > 7){
		finalColor *= texture(textures7, texCoordOut);
	}

	return finalColor;
}

void main() {
	float ambient = 0.20f;
	float diffSum = 0.0;
	float specSum = 0.0;
	pointLight(diffSum, specSum);
	spotLight(diffSum, specSum);
	direcLight(diffSum, specSum);	

	fragColor = (texture(textures0, texCoordOut) * (diffSum + ambient) + texture(textures1, texCoordOut).r * specSum) * lightColor;
}