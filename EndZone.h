#pragma once

#ifndef PONG_END_ZONE_H
#define PONG_END_ZONE_H

using namespace Urho3D;

class EndZone : public Component
{
	URHO3D_OBJECT(EndZone, Component);

public:

	EndZone(Context* context);

protected:

	virtual void OnNodeSet(Node* node);
	SharedPtr<RigidBody2D> body_;
	SharedPtr<CollisionBox2D> collider_;

private:

	void CreateCollider();
	void CreateBody();
};

#endif