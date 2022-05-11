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
    pos.identity();
}

Tank::~Tank()
{
    // TODO: Add your code
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

void Tank::steer( float ForwardBackward, float LeftRight)
{
    forwardBackward = ForwardBackward;
    leftRight = LeftRight;
}

void Tank::aim(const Vector& Target )
{
    // TODO: Add your code
}

void Tank::update(float dtime)
{
    time += dtime;
    float faktor = 2;
    Matrix mTankBot, fahrtrichtung, drehung;
    fahrtrichtung.translation(forwardBackward * faktor * dtime, 0, 0);
    drehung.rotationY(leftRight  * faktor * dtime);
    mTankBot *= fahrtrichtung * drehung;
    pTankBot->transform(pTankBot->transform() * fahrtrichtung * drehung);


    pTankTop->transform(pTankTop->transform() * fahrtrichtung * drehung);

}

void Tank::draw(const BaseCamera& Cam)
{
    pTankBot->draw(Cam);

    pTankTop->draw(Cam);
}
