#pragma once
#include "Interface/IComponent.h"

// 子オブジェクトを持つオブジェクトに継承するインタフェース
class IComposite : public IComponent
{
public:
	// 部品の追加する
	virtual void Attach(std::unique_ptr<IComponent> turretParts) = 0;
	// 砲塔の削除する
	virtual void Detach(std::unique_ptr<IComponent> turretPart) = 0;
};