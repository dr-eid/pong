#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D
{
	class Application;
	class Button;
	class Node;
	class Scene;
	class Text;
}

using namespace Urho3D;

class Ball;
class Bat;
class EndZone;

class Pong : public Application
{
public:

	Pong(Context * context);
	virtual void Setup();
	virtual void Start();
	void GameEnd(bool playerOneWon);
	bool GameIsRunning();

private:

	int framecount_;
	float time_;
	SharedPtr<Text> text_;
	SharedPtr<Scene> scene_;
	SharedPtr<Node> boxNode_;
	SharedPtr<Node> cameraNode_;
	SharedPtr<Bat> playerOneBat_;
	SharedPtr<Bat> playerTwoBat_;
	SharedPtr<Ball> ball_;
	SharedPtr<Text> gameEndText_;
	SharedPtr<Text> newGameText_;
	SharedPtr<Text> welcomeText_;
	bool gameRunning_;

	void CreateScene();
	void CreateBall();
	void CreateWalls();
	void CreateWall(String name, Vector2 position, Vector2 dimensions);
	void CreateBats();
	Bat* CreateBat(String name, Vector2 position, Vector2 dimensions);
	void CreateEndZones();
	void CreateEndZone(String name, Vector2 position, Vector2 dimensions);
	Vector2 GetSizeFromGraphicsSize(float fractionWidth, float fractionHeight);
	void CreateCamera();
	void CreateWelcomeText();
	void SetupViewport();
	void StartGame();
	void CreateGameOverText(String winner);
	void RemoveText();
	void CreateInstructions();
	void HandleClosePressed(StringHash eventType, VariantMap & eventData);
	void HandleUpdate(StringHash eventType, VariantMap & eventData);
	void HandlePostRenderUpdate(StringHash eventType, VariantMap & eventData);
};