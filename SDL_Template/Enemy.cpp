#include "Enemy.h"

std::vector<std::vector<Vector2>> Enemy::sPaths;
Formation * Enemy::sFormation = nullptr;

void Enemy::CreatePaths() {
	int screenMidPoint = (int)(Graphics::Instance()->SCREEN_WIDTH * 0.4f);

	int currentPath = 0;
	BezierPath * path = new BezierPath();
	path->AddCurve({
		Vector2(screenMidPoint + 50.0f, -10.0f),
		Vector2(screenMidPoint + 50.0f, -20.0f),
		Vector2(screenMidPoint + 50.0f, 30.0f),
		Vector2(screenMidPoint + 50.0f, 20.0f) }, 1);
	path->AddCurve({
		Vector2(screenMidPoint + 50.0f, 20.0f),
		Vector2(screenMidPoint + 50.0f, 100.0f),
		Vector2(75.0f, 325.0f),
		Vector2(75.0f, 425.0f) }, 25);
	path->AddCurve({
		Vector2(75.0f, 425.0f),
		Vector2(75.0f, 650.0f),
		Vector2(350.0f, 650.0f),
		Vector2(350.0f, 425.0f) }, 25);

	sPaths.push_back(std::vector<Vector2>());
	path->Sample(&sPaths[currentPath]);
	delete path;

	currentPath = 1;
	path = new BezierPath();
	int fullScreen = screenMidPoint * 2;
	path->AddCurve({
		Vector2(screenMidPoint - 50.0f, -10.0f),
		Vector2(screenMidPoint - 50.0f, -20.0f),
		Vector2(screenMidPoint - 50.0f, 30.0f),
		Vector2(screenMidPoint - 50.0f, 20.0f) }, 1);
	path->AddCurve({
		Vector2(screenMidPoint - 50.0f, 20.0f),
		Vector2(screenMidPoint - 50.0f, 100.0f),
		Vector2(fullScreen - 75.0f, 325.0f),
		Vector2(fullScreen - 75.0f, 425.0f) }, 25);
	path->AddCurve({
		Vector2(fullScreen - 75.0f, 425.0f),
		Vector2(fullScreen - 75.0f, 650.0f),
		Vector2(fullScreen - 350.0f, 650.0f),
		Vector2(fullScreen - 350.0f, 425.0f) }, 25);

	sPaths.push_back(std::vector<Vector2>());
	path->Sample(&sPaths[currentPath]);
	delete path;

	currentPath = 2;
	int temp = screenMidPoint - 100.0f;

	path = new BezierPath();
	path->AddCurve({
		Vector2(-40.0f, 720.0f),
		Vector2(-50.0f, 720.0f),
		Vector2(10.0f, 720.0f),
		Vector2(0.0f, 720.0f) }, 1);
	path->AddCurve({
		Vector2(0.0f, 720.0f),
		Vector2(200.0f, 720.0f),
		Vector2(temp, 500.0f),
		Vector2(temp, 400.0f) }, 15);
	path->AddCurve({
		Vector2(temp, 400.0f),
		Vector2(temp, 200.0f),
		Vector2(40.0f, 200.0f),
		Vector2(40.0f, 400.0f) }, 15);
	path->AddCurve({
		Vector2(40.0f, 400.0f),
		Vector2(40.0f, 500.0f),
		Vector2(temp - 120.0f, 600.0f),
		Vector2(temp - 40.0f, 440.0f) }, 15);

	sPaths.push_back(std::vector<Vector2>());
	path->Sample(&sPaths[currentPath]);
	delete path;

	currentPath = 3;
	temp = screenMidPoint + 60.0f;
	int temp2 = fullScreen - 40.0f;

	path = new BezierPath();
	path->AddCurve({
		Vector2(temp2 + 40.0f, 720.0f),
		Vector2(temp2 + 50.0f, 720.0f),
		Vector2(temp2 + 10.0f, 720.0f),
		Vector2(temp2, 720.0f) }, 1);
	path->AddCurve({
		Vector2(temp2, 720.0f),
		Vector2(temp2 - 200.0f, 720.0f),
		Vector2(temp, 500.0f),
		Vector2(temp, 400.0f) }, 15);
	path->AddCurve({
		Vector2(temp, 400.0f),
		Vector2(temp, 200.0f),
		Vector2(temp2 - 40.0f, 200.0f),
		Vector2(temp2 - 40.0f, 400.0f) }, 15);
	path->AddCurve({
		Vector2(temp2 - 40.0f, 400.0f),
		Vector2(temp2 - 40.0f, 500.0f),
		Vector2(temp + 120.0f, 600.0f),
		Vector2(temp + 40.0f, 440.0f) }, 15);

	sPaths.push_back(std::vector<Vector2>());
	path->Sample(&sPaths[currentPath]);
	delete path;
}

void Enemy::SetFormation(Formation * formation) {
	sFormation = formation;
}

Enemy::Enemy(int path, int index, bool challenge)
	: mCurrentPath(path), mIndex(index), mChallengeStage(challenge) {
	mTimer = Timer::Instance();

	mCurrentState = FlyIn;

	mCurrentWaypoint = 1;
	Position(sPaths[mCurrentPath][0]);

	mTextures[0] = nullptr;
	mTextures[1] = nullptr;

	mSpeed = 400.0f;
}

Enemy::~Enemy() {
	mTimer = nullptr;

	for (int i = 0; i < 2; i++) {
		delete mTextures[i];
		mTextures[i] = nullptr;
	}
}

void Enemy::PathComplete() {
	if (mChallengeStage) {
		mCurrentState = Dead;
	}
}

void Enemy::JoinFormation() {
	Position(WorldFormationPosition());
	Rotation(0);
	Parent(sFormation);
	mCurrentState = InFormation;
}

Vector2 Enemy::WorldFormationPosition() {
	return sFormation->Position() + LocalFormationPosition();
}

void Enemy::FlyInComplete() {
	if (mChallengeStage) {
		mCurrentState = Dead;
	}
	else {
		JoinFormation();
	}
}

void Enemy::HandleFlyInState() {
	if (mCurrentWaypoint < sPaths[mCurrentPath].size()) {
		Vector2 dist = sPaths[mCurrentPath][mCurrentWaypoint] - Position();

		Translate(dist.Normalized() * mSpeed * mTimer->DeltaTime(), World);
		Rotation(atan2(dist.y, dist.x) * RAD_TO_DEG + 90.0f);

		if ((sPaths[mCurrentPath][mCurrentWaypoint] - Position()).MagnitudeSqr() < EPSILON * mSpeed / 25.0f) {
			mCurrentWaypoint++;
		}

		if (mCurrentWaypoint >= sPaths[mCurrentPath].size()) {
			PathComplete();
		}
	}
	else {
		Vector2 dist = WorldFormationPosition() - Position();

		Translate(dist.Normalized() * mSpeed * mTimer->DeltaTime(), World);
		Rotation(atan2(dist.y, dist.x) * RAD_TO_DEG + 90.0f);

		if (dist.MagnitudeSqr() < EPSILON * mSpeed / 25.0f) {
			FlyInComplete();
		}
	}
}

void Enemy::HandleFormationState() {
	Position(LocalFormationPosition());
}

void Enemy::HandleStates() {
	switch (mCurrentState) {
	case FlyIn:
		HandleFlyInState();
		break;
	case InFormation:
		HandleFormationState();
		break;
	case Diving:
		HandleDiveState();
		break;
	case Dead:
		HandleDeadState();
		break;
	}
}

void Enemy::RenderFlyInState() {
	mTextures[0]->Render();
}

void Enemy::RenderFormationState() {
	mTextures[sFormation->GetTick() % 2]->Render();
}

void Enemy::RenderStates() {
	switch (mCurrentState) {
	case FlyIn:
		RenderFlyInState();
		break;
	case InFormation:
		RenderFormationState();
		break;
	case Diving:
		RenderDiveState();
		break;
	case Dead:
		RenderDeadState();
		break;
	}
}

Enemy::States Enemy::CurrentState() {
	return mCurrentState;
}

Enemy::Types Enemy::Type() {
	return mType;
}

int Enemy::Index() {
	return mIndex;
}

void Enemy::Dive(int type) {
	Parent(nullptr);
	mCurrentState = Diving;
	mDiveStartPosition = Position();
	mCurrentWaypoint = 1;
}

void Enemy::Update() {
	if (Active()) {
		HandleStates();
	}
}

void Enemy::Render() {
	if (Active()) {
		RenderStates();
	}
}
