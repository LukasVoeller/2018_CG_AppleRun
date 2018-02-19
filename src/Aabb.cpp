//
//  Aabb.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 02.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Aabb.h"

AABB AABB::UnitBox(Vector(-1,-1,-1), Vector(1,1,1));

AABB::AABB()
{
	
}

AABB::AABB(const Vector& min, const Vector& max) : min(min), max(max)
{
	
}

AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : min(minX, minY, minZ), max(maxX, maxY, maxZ)
{
	
}

Vector AABB::size() const
{
	return max-min;
}

AABB AABB::transform(const Matrix& m) const
{
	Vector c[8];
	corners(c);
	for (int i = 0; i < 8; ++i)
		c[i] = m * c[i];
	AABB r;
	r.fromPoints(c, 8);
	return r;
}

AABB AABB::merge(const AABB& a, const AABB& b) const
{
	AABB r;
	
	r.min.X = a.min.X < b.min.X ? a.min.X : b.min.X;
	r.min.Y = a.min.Y < b.min.Y ? a.min.Y : b.min.Y;
	r.min.Z = a.min.Z < b.min.Z ? a.min.Z : b.min.Z;
	
	r.max.X = a.max.X > b.max.X ? a.max.X : b.max.X;
	r.max.Y = a.max.Y > b.max.Y ? a.max.Y : b.max.Y;
	r.max.Z = a.max.Z > b.max.Z ? a.max.Z : b.max.Z;
	
	return r;
}

AABB& AABB::merge(const AABB& a)
{
	min.X = a.min.X < min.X ? a.min.X : min.X;
	min.Y = a.min.Y < min.Y ? a.min.Y : min.Y;
	min.Z = a.min.Z < min.Z ? a.min.Z : min.Z;
	
	max.X = a.max.X > max.X ? a.max.X : max.X;
	max.Y = a.max.Y > max.Y ? a.max.Y : max.Y;
	max.Z = a.max.Z > max.Z ? a.max.Z : max.Z;
	
	return *this;
}

void AABB::corners(Vector c[8]) const
{
	c[0].X = min.X; c[0].Y = min.Y; c[0].Z = min.Z;
	c[1].X = max.X; c[1].Y = min.Y; c[1].Z = min.Z;
	c[2].X = max.X; c[2].Y = max.Y; c[2].Z = min.Z;
	c[3].X = max.X; c[3].Y = min.Y; c[3].Z = min.Z;
	
	c[4].X = min.X; c[4].Y = min.Y; c[4].Z = max.Z;
	c[5].X = max.X; c[5].Y = min.Y; c[5].Z = max.Z;
	c[6].X = max.X; c[6].Y = max.Y; c[6].Z = max.Z;
	c[7].X = max.X; c[7].Y = min.Y; c[7].Z = max.Z;
}

void AABB::fromPoints(const Vector* Points, unsigned int PointCount)
{
	max = Vector(-1e20f, -1e20f, -1e20f);
	min = Vector(1e20f, 1e20f, 1e20f);
	
	for (unsigned int i = 0; i < PointCount; ++i)
	{
		if (min.X > Points[i].X) min.X = Points[i].X;
		if (min.Y > Points[i].Y) min.Y = Points[i].Y;
		if (min.Z > Points[i].Z) min.Z = Points[i].Z;
		if (max.X < Points[i].X) max.X = Points[i].X;
		if (max.Y < Points[i].Y) max.Y = Points[i].Y;
		if (max.Z < Points[i].Z) max.Z = Points[i].Z;
	}
}

Vector AABB::center() const
{
	return (max + min)*0.5f;
}

const AABB& AABB::unitBox()
{
	return UnitBox;
}
