//
//  SimpleRayTracer.cpp
//  SimpleRayTracer
//
//  Created by Philipp Lensing on 03.09.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "SimpleRayTracer.h"
#include "rgbimage.h"
#include <stdio.h>
#include <math.h>


Camera::Camera( float zvalue, float planedist, float width, float height, unsigned int widthInPixel, unsigned int heightInPixel )
{
	this->zvalue = zvalue;
	this->planedist= planedist;
	this->width = width;
	this->widthInPixel = widthInPixel;
	this->heightInPixel = heightInPixel;
}

Vector Camera::generateRay( unsigned int x, unsigned int y) const
{
	Vector ray = Position();
	ray = ray * planedist;
	ray.normalize();
	ray += Vector(x * (width / widthInPixel) / 2, y * (width / widthInPixel) / 2, 0);
	return ray;
}

Vector Camera::Position() const
{
	return Vector(0.0f, 0.0f, zvalue);
}

SimpleRayTracer::SimpleRayTracer(unsigned int MaxDepth)
{
	this->MaxDepth = MaxDepth;
}


void SimpleRayTracer::traceScene( const Scene& SceneModel, RGBImage& Image)
{
	int widthInPixel = 640;
	int heightInPixel = 480;
	Camera camera(8, 1, 1, 0.75f, widthInPixel, heightInPixel);
	for (int y = 0; y < heightInPixel; y++) {
		for (int x = 0; x < widthInPixel; x++) {
			trace(SceneModel, camera.Position(), camera.generateRay(x,y), MaxDepth);
		}
	}
}

Color SimpleRayTracer::localIllumination( const Vector& Surface, const Vector& Eye, const Vector& N, const PointLight& Light, const Material& Mtrl )
{
	// TODO: Add your code
	return Color(); // dummy (remove)
}

Color SimpleRayTracer::trace( const Scene& SceneModel, const Vector& o, const Vector& d, int depth)
{
	Triangle triangle;
	Triangle closestTriangle;
	float closestDistance = FLT_MAX;
	for (int i = 0; i < SceneModel.getTriangleCount(); i++) {
		triangle = SceneModel.getTriangle(i);
		float s = -1;
		if (o.triangleIntersection(d, triangle.A, triangle.B, triangle.C, s)) {
			if (s < FLT_MAX) {
				closestTriangle = triangle;
				closestDistance = s;
			}
		}
	}


}

