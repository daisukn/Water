
#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

#include <GL/glew.h>
#include <vector>

// コンストラクタ
MeshComponent::MeshComponent(Actor* a, bool isSkeletal)
    : Component(a)
    , mesh(nullptr)
    , textureIndex(0)
    , bVisible(true)
    , bSkeletal(isSkeletal)
    , bToon(false)
    , contourFactor(1.0004f)
{
    // Rendererに追加
	owner->GetApp()->GetRenderer()->AddMeshComp(this);
}

// デストラクタ
MeshComponent::~MeshComponent()
{
    // Rendererから削除
	owner->GetApp()->GetRenderer()->RemoveMeshComp(this);
}

// 描画
void MeshComponent::Draw(Shader* s)
{
	if (mesh)
	{
        // WorldマトリックスをShaderに送る
		s->SetMatrixUniform("uWorldTransform", owner->GetWorldTransform());

		// SpecPowerを送る
        s->SetFloatUniform("uSpecPower", mesh->GetSpecPower());


		// Vertex Array
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
        
        // 輪郭強調用
        if (bToon)
        {
            glFrontFace(GL_CW);
            Matrix4 m = Matrix4::CreateScale(contourFactor);
            s->SetMatrixUniform("uWorldTransform", owner->GetWorldTransform()*m);
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


VertexArray* MeshComponent::GetVertexArray(int id)
{
    return mesh->GetVertexArray()[id];
}

