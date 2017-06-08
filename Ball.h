#pragma once

#include "Pong.h"

namespace Urho3D
{
	class CollisionCircle2D;
	class StaticSprite2D;
}

class Ball : public Component
{
	URHO3D_OBJECT(Ball, Component);

public:

	SharedPtr<Pong> game_;
	Ball(Context* context);
	void SetLinearVelocity(Vector2 velocity);
	void Reset();

protected:

	virtual void OnNodeSet(Node* node);
	SharedPtr<RigidBody2D> body_;
	SharedPtr<CollisionCircle2D> collider_;
	SharedPtr<StaticSprite2D> sprite_;

private:

	void CreateBody();
	void CreateCollider();
	void CreateSprite();
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
};