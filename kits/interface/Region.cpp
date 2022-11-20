#include "Region.h"

#include <OS.h>
#include <include/core/SkRegion.h>
#include <pimpl.h>

#include <iostream>
#include <vector>

class BRegion::impl : public SkRegion
{
};

SkRegion *BRegion::_get_region() const
{
	return &*m;
}

#pragma mark - BRegion

BRegion::BRegion()
{
}

BRegion::BRegion(const BRegion &region) : BRegion()
{
	m->set(*region.m);
}

BRegion::BRegion(const BRect rect) : BRegion()
{
}

BRegion::~BRegion()
{
}

BRegion &BRegion::operator=(const BRegion &from)
{
	m->set(*from.m);
	return *this;
}

BRect BRegion::Frame() const
{
	const auto &bounds = m->getBounds();
	return BRect(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
}

clipping_rect BRegion::FrameInt() const
{
	const auto &bounds = m->getBounds();
	return clipping_rect{bounds.left(), bounds.top(), bounds.right(), bounds.bottom()};
}

BRect BRegion::RectAt(int32 index) const
{
	clipping_rect r = RectAtInt(index);
	return BRect(r.left, r.top, r.right, r.bottom);
}

clipping_rect BRegion::RectAtInt(int32 index) const
{
	if (m->isEmpty()) return {0, 0, 0, 0};

	SkRegion::Iterator		   iter(*m);
	std::vector<clipping_rect> rects;

	do {
		auto &r = iter.rect();
		rects.push_back({r.fLeft, r.fTop, r.fRight, r.fBottom});
		iter.next();
	} while (!iter.done());

	if (index >= rects.size()) return {0, 0, 0, 0};

	return rects.at(index);
}

int32 BRegion::CountRects() const
{
	if (m->isEmpty()) return 0;

	int32			   count = 0;
	SkRegion::Iterator iter(*m);
	do {
		count += 1;
		iter.next();
	} while (!iter.done());

	return count;
}

void BRegion::Set(BRect newBounds)
{
	m->setRect(SkIRect::MakeLTRB(int32_t(newBounds.left), int32_t(newBounds.top), int32_t(newBounds.right), int32_t(newBounds.bottom)));
}

void BRegion::Set(clipping_rect newBounds)
{
	m->setRect(SkIRect::MakeLTRB(newBounds.left, newBounds.top, newBounds.right, newBounds.bottom));
}

bool BRegion::Intersects(BRect r) const
{
	return m->intersects(SkIRect::MakeLTRB(int32_t(r.left), int32_t(r.top), int32_t(r.right), int32_t(r.bottom)));
}

bool BRegion::Intersects(clipping_rect r) const
{
	return m->intersects(SkIRect::MakeLTRB(r.left, r.top, r.right, r.bottom));
}

bool BRegion::Contains(BPoint pt) const
{
	return Contains(int32(pt.x), int32(pt.y));
}

bool BRegion::Contains(int32 x, int32 y) const
{
	return m->contains(x, y);
}

void BRegion::PrintToStream() const
{
	std::cout << *this << std::endl;
}

void BRegion::OffsetBy(int32 dh, int32 dv)
{
	m->translate(dh, dv);
}

void BRegion::MakeEmpty()
{
	m->setEmpty();
}

void BRegion::Include(BRect r)
{
	m->op(
		SkIRect::MakeLTRB(int32_t(r.left), int32_t(r.top), int32_t(r.right), int32_t(r.bottom)),
		SkRegion::kUnion_Op);
}

void BRegion::Include(clipping_rect r)
{
	m->op(
		SkIRect::MakeLTRB(r.left, r.top, r.right, r.bottom),
		SkRegion::kUnion_Op);
}

void BRegion::Include(const BRegion *r)
{
	if (!r) return;
	m->op(
		*(r->m),
		SkRegion::kUnion_Op);
}

void BRegion::Exclude(BRect r)
{
	m->op(
		SkIRect::MakeLTRB(int32_t(r.left), int32_t(r.top), int32_t(r.right), int32_t(r.bottom)),
		SkRegion::kDifference_Op);
}

void BRegion::Exclude(clipping_rect r)
{
	m->op(
		SkIRect::MakeLTRB(r.left, r.top, r.right, r.bottom),
		SkRegion::kDifference_Op);
}

void BRegion::Exclude(const BRegion *r)
{
	if (!r) return;
	m->op(
		*(r->m),
		SkRegion::kDifference_Op);
}

void BRegion::IntersectWith(const BRegion *r)
{
	if (!r) return;
	m->op(
		*(r->m),
		SkRegion::kIntersect_Op);
}

std::ostream &operator<<(std::ostream &os, const BRegion &value)
{
	os << "BRegion(";
	os << value.Frame();
	auto count = value.CountRects();
	if (count > 0) os << ": ";
	for (int32 i = 0; i < count; ++i) {
		if (i > 0) os << ", ";
		os << value.RectAt(i);
	}
	os << ")";
	return os;
}
