#include <string>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h> 
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Urho2D/Drawable2D.h>
#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>

#include "Ball.h"
#include "Bat.h"
#include "EndZone.h"
#include "Wall.h"
#include "Pong.h"

using namespace Urho3D;

const float BAT_SPEED = 4.0f;
const float BAT_OFFSET = 4.0f;

Pong::Pong(Context * context) : Application(context), framecount_(0), time_(0)
{
	context->RegisterFactory<Ball>();
	context->RegisterFactory<Bat>();
	context->RegisterFactory<Wall>();
	context->RegisterFactory<EndZone>();
	gameRunning_ = false;
	SetRandomSeed(Time::GetSystemTime());
}

void Pong::Setup()
{
    engineParameters_["FullScreen"] = false;
	engineParameters_["WindowTitle"] = "Pong";
}

void Pong::Start()
{
	// The game of Pong does not begin until Enter is pressed.
	gameRunning_ = false;

	CreateWelcomeText();
	CreateScene();
	SetupViewport();
		
    SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(Pong,HandleUpdate));
}

void Pong::CreateScene()
{
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();
	scene_->CreateComponent<PhysicsWorld2D>();

	CreateCamera();
	CreateWalls();
	CreateBats();
	CreateEndZones();
	CreateBall();
}

void Pong::CreateCamera()
{
	cameraNode_ = scene_->CreateChild("Camera");
	cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

	Camera* camera = cameraNode_->CreateComponent<Camera>();
	camera->SetOrthographic(true);

	Graphics* graphics = GetSubsystem<Graphics>();
	camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
	camera->SetZoom(Min((float)graphics->GetWidth() / 1280.0f, (float)graphics->GetHeight() / 800.0f));
}

void Pong::CreateWalls()
{
	auto wallDimensions = GetSizeFromGraphicsSize(0.9f, 0.02f);
	CreateWall("BottomWall", Vector2(0.0f, -3.0f), wallDimensions);
	CreateWall("TopWall", Vector2(0.0f, 3.0f), wallDimensions);
}

void Pong::CreateWall(String name, Vector2 position, Vector2 dimensions)
{
	Node* wallNode = scene_->CreateChild(name);
	wallNode->SetPosition2D(position);

	Wall* wall = wallNode->CreateComponent<Wall>();
	wall->SetSize(dimensions);
}

void Pong::CreateBats()
{
	auto batDimensions = GetSizeFromGraphicsSize(0.012f, 0.1f);
	playerOneBat_ = CreateBat("PlayerOne", Vector2(-BAT_OFFSET, 0.0f), batDimensions);
	playerTwoBat_ = CreateBat("PlayerTwo", Vector2(BAT_OFFSET, 0.0f), batDimensions);
}

Bat* Pong::CreateBat(String name, Vector2 position, Vector2 dimensions)
{
	Node* batNode = scene_->CreateChild(name);
	batNode->SetPosition(position);

	Bat* bat = batNode->CreateComponent<Bat>();
	bat->SetSize(dimensions);

	return bat;
}

void Pong::CreateEndZones()
{
	auto endZoneDimensions = GetSizeFromGraphicsSize(0.008f, 0.9f);

	// The end zones should be slightly behind the bats, so that collisions
	// with the bat don't result in collisions with the end zones.
	CreateEndZone("PlayerOneEndZone", Vector2(-(BAT_OFFSET + 0.1f), 0.0f), endZoneDimensions);
	CreateEndZone("PlayerTwoEndZone", Vector2(BAT_OFFSET + 0.1f, 0.0f), endZoneDimensions);
}

void Pong::CreateEndZone(String name, Vector2 position, Vector2 dimensions)
{
	Node* endZoneNode = scene_->CreateChild(name);
	endZoneNode->SetPosition(position);
	endZoneNode->SetScale(dimensions * PIXEL_SIZE);
	endZoneNode->CreateComponent<EndZone>();
}

Vector2 Pong::GetSizeFromGraphicsSize(float fractionWidth, float fractionHeight)
{
	auto graphics = GetSubsystem<Graphics>();
	float width = fractionWidth * graphics->GetWidth();
	float height = fractionHeight * graphics->GetHeight();
	return Vector2(width, height);
}

void Pong::CreateBall()
{
	Node* ballNode = scene_->CreateChild("Ball");
	ballNode->SetScale2D(Vector2(1.0f, 1.0f));
	ball_ = ballNode->CreateComponent<Ball>();
	ball_->game_ = this;
	ballNode->SetEnabledRecursive(false);
}

void Pong::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}

void Pong::StartGame()
{
	// Clear any text displayed previously.
	RemoveText();

	// Move the ball back to the centre and set it moving.
	ball_->Reset();
	gameRunning_ = true;
}

void Pong::GameEnd(bool playerOneWon)
{
	gameRunning_ = false;
	String winner = playerOneWon ? "one" : "two";
	CreateGameOverText(winner);
}

bool Pong::GameIsRunning()
{
	return gameRunning_;
}

void Pong::CreateWelcomeText()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	Graphics* graphics = GetSubsystem<Graphics>();
	
	welcomeText_ = GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
	welcomeText_->SetText("PONG");
	welcomeText_->SetFont(font, 60);
	welcomeText_->SetHorizontalAlignment(HA_CENTER);
	welcomeText_->SetVerticalAlignment(VA_CENTER);

	auto pressEnterText = welcomeText_->CreateChild<Text>();
	pressEnterText->SetText("Press ENTER to start\nESC to exit");
	pressEnterText->SetFont(font, 20);
	pressEnterText->SetHorizontalAlignment(HA_CENTER);
	pressEnterText->SetPosition(0, 100);
	pressEnterText->SetTextAlignment(HA_CENTER);
	
	auto playerOneText = welcomeText_->CreateChild<Text>();
	playerOneText->SetText("Player One\nW & S");
	playerOneText->SetFont(font, 20);
	playerOneText->SetPosition(IntVector2(-graphics->GetWidth() / 2 + 110, 0));
	playerOneText->SetTextAlignment(HA_CENTER);
	
	auto playerTwoText = welcomeText_->CreateChild<Text>();
	playerTwoText->SetText("Player Two\nUp & Down");
	playerTwoText->SetFont(font, 20);
	playerTwoText->SetPosition(IntVector2(graphics->GetWidth() / 2 - 80, 0));
	playerTwoText->SetTextAlignment(HA_CENTER);
}

void Pong::CreateGameOverText(String winner)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
	String gameEndMessage = "Player " + winner + " wins!";

	gameEndText_ = ui->GetRoot()->CreateChild<Text>();
	gameEndText_->SetText(gameEndMessage);
	gameEndText_->SetFont(font, 32);
	gameEndText_->SetHorizontalAlignment(HA_CENTER);
	gameEndText_->SetVerticalAlignment(VA_CENTER);
	gameEndText_->SetTextAlignment(HA_CENTER);

	newGameText_ = ui->GetRoot()->CreateChild<Text>();
	newGameText_->SetText("Press ENTER to start\nESC to exit");
	newGameText_->SetFont(font, 20);
	newGameText_->SetHorizontalAlignment(HA_CENTER);
	newGameText_->SetVerticalAlignment(VA_CENTER);
	newGameText_->SetPosition(0, 100);
	newGameText_->SetTextAlignment(HA_CENTER);

}

void Pong::RemoveText()
{
	if (welcomeText_)
	{
		welcomeText_->Remove();
	}
	if (gameEndText_)
	{
		gameEndText_->Remove();
	}
	if (newGameText_)
	{
		newGameText_->Remove();
	}
}
    
void Pong::HandleUpdate(StringHash eventType,VariantMap& eventData)
{
	Input* input = GetSubsystem<Input>();

	// Player one input
	if (input->GetKeyDown(KEY_W))
	{
		playerOneBat_->SetVelocity(Vector2::UP * BAT_SPEED);
	}
	else if (input->GetKeyDown(KEY_S))
	{
		playerOneBat_->SetVelocity(Vector2::DOWN * BAT_SPEED);
	}
	else
	{
		playerOneBat_->SetVelocity(Vector2::ZERO);
	}

	// Player two input
	if (input->GetKeyDown(KEY_UP))
	{
		playerTwoBat_->SetVelocity(Vector2::UP * BAT_SPEED);
	}
	else if (input->GetKeyDown(KEY_DOWN))
	{
		playerTwoBat_->SetVelocity(Vector2::DOWN * BAT_SPEED);
	}
	else
	{
		playerTwoBat_->SetVelocity(Vector2::ZERO);
	}

	// Start / restart
	if (input->GetKeyPress(KEY_RETURN) && !gameRunning_)
	{
		StartGame();
	}

	// Exit
	if (input->GetKeyDown(KEY_ESCAPE))
	{
		engine_->Exit();
	}
}

void Pong::HandleClosePressed(StringHash eventType, VariantMap& eventData)
{
	engine_->Exit();
}

URHO3D_DEFINE_APPLICATION_MAIN(Pong)