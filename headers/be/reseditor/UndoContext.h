/***************************************************************************
//
//	File:			UndoContext.h
//
//	Description:	Generic infrastructure for undo/redo support.
//
//	Copyright 2000, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

/**************************************************************************
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
// This code is experimental.  Please do not use it in your own
// applications, as future versions WILL break this interface.
//
***************************************************************************/

#ifndef _UNDO_CONTEXT_H
#define _UNDO_CONTEXT_H

#include <List.h>

class BUndoData
{
public:
	BUndoData();
	virtual ~BUndoData();
	
	virtual const void* Owner() const = 0;
	
	virtual bool HasData() const;
	
	virtual void Commit() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

namespace BPrivate {
	class UndoState;
}

class BUndoContext
{
public:
	BUndoContext();
	virtual ~BUndoContext();
	
	bool InUpdate() const;
	int32 UpdateCount() const;
	
	int32 Undo(int32 count=1);
	int32 Undo(const BList* context, int32 count=1);
	int32 Redo(int32 count=1);
	int32 Redo(const BList* context, int32 count=1);
	
	int32 ForgetUndos(int32 count=-1);
	int32 ForgetUndos(const BList* context, int32 count=-1);
	int32 ForgetRedos(int32 count=-1);
	int32 ForgetRedos(const BList* context, int32 count=-1);
	
	int32 CountUndos(const BList* context = 0) const;
	int32 CountRedos(const BList* context = 0) const;
	
	const char* UndoName(const BList* context = 0) const;
	const char* RedoName(const BList* context = 0) const;
	
	void StartUpdate(const char* name = 0);
	void SuggestUndoName(const char* name);
	
	BUndoData* AddData(BUndoData* data);
	BUndoData* FindData(const void* owner);
	BUndoData* RemoveData(BUndoData* data);
	
	void EndUpdate();
	
private:
	BPrivate::UndoState* TopUndo(const BList* context) const;
	BPrivate::UndoState* TopRedo(const BList* context) const;
	bool MatchContext(const BPrivate::UndoState* state, const BList* context) const;
	int32 FindPrevState(const BList* states, const BList* context, int32 from=-1) const;
	int32 FindNextState(const BList* states, const BList* context, int32 from=0) const;
	
	BList fUndos;
	BList fRedos;
	int32 fUpdateCount;
	BPrivate::UndoState* fWorking;
	bool fInUndo;
	bool fUpdateUndo;
};

#endif
