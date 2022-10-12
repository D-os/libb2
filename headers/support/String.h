#ifndef __BSTRING__
#define __BSTRING__

#include <SupportDefs.h>
#include <string.h>

#include <ostream>

class BString
{
   public:
	BString();
	BString(const char *);
	BString(const BString &);
	BString(const char *, int32 maxLength);

	~BString();

	/// Access
	const char *String() const;	 /// returns null-terminated string

	int32 Length() const;  /// length of corresponding string

	int32 CountChars() const;  /// returns number of UTF8 characters in string

	/// Assignment
	BString &operator=(const BString &);
	BString &operator=(const char *);
	BString &operator=(char);

	BString &SetTo(const char *);
	BString &SetTo(const char *, int32 length);

	BString &SetTo(const BString &from);
	BString &Adopt(BString &from);	/// leaves <from> empty, avoiding a copy

	BString &SetTo(const BString &, int32 length);
	BString &Adopt(BString &from, int32 length);  /// leaves <from> empty, avoiding a copy

	BString &SetTo(char, int32 count);

	/// Substring copying

	/// returns <into> ref as it's result; doesn't do anything if <into> is <this>
	BString &CopyInto(BString &into, int32 fromOffset, int32 length) const;

	/// caller guarantees that <into> is large enough
	void CopyInto(char *into, int32 fromOffset, int32 length) const;

	/// Appending
	BString &operator+=(const BString &);
	BString &operator+=(const char *);
	BString &operator+=(char);

	BString &Append(const BString &);
	BString &Append(const char *);

	BString &Append(const BString &, int32 length);
	BString &Append(const char *, int32 length);
	BString &Append(char, int32 count);

	/// Prepending
	BString &Prepend(const char *);
	BString &Prepend(const BString &);
	BString &Prepend(const char *, int32);
	BString &Prepend(const BString &, int32);
	BString &Prepend(char, int32 count);

	/// Inserting
	BString &Insert(const char *, int32 pos);
	BString &Insert(const char *, int32 length, int32 pos);
	BString &Insert(const char *, int32 fromOffset, int32 length, int32 pos);

	BString &Insert(const BString &, int32 pos);
	BString &Insert(const BString &, int32 length, int32 pos);
	BString &Insert(const BString &, int32 fromOffset, int32 length, int32 pos);
	BString &Insert(char, int32 count, int32 pos);

	/// Removing

	/// pass false in <lazy> to ensure freeing up the truncated memory
	BString &Truncate(int32 newLength, bool lazy = true);

	BString &Remove(int32 from, int32 length);

	BString &RemoveFirst(const BString &);
	BString &RemoveLast(const BString &);
	BString &RemoveAll(const BString &);

	BString &RemoveFirst(const char *);
	BString &RemoveLast(const char *);
	BString &RemoveAll(const char *);

	BString &RemoveSet(const char *setOfCharsToRemove);

	BString &MoveInto(BString &into, int32 from, int32 length);
	void	 MoveInto(char *into, int32 from, int32 length);  /// caller guarantees that <into> is large enough

	/// Compare functions
	bool operator<(const BString &) const;
	bool operator<=(const BString &) const;
	bool operator==(const BString &) const;
	bool operator>=(const BString &) const;
	bool operator>(const BString &) const;
	bool operator!=(const BString &) const;

	bool operator<(const char *) const;
	bool operator<=(const char *) const;
	bool operator==(const char *) const;
	bool operator>=(const char *) const;
	bool operator>(const char *) const;
	bool operator!=(const char *) const;

	/// strcmp-style compare functions
	int Compare(const BString &) const;
	int Compare(const char *) const;
	int Compare(const BString &, int32 n) const;
	int Compare(const char *, int32 n) const;
	int ICompare(const BString &) const;
	int ICompare(const char *) const;
	int ICompare(const BString &, int32 n) const;
	int ICompare(const char *, int32 n) const;

	/// Searching
	int32 FindFirst(const BString &) const;
	int32 FindFirst(const char *) const;
	int32 FindFirst(const BString &, int32 fromOffset) const;
	int32 FindFirst(const char *, int32 fromOffset) const;
	int32 FindFirst(char) const;
	int32 FindFirst(char, int32 fromOffset) const;

	int32 FindLast(const BString &) const;
	int32 FindLast(const char *) const;
	int32 FindLast(const BString &, int32 beforeOffset) const;
	int32 FindLast(const char *, int32 beforeOffset) const;
	int32 FindLast(char) const;
	int32 FindLast(char, int32 fromOffset) const;

	int32 IFindFirst(const BString &) const;
	int32 IFindFirst(const char *) const;
	int32 IFindFirst(const BString &, int32 fromOffset) const;
	int32 IFindFirst(const char *, int32 fromOffset) const;

	int32 IFindLast(const BString &) const;
	int32 IFindLast(const char *) const;
	int32 IFindLast(const BString &, int32 beforeOffset) const;
	int32 IFindLast(const char *, int32 beforeOffset) const;

	/// Replacing
	BString &ReplaceFirst(char replaceThis, char withThis);
	BString &ReplaceLast(char replaceThis, char withThis);
	BString &ReplaceAll(char replaceThis, char withThis, int32 fromOffset = 0);
	BString &Replace(char replaceThis, char withThis, int32 maxReplaceCount, int32 fromOffset = 0);
	BString &ReplaceFirst(const char *replaceThis, const char *withThis);
	BString &ReplaceLast(const char *replaceThis, const char *withThis);
	BString &ReplaceAll(const char *replaceThis, const char *withThis, int32 fromOffset = 0);
	BString &Replace(const char *replaceThis, const char *withThis, int32 maxReplaceCount, int32 fromOffset = 0);

	BString &IReplaceFirst(char replaceThis, char withThis);
	BString &IReplaceLast(char replaceThis, char withThis);
	BString &IReplaceAll(char replaceThis, char withThis, int32 fromOffset = 0);
	BString &IReplace(char replaceThis, char withThis, int32 maxReplaceCount, int32 fromOffset = 0);
	BString &IReplaceFirst(const char *replaceThis, const char *withThis);
	BString &IReplaceLast(const char *replaceThis, const char *withThis);
	BString &IReplaceAll(const char *replaceThis, const char *withThis, int32 fromOffset = 0);
	BString &IReplace(const char *replaceThis, const char *withThis, int32 maxReplaceCount, int32 fromOffset = 0);

	BString &ReplaceSet(const char *setOfChars, char with);
	BString &ReplaceSet(const char *setOfChars, const char *with);

	/// Unchecked char access
	char  operator[](int32 index) const;
	char &operator[](int32 index);

	/// Checked char access
	char ByteAt(int32 index) const;

	/// Fast low-level manipulation

	/// Make room for characters to be added by C-string like manipulation.
	/// Returns the equivalent of String(), <maxLength> includes space for
	/// trailing zero while used as C-string, it is illegal to call other
	/// BString routines that rely on data/length consistency until
	/// UnlockBuffer sets things up again.
	char *LockBuffer(int32 maxLength);

	/// Finish using BString as C-string, adjusting length. If no length
	/// passed in, strlen of internal data is used to determine it.
	/// BString is in consistent state after this.
	BString &UnlockBuffer(int32 length = -1);

	/// Upercase<->Lowercase
	BString &ToLower();
	BString &ToUpper();

	BString &Capitalize();
	/* Converts first character to upper-case, rest to
	 * lower-case
	 */

	BString &CapitalizeEachWord();
	/* Converts first character in each white-space-separated
	 * word to upper-case, rest to lower-case
	 */

	/// Escaping and Deescaping
	BString &CharacterEscape(const char *original, const char *setOfCharsToEscape, char escapeWith);
	/* copies original into <this>, escaping characters
	 * specified in <setOfCharsToEscape> by prepending
	 * them with <escapeWith>
	 */
	BString &CharacterEscape(const char *setOfCharsToEscape,
							 char		 escapeWith);
	/* escapes characters specified in <setOfCharsToEscape>
	 * by prepending them with <escapeWith>
	 */

	BString &CharacterDeescape(const char *original, char escapeChar);
	/* copy <original> into the string removing the escaping
	 * characters <escapeChar>
	 */
	BString &CharacterDeescape(char escapeChar);
	/* remove the escaping characters <escapeChar> from
	 * the string
	 */

	/// Simple sprintf replacement calls
	/// Slower than sprintf but type and overflow safe
	BString &operator<<(const char *);
	BString &operator<<(const BString &);
	BString &operator<<(char);
	BString &operator<<(uint32);
	BString &operator<<(int32);
	BString &operator<<(uint64);
	BString &operator<<(int64);
	BString &operator<<(float);

   private:
	void *data;
};

/// Comutative compare operators
bool operator<(const char *, const BString &);
bool operator<=(const char *, const BString &);
bool operator==(const char *, const BString &);
bool operator>(const char *, const BString &);
bool operator>=(const char *, const BString &);
bool operator!=(const char *, const BString &);

/// Non-member compare for sorting, etc.
int Compare(const BString &, const BString &);
int ICompare(const BString &, const BString &);
int Compare(const BString *, const BString *);
int ICompare(const BString *, const BString *);

/// C++ standard way of providing string conversions
std::ostream &operator<<(std::ostream &os, const BString &value);

#endif /* __BSTRING__ */
