//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel( float Width, float Height, float Depth )
{
	VB.begin();
	//vorne unten
	VB.addVertex(0,0,0); 
	VB.addVertex(Width,0,0);
	//vorne rechts
	VB.addVertex(Width, 0, 0);
	VB.addVertex(Width, Height, 0);
	//vorne oben
	VB.addVertex(Width, Height, 0);
	VB.addVertex(0, Height, 0);
	//vorne links
	VB.addVertex(0, Height, 0);
	VB.addVertex(0, 0, 0);

	//hinten unten
	VB.addVertex(0, 0, Depth);
	VB.addVertex(Width, 0, Depth);
	//hinten rechts
	VB.addVertex(Width, 0, Depth);
	VB.addVertex(Width, Height, Depth);
	//hinten unten
	VB.addVertex(Width, Height, Depth);
	VB.addVertex(0, Height, Depth);
	//hinten links
	VB.addVertex(0, Height, Depth);
	VB.addVertex(0, 0, Depth);

	//links unten
	VB.addVertex(0, 0, 0);
	VB.addVertex(0, 0, Depth);
	//links oben
	VB.addVertex(0, Height, 0);
	VB.addVertex(0, Height, Depth);

	//rechts unten
	VB.addVertex(Width, 0, 0);
	VB.addVertex(Width, 0, Depth);
	//rechts oben
	VB.addVertex(Width, Height, 0);
	VB.addVertex(Width, Height, Depth);

	VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
  
	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}
