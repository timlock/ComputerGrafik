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
#include <algorithm>

#define EPSILON 1e-6

Camera::Camera(float zvalue, float planedist, float width, float height, unsigned int widthInPixel, unsigned int heightInPixel)
	:zvalue(zvalue), planedist(planedist), width(width), height(height), widthInPixel(widthInPixel), heightInPixel(heightInPixel)
{
}

Vector Camera::generateRay(unsigned int x, unsigned int y) const
{
	Vector ray = Position();
	float leftBorder = -width * 0.5f;
	float topBorder = height * 0.5f;
	float widthPerPixel = width / (float)widthInPixel;
	float heightPerPixel = height / (float)heightInPixel;
	ray = Vector(leftBorder + x * widthPerPixel, topBorder - y * heightPerPixel, planedist);
	return ray.normalize();
}
Vector Camera::Position() const
{
	return Vector(0.0f, 0.0f, zvalue);
}

SimpleRayTracer::SimpleRayTracer(unsigned int MaxDepth) : MaxDepth(MaxDepth)
{
}

void SimpleRayTracer::traceScene(const Scene& SceneModel, RGBImage& Image, bool debug)
{

	Camera camera(-8.0f, 1.0f, 1.0f, 0.75f, Image.width(), Image.height());
	for (unsigned int y = 0; y < Image.height(); y++) {
		for (unsigned int x = 0; x < Image.width(); x++) {
			//2. Berechne Strahl s von Augpunkt zu Pixel;
			Vector s = camera.generateRay(x, y);
			//3. F = Raytracing(s);
			Color color = trace(SceneModel, camera.Position(), s, MaxDepth);
			Image.setPixelColor(x, y, color);
			if (x % 100 == 0 && debug) {
				std::cout << ".";
			}
		}
		if (y % 10 == 0) {
			std::cout << "\n" << (((float)y) / Image.height()) * 100.0f << " " << std::endl;
		}
	}
}

Color SimpleRayTracer::localIllumination(const Vector& Surface, const Vector& Eye, const Vector& N, const PointLight& Light, const Material& Mtrl)
{
	//Diffuse Component
	Vector L = (Light.Position - Surface).normalize();
	Color D = Mtrl.getDiffuseCoeff(Surface) * Light.Intensity * max(N.dot(L), 0.0f);
	//Specular Component S
	Vector R = (N * 2.0f * (N.dot(L)) - L).normalize();
	Vector E = (Eye - Surface).normalize();
	Color S = Mtrl.getSpecularCoeff(Surface) * Light.Intensity * powf(max(E.dot(R), 0.0f), Mtrl.getSpecularExp(Surface));
	return  S + D;
}

float SimpleRayTracer::calculateShortestDistance(const Scene& SceneModel, const Vector& o, const Vector& d, Triangle& closestTriangle) {
	float shortestDistance = FLT_MAX;
	for (unsigned int i = 0; i < SceneModel.getTriangleCount(); i++) {
		Triangle triangle = SceneModel.getTriangle(i);
		float s = -1;
		if (o.triangleIntersection(d, triangle.A, triangle.B, triangle.C, s)) {
			if (s < shortestDistance && s > 0) {
				closestTriangle = triangle;
				shortestDistance = s;
			}
		}
	}
	return shortestDistance;
}

bool SimpleRayTracer::isIlluminated(const Vector& p, const PointLight& light, const Scene& SceneModel) {
	//	/*
	Vector lightPosition = light.Position;
	Vector lightDirection = (p - lightPosition);
	float lightLength = lightDirection.length();
	lightDirection;
	float shortestDistance = FLT_MAX;
	for (unsigned int j = 0; j < SceneModel.getTriangleCount(); j++) {
		Triangle triangle = SceneModel.getTriangle(j);
		float s = -1.0f;
		bool tmp = lightPosition.triangleIntersection(lightDirection, triangle.A, triangle.B, triangle.C, s);
		if (s > 0 && s < shortestDistance) {
			shortestDistance = s;
		}
	}
	if (shortestDistance > lightLength) {
		return false;
	}
	return true;
	/*	if (lightPosition.triangleIntersection(lightDirection, triangle.A, triangle.B, triangle.C, s) && s > -EPSILON && s < 1 - EPSILON) {
			return false;
		}
	}
	return true;*/
}

Color SimpleRayTracer::trace(const Scene& SceneModel, const Vector& o, const Vector& d, unsigned int depth)
{
	//1. Setze Farbe F für Pixel (x,y) auf Schwarz;
	Color color;
	if (depth == 0) return color;
	Triangle closestTriangle;
	//4. Berechne ersten Auftreffpunkt p des Strahls s auf der Objektoberfläche;
	float s = calculateShortestDistance(SceneModel, o, d, closestTriangle);
	if (s == FLT_MAX) {
		return color;
	}
	depth--;
	Vector p = o + d * s;
	//5. Prüfe, ob von Punkt p Sichtverbindung zur Lichtquelle besteht :

	Vector normale = closestTriangle.calcNormal(p);
	for (unsigned int i = 0; i < SceneModel.getLightCount(); i++) {
		PointLight light = SceneModel.getLight(i);
		if (isIlluminatedBy(p, light, SceneModel)) {
			//5.1. Wenn JA: F = Berechne_Lokales_Beleuchtungsmodell(...)
			color += localIllumination(p, o, normale, SceneModel.getLight(i), *(closestTriangle.pMtrl));
		}
	}
	if (depth == MaxDepth) {
		color += closestTriangle.pMtrl->getAmbientCoeff(p);
	}
	//6. Berechne Reflexionsstrahl r von s im Punkt p;
	Vector r = d.reflection(normale);
	//F = F + Raytracing(r) * Reflexionskoeffizient;
	color += trace(SceneModel, p, r, depth) * closestTriangle.pMtrl->getReflectivity(p);
	return color;
}
bool SimpleRayTracer::isIlluminatedBy(const Vector& p, const PointLight& light, const Scene& SceneModel) {
	// calc if direct light hits the position p
	for (unsigned int i = 0; i < SceneModel.getTriangleCount(); i++) {
		Vector q = light.Position;
		Vector e = p - q;
		Triangle triangle = SceneModel.getTriangle(i);
		float s = -1;
		if (q.triangleIntersection(e, triangle.A, triangle.B, triangle.C, s) && s > -EPSILON && s < 1 - EPSILON) {
			return false;
		}
	}
	return true;
}
/*
#include "SimpleRayTracer.h"
#include "rgbimage.h"
#include <stdio.h>
#include <math.h>

#define EPSILON 1e-6

Camera::Camera(float zvalue, float planedist, float width, float height, unsigned int widthInPixel, unsigned int heightInPixel)
{
	this->zvalue = zvalue;
	this->planedist = planedist;
	this->width = width;
	this->height = height;
	this->widthInPixel = widthInPixel;
	this->heightInPixel = heightInPixel;
}

Vector Camera::generateRay(unsigned int x, unsigned int y) const
{
	float xPixelPosition = (float)x * (width / (float)widthInPixel) - (width * 0.5f);
	float yPixelPosition = -(float)y * (height / (float)heightInPixel) + (height * 0.5f);
	return Vector(xPixelPosition, yPixelPosition, planedist).normalize();
}

Vector Camera::Position() const
{
	return Vector(0, 0, zvalue);
}

SimpleRayTracer::SimpleRayTracer(unsigned int MaxDepth)
{
	this->MaxDepth = MaxDepth;
}


void SimpleRayTracer::traceScene(const Scene& SceneModel, RGBImage& Image, bool debug)
{
	Camera camera = Camera(-8, 1, 1, 0.75f, Image.width(), Image.height());
	for (int y = 0; y < Image.height(); y++) {
		for (int x = 0; x < Image.width(); x++) {
			Image.setPixelColor(x, y, trace(SceneModel, camera.Position(), camera.generateRay(x, y), MaxDepth));
		}
		if (y % 10 == 0) {
			cout << "*";
		}
	}
}

Color SimpleRayTracer::localIllumination(const Vector& Surface, const Vector& Eye, const Vector& N, const PointLight& Light, const Material& Mtrl)
{
	Color I = Light.Intensity;
	Color kd = Mtrl.getDiffuseCoeff(Surface);
	Color ks = Mtrl.getSpecularCoeff(Surface);

	Vector L = (Light.Position - Surface).normalize();
	Vector E = (Eye - Surface).normalize();
	Vector R = ((N * N.dot(L)) * 2 - L).normalize();

	Color diffuseComponent = I * kd * fmax(0.0f, N.dot(L));
	Color specularComponent = I * ks * fmax(0.0f, E.dot(R));

	return diffuseComponent + specularComponent;
}

Color SimpleRayTracer::trace(const Scene& SceneModel, const Vector& o, const Vector& d, unsigned int depth)
{
	// 4. Berechne ersten Auftreffpunkt p des Strahls s auf der Objektoberfläche;
	Triangle triangle;
	float s = -1;

	for (int triangleNo = 0; triangleNo < SceneModel.getTriangleCount(); triangleNo++) {
		Triangle currentTriangle = SceneModel.getTriangle(triangleNo);
		float currentS = -1;

		if (o.triangleIntersection(d, currentTriangle.A, currentTriangle.B, currentTriangle.C, currentS) && currentS > 0 && (currentS < s || s < 0)) {
			triangle = currentTriangle;
			s = currentS;
		}
	}

	Color F = Color();

	// 5. Prüfe, ob von Punkt p Sichtverbindung zur Lichtquelle besteht :
	if (s < 0) {
		return F;
	}
	Vector p = o + d * s;

	if (depth == MaxDepth) {
		F += triangle.pMtrl->getAmbientCoeff(p);
	}

	for (int lightNo = 0; lightNo < SceneModel.getLightCount(); lightNo++) {
		PointLight light = SceneModel.getLight(lightNo);
		// 5.1.Wenn JA : F = Berechne_Lokales_Beleuchtungsmodell(...)
		if (isIlluminatedBy(p, light, SceneModel)) {
			F += localIllumination(p, o, triangle.calcNormal(p), light, *triangle.pMtrl);
		}
	}

	if (depth > 0) {
		// 6. Berechne Reflexionsstrahl r von s im Punkt p;
		Vector n = triangle.calcNormal(p);
		Vector r = d.reflection(n);
		// 7. F = F + Raytracing(r) * Reflexionskoeffizient;
		depth--;
		return F + trace(SceneModel, p, r, depth) * triangle.pMtrl->getReflectivity(p);
	}


	// TODO Zusatzaufgabe!
	// 8. Berechne Transmissionsstrahl t von s im Punkt p;
	// 9. F = F + Raytracing(t) * Transmissionskoeffizient;

	// 10. Return F;
	return F;
}

bool SimpleRayTracer::isIlluminatedBy(const Vector& p, const PointLight& light, const Scene& SceneModel){

	for (unsigned int i = 0; i < SceneModel.getTriangleCount(); i++) {
		Vector q = light.Position;
		Vector e = p - q;
		Triangle triangle = SceneModel.getTriangle(i);
		float s = -1;
		if (q.triangleIntersection(e, triangle.A, triangle.B, triangle.C, s) && s > -EPSILON && s < 1 - EPSILON) {
			return false;
		}
	}
	return true;
}*/