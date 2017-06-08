#include <Box2D/Box2D.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "Bat.h"

using namespace Urho3D;

const int SPRITE_SIZE_IN_PIXELS = 32;
const float SPRITE_SIZE = SPRITE_SIZE_IN_PIXELS * PIXEL_SIZE;

Bat::Bat(Context* context) : Component(context)
{
}

void Bat::SetSize(Vector2 dimensions)
{
	if (node_)
	{
		node_->SetScale2D(dimensions / SPRITE_SIZE_IN_PIXELS);
	}
}

void Bat::SetVelocity(Vector2 velocity)
{
	node_->GetComponent<RigidBody2D>()->SetLinearVelocity(velocity);
}

void Bat::OnNodeSet(Node* node)
{
	if (node)
	{
		CreateBody();
		CreateCollider();
		CreateSprite();
		node_->AddTag("Bat");
		SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, URHO3D_HANDLER(Bat, HandleNodeCollision));
	}
}

void Bat::CreateBody()
{
	body_ = node_->CreateComponent<RigidBody2D>();
	body_->SetBodyType(BT_DYNAMIC);
	body_->SetGravityScale(0.0f);
}

void Bat::CreateCollider()
{
	collider_ = node_->CreateComponent<CollisionBox2D>();
	collider_->SetSize(SPRITE_SIZE, SPRITE_SIZE);
	collider_->SetTrigger(true);
}

void Bat::CreateSprite()
{
	sprite_ = node_->CreateComponent<StaticSprite2D>();
	sprite_->SetSprite(GetSubsystem<ResourceCache>()->GetResource<Sprite2D>("Urho2D/Box.png"));
}

void Bat::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
	using namespace PhysicsBeginContact2D;

	auto bodyA = static_cast<RigidBody2D*>(eventData[P_BODYA].GetPtr());
	auto bodyB = static_cast<RigidBody2D*>(eventData[P_BODYB].GetPtr());
	auto contact = static_cast<b2Contact*>(eventData[P_CONTACT].GetVoidPtr());

	if (bodyB == body_)
	{
		body_->SetLinearVelocity(Vector2::ZERO);
		MoveOutOfWall(bodyA);
	}
}

/// Move the bat just away from the wall body so that it is not quite
/// touching it.
void Bat::MoveOutOfWall(RigidBody2D* wallBody)
{	
	Node* batNode = body_->GetNode();
	Vector2 batPosition = body_->GetNode()->GetWorldPosition2D();
	float batHeight = body_->GetComponent<CollisionBox2D>()->GetSize().y_ * batNode->GetWorldScale2D().y_;
	float wallBodyY = wallBody->GetNode()->GetWorldPosition2D().y_;
	float wallBodyHeight = wallBody->GetComponent<CollisionBox2D>()->GetSize().y_ * wallBody->GetNode()->GetWorldScale2D().y_;

	// Add on a delta to the offset, as otherwise the trigger collider won't
	// fire again until the bat moves away, allowing the bat to pass through
	// the wall.
	float delta = 0.025f;
	float offsetSize = (batHeight + wallBodyHeight) / 2 + delta;
	bool batIsAboveWall = batPosition.y_ > wallBodyY;
	float correctedBatY = wallBodyY + (batIsAboveWall ? 1 : -1) * offsetSize;
	auto correctedBatPosition = Vector2(batPosition.x_, correctedBatY);
	body_->GetNode()->SetWorldPosition2D(correctedBatPosition);
}
