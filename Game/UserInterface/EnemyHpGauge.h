#pragma once
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include "Framework/Graphics.h"

class EnemyHpGauge
{
public:
    EnemyHpGauge();
    ~EnemyHpGauge();

    void Render(
        DirectX::SimpleMath::Vector3 position,
        float hpRatio
    );

private:
    //	入力レイアウト 
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    //	共通ステートオブジェクトへのポインタ
    //std::unique_ptr<DirectX::CommonStates> m_status;

    //	エフェクト 
    std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

    //	プリミティブバッチ 
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

    // テクスチャの移動
    DirectX::SimpleMath::Vector3 m_position;

    // 透明度
    float m_alpha;

    // 経過時間
    float m_elapsedTime;

    // グラフィックス
    Graphics* m_graphics;
};