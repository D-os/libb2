#ifndef _REGION_H
#define _REGION_H

#include <Rect.h>

/// Integer rect used to define a clipping rectangle. All bounds are included.
typedef struct
{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
} clipping_rect;

class SkRegion;

class BRegion
{
   public:
	BRegion();
	BRegion(const BRegion &region);
	BRegion(const BRect rect);
	virtual ~BRegion();

	BRegion &operator=(const BRegion &from);

	BRect		  Frame() const;
	clipping_rect FrameInt() const;
	BRect		  RectAt(int32 index) const;
	clipping_rect RectAtInt(int32 index) const;
	int32		  CountRects() const;
	void		  Set(BRect newBounds);
	void		  Set(clipping_rect newBounds);
	bool		  Intersects(BRect r) const;
	bool		  Intersects(clipping_rect r) const;
	bool		  Contains(BPoint pt) const;
	bool		  Contains(int32 x, int32 y) const;
	void		  PrintToStream() const;
	void		  OffsetBy(int32 dh, int32 dv);
	void		  MakeEmpty();
	void		  Include(BRect r);
	void		  Include(clipping_rect r);
	void		  Include(const BRegion *);
	void		  Exclude(BRect r);
	void		  Exclude(clipping_rect r);
	void		  Exclude(const BRegion *);
	void		  IntersectWith(const BRegion *);

   private:
	friend class BView;

	class impl;
	pimpl<impl> m;
	SkRegion	 *_get_region();
};

/// C++ standard way of providing string conversions
std::ostream &operator<<(std::ostream &, const BRegion &);

#endif /* _REGION_H */
