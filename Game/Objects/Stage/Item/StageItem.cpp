#include "pch.h"
#include "Game/Objects/Stage/Item/StageItem.h"
#include "Game/Collider/SphereCollider.h"

StageItem::StageItem()
{
}

void StageItem::Initialize(DirectX::SimpleMath::Vector3 position)
{
	// ステージアイテムの生成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(position, 1.0f);
}

void StageItem::Update()
{
}

void StageItem::Render()
{
}
