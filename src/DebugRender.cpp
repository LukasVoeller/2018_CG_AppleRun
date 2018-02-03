//
//  DebugRender.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 08.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "DebugRender.h"
#include <assert.h>

DebugRender* DebugRender::Instance = NULL;

DebugRender& DebugRender::ref() {
	if(Instance==NULL)
		Instance = new DebugRender();
	
	return *Instance;
}

void DebugRender::init(){
	MaxVertices = 16 * 1024;
	
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, MaxVertices*sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray (VAO);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (0)));
	glEnableVertexAttribArray (1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char *)NULL + (12)));
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void DebugRender::drawLine( const Vector& a, const Vector& b, const Color& c){
	Lines.push_back(Line(a,b,c));
}

void DebugRender::drawMatrix( const Matrix& m){
	Lines.push_back( Line ( m.translation(), m.translation()+m.right(), Color(1,0,0)));
	Lines.push_back( Line ( m.translation(), m.translation()+m.up(), Color(0,1,0)));
	Lines.push_back( Line ( m.translation(), m.translation()+m.forward(), Color(0,0,1)));
	Lines.push_back( Line ( m.translation(), Vector(0,0,0), Color(1,1,1)));
}


void DebugRender::render(const BaseCamera& Cam){
	if(Lines.size()<=0 || MaxVertices<=0)
		return;
	
	GLboolean success;
	GLuint VertexCount = (GLuint)Lines.size()* 2;
	if(VertexCount > MaxVertices)
		VertexCount = MaxVertices;
	
	// Bind and map buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	Vertex *vertices = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, VertexCount*sizeof(Vertex), GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
	
	// Wait for fence (set below) before modifying buffer.
	glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
	
	// Modify buffer, flush, and unmap.
	for( int i=0, j=0; i<Lines.size() && j<VertexCount; ++i, j+=2){
		Line& l = Lines[i];
		vertices->p = l.v[0];
		vertices->c = l.c;
		++vertices;
		vertices->p = l.v[1];
		vertices->c = l.c;
		++vertices;
	}
	
	glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, VertexCount*sizeof(Vertex));
	success = glUnmapBuffer(GL_ARRAY_BUFFER);
	
	assert(success==GL_TRUE);
	
	// Draw vbo
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	Shader.activate(Cam);
	glDrawArrays(GL_LINES, 0, VertexCount);
	glBindVertexArray(0);
	Shader.deactivate();
	glEnable(GL_DEPTH_TEST);
	
	// Create a fence that the next frame will wait for.
	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	
	Lines.clear();
}

