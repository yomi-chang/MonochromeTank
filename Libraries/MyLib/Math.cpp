/*
	@file	Math.cpp
	@brief	•Ö—˜ŠÖ”
*/
#include "Libraries/MyLib/Math.h"
#include "pch.h"
#include <random>

namespace mylib
{
	// ƒNƒ‰ƒ“ƒvŠÖ”
	float Clamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));;
	}

	// —”¶¬
	float Random(float min, float max)
	{
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());

		std::normal_distribution<double> dist(min, max);

		return dist(engine);
	}
}