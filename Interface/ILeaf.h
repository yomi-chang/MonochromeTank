#pragma once
#include "Interface/IComponent.h"

// 子オブジェクトを持たない末端のオブジェクトに継承するクラス
class ILeaf : public IComponent
{
};