#ifndef _FONT_H
#define _FONT_H

#include <InterfaceDefs.h>
#include <SupportDefs.h>

#define B_FONT_FAMILY_LENGTH 63
typedef char font_family[B_FONT_FAMILY_LENGTH + 1];
#define B_FONT_STYLE_LENGTH 63
typedef char font_style[B_FONT_STYLE_LENGTH + 1];

enum {
	B_CHAR_SPACING	 = 0,
	B_STRING_SPACING = 1,
	B_BITMAP_SPACING = 2,
	B_FIXED_SPACING	 = 3
};

enum font_direction {
	B_FONT_LEFT_TO_RIGHT = 0,
	B_FONT_RIGHT_TO_LEFT = 1
};

enum {
	B_DISABLE_ANTIALIASING = 0x00000001,
	B_FORCE_ANTIALIASING   = 0x00000002
};

enum {
	B_NO_TRUNCATION		 = ~0U,
	B_TRUNCATE_END		 = 0,
	B_TRUNCATE_BEGINNING = 1,
	B_TRUNCATE_MIDDLE	 = 2,
	B_TRUNCATE_SMART	 = 3
};

enum {
	B_UNICODE_UTF8	  = 0,
	B_ISO_8859_1	  = 1,
	B_ISO_8859_2	  = 2,
	B_ISO_8859_3	  = 3,
	B_ISO_8859_4	  = 4,
	B_ISO_8859_5	  = 5,
	B_ISO_8859_6	  = 6,
	B_ISO_8859_7	  = 7,
	B_ISO_8859_8	  = 8,
	B_ISO_8859_9	  = 9,
	B_ISO_8859_10	  = 10,
	B_MACINTOSH_ROMAN = 11
};

enum {
	B_SCREEN_FONT_CACHE		= 0x0001,
	B_PRINTING_FONT_CACHE	= 0x0002,
	B_DEFAULT_CACHE_SETTING = 0x0004,
	B_APP_CACHE_SETTING		= 0x0008
};

enum {
	B_HAS_TUNED_FONT = 0x0001,
	B_IS_FIXED		 = 0x0002
};

enum {
	B_ITALIC_FACE	  = 0x0001,
	B_UNDERSCORE_FACE = 0x0002,
	B_NEGATIVE_FACE	  = 0x0004,
	B_OUTLINED_FACE	  = 0x0008,
	B_STRIKEOUT_FACE  = 0x0010,
	B_BOLD_FACE		  = 0x0020,
	B_REGULAR_FACE	  = 0x0040
};

enum font_metric_mode {
	B_SCREEN_METRIC	  = 0,
	B_PRINTING_METRIC = 1
};

enum font_file_format {
	B_TRUETYPE_WINDOWS		   = 0,
	B_POSTSCRIPT_TYPE1_WINDOWS = 1
};

class unicode_block
{
   public:
	inline unicode_block();
	inline unicode_block(uint64 block2, uint64 block1);

	inline bool			  Includes(const unicode_block &block) const;
	inline unicode_block  operator&(const unicode_block &block) const;
	inline unicode_block  operator|(const unicode_block &block) const;
	inline unicode_block &operator=(const unicode_block &block);
	inline bool			  operator==(const unicode_block &block) const;
	inline bool			  operator!=(const unicode_block &block) const;

   private:
	uint64 fData[2];
};

struct edge_info
{
	float left;
	float right;
};

struct font_height
{
	float ascent;
	float descent;
	float leading;
	float x_height;
};

struct escapement_delta
{
	float nonspace;
	float space;
};

struct font_cache_info
{
	int32 sheared_font_penalty;
	int32 rotated_font_penalty;
	float oversize_threshold;
	int32 oversize_penalty;
	int32 cache_size;
	float spacing_size_threshold;
};

struct tuned_font_info
{
	float  size;
	float  shear;
	float  rotation;
	uint32 flags;
	uint16 face;
};

class BShape;
class BString;
class BPoint;

class SkFont;

class BFont
{
   public:
	BFont();
	BFont(const BFont &font);
	BFont(const BFont *font);
	~BFont();

	status_t SetFamilyAndStyle(const font_family family,
							   const font_style	 style);
	void	 SetFamilyAndStyle(uint32 code);
	status_t SetFamilyAndFace(const font_family family, uint16 face);

	void SetSize(float size);
	void SetShear(float shear);
	void SetRotation(float rotation);
	void SetSpacing(uint8 spacing);
	void SetEncoding(uint8 encoding);
	void SetFace(uint16 face);
	void SetFlags(uint32 flags);

	void   GetFamilyAndStyle(font_family *family,
							 font_style	*style) const;
	uint32 FamilyAndStyle() const;
	float  Size() const;
	float  Shear() const;
	float  Rotation() const;
	uint8  Spacing() const;
	uint8  Encoding() const;
	uint16 Face() const;
	uint32 Flags() const;

	font_direction	 Direction() const;
	bool			 IsFixed() const;
	bool			 IsFullAndHalfFixed() const;
	BRect			 BoundingBox() const;
	unicode_block	 Blocks() const;
	font_file_format FileFormat() const;

	int32 CountTuned() const;
	void  GetTunedInfo(int32 index, tuned_font_info *info) const;

	void TruncateString(BString *in_out,
						uint32	 mode,
						float	 width) const;
	void GetTruncatedStrings(const char *stringArray[],
							 int32		 numStrings,
							 uint32		 mode,
							 float		 width,
							 BString	 resultArray[]) const;
	void GetTruncatedStrings(const char *stringArray[],
							 int32		 numStrings,
							 uint32		 mode,
							 float		 width,
							 char		  *resultArray[]) const;

	float StringWidth(const char *string) const;
	float StringWidth(const char *string, int32 length) const;
	void  GetStringWidths(const char *stringArray[],
						  const int32 lengthArray[],
						  int32		  numStrings,
						  float		  widthArray[]) const;

	void GetEscapements(const char charArray[],
						int32	   numChars,
						float	   escapementArray[]) const;
	void GetEscapements(const char		  charArray[],
						int32			  numChars,
						escapement_delta *delta,
						float			  escapementArray[]) const;
	void GetEscapements(const char		  charArray[],
						int32			  numChars,
						escapement_delta *delta,
						BPoint			  escapementArray[]) const;
	void GetEscapements(const char		  charArray[],
						int32			  numChars,
						escapement_delta *delta,
						BPoint			  escapementArray[],
						BPoint			  offsetArray[]) const;

	void GetEdges(const char charArray[],
				  int32		 numBytes,
				  edge_info	 edgeArray[]) const;
	void GetHeight(font_height *height) const;

	void GetBoundingBoxesAsGlyphs(const char	   charArray[],
								  int32			   numChars,
								  font_metric_mode mode,
								  BRect			   boundingBoxArray[]) const;
	void GetBoundingBoxesAsString(const char		charArray[],
								  int32				numChars,
								  font_metric_mode	mode,
								  escapement_delta *delta,
								  BRect				boundingBoxArray[]) const;
	void GetBoundingBoxesForStrings(const char	   *stringArray[],
									int32			 numStrings,
									font_metric_mode mode,
									escapement_delta deltas[],
									BRect			 boundingBoxArray[]) const;

	void GetGlyphShapes(const char charArray[],
						int32	   numChars,
						BShape	   *glyphShapeArray[]) const;

	void GetHasGlyphs(const char charArray[],
					  int32		 numChars,
					  bool		 hasArray[]) const;

	BFont &operator=(const BFont &font);
	bool   operator==(const BFont &font) const;
	bool   operator!=(const BFont &font) const;

	void PrintToStream() const;

   private:
	// friend class BApplication;
	friend class BView;
	friend std::ostream &operator<<(std::ostream &, const BFont &);

	class impl;
	impl *m;
	SkFont &_get_font() const;
};

/// C++ standard way of providing string conversions
std::ostream &operator<<(std::ostream &, const BFont &);

// BFont related declarations
extern const BFont *be_plain_font;
extern const BFont *be_bold_font;
extern const BFont *be_fixed_font;

int32	 count_font_families();
status_t get_font_family(int32		  index,
						 font_family *name,
						 uint32		*flags = nullptr);

int32	 count_font_styles(font_family name);
status_t get_font_style(font_family family,
						int32		index,
						font_style *name,
						uint32	   *flags = nullptr);
status_t get_font_style(font_family family,
						int32		index,
						font_style *name,
						uint16	   *face,
						uint32	   *flags = nullptr);

bool update_font_families(bool check_only);

status_t get_font_cache_info(uint32 id, void *set);
status_t set_font_cache_info(uint32 id, void *set);

// unicode_block inlines

unicode_block::unicode_block() { fData[0] = fData[1] = 0LL; }

unicode_block::unicode_block(uint64 block2, uint64 block1)
{
	fData[0] = block1;
	fData[1] = block2;
}

bool unicode_block::Includes(const unicode_block &block) const
{
	return (((fData[0] & block.fData[0]) == block.fData[0]) && ((fData[1] & block.fData[1]) == block.fData[1]));
}

unicode_block unicode_block::operator&(const unicode_block &block) const
{
	unicode_block res;

	res.fData[0] = fData[0] & block.fData[0];
	res.fData[1] = fData[1] & block.fData[1];
	return res;
}

unicode_block unicode_block::operator|(const unicode_block &block) const
{
	unicode_block res;

	res.fData[0] = fData[0] | block.fData[0];
	res.fData[1] = fData[1] | block.fData[1];
	return res;
}

unicode_block &unicode_block::operator=(const unicode_block &block)
{
	fData[0] = block.fData[0];
	fData[1] = block.fData[1];
	return *this;
}

bool unicode_block::operator==(const unicode_block &block) const
{
	return ((fData[0] == block.fData[0]) && (fData[1] == block.fData[1]));
}

bool unicode_block::operator!=(const unicode_block &block) const
{
	return ((fData[0] != block.fData[0]) || (fData[1] != block.fData[1]));
}

#endif /* _FONT_H */
