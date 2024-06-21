/*
	@file	CollisionMesh.cpp
	@brief	Obj形式のメッシュをコリジョンにするクラス
*/
#include "pch.h"
#include "CollisionMesh.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//-----------------------------------------------------
// コンストラクタ
//-----------------------------------------------------
mylib::CollisionMesh::CollisionMesh()
	:
	m_position{},
	m_rotation{},
	m_vertices{},
	m_indices{},
	m_triangles{},
	m_inputLayout{},
	m_basicEffect{},
	m_primitiveBatch{}
{
}

//-----------------------------------------------------
// 初期化する
//-----------------------------------------------------
void mylib::CollisionMesh::Initialize(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	const std::wstring& objName
)
{
	assert(device);
	assert(context);

	// Objファイルを読み込む
	this->LoadObjFile(objName);

	// ポリゴン配列を作成する
	for (size_t i = 0; i < m_indices.size() / 3; i++)
	{
		this->AddTriangle(
			m_vertices[m_indices[i * 3 + 0]].position,
			m_vertices[m_indices[i * 3 + 1]].position,
			m_vertices[m_indices[i * 3 + 2]].position
		);
	}

	// 描画オブジェクトを準備する
	m_basicEffect = std::make_unique<BasicEffect>(device);

	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPosition>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPosition>>(context);
}

//-----------------------------------------------------
// 描画する
//-----------------------------------------------------
void mylib::CollisionMesh::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection
)
{
	assert(context);
	assert(states);

	// デバイスコンテキストのパラメータを設定する
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthRead(), 0);	// ステンシルバッファーへ書き込まない
	context->RSSetState(states->Wireframe());
	context->IASetInputLayout(m_inputLayout.Get());

	// ワールド行列を計算する
	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
	Matrix translation = Matrix::CreateTranslation(m_position);
	Matrix world = rotation * translation;

	// ベーシックエフェクトのパラメータを設定する
	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(projection);
	m_basicEffect->Apply(context);

	// メッシュを描画する
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&m_indices[0], m_indices.size(),
		&m_vertices[0], m_vertices.size()
	);
	m_primitiveBatch->End();
}

//-----------------------------------------------------
// メッシュとRayとの衝突判定
//-----------------------------------------------------
//bool mylib::CollisionMesh::IntersectRay(
//	const DirectX::SimpleMath::Ray& ray,		// レイ
//	DirectX::SimpleMath::Vector3* hitPosition,	// 衝突座標
//	DirectX::SimpleMath::Vector3* normal,		// 衝突したポリゴンの法線
//	int* polygonNo								// 衝突したポリゴンの番号
//)
//{
//	assert(hitPosition);
//	assert(normal);
//
//	/*
//		レイにコリジョンメッシュの逆行列をかける
//	*/
//	// コリジョンメッシュの回転や座標情報から、worldの逆行列を求める
//	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
//	Matrix transration = Matrix::CreateTranslation(m_position);
//	Matrix world = rotation * transration;
//	Matrix invertWorld = world.Invert();
//
//	// レイにコリジョンメッシュの逆行列をかける
//	// →レイをコリジョンメッシュのローカル座標系に持っていく
//	Ray unitRay{};
//	unitRay.position = Vector3::Transform(ray.position, invertWorld);
//	unitRay.direction = Vector3::Transform(ray.direction, invertWorld);
//
//	// レイと衝突点までの距離
//	float distance = 0.0f;
//
//	// 衝突判定
//	for (int i = 0; i < m_triangles.size(); i++)
//	{
//		//** ToDo::明らかに衝突しない場合は早期リターンするように変更する
//
//		// レイとポリゴンの衝突判定をとる
//		if (unitRay.Intersects(
//				m_triangles[i].triangle[0],
//				m_triangles[i].triangle[1],
//				m_triangles[i].triangle[2],
//				distance
//			)
//		)
//		{
//			// 衝突点の座標を計算する
//			*hitPosition = Vector3{ unitRay.position + unitRay.direction * distance };
//
//			// 衝突点をワールド座標に変換する
//			*hitPosition = Vector3::Transform(*hitPosition, world);
//
//			// ポリゴンの法線にコリジョンメッシュの回転情報を加味する
//			*normal = Vector3::Transform(m_triangles[i].normal, m_rotation);
//
//			// 衝突したポリゴン番号
//			*polygonNo = i;
//
//			return true;
//		}
//	}
//
//	// 衝突しなかった
//	return false;
//}

//-----------------------------------------------------
// Obj形式のファイルをロードする
//-----------------------------------------------------
void mylib::CollisionMesh::LoadObjFile(const std::wstring& objName)
{
	// Obj形式のファイルを開く
	std::wstring fileName = L"Resources/Models/Obj/" + objName + L".obj";
	std::ifstream ifs(fileName);

	assert(ifs && ".objファイルが読み込めませんでした");

	// 1行ごとに処理する
	std::string line;
	while (getline(ifs, line))
	{
		// 読み取った１行分のデータをストリングストリームで扱う
		std::stringstream iss(line);
		std::string type;

		// 先頭要素を受け取る
		iss >> type;

		// 頂点データを作成する
		// ※Objデータにvt,vnが入る時もあるので注意する
		if (type.compare("v") == 0)
		{
			Vector3 vec;
			iss >> vec.x >> vec.y >> vec.z;
			m_vertices.emplace_back(vec);
		}
		// インデックスデータを作成する
		else if (type.compare("f") == 0)
		{
			int a, b, c;
			iss >> a >> b >> c;

			// 三角形の頂点インデックス番号が同じ物を含む場合は無視する：３点が異なる点を指している
			if (a != b && a != c && b != c)
			{
				/*
					・Objデータのポリゴンの頂点は反時計回りに記録されている
					・左手座標系では頂点は時計回りに指定する
					・DirectXTKでこちらを使用する場合は法線情報が反転するので、注意が必要
				*/
				//m_indices.push_back(static_cast<uint16_t>(a - 1));
				//m_indices.push_back(static_cast<uint16_t>(c - 1));
				//m_indices.push_back(static_cast<uint16_t>(b - 1));

				/*
					・Objデータのポリゴンの頂点は反時計回りに記録されている
					・右手座標系ではそのほうが都合がよいので、そのまま利用する
					・法線を求める際も反転しなくてよい
				*/
				m_indices.emplace_back(static_cast<uint16_t>(a - 1));
				m_indices.emplace_back(static_cast<uint16_t>(b - 1));
				m_indices.emplace_back(static_cast<uint16_t>(c - 1));
			}
		}
	}

	// 開いたファイルを閉じる
	ifs.close();
}

//-----------------------------------------------------
// 三角形ポリゴンを作り、ポリゴン配列に登録する
//-----------------------------------------------------
void mylib::CollisionMesh::AddTriangle(
	DirectX::SimpleMath::Vector3 a,
	DirectX::SimpleMath::Vector3 b,
	DirectX::SimpleMath::Vector3 c
)
{
	m_triangles.emplace_back(Triangle(a, b, c));
}

//-----------------------------------------------------
// 衝突したポリゴンの頂点情報を返す
//-----------------------------------------------------
std::vector<DirectX::SimpleMath::Vector3> mylib::CollisionMesh::GetHitPolygon(unsigned int hitPolygonNo)
{
	std::vector<DirectX::SimpleMath::Vector3> hitPolygon;
	hitPolygon.push_back(m_triangles[hitPolygonNo].triangle[0]);
	hitPolygon.push_back(m_triangles[hitPolygonNo].triangle[1]);
	hitPolygon.push_back(m_triangles[hitPolygonNo].triangle[2]);

	return hitPolygon;
}
