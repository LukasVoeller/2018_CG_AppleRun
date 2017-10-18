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
    /// TODO: Add your code (Exercise 2)
    Width = Width / 2;
    Height = Height / 2;
    Depth = Depth / 2;
    
    // 1. setup vertex buffer
    VB.begin();
    
    // 1.1 Hinten
    VB.addNormal(0, 0, -1);
    VB.addTexcoord0(1, 1);
    VB.addVertex(-Width, -Height, -Depth);
    
    VB.addNormal(0, 0, -1);
    VB.addTexcoord0(1, 0);
    VB.addVertex(-Width, Height, -Depth);
    
    VB.addNormal(0, 0, -1);
    VB.addTexcoord0(0, 0);
    VB.addVertex(Width, Height, -Depth);
    
    VB.addNormal(0, 0, -1);
    VB.addTexcoord0(0, 1);
    VB.addVertex(Width, -Height, -Depth);
    
    // 1.2 Vorne
    VB.addNormal(0, 0, 1);
    VB.addTexcoord0(1, 1);
    VB.addVertex(-Width, -Height, Depth);
    
    VB.addNormal(0, 0, 1);
    VB.addTexcoord0(1, 0);
    VB.addVertex(-Width, Height, Depth);
    
    VB.addNormal(0, 0, 1);
    VB.addTexcoord0(0, 0);
    VB.addVertex(Width, Height, Depth);
    
    VB.addNormal(0, 0, 1);
    VB.addTexcoord0(0, 1);
    VB.addVertex(Width, -Height, Depth);
    
    // 1.3 Links
    VB.addNormal(-1, 0, 0);
    VB.addTexcoord0(1, 1);
    VB.addVertex(-Width, -Height, Depth);
    
    VB.addNormal(-1, 0, 0);
    VB.addTexcoord0(1, 0);
    VB.addVertex(-Width, Height, Depth);
    
    VB.addNormal(-1, 0, 0);
    VB.addTexcoord0(0, 0);
    VB.addVertex(-Width, Height, -Depth);
    
    VB.addNormal(-1, 0, 0);
    VB.addTexcoord0(0, 1);
    VB.addVertex(-Width, -Height, -Depth);
    
    // 1.4 Rechts
    VB.addNormal(1, 0, 0);
    VB.addTexcoord0(0, 1);
    VB.addVertex(Width, -Height, Depth);
    
    VB.addNormal(1, 0, 0);
    VB.addTexcoord0(0, 0);
    VB.addVertex(Width, Height, Depth);
    
    VB.addNormal(1, 0, 0);
    VB.addTexcoord0(1, 0);
    VB.addVertex(Width, Height, -Depth);
    
    VB.addNormal(1, 0, 0);
    VB.addTexcoord0(1, 1);
    VB.addVertex(Width, -Height, -Depth);
    
    // 1.4 Obere Seite
    VB.addNormal(0, 1, 0);
    VB.addTexcoord0(0, 1);
    VB.addVertex(-Width, Height, Depth);
    
    VB.addNormal(0, 1, 0);
    VB.addTexcoord0(0, 0);
    VB.addVertex(-Width, Height, -Depth);
    
    VB.addNormal(0, 1, 0);
    VB.addTexcoord0(1, 0);
    VB.addVertex(Width, Height, -Depth);
    
    VB.addNormal(0, 1, 0);
    VB.addTexcoord0(1, 1);
    VB.addVertex(Width, Height, Depth);
    
    // 1.5 Untere Seite
    VB.addNormal(0, -1, 0);
    VB.addTexcoord0(0, 0);
    VB.addVertex(-Width, -Height, Depth);
    
    VB.addNormal(0, -1, 0);
    VB.addTexcoord0(0, 1);
    VB.addVertex(-Width, -Height, -Depth);
    
    VB.addNormal(0, -1, 0);
    VB.addTexcoord0(1, 1);
    VB.addVertex(Width, -Height, -Depth);
    
    VB.addNormal(0, -1, 0);
    VB.addTexcoord0(1, 0);
    VB.addVertex(Width, -Height, Depth);
    
    VB.end();
    
    // 2. setup index buffer
    IB.begin();
    
    // Rückseite zeichnen
    IB.addIndex(0); //1. Dreieck
    IB.addIndex(1);
    IB.addIndex(2);
    
    IB.addIndex(2); //2. Dreieck
    IB.addIndex(3);
    IB.addIndex(0);
    
    // Vorderseite zeichnen
    IB.addIndex(4);
    IB.addIndex(7);
    IB.addIndex(6);
    
    IB.addIndex(6);
    IB.addIndex(5);
    IB.addIndex(4);
    
    // Linke Seite zeichnen
    IB.addIndex(8);
    IB.addIndex(9);
    IB.addIndex(10);
    
    IB.addIndex(10);
    IB.addIndex(11);
    IB.addIndex(8);
    
    // Rechte Seite zeichnen
    IB.addIndex(12);
    IB.addIndex(15);
    IB.addIndex(14);
    
    IB.addIndex(14);
    IB.addIndex(13);
    IB.addIndex(12);
    
    // Obere Seite zeichen
    IB.addIndex(16);
    IB.addIndex(19);
    IB.addIndex(18);
    
    IB.addIndex(18);
    IB.addIndex(17);
    IB.addIndex(16);
    
    // Untere Seite zeichen
    IB.addIndex(20);
    IB.addIndex(21);
    IB.addIndex(22);
    
    IB.addIndex(22);
    IB.addIndex(23);
    IB.addIndex(20);
    
    IB.end();
}

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
    
    // TODO: Add your code (Exercise 2)
    VB.activate();
    IB.activate();
    
    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
    
    IB.deactivate();
    VB.deactivate();
}
