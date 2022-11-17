#include "Rect.h"

#include <iomanip>
#include <iostream>

void BRect::PrintToStream() const
{
	std::cout << *this << std::endl;
}

void BRect::SetLeftTop(const BPoint point)
{
	left = point.x;
	top	 = point.y;
}

void BRect::SetRightBottom(const BPoint point)
{
	right  = point.x;
	bottom = point.y;
}

void BRect::SetLeftBottom(const BPoint point)
{
	left   = point.x;
	bottom = point.y;
}

void BRect::SetRightTop(const BPoint point)
{
	right = point.x;
	top	  = point.y;
}

void BRect::InsetBy(BPoint point)
{
	left += point.x;
	right -= point.x;
	top += point.y;
	bottom -= point.y;
}

void BRect::InsetBy(float dx, float dy)
{
	left += dx;
	right -= dx;
	top += dy;
	bottom -= dy;
}

void BRect::OffsetBy(BPoint point)
{
	left += point.x;
	right += point.x;
	top += point.y;
	bottom += point.y;
}

void BRect::OffsetBy(float dx, float dy)
{
	left += dx;
	right += dx;
	top += dy;
	bottom += dy;
}

void BRect::OffsetTo(BPoint point)
{
	right  = (right - left) + point.x;
	left   = point.x;
	bottom = (bottom - top) + point.y;
	top	   = point.y;
}

void BRect::OffsetTo(float x, float y)
{
	right  = (right - left) + x;
	left   = x;
	bottom = (bottom - top) + y;
	top	   = y;
}

BRect& BRect::InsetBySelf(BPoint point)
{
	InsetBy(point);
	return *this;
}

BRect& BRect::InsetBySelf(float dx, float dy)
{
	InsetBy(dx, dy);
	return *this;
}

BRect BRect::InsetByCopy(BPoint point) const
{
	BRect copy(*this);
	copy.InsetBy(point);
	return copy;
}

BRect BRect::InsetByCopy(float dx, float dy) const
{
	BRect copy(*this);
	copy.InsetBy(dx, dy);
	return copy;
}

BRect& BRect::OffsetBySelf(BPoint point)
{
	OffsetBy(point);
	return *this;
}

BRect& BRect::OffsetBySelf(float dx, float dy)
{
	OffsetBy(dx, dy);
	return *this;
}

BRect BRect::OffsetByCopy(BPoint point) const
{
	BRect copy(*this);
	copy.OffsetBy(point);
	return copy;
}

BRect BRect::OffsetByCopy(float dx, float dy) const
{
	BRect copy(*this);
	copy.OffsetBy(dx, dy);
	return copy;
}

BRect& BRect::OffsetToSelf(BPoint point)
{
	OffsetTo(point);
	return *this;
}

BRect& BRect::OffsetToSelf(float x, float y)
{
	OffsetTo(x, y);
	return *this;
}

BRect BRect::OffsetToCopy(BPoint point) const
{
	BRect copy(*this);
	copy.OffsetTo(point);
	return copy;
}

BRect BRect::OffsetToCopy(float x, float y) const
{
	BRect copy(*this);
	copy.OffsetTo(x, y);
	return copy;
}

bool BRect::operator==(BRect other) const
{
	return left == other.left && right == other.right && top == other.top && bottom == other.bottom;
}

bool BRect::operator!=(BRect other) const
{
	return !(*this == other);
}

BRect BRect::operator&(BRect other) const
{
	return BRect(std::max(left, other.left), std::max(top, other.top),
				 std::min(right, other.right), std::min(bottom, other.bottom));
}

BRect BRect::operator|(BRect other) const
{
	return BRect(std::min(left, other.left), std::min(top, other.top),
				 std::max(right, other.right), std::max(bottom, other.bottom));
}

bool BRect::Intersects(BRect rect) const
{
	if (!IsValid() || !rect.IsValid())
		return false;

	return !(rect.left > right || rect.right < left
			 || rect.top > bottom || rect.bottom < top);
}

bool BRect::Contains(BPoint point) const
{
	return point.x >= left && point.x <= right
		   && point.y >= top && point.y <= bottom;
}

bool BRect::Contains(BRect rect) const
{
	return rect.left >= left && rect.right <= right
		   && rect.top >= top && rect.bottom <= bottom;
}

std::ostream& operator<<(std::ostream& os, const BRect& value)
{
	os << "BRect(" << std::fixed << std::setprecision(1);
	os << "l:" << value.left;
	os << ", t:" << value.top;
	os << ", r:" << value.right;
	os << ", b:" << value.bottom;
	os << ")";
	return os;
}
