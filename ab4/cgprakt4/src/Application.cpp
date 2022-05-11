//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "scene.h"

#define _USE_MATH_DEFINES
#include <math.h>


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
    BaseModel* pModel;
    ConstantShader* pConstShader;
    PhongShader* pPhongShader;
    
    // create LineGrid model with constant color shader
    pModel = new LinePlaneModel(10, 10, 10, 10);
    pConstShader = new ConstantShader();
	pConstShader->color( Color(1,1,1));
    pModel->shader(pConstShader, true);
    Models.push_back(pModel);
    // Exercise 1
    // TODO Load pTankTop & pTankBot
    /*pTankBot = new Model(ASSET_DIRECTORY "tank_bottom.dae");
    pPhongShader = new PhongShader();
    pTankBot->shader(pPhongShader, true);
    Models.push_back(pTankBot);

    pTankTop = new Model(ASSET_DIRECTORY "tank_top.dae");
    pPhongShader = new PhongShader();
    pTankTop->shader(pPhongShader, true);
    Models.push_back(pTankTop);*/
    
    // Exercise 2
    
    pPhongShader = new PhongShader();
    pTank = new Tank();
    pTank->shader(pPhongShader, true);
    pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
    Models.push_back( pTank );
    
    
    // Exercise 3
    /*
    Scene* pScene = new Scene();
    pScene->shader(new PhongShader(), true);
    pScene->addSceneFile(ASSET_DIRECTORY "scene.osh");
    Models.push_back(pScene);
    */
    

    
}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float deltaTime)
{

    time += deltaTime;
    float timeFactor = 0.5f;
    // Exercise 1
    // TODO: add your code here
   /* Matrix translation, rotationTankBot, rotationTankTop, wendeKreis, zNeigung;
    translation.translation(0, 0, 4);
    rotationTankBot.rotationY(M_PI * (time * timeFactor));

    pTankBot->transform(rotationTankBot * translation);
    
    zNeigung.rotationY(M_PI * 1.5f);
    rotationTankTop.rotationY(-(M_PI * (time * timeFactor)));
    pTankTop->transform(rotationTankBot * translation * zNeigung * rotationTankTop);*/
    // Exercise 2
    // TODO: add your code here
    int up, down, left, right;
    up = glfwGetKey(pWindow, GLFW_KEY_UP);
    down = glfwGetKey(pWindow, GLFW_KEY_DOWN);
    left = glfwGetKey(pWindow, GLFW_KEY_LEFT);
    right = glfwGetKey(pWindow, GLFW_KEY_RIGHT);
    pTank->steer(up - down, left - right);
    double xPos, yPos;
    glfwGetCursorPos(pWindow, &xPos, &yPos);
    Vector Pos;
    calc3DRay(xPos, yPos, Pos);
    pTank->update(deltaTime);
    Cam.update();
}

Vector Application::calc3DRay( float x, float y, Vector& Pos)
{
    // TODO: Add your code here
    // Pos:Ray Origin
    // return:Ray Direction
    Vector vMaus(x, y, 0);
    vMaus.normalize();
    Matrix kamera(Cam.getProjectionMatrix());
    kamera.invert();
    Pos = kamera* vMaus;
    return Pos; // dummy return value
}

void Application::draw()
{
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
    
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}