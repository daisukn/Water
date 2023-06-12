#pragma once

#include "Component.h"
//#include <cstddef>

// Meshを管理するComponent（Rendererから呼ばれる）
class MeshComponent : public Component
{
public:
    MeshComponent(class Actor* owner,  bool isSkeletal = false);
    ~MeshComponent();
        
    // 描画 override
    virtual void Draw(class Shader* s);
    virtual void SetMesh(class Mesh* m) { mesh = m; }              // メッシュセット
    void SetTextureIndex(unsigned int index) { textureIndex = index; }    // テクスチャGetter

    void SetVisible(bool visible) { bVisible = visible; }
    bool GetVisible() const { return bVisible; }
    
    bool GetIsSkeletal() const { return bSkeletal; }
    class VertexArray* GetVertexArray(int id);
    
    void SetToonRender(const bool t, const float f = 1.0004f) { bToon = t; contourFactor = f; }
    void SetContourFactor(const float f) { contourFactor = f; }
    
protected:
    class Mesh* mesh;      // メッシュ
    unsigned int textureIndex;    // TextureID
    
    bool bVisible;
    bool bSkeletal;
    
    // 輪郭強調
    bool bToon;
    float contourFactor;
    
};

