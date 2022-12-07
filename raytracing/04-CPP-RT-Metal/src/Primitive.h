#pragma once

#include "Ray.h"
#include "Record.h"

class Primitive 
{
public:
	virtual ~Primitive() {};

	virtual bool hit(const Ray& ray, Record& record) = 0;
};