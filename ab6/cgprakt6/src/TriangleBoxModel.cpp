//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"


TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth)
{
	/* Vector vertices[] = {
		 Vector(0,0,0), Vector(Width,0,0), Vector(0,Height,0), Vector(Width,Height,0),
		 Vector(0,0,Depth), Vector(Width,0,Depth), Vector(0,Height,Depth), Vector(Width,Height,Depth),
	 };*/

	Vector vertices[] = {
		Vector(0,0,0), Vector(0,Height,0), Vector(Width,Height,0), Vector(Width,0,0),   //vorne
		Vector(0,0,Depth), Vector(0,Height,Depth),Vector(Width,Height,Depth), Vector(Width,0,Depth),  //hinten
		//Vector(0,0,0), Vector(0,0,Depth), Vector(0,Height,Depth), Vector(0,Height,0), //links
		 Vector(0,0,0), Vector(0,Height,0), Vector(0,Height,Depth), Vector(0,0,Depth), //links
		Vector(Width,0,0), Vector(Width,Height,0), Vector(Width,Height,Depth), Vector(Width,0,Depth), //rechts
		Vector(0,0,0), Vector(Width,0,0), Vector(Width,0,Depth), Vector(0,0,Depth), //unten
		Vector(0,Height,0), Vector(0,Height,Depth), Vector(Width,Height,Depth), Vector(Width,Height,0) //oben
	};

	unsigned short indices[] = {
	0,1,2,  0,2,3, //vorne
	6,5,4,  7,6,4, //hinten
	//8,9,11, 11,9,10, //links
	8,11,9, 9,11,10, //links
	12,13,15,  13,14,15, //rechts
	17,19,16,  18,19,17, //unten
	20,21,23,  23,21,22 //oben
	};
	VB.begin();
	for (int i = 0; i < 24; i += 4) {
		VB.addNormal((vertices[i + 1] - vertices[i]).cross(vertices[i + 2] - vertices[i]).normalize());
		VB.addTexcoord0(1, 1);
		VB.addVertex(vertices[i]);
		VB.addTexcoord0(1, 0);
		VB.addVertex(vertices[i + 1]);
		VB.addTexcoord0(0, 0);
		VB.addVertex(vertices[i + 2]);
		VB.addTexcoord0(0, 1);
		VB.addVertex(vertices[i + 3]);
	}
	VB.end();

	IB.begin();
	for (int i = 0; i < 36; i++) {
		IB.addIndex(indices[i]);
	}

	IB.end();
}

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();
	IB.activate();
	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
	IB.deactivate();
	VB.deactivate();
}
