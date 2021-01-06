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
    float W = Width / 2; float H = Height / 2; float D = Depth / 2;
    VB.begin();

    // BOTTOM
    VB.addNormal(0, 0, 1); //Oben links 0
    VB.addTexcoord0(0, 0);
    VB.addVertex(-W, -H, D);

    VB.addNormal(0, 0, 1); //Oben rechts 1
    VB.addTexcoord0(1, 0);
    VB.addVertex(W, -H, D);

    VB.addNormal(0, 0, 1); //Unten rechts 2
    VB.addTexcoord0(1, 1);
    VB.addVertex(W, -H, -D);

    VB.addNormal(0, 0, 1); //Unten links 3
    VB.addTexcoord0(0, 1);
    VB.addVertex(-W, -H, -D);

    // FRONT
    VB.addNormal(0, 0, 1); //Oben links 4
    VB.addTexcoord0(0, 0);
    VB.addVertex(-W, H, D);

    VB.addNormal(0, 0, 1); //Oben rechts 5
    VB.addTexcoord0(1, 0);
    VB.addVertex(W, H, D);

    VB.addNormal(0, 0, 1); //Unten rechts 6
    VB.addTexcoord0(1, 1);
    VB.addVertex(W, -H, D);

    VB.addNormal(0, 0, 1); //Unten links 7
    VB.addTexcoord0(0, 1);
    VB.addVertex(-W, -H, D);

    // RIGHT
    VB.addNormal(0, 0, 1); //Oben links 8
    VB.addTexcoord0(0, 0);
    VB.addVertex(W, H, D);

    VB.addNormal(0, 0, 1); //Oben rechts 9
    VB.addTexcoord0(1, 0);
    VB.addVertex(W, H, -D);

    VB.addNormal(0, 0, 1); //Unten rechts 10
    VB.addTexcoord0(1, 1);
    VB.addVertex(W, -H, -D);

    VB.addNormal(0, 0, 1); //Unten links 11
    VB.addTexcoord0(0, 1);
    VB.addVertex(W, -H, D);

    // BACK
    VB.addNormal(0, 0, 1); //Oben links 12
    VB.addTexcoord0(0, 0);
    VB.addVertex(W, H, -D);

    VB.addNormal(0, 0, 1); //Oben rechts 13
    VB.addTexcoord0(1, 0);
    VB.addVertex(-W, H, -D);

    VB.addNormal(0, 0, 1); //Unten rechts 14
    VB.addTexcoord0(1, 1);
    VB.addVertex(-W, -H, -D);

    VB.addNormal(0, 0, 1); //Unten links 15
    VB.addTexcoord0(0, 1);
    VB.addVertex(W, -H, -D);

    // LEFT
    VB.addNormal(0, 0, 1); //Oben links 16
    VB.addTexcoord0(0, 0);
    VB.addVertex(-W, H, -D);

    VB.addNormal(0, 0, 1); //Oben rechts 17
    VB.addTexcoord0(1, 0);
    VB.addVertex(-W, H, D);

    VB.addNormal(0, 0, 1); //Unten rechts 18
    VB.addTexcoord0(1, 1);
    VB.addVertex(-W, -H, D);

    VB.addNormal(0, 0, 1); //Unten links 19
    VB.addTexcoord0(0, 1);
    VB.addVertex(-W, -H, -D);

    // TOP
    VB.addNormal(0, 0, 1); //Oben links 20
    VB.addTexcoord0(0, 0);
    VB.addVertex(-W, H, -D);

    VB.addNormal(0, 0, 1); //Oben rechts 21
    VB.addTexcoord0(1, 0);
    VB.addVertex(W, H, -D);

    VB.addNormal(0, 0, 1); //Unten rechts 22
    VB.addTexcoord0(1, 1);
    VB.addVertex(W, H, D);

    VB.addNormal(0, 0, 1); //Unten links 23
    VB.addTexcoord0(0, 1);
    VB.addVertex(-W, H, D);

    VB.end();

    IB.begin();
    // BOTTOM
    IB.addIndex(0); IB.addIndex(3); IB.addIndex(1);
    IB.addIndex(1); IB.addIndex(3); IB.addIndex(2);
    // FRONT
    IB.addIndex(4); IB.addIndex(7); IB.addIndex(5);
    IB.addIndex(5); IB.addIndex(7); IB.addIndex(6);
    // RIGHT
    IB.addIndex(8); IB.addIndex(11); IB.addIndex(9);
    IB.addIndex(9); IB.addIndex(11); IB.addIndex(10);
    // BACK
    IB.addIndex(12); IB.addIndex(15); IB.addIndex(13);
    IB.addIndex(13); IB.addIndex(15); IB.addIndex(14);
    // LEFT
    IB.addIndex(16); IB.addIndex(19); IB.addIndex(17);
    IB.addIndex(17); IB.addIndex(19); IB.addIndex(18);
    // TOP
    IB.addIndex(20); IB.addIndex(23); IB.addIndex(21);
    IB.addIndex(21); IB.addIndex(23); IB.addIndex(22);

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
