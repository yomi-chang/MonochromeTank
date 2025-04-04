/*
	@file	Parameter.cpp
	@brief	パラメータークラス
*/
#include "pch.h"
#include "Parameter.h"
#include "Libraries/MyLib/DebugLog.h"
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
    m_playerSpeed{},
    m_playerRotationSpeed{},
    m_enemySpeed{},
    m_enemyRotationSpeed{}
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
    m_turretAngleMin = j["TURRET_ANGLE_MIN"].get<float>();
    m_turretAngleMax = j["TURRET_ANGLE_MAX"].get<float>();
    m_cannonAngleMin = j["CANNON_ANGLE_MIN"].get<float>();
    m_cannonAngleMax = j["CANNON_ANGLE_MAX"].get<float>();
    m_playerSpeed = j["PLAYER_SPEED"].get<float>();
    m_playerRotationSpeed = j["PLAYER_ROTATION_SPEED"].get<float>();
    m_enemySpeed = j["ENEMY_SPEED"].get<float>();
    m_enemyRotationSpeed = j["ENEMY_ROTATION_SPEED"].get<float>();
}