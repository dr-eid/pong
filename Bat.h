#pragma once


namespace Urho3D
{
	class CollisionBox2D;
}

using namespace Urho3D;

class Bat : public Component
{
	URHO3D_OBJECT(Bat, Component);

public:

	Bat(Context* context);
	void SetSize(Vector2 dimensions);
	void SetVelocity(Vector2 velocity);

protected:

	virtual void OnNodeSet(Node* node);
	SharedPtr<RigidBody2D> body_;
	SharedPtr<CollisionBox2D> collider_;
	SharedPtr<StaticSprite2D> sprite_;

private:

	void CreateBody();
	void CreateCollider();
	void CreateSprite();
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
	void MoveOutOfWall(RigidBody2D * wallBody);
};
