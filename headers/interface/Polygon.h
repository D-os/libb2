#ifndef _POLYGON_H
#define _POLYGON_H

#include <Rect.h>

#include <ostream>

class BPolygon
{
   public:
	BPolygon(const BPoint *ptArray, int32 numPoints);
	BPolygon();
	BPolygon(const BPolygon *poly);
	virtual ~BPolygon();

	BPolygon &operator=(const BPolygon &from);

	BRect		  Frame() const;
	void		  AddPoints(const BPoint *ptArray, int32 numPoints);
	int32		  CountPoints() const;
	const BPoint *Points() const;
	void		  MapTo(BRect srcRect, BRect dstRect);
	void		  PrintToStream() const;

   private:
	class impl;
	pimpl<impl> m;
};

/// C++ standard way of providing string conversions
std::ostream &operator<<(std::ostream &, const BPolygon &);

#endif /* _POLYGON_H */
