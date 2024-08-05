#include "Libraries/MyLib/Math.h"
#include "pch.h"

namespace mylib
{
	float Clamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));;
	}
}