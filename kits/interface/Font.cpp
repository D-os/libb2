#include "Font.h"

#define LOG_TAG "BFont"

#include <Rect.h>
#include <String.h>
#include <doctest/doctest.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkTextBlob.h>
#include <include/core/SkTypeface.h>
#include <log/log.h>

#include <format>
#include <iostream>

#include "./truncate_string.h"

class BFont::impl
{
   public:
	SkFont font;

	float  shear;
	uint32 flags;

	impl() : font(nullptr, 9.0f), shear{90.0f}, flags{0}
	{
		// TODO: Get from Control Panel settings
		font.setSubpixel(true);
		font.setEdging(SkFont::Edging::kSubpixelAntiAlias);
		font.setHinting(SkFontHinting::kNormal);
	}
};

BFont::BFont() : BFont(be_plain_font) {}

BFont::BFont(const BFont &font) : BFont(&font) {}

BFont::BFont(const BFont *font) : m{new BFont::impl()}
{
	if (font && this != font) {
		*this->m = *font->m;
		return;
	}

	if (this != be_plain_font) {
		// copy be_plain_font, which might be modified
		*this->m = *be_plain_font->m;
		return;
	}
}

BFont::~BFont()
{
	delete m;
}

BFont &BFont::operator=(const BFont &font)
{
	*m = *font.m;
	return *this;
}

bool BFont::operator==(const BFont &font) const
{
	return FamilyAndStyle() == font.FamilyAndStyle()
		   && Size() == font.Size()
		   && Shear() == font.Shear()
		   && Rotation() == font.Rotation()
		   && Spacing() == font.Spacing()
		   && Encoding() == font.Encoding()
		   && Face() == font.Face();
}

bool BFont::operator!=(const BFont &font) const
{
	return FamilyAndStyle() != font.FamilyAndStyle()
		   || Size() != font.Size()
		   || Shear() != font.Shear()
		   || Rotation() != font.Rotation()
		   || Spacing() != font.Spacing()
		   || Encoding() != font.Encoding()
		   || Face() != font.Face();
}

status_t BFont::SetFamilyAndStyle(const font_family family, const font_style style)
{
	auto mgr = SkFontMgr::RefDefault();
	SkTypeface *typeface = m->font.getTypeface();

	SkString familyName;
	if (family) {
		familyName.set(family);
	}
	else {
		if (typeface) typeface->getFamilyName(&familyName);
	}

	if (!style) {
		m->font.setTypeface(SkTypeface::MakeFromName(
			familyName.c_str(),
			typeface ? typeface->fontStyle() : SkFontStyle::Normal()));
		return B_OK;
	}

	SkFontStyleSet *styleSet = mgr->matchFamily(familyName.c_str());
	for (auto index = 0, count = styleSet->count(); index < count; ++index) {
		SkFontStyle fontStyle;
		SkString	styleName;
		styleSet->getStyle(index, &fontStyle, &styleName);
		if (styleName.equals(style)) {
			SkTypeface *typeface = styleSet->matchStyle(fontStyle);
			m->font.setTypeface(sk_sp<SkTypeface>(typeface));
			styleSet->unref();
			return B_OK;
		}
	}
	styleSet->unref();
	return B_BAD_VALUE;
}

void BFont::SetFamilyAndStyle(uint32 code)
{
	auto	 mgr = SkFontMgr::RefDefault();
	SkString familyName;
	for (auto index = 0, count = mgr->countFamilies(); index < count; ++index) {
		mgr->getFamilyName(index, &familyName);
		auto styleSet = mgr->matchFamily(familyName.c_str());
		for (auto index = 0, count = styleSet->count(); index < count; ++index) {
			auto typeface = styleSet->createTypeface(index);
			if (typeface->uniqueID() == code) {
				m->font.setTypeface(sk_sp<SkTypeface>(typeface));
				break;
			}
		}
		styleSet->unref();
	}
}

void BFont::SetSize(float size)
{
	m->font.setSize(size);
}

void BFont::SetShear(float shear)
{
	m->shear = shear;
	// SkewX is computed as x' = x + xSkew · y (positive value skews to the left)
	// BFont Shear is 45.0° (slanted to the right) through 135.0°, with 90.0° default.
	// We round to 2 decimal places to have straight up as 0.00, not 0.000000000000something.
	m->font.setSkewX(std::round(((shear - 90.0f) * 100.0f) / 45.0f) / 100.0f);
}

void BFont::SetRotation(float rotation)
{
	// not supported
}

void BFont::SetSpacing(uint8 spacing)
{
	// not supported
}

void BFont::SetEncoding(uint8 encoding)
{
	// not supported
}

void BFont::SetFace(uint16 face)
{
	SkTypeface *typeface = m->font.getTypeface();
	if (typeface) {
		auto		mgr = SkFontMgr::RefDefault();
		SkFontStyle faceStyle(
			face & B_REGULAR_FACE ? SkFontStyle::kNormal_Weight : (face & B_BOLD_FACE ? SkFontStyle::kBold_Weight : SkFontStyle::kNormal_Weight),
			SkFontStyle::kNormal_Width,
			face & B_REGULAR_FACE ? SkFontStyle::kUpright_Slant : (face & B_ITALIC_FACE ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant));

		SkString familyName;
		typeface->getFamilyName(&familyName);

		SkFontStyleSet *styleSet	  = mgr->matchFamily(familyName.c_str());
		SkTypeface	   *styleTypeface = styleSet->matchStyle(faceStyle);
		m->font.setTypeface(sk_sp<SkTypeface>(styleTypeface));
	}
}

void BFont::SetFlags(uint32 flags)
{
	m->flags = flags;
	m->font.setEdging((flags & B_DISABLE_ANTIALIASING) ? SkFont::Edging::kAlias : SkFont::Edging::kSubpixelAntiAlias);
}

void BFont::GetFamilyAndStyle(font_family *family, font_style *style) const
{
	if (!family && !style) return;

	if (family) {
		*family[0] = '\0';
	}
	if (style) {
		*style[0] = '\0';
	}

	SkTypeface *typeface = m->font.getTypeface();
	if (typeface) {
		SkString familyName;
		typeface->getFamilyName(&familyName);

		if (family) {
			strncpy(reinterpret_cast<char *>(family), familyName.c_str(), B_FONT_FAMILY_LENGTH);
			(*family)[B_FONT_FAMILY_LENGTH] = '\0';
		}

		if (style) {
			SkString familyName;
			typeface->getFamilyName(&familyName);
			SkFontStyle fontStyle = typeface->fontStyle();

			auto		mgr		 = SkFontMgr::RefDefault();
			auto		styleSet = mgr->matchFamily(familyName.c_str());
			SkFontStyle setFontStyle;
			SkString	setStyleName;
			for (auto index = 0, count = styleSet->count(); index < count; ++index) {
				styleSet->getStyle(index, &setFontStyle, &setStyleName);
				if (setFontStyle == fontStyle) {
					strncpy(reinterpret_cast<char *>(style), setStyleName.c_str(), B_FONT_STYLE_LENGTH);
					(*style)[B_FONT_STYLE_LENGTH] = '\0';
					break;
				}
			}
			styleSet->unref();
		}
	}
}

uint32 BFont::FamilyAndStyle() const
{
	SkTypeface *typeface = m->font.getTypeface();
	return typeface ? typeface->uniqueID() : 0;
}

float BFont::Size() const
{
	return m->font.getSize();
}

float BFont::Shear() const
{
	return m->shear;
}

float BFont::Rotation() const
{
	return 0.0f;
}

uint8 BFont::Spacing() const
{
	return B_BITMAP_SPACING;
}

uint8 BFont::Encoding() const
{
	return B_UNICODE_UTF8;
}

uint16 BFont::Face() const
{
	uint16		face	 = 0;
	SkTypeface *typeface = m->font.getTypeface();
	if (typeface) {
		if (typeface->isItalic()) face |= B_ITALIC_FACE;
		if (typeface->isBold()) face |= B_BOLD_FACE;
		if (face == 0) face |= B_REGULAR_FACE;
	}

	return face;
}

uint32 BFont::Flags() const
{
	return m->flags;
}

font_direction BFont::Direction() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_FONT_LEFT_TO_RIGHT;
}

bool BFont::IsFixed() const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

bool BFont::IsFullAndHalfFixed() const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

BRect BFont::BoundingBox() const
{
	debugger(__PRETTY_FUNCTION__);
	return BRect();
}

unicode_block BFont::Blocks() const
{
	debugger(__PRETTY_FUNCTION__);
	return unicode_block();
}

font_file_format BFont::FileFormat() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_TRUETYPE_WINDOWS;
}

void BFont::TruncateString(BString *inOut, uint32 mode, float width) const
{
	if (mode == B_NO_TRUNCATION)
		return;

	// NOTE: Careful, we cannot directly use "inOut->String()" as result
	// array, because the string length increases by 3 bytes in the worst
	// case scenario.
	const char *string = inOut->String();
	GetTruncatedStrings(&string, 1, mode, width, inOut);
}

void BFont::GetTruncatedStrings(const char *stringArray[], int32 numStrings,
								uint32 mode, float width, BString resultArray[]) const
{
	if (stringArray && numStrings > 0) {
		// the width of the "…" glyph
		float ellipsisWidth = StringWidth(B_UTF8_ELLIPSIS);

		for (int32 i = 0; i < numStrings; i++) {
			resultArray[i] = stringArray[i];
			int32 numChars = resultArray[i].CountChars();

			// get the escapement of each glyph in font units
			float *escapementArray = new float[numChars];
			GetEscapements(stringArray[i], numChars, nullptr, escapementArray);

			truncate_string(resultArray[i], mode, width, escapementArray,
							Size(), ellipsisWidth, numChars);

			delete[] escapementArray;
		}
	}
}

void BFont::GetTruncatedStrings(const char *stringArray[], int32 numStrings,
								uint32 mode, float width, char *resultArray[]) const
{
	if (stringArray && numStrings > 0) {
		for (int32 i = 0; i < numStrings; i++) {
			BString *strings = new BString[numStrings];
			GetTruncatedStrings(stringArray, numStrings, mode, width, strings);

			for (int32 i = 0; i < numStrings; i++)
				strcpy(resultArray[i], strings[i].String());

			delete[] strings;
		}
	}
}

float BFont::StringWidth(const char *string) const
{
	if (!string) return 0.0f;

	return StringWidth(string, strlen(string));
}

float BFont::StringWidth(const char *string, int32 length) const
{
	if (!string) return 0.0f;

	return m->font.measureText(string, length, SkTextEncoding::kUTF8);
}

void BFont::GetStringWidths(const char *stringArray[], const int32 lengthArray[], int32 numStrings, float widthArray[]) const
{
	if (!stringArray || !lengthArray || numStrings < 1 || !widthArray) {
		return;
	}

	for (int32 i = 0; i < numStrings; i++)
		widthArray[i] = StringWidth(stringArray[i], lengthArray[i]);
}

void BFont::GetEscapements(const char charArray[], int32 numChars, float escapementArray[]) const
{
	GetEscapements(charArray, numChars, nullptr, escapementArray);
}

void BFont::GetEscapements(const char charArray[], int32 numChars, escapement_delta *delta, float escapementArray[]) const
{
	if (delta) {
		debugger(__PRETTY_FUNCTION__);
	}

	SkGlyphID glyphs[numChars];
	int		  count = m->font.textToGlyphs(charArray, numChars, SkTextEncoding::kUTF8, glyphs, numChars);

	SkScalar widths[count];
	m->font.getWidths(glyphs, count, widths);
	for (int32 i = 0; i < count; i++)
		escapementArray[i] = SkScalarToFloat(widths[i]);
}

void BFont::GetEscapements(const char charArray[], int32 numChars, escapement_delta *delta, BPoint escapementArray[]) const
{
	GetEscapements(charArray, numChars, delta, escapementArray, nullptr);
}

void BFont::GetEscapements(const char charArray[], int32 numChars, escapement_delta *delta, BPoint escapementArray[], BPoint offsetArray[]) const
{
	if (delta) {
		debugger(__PRETTY_FUNCTION__);
	}
	if (offsetArray) {
		debugger(__PRETTY_FUNCTION__);
	}

	SkGlyphID glyphs[numChars];
	int		  count = m->font.textToGlyphs(charArray, numChars, SkTextEncoding::kUTF8, glyphs, numChars);

	SkPoint pos[count];
	m->font.getPos(glyphs, count, pos);
	for (int32 i = 0; i < count; i++) {
		escapementArray->Set(pos[i].x(), pos[i].y());
	};
}

void BFont::GetHeight(font_height *height) const
{
	if (!height) return;

	SkFontMetrics metrics;
	m->font.getMetrics(&metrics);
	*height = font_height{metrics.fCapHeight, metrics.fDescent, metrics.fLeading, metrics.fXHeight};
}

void BFont::PrintToStream() const
{
	std::cout << *this << std::endl;
}

SkFont &BFont::_get_font() const
{
	return m->font;
}

#pragma mark globals

std::ostream &operator<<(std::ostream &os, const BFont &value)
{
	font_family family;
	font_style	style;
	value.GetFamilyAndStyle(&family, &style);
	font_height height;
	value.GetHeight(&height);

	os << std::format("BFont('{}', '{}' ({}) {:#x}/{:#b} {:.1f}deg/{:.1f}deg {:.1f}pt ({:.1f} {:.1f} {:.1f}/{:.1f}) {:#x})",
					  reinterpret_cast<const char *>(family), reinterpret_cast<const char *>(style), value.FamilyAndStyle(),
					  value.Face(), value.Flags(), value.Shear(), value.Rotation(), value.Size(),
					  height.ascent, height.descent, height.leading, height.x_height, value.Encoding());
	return os;
}

static BFont sPlainFont;
static BFont sBoldFont;
static BFont sFixedFont;

const BFont *be_plain_font = &sPlainFont;
const BFont *be_bold_font  = &sBoldFont;
const BFont *be_fixed_font = &sFixedFont;

int32 count_font_families()
{
	return SkFontMgr::RefDefault()->countFamilies();
}

status_t get_font_family(int32 index, font_family *name, uint32 *flags)
{
	auto	 mgr = SkFontMgr::RefDefault();
	SkString familyName;
	mgr->getFamilyName(index, &familyName);

	if (familyName.isEmpty())
		return B_BAD_INDEX;

	if (name) {
		strncpy(reinterpret_cast<char *>(name), familyName.c_str(), B_FONT_FAMILY_LENGTH);
		(*name)[B_FONT_FAMILY_LENGTH] = '\0';
	}

	if (flags) {
		*flags = 0;
		// if (std::all_of(font_family.begin(), font_family.end(), [](auto style) {
		// 		return style & B_IS_FIXED;
		// 	})) {
		// 	*flags |= B_IS_FIXED;
		// }
	}

	return B_OK;
}

int32 count_font_styles(font_family name)
{
	auto mgr = SkFontMgr::RefDefault();

	auto styleSet = mgr->matchFamily(name);
	auto count	  = styleSet->count();
	styleSet->unref();

	return count;
}

status_t get_font_style(font_family family, int32 index, font_style *name, uint32 *flags)
{
	auto mgr = SkFontMgr::RefDefault();

	SkFontStyleSet *styles = mgr->matchFamily(family);

	SkFontStyle style;
	SkString	styleName;
	styles->getStyle(index, &style, &styleName);
	styles->unref();

	if (styleName.isEmpty())
		return B_BAD_INDEX;

	if (name) {
		strncpy(reinterpret_cast<char *>(name), styleName.c_str(), B_FONT_STYLE_LENGTH);
		(*name)[B_FONT_STYLE_LENGTH] = '\0';
	}

	if (flags) {
		*flags = 0;

		SkTypeface *typeface = mgr->matchFamilyStyle(family, style);
		if (typeface) {
			if (typeface->isFixedPitch()) {
				*flags |= B_IS_FIXED;
			}

			typeface->unref();
		}
	}

	return B_OK;
}

status_t get_font_style(font_family family, int32 index, font_style *name, uint16 *face, uint32 *flags)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool update_font_families(bool check_only)
{
	// SkFontMgr is stand alone, so this is a no-op
	return false;
}

status_t get_font_cache_info(uint32 id, void *set)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t set_font_cache_info(uint32 id, void *set)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

TEST_SUITE("BFont")
{
	font_family ui_family{"Inter"};
	font_style	regular_style{"Regular"};
	font_style	bold_style{"Bold"};

	TEST_CASE("FamilyAndStyle")
	{
		BFont		base;
		font_family family;
		font_style	style;

		CHECK(base.SetFamilyAndStyle(ui_family, regular_style) == B_OK);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == std::string(regular_style));

		CHECK(base.SetFamilyAndStyle(ui_family, nullptr) == B_OK);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == std::string(regular_style));

		CHECK(base.SetFamilyAndStyle(nullptr, bold_style) == B_OK);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == std::string(bold_style));

		CHECK(base.SetFamilyAndStyle(nullptr, bold_style) == B_OK);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == std::string(bold_style));
	}

	TEST_CASE("Face")
	{
		BFont base;
		CHECK(base.SetFamilyAndStyle(ui_family, regular_style) == B_OK);

		font_family family;
		font_style	style;
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Regular");

		base.SetFace(B_ITALIC_FACE);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");

		base.SetFace(B_BOLD_FACE);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Bold");

		base.SetFace(B_REGULAR_FACE);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Regular");

		base.SetFace(B_ITALIC_FACE | B_BOLD_FACE);
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Bold Italic");
	}

	TEST_CASE("Assign")
	{
		BFont base;
		CHECK(base.SetFamilyAndStyle(ui_family, regular_style) == B_OK);
		base.SetSize(123);
		base.SetShear(11);
		// base.SetRotation();	 // not supported
		// base.SetSpacing();	 // not supported
		// base.SetEncoding();	 // not supported
		base.SetFace(B_ITALIC_FACE);
		base.SetFlags(B_DISABLE_ANTIALIASING);

		font_family family;
		font_style	style;
		base.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");
		CHECK(base.Size() == 123.0f);
		CHECK(base.Shear() == 11.0f);
		CHECK(base.Rotation() == 0.0f);
		CHECK(base.Spacing() == B_BITMAP_SPACING);
		CHECK(base.Encoding() == B_UNICODE_UTF8);
		CHECK(base.Face() == B_ITALIC_FACE);
		CHECK(base.Flags() == B_DISABLE_ANTIALIASING);

		BFont copy1;
		copy1 = base;
		copy1.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");
		CHECK(copy1.Size() == 123.0f);
		CHECK(copy1.Shear() == 11.0f);
		CHECK(copy1.Rotation() == 0.0f);
		CHECK(copy1.Spacing() == B_BITMAP_SPACING);
		CHECK(copy1.Encoding() == B_UNICODE_UTF8);
		CHECK(copy1.Face() == B_ITALIC_FACE);
		CHECK(copy1.Flags() == B_DISABLE_ANTIALIASING);

		BFont copy2(base);
		copy2.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");
		CHECK(copy2.Size() == 123.0f);
		CHECK(copy2.Shear() == 11.0f);
		CHECK(copy2.Rotation() == 0.0f);
		CHECK(copy2.Spacing() == B_BITMAP_SPACING);
		CHECK(copy2.Encoding() == B_UNICODE_UTF8);
		CHECK(copy2.Face() == B_ITALIC_FACE);
		CHECK(copy2.Flags() == B_DISABLE_ANTIALIASING);

		BFont copy3(&base);
		copy3.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");
		CHECK(copy3.Size() == 123.0f);
		CHECK(copy3.Shear() == 11.0f);
		CHECK(copy3.Rotation() == 0.0f);
		CHECK(copy3.Spacing() == B_BITMAP_SPACING);
		CHECK(copy3.Encoding() == B_UNICODE_UTF8);
		CHECK(copy3.Face() == B_ITALIC_FACE);
		CHECK(copy3.Flags() == B_DISABLE_ANTIALIASING);

		BFont copy4 = &base;
		copy4.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == "Italic");
		CHECK(copy4.Size() == 123.0f);
		CHECK(copy4.Shear() == 11.0f);
		CHECK(copy4.Rotation() == 0.0f);
		CHECK(copy4.Spacing() == B_BITMAP_SPACING);
		CHECK(copy4.Encoding() == B_UNICODE_UTF8);
		CHECK(copy4.Face() == B_ITALIC_FACE);
		CHECK(copy4.Flags() == B_DISABLE_ANTIALIASING);
	}

	TEST_CASE("SetFont")
	{
		BFont base;
		CHECK(base.SetFamilyAndStyle(ui_family, bold_style) == B_OK);

		BFont font;

		font.SetFamilyAndStyle(base.FamilyAndStyle());
		font_family family;
		font_style	style;
		font.GetFamilyAndStyle(&family, &style);
		CHECK(std::string(family) == std::string(ui_family));
		CHECK(std::string(style) == std::string(bold_style));

		font.SetSize(23);
		CHECK(font.Size() == 23.0f);

		font.SetShear(11);
		CHECK(font.Shear() == 11.0f);

		font.SetRotation(0);
		CHECK(font.Rotation() == 0.0f);

		font.SetSpacing(B_BITMAP_SPACING);
		CHECK(font.Spacing() == B_BITMAP_SPACING);

		font.SetEncoding(B_UNICODE_UTF8);
		CHECK(font.Encoding() == B_UNICODE_UTF8);

		font.SetFace(B_BOLD_FACE | B_REGULAR_FACE);
		CHECK((font.Face() & B_REGULAR_FACE) == B_REGULAR_FACE);
		CHECK((font.Face() & B_BOLD_FACE) == 0);

		font.SetFlags(B_FORCE_ANTIALIASING);
		CHECK(font.Flags() == B_FORCE_ANTIALIASING);
	}
}
