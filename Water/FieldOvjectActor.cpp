#include "FieldOvjectActor.h"
#include "Application.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "ColliderComponent.h"
#include "BoundingVolumeComponet.h"
#include "Mesh.h"


FieldObjectActor::FieldObjectActor(Application* a)
    : Actor(a)
{
    meshComp = new MeshComponent(this);
    collComp = new ColliderComponent(this);
}

FieldObjectActor::~FieldObjectActor()
{
    delete meshComp;
    delete collComp;
}


void FieldObjectActor::Create(std::string fileName)
{
    meshComp->SetMesh(GetApp()->GetRenderer()->GetMesh(fileName));
    
    
    collComp->SetColliderType(C_OBSTACLE);
    collComp->GetBoundingVolume()->ComputeBoundingVolume(GetApp()->GetRenderer()->GetMesh(fileName)->GetVertexArray());
    //collComp->GetBoundingVolume()->AdjustBoundingBox(Vector3(0, 0, 0), Vector3(0.4, 1, 0.3));
    collComp->GetBoundingVolume()->CreateVArray();
}

void FieldObjectActor::UpdateActor(float deltaTime)
{
    Vector3 v = GetPosition();
    
    SetPosition(Vector3(v.x, collComp->GetGroundHeight(), v.z));
}
