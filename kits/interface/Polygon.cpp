#include "Polygon.h"

#include <OS.h>
#include <pimpl.h>

#include <iostream>
#include <vector>

class BPolygon::impl : public std::vector<BPoint>
{
};

BPolygon::BPolygon(const BPoint *ptArray, int32 numPoints) : BPolygon()
{
	m->reserve(numPoints);
	AddPoints(ptArray, numPoints);
}

BPolygon::BPolygon()
{
}

BPolygon::BPolygon(const BPolygon *poly) : BPolygon()
{
	if (!poly) return;
	*m = *poly->m;
}

BPolygon::~BPolygon()
{
}

BPolygon &BPolygon::operator=(const BPolygon &from)
{
	*m = *from.m;
	return *this;
}

void BPolygon::AddPoints(const BPoint *ptArray, int32 numPoints)
{
	if (!ptArray) return;
	for (int32 i = 0; i < numPoints; ++i) {
		m->push_back(*ptArray);
		ptArray += 1;
	}
}

int32 BPolygon::CountPoints() const
{
	return m->size();
}

const BPoint *BPolygon::Points() const
{
	return m->data();
}

void BPolygon::PrintToStream() const
{
	std::cout << *this << std::endl;
}

std::ostream &operator<<(std::ostream &os, const BPolygon &value)
{
	os << "BPolygon(";

	const BPoint *ptArray	= value.Points();
	const int32	  numPoints = value.CountPoints();
	for (int32 i = 0; i < numPoints; ++i) {
		if (i > 0) os << ", ";
		os << *ptArray;
		ptArray += 1;
	}

	os << ")";
	return os;
}
