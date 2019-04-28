/*******************************************************************************
/
/	File:			ColumnListView.h
/
/   Description:    Experimental multi-column list view.
/
/	Copyright 2000, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _COLUMN_LIST_VIEW_H
#define _COLUMN_LIST_VIEW_H

#include <BeBuild.h>
#include <View.h>
#include <List.h>
#include <Invoker.h>
#include <ListView.h>

class BScrollBar;

namespace BExperimentalPrivate {

class OutlineView;
class TitleView;
class BRowContainer;
class RecursiveOutlineIterator;

}

using namespace BExperimentalPrivate;

namespace BExperimental {

enum LatchType {
	B_NO_LATCH,
	B_OPEN_LATCH,
	B_PRESSED_LATCH,
	B_CLOSED_LATCH
};

// A single row/column intersection in the list.
class BField {
public:
	BField();
	virtual ~BField();
};

// A single line in the list.  Each line contains a BField object
// for each column in the list, associated by their "logical field"
// index.  Hierarchies are formed by adding other BRow objects as
// a parent of a row, using the AddRow() function in BColumnListView().
class BRow {
public:
	BRow(float height = 16.0);
	virtual ~BRow();
	virtual bool HasLatch() const;

	int32 CountFields() const;
	BField* GetField(int32 logicalFieldIndex);
	const BField* GetField(int32 logicalFieldIndex) const;
	void SetField(BField*, int32 logicalFieldIndex);

	float Height() const;
	bool IsExpanded() const;

private:
	BList fFields;
	BRowContainer *fChildList;
	bool fIsExpanded;
	float fHeight;	
	BRow *fNextSelected;
	BRow *fPrevSelected;
	BRow *fParent;

	friend class BColumnListView;
	friend class RecursiveOutlineIterator;
	friend class OutlineView;
};

// Information about a single column in the list.  A column knows
// how to display the BField objects that occur at its location in
// each of the list's rows.  See ColumnTypes.h for particular
// subclasses of BField and BColumn that handle common data types.
class BColumn {
public:
	BColumn(float width, float minWidth, float maxWidth);
	virtual ~BColumn();
	
	float Width() const;
	void SetWidth(float width);
	float MinWidth() const;
	float MaxWidth() const;

	virtual void DrawTitle(BRect rect, BView *targetView);
	virtual void DrawField(BField *field, BRect rect, BView *targetView);
	virtual int CompareFields(BField *field1, BField *field2);

	bool IsVisible() const;
	void SetVisible(bool);

	int32 LogicalFieldNum() const;
	
private:
	float fWidth;
	float fMinWidth, fMaxWidth;
	bool fVisible;
	int32 fFieldID;
	BColumnListView *fList;
	bool fSortAscending;

	friend class OutlineView;
	friend class BColumnListView;
	friend class TitleView;
};

// The column list view class.
class BColumnListView : public BView, public BInvoker {
public:
	BColumnListView(BRect rect, const char *name, uint32 resizingMode,
		uint32 drawFlags, border_style = B_FANCY_BORDER);
	virtual ~BColumnListView();

	// Interaction
	virtual void InitiateDrag(BPoint, bool wasSelected);
	virtual void MessageDropped(BMessage*, BPoint point);
	virtual void ExpandOrCollapse(BRow *row, bool expand);
	virtual status_t Invoke(BMessage *message = NULL);
	virtual void ItemInvoked();
	virtual void SetInvocationMessage(BMessage*);
	BMessage *InvocationMessage() const;
	uint32 InvocationCommand() const;
	BRow *FocusRow() const;
	void SetMouseTrackingEnabled(bool);

	// Selection 
	list_view_type SelectionMode() const;
	void Deselect(BRow *row);
	void AddToSelection(BRow *row);
	void DeselectAll();
	BRow* CurrentSelection(BRow *lastSelected = 0) const;
	virtual void SelectionChanged();
	virtual void SetSelectionMessage(BMessage *);
	BMessage *SelectionMessage();
	uint32 SelectionCommand() const;
	void SetSelectionMode(list_view_type);  // list_view_type is defined in ListView.h.

	// Sorting
	void SetSortingEnabled(bool);
	bool SortingEnabled() const;
	void SetSortColumn(BColumn *column, bool add, bool ascending);
	void ClearSortColumns();

	// The status view is a little area in the lower left hand corner.
	void AddStatusView(BView *view);
	BView *RemoveStatusView();

	// Column Manipulation	
	void AddColumn(BColumn*, int32 logicalFieldIndex);
	void RemoveColumn(BColumn*);
	int32 CountColumns() const;
	BColumn* ColumnAt(int32 index) const;
	void SetColumnVisible(BColumn*, bool isVisible);
	void SetColumnVisible(int32, bool);	
	bool IsColumnVisible(int32) const;

	// Row manipulation
	const BRow* RowAt(int32 index, BRow *parent = 0) const;
	BRow* RowAt(int32 index, BRow *parent = 0);
	const BRow* RowAt(BPoint) const;
	BRow* RowAt(BPoint);
	bool GetRowRect(BRow *row, BRect *outRect);
	bool FindParent(BRow *row, BRow **outs_parent, bool *out_isVisible) const;
	int32 IndexOf(BRow *row);
	int32 CountRows(BRow *parent = 0) const;
	void AddRow(BRow*, BRow *parent = 0);
	void AddRow(BRow*, int32 index, BRow *parent = 0);
	void RemoveRow(BRow*); // currently does not delete row or its children.
	void UpdateRow(BRow*);
	void Clear();

	// Appearance
	virtual void SetFont(const BFont *font, uint32 mask = B_FONT_ALL);
	virtual void SetHighColor(rgb_color);
	void SetSelectionColor(rgb_color);
	void SetBackgroundColor(rgb_color); 
	const rgb_color& SelectionColor() const;
	const rgb_color& BackgroundColor() const;

	void SetLatchWidth(float width);
	float LatchWidth() const;
	virtual void DrawLatch(BView*, BRect, LatchType, BRow*);
	virtual void MakeFocus(bool isfocus = true);

protected:
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void AttachedToWindow();
	virtual void WindowActivated(bool active);
	virtual void Draw(BRect);

private:
	TitleView *fTitleView;
	OutlineView *fOutlineView;
	BList fColumns;
	rgb_color fBackgroundColor;
	rgb_color fSelectionColor;
	BScrollBar *fHorizontalScrollBar;
	BScrollBar *fVerticalScrollBar;
	BList fSortColumns;
	BView *fStatusView;
	BMessage *fSelectionMessage;
	bool fSortingEnabled;
	float fLatchWidth;
	border_style fBorderStyle;	
};

}	// namespace BExperimental

#endif
