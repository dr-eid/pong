#pragma once

#ifndef PONG_WALL_H
#define PONG_WALL_H

using namespace Urho3D;

class Wall : public Component
{
	URHO3D_OBJECT(Wall, Component);

public:

	Wall(Context* context);
	void SetSize(Vector2 dimensions);

protected:

	virtual void OnNodeSet(Node* node);
	SharedPtr<RigidBody2D> body_;
	SharedPtr<CollisionBox2D> collider_;
	SharedPtr<StaticSprite2D> sprite_;

private:

	void CreateBody();
	void CreateCollider();
	void CreateSprite();
};

#endif