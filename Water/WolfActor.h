#pragma once

#include "Actor.h"



class WolfActor : public Actor
{
public:
    WolfActor(class Application* app);
    
    // Update Override
    void UpdateActor(float deltaTime) override;
    
private:
    class MoveComponent* moveComp;
    class SkeletalMeshComponent* meshComp;
    class ColliderComponent* collComp;
    
    //class BoundingVolumeComponent* boundingVolume;
};
