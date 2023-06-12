#pragma once

#include "Math.h"
#include <vector>

/*
// OBB計算用のStruct
struct TCube
{
    Vector3 pos;    // 中心座標
    Vector3 radius; // 半径
    Vector3 rot;    // 回転角度
    Vector3 axisX;  // 分離軸X
    Vector3 axisY;  // 分離軸Y
    Vector3 axisZ;  // 分離軸Z

    void operator = (const TCube& src){
        TCube::pos = src.pos;
        TCube::radius = src.radius;
        TCube::rot = src.rot;
        TCube::axisX = src.axisX;
        TCube::axisY = src.axisY;
        TCube::axisZ = src.axisZ;
    }


};
*/

// 物理計算系のクラス
class PhysWorld
{
public:
    PhysWorld();
    ~PhysWorld();
    
    void SetGroundVertex(class VertexArray* v) { ground = v;  }
    void ComputeGroundHeight();
    
    // 暫定
    void Test();
    
    // コライダーを追加
    void AddCollider(class ColliderComponent* c);
    void RemoveCollider(class ColliderComponent* c);

    
private:
    
///衝突判定系
    bool CompareLengthOBB(const struct OBB* cA, const struct OBB* cB, const Vector3 vSep, const Vector3 vDistance);
    bool JudgeWithOBB(class ColliderComponent* col1, class ColliderComponent* col2);
    bool IsCollideBoxOBB(const OBB* cA, const OBB* cB);

    
    
///地表計算系
    bool IsInPolygon(const struct Polygon* pl, const struct Vector3 p);
    float PolygonHeight(const struct Polygon* pl, const struct Vector3 p);

    // 物理計算をするアクターを保持
    //std::vector<class Actor*> actors;
    std::vector<class ColliderComponent*> colliders;


    // 地表面をVarrayで持つ（WorldTransform未対応）
    class VertexArray* ground;

    
};

