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

#include "Wall.h"

using namespace Urho3D;

const int SPRITE_SIZE_IN_PIXELS = 32;
const float SPRITE_SIZE = SPRITE_SIZE_IN_PIXELS * PIXEL_SIZE;

Wall::Wall(Context* context) : Component(context)
{
}

void Wall::SetSize(Vector2 dimensions)
{
	if (node_)
	{
		node_->SetScale2D(dimensions / SPRITE_SIZE_IN_PIXELS);
	}
}

void Wall::OnNodeSet(Node* node)
{
	if (node)
	{
		CreateBody();
		CreateSprite();
		CreateCollider();
		node_->AddTag("Wall");
	}
}

void Wall::CreateBody()
{
	body_ = node_->CreateComponent<RigidBody2D>();
	body_->SetBodyType(BT_STATIC);
}

void Wall::CreateCollider()
{
	collider_ = node_->CreateComponent<CollisionBox2D>();
	collider_->SetSize(SPRITE_SIZE, SPRITE_SIZE);
	collider_->SetDensity(1.0f);
	collider_->SetFriction(0.0f); // Frictionless
}

void Wall::CreateSprite()
{
	sprite_ = node_->CreateComponent<StaticSprite2D>();
	sprite_->SetSprite(GetSubsystem<ResourceCache>()->GetResource<Sprite2D>("Urho2D/Box.png"));
}