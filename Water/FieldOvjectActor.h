#pragma once

#include "Actor.h"

#include <string>

// フィールドにある障害物など
class FieldObjectActor : public Actor
{
public:
    // コンストラクタ
    FieldObjectActor(class Application* a);
    ~FieldObjectActor();
    
    void Create(std::string fileName);
    
    void UpdateActor(float deltaTime) override;

private:
    class MeshComponent* meshComp;
    class ColliderComponent* collComp;
};
