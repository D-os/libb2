/*******************************************************************************
/
/	File:		Query.h
/
/	Description:	Interface to the attribute query system.
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _QUERY_H
#define _QUERY_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <limits.h>

#include <SupportDefs.h>
#include <EntryList.h>
#include <OS.h>

typedef enum {
	B_INVALID_OP = 0,
	B_EQ,
	B_GT,
	B_GE,
	B_LT,
	B_LE,
	B_NE,
	B_CONTAINS,
	B_BEGINS_WITH,
	B_ENDS_WITH,
	B_AND = 0x101,
	B_OR,
	B_NOT,
	_B_RESERVED_OP_ = 0x100000
} query_op;

class	BMessenger;
class	BVolume;
struct	entry_ref;
class	BQueryStack;

class BQuery : public BEntryList {
public:
					BQuery();
virtual				~BQuery();

		status_t	Clear();

		void	PushAttr(const char *);
		void	PushOp(query_op op);

		void	PushUInt32(uint32);
		void	PushInt32(int32);
		void	PushUInt64(uint64);
		void	PushInt64(int64);
		void	PushFloat(float);
		void	PushDouble(double);
		void	PushString(const char *string, bool case_insensitive = false);
		status_t	PushDate(const char *date);

		status_t	SetVolume(const BVolume *vol);
		status_t	SetPredicate(const char *expr);
		status_t	SetTarget(BMessenger msngr);

		bool 		IsLive(void) const;

		status_t	GetPredicate(char *buf, size_t length);
		size_t		PredicateLength();			
		
		status_t	GetPredicate(BString *);

		dev_t		TargetDevice() const;
		
		status_t	Fetch();

virtual	status_t	GetNextEntry(BEntry *entry, bool traverse = FALSE);
virtual	status_t	GetNextRef(entry_ref *ref);
virtual	int32		GetNextDirents(struct dirent *buf, size_t length, 
						int32 num = INT_MAX);

/* DON'T CALL THESE.  They're no-ops inherited from BEntryList. */
virtual status_t		Rewind();
virtual int32			CountEntries();

private:

		/* FBC */
virtual	void		_QwertyQuery1();
virtual	void		_QwertyQuery2();
virtual	void		_QwertyQuery3();
virtual	void		_QwertyQuery4();
virtual	void		_QwertyQuery5();
virtual	void		_QwertyQuery6();
		int32		_qwertyData[4];

		BQueryStack *fStack;
		void		commit_stack();			
					
		char		*fPredicate;
		dev_t		fDev;
		bool		fLive;
		port_id		fPort;
		long		fToken;
		int			fQueryFd;
};

inline status_t BQuery::Rewind(void) { return (B_ERROR); }
inline int32 BQuery::CountEntries(void) { return (B_ERROR); }

#endif

