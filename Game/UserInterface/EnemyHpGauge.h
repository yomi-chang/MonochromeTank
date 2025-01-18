#pragma once
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include "Framework/Graphics.h"

class EnemyHpGauge
{
public:
    // 体力の設定
    void SetMaxHp(float maxHp) 
    {
        m_maxHp = maxHp;
        m_hp = maxHp;
    }
    // 最大体力の取得
    float GetMaxHp() { return m_maxHp; }
    // 現在の体力の設定
    void SetHp(float hp) { m_hp = hp; }
    // 現在の体力の取得
    float GetHp() { return m_hp; }
    // ダメージ
    void Damage(float damage) { m_hp -= damage; }
    // 死亡しているかどうか
    bool GetDead() { return m_isDead; }

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

    // 最大体力
    float m_maxHp;

    // 現在の体力
    float m_hp;

    // 死亡判定
    float m_isDead;
};