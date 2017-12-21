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
    //TODO: Add your code (Exercise 1)
    VB.begin();
    
    float BeginX = -Width/2.0f;
    float BeginY = -Height/2.0f;
    float BeginZ = -Depth/2.0f;
    
    //In Tiefenrichtung
    //-Begin = Depth/2 = End
    VB.addVertex(BeginX, BeginY, BeginZ);
    VB.addVertex(BeginX, BeginY, -BeginZ);
    
    VB.addVertex(BeginX, -BeginY, BeginZ);
    VB.addVertex(BeginX, -BeginY, -BeginZ);
    
    VB.addVertex(-BeginX, BeginY, BeginZ);
    VB.addVertex(-BeginX, BeginY, -BeginZ);
    
    VB.addVertex(-BeginX, -BeginY, BeginZ);
    VB.addVertex(-BeginX, -BeginY, -BeginZ);
    
    //In Höhenrichtung
    VB.addVertex(BeginX, BeginY, BeginZ);
    VB.addVertex(BeginX, -BeginY, BeginZ);
    
    VB.addVertex(BeginX, BeginY, -BeginZ);
    VB.addVertex(BeginX, -BeginY, -BeginZ);
    
    VB.addVertex(-BeginX, BeginY, BeginZ);
    VB.addVertex(-BeginX, -BeginY, BeginZ);
    
    VB.addVertex(-BeginX, BeginY, -BeginZ);
    VB.addVertex(-BeginX, -BeginY, -BeginZ);
    
    //In Breitenrichtung
    VB.addVertex(BeginX, BeginY, BeginZ);
    VB.addVertex(-BeginX, BeginY, BeginZ);
    
    VB.addVertex(BeginX, -BeginY, -BeginZ);
    VB.addVertex(-BeginX, -BeginY, -BeginZ);
    
    VB.addVertex(BeginX, -BeginY, BeginZ);
    VB.addVertex(-BeginX, -BeginY, BeginZ);
    
    VB.addVertex(BeginX, BeginY, -BeginZ);
    VB.addVertex(-BeginX, BeginY, -BeginZ);
    
    VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
    VB.activate();
    
    glDrawArrays(GL_LINES, 0, VB.vertexCount());
    
    VB.deactivate();
}
