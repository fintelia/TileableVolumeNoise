
#include "TileableVolumeNoise.h"

#include <glm/gtc/noise.hpp>
#include <math.h>

// Perlin noise based on GLM http://glm.g-truc.net
// Worley noise based on https://www.shadertoy.com/view/Xl2XRR by Marc-Andre Loyer

float Tileable3dNoise::hash(float n)
{
	return glm::fract(sin(n+1.951f) * 43758.5453f);
}

// hash based 3d value noise
float Tileable3dNoise::noise(const glm::vec3& x)
{
	glm::vec3 p = glm::floor(x);
	glm::vec3 f = glm::fract(x);

	f = f*f*(glm::vec3(3.0f) - glm::vec3(2.0f) * f);
	float n = p.x + p.y*57.0f + 113.0f*p.z;
	return glm::mix(
		glm::mix(
			glm::mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
			glm::mix(hash(n + 57.0f), hash(n + 58.0f), f.x),
			f.y),
		glm::mix(
			glm::mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
			glm::mix(hash(n + 170.0f), hash(n + 171.0f), f.x),
			f.y),
		f.z);
}

float Tileable3dNoise::Cells(const glm::vec3& p, float cellCount)
{
	const glm::vec3 pCell = p * cellCount;
	float d = 1.0e10;
	for (int xo = -1; xo <= 1; xo++)
	{
		for (int yo = -1; yo <= 1; yo++)
		{
			for (int zo = -1; zo <= 1; zo++)
			{
				glm::vec3 tp = glm::floor(pCell) + glm::vec3(xo, yo, zo);

				tp = pCell - tp - noise(glm::mod(tp, cellCount / 1));

				d = glm::min(d, dot(tp, tp));
			}
		}
	}
	d = std::fminf(d, 1.0f);
	d = std::fmaxf(d, 0.0f);
	return d;
}


float Tileable3dNoise::WorleyNoise(const glm::vec3& p, float cellCount)
{
	return Cells(p, cellCount);
}



float Tileable3dNoise::PerlinNoise(const glm::vec3& pIn, float frequency, int octaveCount)
{
	const float octaveFrenquencyFactor = 2;			// noise frequency factor between octave, forced to 2

	// Compute the sum for each octave
	float sum = 0.0f;
	float weightSum = 0.0f;
	float weight = 0.5f;
	for (int oct = 0; oct < octaveCount; oct++)
	{
		// Perlin vec3 is bugged in GLM on the Z axis :(, black stripes are visible
		// So instead we use 4d Perlin and only use xyz...
		//glm::vec3 p(x * freq, y * freq, z * freq);
		//float val = glm::perlin(p, glm::vec3(freq)) *0.5 + 0.5;

		glm::vec4 p = glm::vec4(pIn.x, pIn.y, pIn.z, 0.0f) * glm::vec4(frequency);
		float val = glm::perlin(p, glm::vec4(frequency));

		sum += val * weight;
		weightSum += weight;

		weight *= weight;
		frequency *= octaveFrenquencyFactor;
	}

	float noise = (sum / weightSum) *0.5f + 0.5f;
	noise = std::fminf(noise, 1.0f);
	noise = std::fmaxf(noise, 0.0f);
	return noise;
 }




