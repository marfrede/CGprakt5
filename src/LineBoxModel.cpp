//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel(float Width, float Height, float Depth)
{
	float scale = 1; // 0.7 passt zum Bild auf PBlatt
	Width *= scale;
	Height *= scale;
	Depth *= scale;

	VB.begin();
	// Area front
	VB.addVertex(0, 0, 0);          VB.addVertex(Width, 0, 0);
	VB.addVertex(Width, 0, 0);      VB.addVertex(Width, Height, 0);
	VB.addVertex(Width, Height, 0); VB.addVertex(0, Height, 0);
	VB.addVertex(0, Height, 0);     VB.addVertex(0, 0, 0);

	//Area Back
	VB.addVertex(0, 0, Depth);          VB.addVertex(Width, 0, Depth);
	VB.addVertex(Width, 0, Depth);      VB.addVertex(Width, Height, Depth);
	VB.addVertex(Width, Height, Depth); VB.addVertex(0, Height, Depth);
	VB.addVertex(0, Height, Depth);     VB.addVertex(0, 0, Depth);

	// Lines from Front to Back
	VB.addVertex(0, 0, 0);          VB.addVertex(0, 0, Depth);
	VB.addVertex(Width, 0, 0);      VB.addVertex(Width, 0, Depth);
	VB.addVertex(Width, Height, 0); VB.addVertex(Width, Height, Depth);
	VB.addVertex(0, Height, 0);     VB.addVertex(0, Height, Depth);

	VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}
