/*
 * Copyright 2001-2012, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef	_RECT_H
#define	_RECT_H


#include <math.h>

#include <Point.h>
#include <Size.h>


class BRect {
public:
			float				left;
			float				top;
			float				right;
			float				bottom;

								BRect();
								BRect(const BRect& other);
								BRect(float left, float top, float right,
									float bottom);
								BRect(BPoint leftTop, BPoint rightBottom);
								BRect(BPoint leftTop, BSize size);
								BRect(float side);

			BRect&				operator=(const BRect& other);
			void				Set(float left, float top, float right,
									float bottom);

			void				PrintToStream() const;

			BPoint				LeftTop() const;
			BPoint				RightBottom() const;
			BPoint				LeftBottom() const;
			BPoint				RightTop() const;

			void				SetLeftTop(const BPoint point);
			void				SetRightBottom(const BPoint point);
			void				SetLeftBottom(const BPoint point);
			void				SetRightTop(const BPoint point);

	// Transformation
			void				InsetBy(BPoint inset);
			void				InsetBy(float dx, float dy);
			void				OffsetBy(BPoint delta);
			void				OffsetBy(float dx, float dy);
			void				OffsetTo(BPoint offset);
			void				OffsetTo(float x, float y);

	// Expression transformations
			BRect&				InsetBySelf(BPoint inset);
			BRect&				InsetBySelf(float dx, float dy);
			BRect				InsetByCopy(BPoint inset) const;
			BRect				InsetByCopy(float dx, float dy) const;
			BRect&				OffsetBySelf(BPoint offset);
			BRect&				OffsetBySelf(float dx, float dy);
			BRect				OffsetByCopy(BPoint offset) const;
			BRect				OffsetByCopy(float dx, float dy) const;
			BRect&				OffsetToSelf(BPoint offset);
			BRect&				OffsetToSelf(float x, float y);
			BRect				OffsetToCopy(BPoint offset) const;
			BRect				OffsetToCopy(float x, float y) const;

	// Comparison
			bool				operator==(BRect other) const;
			bool				operator!=(BRect other) const;

	// Intersection and union
			BRect				operator&(BRect other) const;
			BRect				operator|(BRect other) const;

			bool				IsValid() const;
			float				Width() const;
			int32				IntegerWidth() const;
			float				Height() const;
			int32				IntegerHeight() const;
			BSize				Size() const;

			bool				Intersects(BRect rect) const;
			bool				Contains(BPoint point) const;
			bool				Contains(BRect rect) const;
};


// #pragma mark - inline definitions

inline BPoint
BRect::LeftTop() const
{
	return *(const BPoint*)&left;
}


inline BPoint
BRect::RightBottom() const
{
	return *(const BPoint*)&right;
}


inline BPoint
BRect::LeftBottom() const
{
	return BPoint(left, bottom);
}


inline BPoint
BRect::RightTop() const
{
	return BPoint(right, top);
}


inline
BRect::BRect()
	:
	left(0),
	top(0),
	right(-1),
	bottom(-1)
{
}


inline
BRect::BRect(float left, float top, float right, float bottom)
	:
	left(left),
	top(top),
	right(right),
	bottom(bottom)
{
}


inline
BRect::BRect(const BRect& other)
	:
	left(other.left),
	top(other.top),
	right(other.right),
	bottom(other.bottom)
{
}


inline
BRect::BRect(BPoint leftTop, BPoint rightBottom)
	:
	left(leftTop.x),
	top(leftTop.y),
	right(rightBottom.x),
	bottom(rightBottom.y)
{
}


inline
BRect::BRect(BPoint leftTop, BSize size)
	:
	left(leftTop.x),
	top(leftTop.y),
	right(leftTop.x + size.width),
	bottom(leftTop.y + size.height)
{
}


inline
BRect::BRect(float side)
	:
	left(0),
	top(0),
	right(side - 1),
	bottom(side - 1)
{
}


inline BRect&
BRect::operator=(const BRect& other)
{
	left = other.left;
	top = other.top;
	right = other.right;
	bottom = other.bottom;
	return *this;
}


inline void
BRect::Set(float left, float top, float right, float bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}


inline bool
BRect::IsValid() const
{
	return left <= right && top <= bottom;
}


inline int32
BRect::IntegerWidth() const
{
	return (int32)ceil(right - left);
}


inline float
BRect::Width() const
{
	return right - left;
}


inline int32
BRect::IntegerHeight() const
{
	return (int32)ceil(bottom - top);
}


inline float
BRect::Height() const
{
	return bottom - top;
}


inline BSize
BRect::Size() const
{
	return BSize(right - left, bottom - top);
}


#endif	// _RECT_H
