/**
 * @file   MemoryLeakDetector.h
 * @brief  ƒƒ‚ƒŠƒŠ[ƒN‚ÌŒŸoŠí
 */
#pragma once

#if defined(_DEBUG)
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace mylib
{
	inline void MemoryLeakDetector()
	{
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	}
}
