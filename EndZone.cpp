#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "EndZone.h"

using namespace Urho3D;

EndZone::EndZone(Context* context) : Component(context)
{
}

void EndZone::OnNodeSet(Node* node)
{
	if (node)
	{
		CreateBody();
		CreateCollider();
		node_->AddTag("EndZone");
	}
}

void EndZone::CreateBody()
{
	body_ = node_->CreateComponent<RigidBody2D>();
	body_->SetBodyType(BT_STATIC);
}

void EndZone::CreateCollider()
{
	collider_ = node_->CreateComponent<CollisionBox2D>();
	collider_->SetSize(1.0f, 1.0f);
	collider_->SetTrigger(true);
}