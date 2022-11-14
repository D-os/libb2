#include "Point.h"

#include <Rect.h>

#include <iomanip>
#include <iostream>

const BPoint B_ORIGIN(0, 0);

std::ostream& operator<<(std::ostream& os, const BPoint& value)
{
	os << "BPoint(" << std::fixed << std::setprecision(1);
	os << "x:" << value.x;
	os << ", y:" << value.y;
	os << ")";
	return os;
}

void BPoint::ConstrainTo(BRect rect)
{
	x = std::max(std::min(x, rect.right), rect.left);
	y = std::max(std::min(y, rect.bottom), rect.top);
}

void BPoint::PrintToStream() const
{
	std::cout << *this << std::endl;
}

BPoint BPoint::operator-() const
{
	return BPoint(-x, -y);
}

BPoint BPoint::operator+(const BPoint& other) const
{
	return BPoint(x + other.x, y + other.y);
}

BPoint BPoint::operator-(const BPoint& other) const
{
	return BPoint(x - other.x, y - other.y);
}

BPoint& BPoint::operator+=(const BPoint& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

BPoint& BPoint::operator-=(const BPoint& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

bool BPoint::operator!=(const BPoint& other) const
{
	return x != other.x || y != other.y;
}

bool BPoint::operator==(const BPoint& other) const
{
	return x == other.x && y == other.y;
}
