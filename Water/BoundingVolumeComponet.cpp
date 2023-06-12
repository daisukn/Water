#include "BoundingVolumeComponet.h"
#include "Actor.h"
#include "Polygon.h"
#include "VertexArray.h"
#include "Shader.h"

#include "Application.h"
#include "Renderer.h"
#include "Texture.h"

#include "Polygon.h"

#include <vector>

// コンストラクタ
BoundingVolumeComponent::BoundingVolumeComponent(Actor* a)
//    : Component(a)
    : DebuggerComponent(a)
    , radius(0.0f)
    , bVisible(false)
{
    // new するタイミングは要検討
    boundingBox = new Cube();
    obb = new OBB();
    polygons = new Polygon[12];
}

// デストラクタ
BoundingVolumeComponent::~BoundingVolumeComponent()
{
    if(boundingBox) { delete boundingBox; boundingBox = nullptr; }
    if(obb) { delete obb; obb = nullptr; }
    if(polygons) { delete [] polygons; polygons = nullptr; }
}


void BoundingVolumeComponent::OnUpdateWorldTransform()
{
    // バウンディングボックスパラメータをセット
    obb->pos = owner->GetPosition();
    float sc1 = owner->GetScale();
    obb->max = boundingBox->max * sc1;
    obb->min = boundingBox->min * sc1;
    
    Quaternion q1 = owner->GetRotation();
    Matrix4 mRot1 = Matrix4::CreateFromQuaternion(q1);
      
    obb->axisX = mRot1.GetXAxis();
    obb->axisY = mRot1.GetYAxis();
    obb->axisZ = mRot1.GetZAxis();

    // 半径を設定
    obb->radius = //Vector3::Dot(comp1->GetBoundingBox()->max, comp1->GetBoundingBox()->min);
              Vector3((fabsf(obb->max.x) + fabsf(obb->min.x)) / 2,
                        (fabsf(obb->max.y) + fabsf(obb->min.y)) / 2,
                        (fabsf(obb->max.z) + fabsf(obb->min.z)) / 2);
    
      // 角度、AXIS、ちょっと怪しいかも。。。
    obb->rot = Vector3(obb->axisX.x, obb->axisY.y, obb->axisZ.z);
    
    
}

// バウンディングボリュームを計算
void BoundingVolumeComponent::ComputeBoundingVolume(std::vector<VertexArray *> va)
{
    
    // 複数VertexArrayをまとめて計算
    for ( auto v : va )
    {
        std::vector<Polygon*> pl = v->GetPolygon();
        for ( auto p : pl)
        {
            // xの最小を探す
            if( p->a.x < boundingBox->min.x ) boundingBox->min.x = p->a.x;
            if( p->b.x < boundingBox->min.x ) boundingBox->min.x = p->b.x;
            if( p->c.x < boundingBox->min.x ) boundingBox->min.x = p->c.x;

            // xの最大を探す
            if( p->a.x > boundingBox->max.x ) boundingBox->max.x = p->a.x;
            if( p->b.x > boundingBox->max.x ) boundingBox->max.x = p->b.x;
            if( p->c.x > boundingBox->max.x ) boundingBox->max.x = p->c.x;
            
            // yの最小を探す
            if( p->a.y < boundingBox->min.y ) boundingBox->min.y = p->a.y;
            if( p->b.y < boundingBox->min.y ) boundingBox->min.y = p->b.y;
            if( p->c.y < boundingBox->min.y ) boundingBox->min.y = p->c.y;

            // yの最大を探す
            if( p->a.y > boundingBox->max.y ) boundingBox->max.y = p->a.y;
            if( p->b.y > boundingBox->max.y ) boundingBox->max.y = p->b.y;
            if( p->c.y > boundingBox->max.y ) boundingBox->max.y = p->c.y;

            // xの最小を探す
            if( p->a.z < boundingBox->min.z ) boundingBox->min.z = p->a.z;
            if( p->b.z < boundingBox->min.z ) boundingBox->min.z = p->b.z;
            if( p->c.z < boundingBox->min.z ) boundingBox->min.z = p->c.z;

            // xの最大を探す
            if( p->a.z > boundingBox->max.z ) boundingBox->max.z = p->a.z;
            if( p->b.z > boundingBox->max.z ) boundingBox->max.z = p->b.z;
            if( p->c.z > boundingBox->max.z ) boundingBox->max.z = p->c.z;
            
        }
        
    }
    
    CreateVArray();
    CreatePolygons();
    
}

// ポリゴンデータを生成
void BoundingVolumeComponent::CreatePolygons()
{


    
    // 0, 1, 3
    polygons[0].a = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z);
    polygons[0].b = Vector3(boundingBox->min.x, boundingBox->max.y, boundingBox->min.z);
    polygons[0].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);
    
    // 3, 2, 0
    polygons[1].a = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);
    polygons[1].b = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->min.z);
    polygons[1].c = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z);
    
    // 4, 2, 3
    polygons[2].a = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->max.z);
    polygons[2].b = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->min.z);
    polygons[2].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);
    
    // 3, 5, 4
    polygons[3].a = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);
    polygons[3].b = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    polygons[3].c = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->max.z);
    
    // 5, 4, 6
    polygons[4].a = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    polygons[4].b = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->max.z);
    polygons[4].c = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->max.z);
    
    // 6, 7, 5
    polygons[5].a = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->max.z);
    polygons[5].b = Vector3(boundingBox->min.x, boundingBox->max.y, boundingBox->max.z);
    polygons[5].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    
    // 0, 6, 7
    polygons[6].a = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z);
    polygons[6].b = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->max.z);
    polygons[6].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    
    // 7. 1. 0
    polygons[7].a = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    polygons[7].b = Vector3(boundingBox->min.x, boundingBox->max.y, boundingBox->min.z);
    polygons[7].c = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z);
    
    // 3, 1, 7
    polygons[8].a = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);
    polygons[8].b = Vector3(boundingBox->min.x, boundingBox->max.y, boundingBox->min.z);
    polygons[8].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    
    // 7, 5, 3
    polygons[9].a = Vector3(boundingBox->min.x, boundingBox->max.y, boundingBox->max.z);
    polygons[9].b = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->max.z);
    polygons[9].c = Vector3(boundingBox->max.x, boundingBox->max.y, boundingBox->min.z);

    // 2, 4, 6
    polygons[10].a = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->min.z);
    polygons[10].b = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->max.z);
    polygons[10].c = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->max.z);
    
    // 6, 0, 2
    polygons[11].a = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->max.z);
    polygons[11].b = Vector3(boundingBox->min.x, boundingBox->min.y, boundingBox->min.z);
    polygons[11].c = Vector3(boundingBox->max.x, boundingBox->min.y, boundingBox->min.z);
}


// Min Maxを直接指定
void BoundingVolumeComponent::ComputeBoundingVolume(const Vector3 min, const Vector3 max)
{
    boundingBox->min = min;
    boundingBox->max = max;
}

// バウンディングボックスを手動調整する（場所とサイズ（倍率））
void BoundingVolumeComponent::AdjustBoundingBox(const Vector3 pos, const Vector3 sc)
{
    
    boundingBox->max += pos;
    boundingBox->min += pos;
    
    boundingBox->max.x *= sc.x;
    boundingBox->min.x *= sc.x;
    boundingBox->max.y *= sc.y;
    boundingBox->min.y *= sc.y;
    boundingBox->max.z *= sc.z;
    boundingBox->min.z *= sc.z;

}




// デバッグ用
void BoundingVolumeComponent::CreateVArray()
{
    // ボックス用頂点バッファ（座標、法線、UV）
    float BBverts[] =
    {
        boundingBox->min.x, boundingBox->min.y, boundingBox->min.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 0
        boundingBox->min.x, boundingBox->max.y, boundingBox->min.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 1
        boundingBox->max.x, boundingBox->min.y, boundingBox->min.z, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 2
        boundingBox->max.x, boundingBox->max.y, boundingBox->min.z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 3
        boundingBox->max.x, boundingBox->min.y, boundingBox->max.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 4
        boundingBox->max.x, boundingBox->max.y, boundingBox->max.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 5
        boundingBox->min.x, boundingBox->min.y, boundingBox->max.z, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 6
        boundingBox->min.x, boundingBox->max.y, boundingBox->max.z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f  // 7
        
    };
    
    unsigned int BBindex[] = {
        0, 1, 3,
        3, 2, 0,
        
        4, 2, 3,
        3, 5, 4,
        
        5, 4, 6,
        6, 7, 5,
        
        0, 6, 7,
        7, 1, 0,
        
        3, 1, 7,
        7, 5, 3,
        
        2, 4, 6,
        6, 0, 2
        

    };
    
    
    vArray = new VertexArray(BBverts, 8, (unsigned int*)BBindex, (unsigned int)36);
    // マテリアル非対応なのでTextureで代用
    texture = owner->GetApp()->GetRenderer()->GetTexture("Assets/BoundingVolume.png");
    
    bVisible = true;
}


// バウンディングボリューム表示（ワイヤフレーム）
void BoundingVolumeComponent::Draw(Shader* shader)
{
    if (bVisible)
    {
        // WorldマトリックスをShaderに送る
        shader->SetMatrixUniform("uWorldTransform", owner->GetWorldTransform());

        // SpecPowerを送る
        shader->SetFloatUniform("uSpecPower", 100);


        texture->SetActive();
            
        vArray->SetActive();
        glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, nullptr);
    }
}
