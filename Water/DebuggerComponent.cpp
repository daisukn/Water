#include "DebuggerComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


DebuggerComponent::DebuggerComponent(Actor* a)
    : Component(a)
{
    owner->GetApp()->GetRenderer()->AddDebuggerComp(this);
}

DebuggerComponent::~DebuggerComponent()
{
    owner->GetApp()->GetRenderer()->RemoveDebuggerComp(this);
}
