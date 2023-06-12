#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"


SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
    : MeshComponent(owner, true)
    , animTime(0.0f)
{
}

void SkeletalMeshComponent::SetAnimID(const unsigned int animID, const PlayMode mode)
{
    mesh->SetAnimID(animID, mode);
}

bool SkeletalMeshComponent::GetIsPlaing()
{
    return mesh->GetIsPlaying();
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
    if (mesh)
    {

        // ワールドマトリックスを送る
        shader->SetMatrixUniform("uWorldTransform", owner->GetWorldTransform());

        std::vector<Matrix4> transform;
        mesh->BoneTransform(animTime, transform);

        shader->SetMatrixUniforms("uMatrixPalette", transform.data(), (unsigned int)transform.size());
        shader->SetFloatUniform("uSpecPower", mesh->GetSpecPower());


        // Vertex Arrayを描画
        std::vector<VertexArray*> va = mesh->GetVertexArray();
        for (auto v : va)
        {
            Texture* t = mesh->GetTexture(v->GetTextureID());
            
            if (t)
            {
                t->SetActive();
            }
            
            v->SetActive();
            glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }
        
        if (bToon)
        {
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(contourFactor);
            shader->SetMatrixUniform("uWorldTransform", owner->GetWorldTransform()*m);
            for (auto v : va)
            {
                Texture* t = owner->GetApp()->GetRenderer()->GetTexture("Assets/black.png");
                if (t)
                {
                    t->SetActive();
                }
                v->SetActive();
                glDrawElements(GL_TRIANGLES, v->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            }
            glFrontFace(GL_CCW);
        }
    }
}

void SkeletalMeshComponent::Update(float deltaTime)
{
    animTime = deltaTime;
    
}
