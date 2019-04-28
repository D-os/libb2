/*******************************************************************************
/
/	File:			ColumnTypes.h
/
/   Description:    Experimental classes that implement particular column/field
/					data types for use in BColumnListView.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _COLUMN_TYPES_H
#define _COLUMN_TYPES_H

#include <experimental/ColumnListView.h>
#include <String.h>
#include <Font.h>

namespace BExperimental {

// Common base-class: a column that draws a standard title at its top.

class BTitledColumn : public BColumn {
public:

	BTitledColumn(const char *title, float width, float minWidth, float maxWidth);
	virtual void DrawTitle(BRect rect, BView *parent);
	
	void SetTitle(const char *title);
	void Title(BString *forTitle) const; // sets the BString arg to be the title

	float FontHeight() const;
	
private:

	float fFontHeight;
	BString fTitle;

};

// ---------------------------------------------------------------------------

// Field and column classes for strings.

class BStringField : public BField {
public:

	BStringField(const char *string);
	void SetTo(const char *);
	const char *Value();
	
private:

	BString	contents;
	BString clippedString;
	float lastWidth;
	friend class BStringColumn;
};

class BStringColumn : public BTitledColumn {
public:

	BStringColumn(const char *title, float width, float maxWidth, float minWidth,
		uint32 truncate);
	virtual void DrawField(BField *field, BRect rect, BView *parent);
	virtual int CompareFields(BField *field1, BField *field2);
private:

	uint32 fTruncate;
};

// ---------------------------------------------------------------------------

// Field and column classes for dates.

class BDateField : public BField {
public:

	BDateField(time_t *t);

private:	

	struct tm fTime;
	time_t fSeconds;
	BString clippedString;
	float lastWidth;
	friend class BDateColumn;
};

class BDateColumn : public BTitledColumn {
public:
	
	BDateColumn(const char *title, float width, float minWidth, float maxWidth);
	virtual void DrawField(BField *field, BRect rect, BView *parent);
	virtual int CompareFields(BField *field1, BField *field2);

private:
	
	BString fTitle;
};

// ---------------------------------------------------------------------------

// Field and column classes for numeric sizes.

class BSizeField : public BField {
public:

	BSizeField(uint32 size);
	uint32 fSize;
};

class BSizeColumn : public BTitledColumn {
public:

	BSizeColumn(const char *title, float width, float minWidth, float maxWidth);
	virtual void DrawField(BField *field, BRect rect, BView *parent);
	virtual int CompareFields(BField *field1, BField *field2);

};

// ---------------------------------------------------------------------------

// Field and column classes for integers.

class BIntegerField : public BField {
public:

	BIntegerField(int32 number);
	int32 fInteger;
};

class BIntegerColumn : public BTitledColumn {
public:

	BIntegerColumn(const char *title, float width, float minWidth, float maxWidth);
	virtual void DrawField(BField *field, BRect rect, BView *parent);
	virtual int CompareFields(BField *field1, BField *field2);
};

// Column to display BIntegerField objects as a graph.

class GraphColumn : public BIntegerColumn {
public:

	GraphColumn(const char *name, float width, float minWidth, float maxWidth);
	virtual void DrawField(BField *field, BRect rect, BView *parent);
};

}	// namespace BExperimental

using namespace BExperimental;

#endif

