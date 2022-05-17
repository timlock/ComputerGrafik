//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"

#define _USE_MATH_DEFINES
#include <math.h>

Tank::Tank()
{
	Transform.identity();
}

Tank::~Tank()
{
	delete pTankBot;
	delete pTankBot;
}

bool Tank::loadModels(const char* ChassisFile, const char* CannonFile)
{
	pTankBot = new Model(ChassisFile);
	PhongShader* pPhongShader = new PhongShader();
	pTankBot->shader(pPhongShader, true);

	pTankTop = new Model(CannonFile);
	pPhongShader = new PhongShader();
	pTankTop->shader(pPhongShader, true);
	return true;
}

void Tank::steer(float ForwardBackward, float LeftRight)
{
	forwardBackward = ForwardBackward;
	leftRight = LeftRight;
}

void Tank::aim(const Vector& Target)
{
	Matrix tankTopPos = pTankTop->transform();
	Vector ortsVecCam = Target - tankTopPos.translation();
	ortsVecCam.normalize();
	drehWinkel = ortsVecCam.dot(Vector(1, 0, 0));
	drehWinkel = acos(drehWinkel);
	if (ortsVecCam.Z > 0) {
		drehWinkel *= -1;
	}
}

void Tank::update(float dtime)
{
	float faktor = 2;
	Matrix mTankBot, fahrtrichtung, drehung;
	fahrtrichtung.translation(forwardBackward * faktor * dtime, 0, 0);
	drehung.rotationY(leftRight * faktor * dtime);
	mTankBot = pTankBot->transform() * fahrtrichtung;
	pTankBot->transform(pTankBot->transform() * fahrtrichtung * drehung);

	Matrix drehRichtung, rn;
	drehRichtung.rotationY(drehWinkel);
	rn.identity();
	rn.m00 = pTankBot->transform().m00;
	rn.m20 = pTankBot->transform().m20;
	rn.m02 = pTankBot->transform().m02;
	rn.m22 = pTankBot->transform().m22;
	rn.invert();
	pTankTop->transform(pTankBot->transform() * rn * drehRichtung);

}

void Tank::draw(const BaseCamera& Cam)
{
	pTankBot->draw(Cam);

	pTankTop->draw(Cam);
}
