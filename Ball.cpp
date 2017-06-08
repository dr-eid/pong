#include <Box2D/Box2D.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include <Urho3D/Core/CoreEvents.h>

#include "Ball.h"

using namespace Urho3D;

const int SPRITE_SIZE_IN_PIXELS = 32;
const float SPRITE_SIZE = SPRITE_SIZE_IN_PIXELS * PIXEL_SIZE;
const float INITIAL_BALL_SPEED = 4.0f;

Ball::Ball(Context* context) : Component(context)
{
}

void Ball::SetLinearVelocity(Vector2 velocity)
{
	if (body_)
	{
		body_->SetLinearVelocity(velocity);
	}
}

void Ball::OnNodeSet(Node* node)
{
	if (node)
	{
		CreateBody();
		CreateCollider();
		CreateSprite();
		SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, URHO3D_HANDLER(Ball, HandleNodeCollision));
	}
}

void Ball::Reset()
{
	// Move the ball to the origin. Just setting the node's position did not seem to
	// work, as the old position was still stored in the rigid body component's 
	// Box 2D internals and overwrote the new position in the next physics update.
	body_->GetBody()->SetTransform(b2Vec2(0.0f, 0.0f), 0.0f);

	// Set the ball moving in a random one of the four diagonals.
	Vector2 ballVelocity = Vector2::UP * INITIAL_BALL_SPEED;
	Vector3 rotatedVelocity = (Quaternion(45.0f + Random(0, 4) * 90.0f) * Vector3(ballVelocity));
	ballVelocity = Vector2(rotatedVelocity.x_, rotatedVelocity.y_);
	body_->SetLinearVelocity(ballVelocity);

	// Enable the node, as it may have been disabled at the end a prior game.
	node_->SetEnabledRecursive(true);
}

void Ball::CreateBody()
{
	body_ = node_->CreateComponent<RigidBody2D>();

	// Use a dynamic body type, as the ball needs to interact with static and
	// kinematic bodies (the walls and bat respectively).
	body_->SetBodyType(BT_DYNAMIC);

	// Static by default.
	body_->SetLinearVelocity(Vector2::ZERO);
	body_->SetAngularVelocity(0.0f);

	// No damping or gravity.
	body_->SetLinearDamping(0.0f);
	body_->SetAngularDamping(0.0f);
	body_->SetGravityScale(0.0f);
}


void Ball::CreateCollider()
{
	collider_ = node_->CreateComponent<CollisionCircle2D>();
	collider_->SetRadius(SPRITE_SIZE / 2.0f);

	// This needs to be a trigger collider, so that collisions can be handled
	// manually, rather than by the physics engine.
	collider_->SetTrigger(true);
}

void Ball::CreateSprite()
{
	sprite_ = node_->CreateComponent<StaticSprite2D>();
	sprite_->SetSprite(GetSubsystem<ResourceCache>()->GetResource<Sprite2D>("Urho2D/Ball.png"));
}

/// If the collision is with a bat or wall, reflect off it. If it is a
/// collision with an end zone, make the game end.
void Ball::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
	using namespace PhysicsBeginContact2D;

	auto bodyA = static_cast<RigidBody2D*>(eventData[P_BODYA].GetPtr());
	auto bodyB = static_cast<RigidBody2D*>(eventData[P_BODYB].GetPtr());
	Vector2 velocity = body_->GetLinearVelocity();
	Node* otherNode = NULL;

	if (bodyA == body_)
	{
		otherNode = bodyB->GetNode();
	}
	else if (bodyB == body_)
	{
		otherNode = bodyA->GetNode();
	}

	if (otherNode)
	{
		if (otherNode->HasTag("EndZone"))
		{
			node_->SetEnabledRecursive(false);
			bool playerOneIsWinner = otherNode->GetName() == "PlayerTwoEndZone";
			game_->GameEnd(playerOneIsWinner);
		}
		else
		{
			if (otherNode->HasTag("Bat"))
			{
				// Reflect horizontally, a speed up slightly.
				velocity.x_ *= -1;
				velocity *= 1.03f;
			}
			else if (otherNode->HasTag("Wall"))
			{
				// Reflect vertically.
				velocity.y_ *= -1;
			}
			SetLinearVelocity(velocity);
		}
	}
}