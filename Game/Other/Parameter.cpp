/*
	@file	Parameter.cpp
	@brief	パラメータークラス
*/
#include "pch.h"
#include "Parameter.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>      

std::unique_ptr<Parameter> Parameter::m_parameter = nullptr;

// パラメーターのインスタンスを取得する
Parameter* const Parameter::GetInstance()
{
    if (m_parameter == nullptr)
    {
        // パラメーターのインスタンスを生成する
        m_parameter.reset(new Parameter());
    }
    // パラメーターのインスタンスを返す
    return m_parameter.get();
}

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Parameter::Parameter()
    :
    m_turretAngleMin{},
    m_turretAngleMax{},
    m_cannonAngleMin{},
    m_cannonAngleMax{},
    m_playerPosition{},
    m_playerSpeed{},
    m_playerRotationSpeed{},
    m_playerHp{},
    m_enemyPositions{},
    m_enemySpeed{},
    m_enemyRotationSpeed{},
    m_enemyHp{},
    m_shotInterval{},
    m_bulletCount{},
    m_bulletReloadTime{},
    m_bulletSurvivalTime{},
    m_bulletSpeed{},
    m_bulletColliderRadius{},
    m_bulletDamage{},
    m_bulletMaxTrail{},
    m_bulletTrailWidth{},
    m_cannonBallReloadTime{},
    m_cannonBallSurvivalTime{},
    m_cannonBallSpeed{},
    m_cannonBallGravity{},
    m_cannonBallColliderRadius{},
    m_cannonBallDamage{},
    m_cannonBallMaxTrail{},
    m_cannonBallTrailWidth{},
    m_wallSize{},
    m_scoutRadius{},
    m_attackStartRadius{},
    m_attackFinishRadius{},
    m_fadeSpeed{},
    m_maxRange{},
    m_damageEffectTime{},
    m_patrolRoutes{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
Parameter::~Parameter()
{
}

//-------------------------------------------------------------------
// パラメータの読み込み
//-------------------------------------------------------------------
void Parameter::LoadParameter()
{
    using namespace DirectX;
    std::string fileName = "Resources\\Data\\parameter.json";

    // ファイル読み込み
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open " << fileName << std::endl;
        return;
    }

    // JSONデータの読み込み
    nlohmann::json j;
    file >> j;
    file.close();

    // JSONの内容を代入
    m_turretAngleMin = XMConvertToRadians(j["TURRET_ANGLE_MIN"].get<float>());
    m_turretAngleMax = XMConvertToRadians(j["TURRET_ANGLE_MAX"].get<float>());
    m_cannonAngleMin = XMConvertToRadians(j["CANNON_ANGLE_MIN"].get<float>());
    m_cannonAngleMax = XMConvertToRadians(j["CANNON_ANGLE_MAX"].get<float>());
    m_playerPosition.x = j["PLAYER_POSITION"]["x"].get<float>();
    m_playerPosition.y = j["PLAYER_POSITION"]["y"].get<float>();
    m_playerPosition.z = j["PLAYER_POSITION"]["z"].get<float>();
    m_playerSpeed    = j["PLAYER_SPEED"].get<float>();
    m_playerRotationSpeed = j["PLAYER_ROTATION_SPEED"].get<float>();
    m_playerHp = j["PLAYER_HP"].get<int>();
    const auto& enemyPositions = j["ENEMY_POSITIONS"];
    for (auto& enemyPosition : enemyPositions)
    {
        DirectX::SimpleMath::Vector3 vector;
        vector.x = enemyPosition["x"].get<float>();
        vector.y = enemyPosition["y"].get<float>();
        vector.z = enemyPosition["z"].get<float>();
        m_enemyPositions.push_back(vector);
    }
    m_enemySpeed = j["ENEMY_SPEED"].get<float>();
    m_enemyRotationSpeed = j["ENEMY_ROTATION_SPEED"].get<float>();
    m_enemyHp = j["ENEMY_HP"].get<int>();
    m_shotInterval = j["SHOT_INTERVAL"].get<float>();
    m_bulletCount = j["BULLET_COUNT"].get<int>();
    m_bulletReloadTime = j["BULLET_RELOAD_TIME"].get<float>();
    m_bulletSurvivalTime = j["BULLET_SURVIVAL_TIME"].get<float>();
    m_bulletSpeed.x = j["BULLET_SPEED"]["x"].get<float>();
    m_bulletSpeed.y = j["BULLET_SPEED"]["y"].get<float>();
    m_bulletSpeed.z = j["BULLET_SPEED"]["z"].get<float>();
    m_bulletColliderRadius = j["BULLET_COLLIDER_RADIUS"].get<float>();
    m_bulletDamage = j["BULLET_DAMAGE"].get<int>();
    m_bulletMaxTrail = j["BULLET_MAX_TRAIL"].get<int>();
    m_bulletTrailWidth = j["BULLET_TRAIL_WIDTH"].get<float>();
    m_cannonBallReloadTime = j["CANNONBALL_RELOAD_TIME"].get<float>();
    m_cannonBallSurvivalTime = j["CANNONBALL_SURVIVAL_TIME"].get<float>();
    m_cannonBallSpeed.x = j["CANNONBALL_SPEED"]["x"].get<float>();
    m_cannonBallSpeed.y = j["CANNONBALL_SPEED"]["y"].get<float>();
    m_cannonBallSpeed.z = j["CANNONBALL_SPEED"]["z"].get<float>();
    m_cannonBallGravity.x = j["CANNONBALL_GRAVITY"]["x"].get<float>();
    m_cannonBallGravity.y = j["CANNONBALL_GRAVITY"]["y"].get<float>();
    m_cannonBallGravity.z = j["CANNONBALL_GRAVITY"]["z"].get<float>();
    m_cannonBallColliderRadius = j["CANNONBALL_COLLIDER_RADIUS"].get<float>();
    m_cannonBallDamage = j["CANNONBALL_DAMAGE"].get<int>();
    m_cannonBallMaxTrail = j["CANNONBALL_MAX_TRAIL"].get<int>();
    m_cannonBallTrailWidth = j["CANNONBALL_TRAIL_WIDTH"].get<float>();
    m_wallSize.x = j["WALL_SIZE"]["x"].get<float>();
    m_wallSize.y = j["WALL_SIZE"]["y"].get<float>();
    m_wallSize.z = j["WALL_SIZE"]["z"].get<float>();
    m_scoutRadius = j["SCOUT_RADIUS"].get<float>();
    m_attackStartRadius = j["ATTACK_START_RADIUS"].get<float>();
    m_attackFinishRadius = j["ATTACK_FINISH_RADIUS"].get<float>();
    m_fadeSpeed = j["FADE_SPEED"].get<float>();
    m_maxRange = j["MAX_RANGE"].get<float>();
    m_damageEffectTime = j["DAMAGE_EFFECT_TIME"].get<float>();
    const auto& patrolRoutes = j["PATROL_ROUTES"];

    // 巡回ルートの読み込み
    for (const auto& route : patrolRoutes) 
    {
        PatrolRoute routePoints; 
        // 各ポイントを読み込み
        for (const auto& point : route) 
        {
            DirectX::SimpleMath::Vector3 vector;
            vector.x = point["x"].get<float>();
            vector.y = point["y"].get<float>();
            vector.z = point["z"].get<float>();
            routePoints.push_back(vector);
        }
        m_patrolRoutes.push_back(routePoints);
    }
}
