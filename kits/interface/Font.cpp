#include "Font.h"

#define LOG_TAG "BFont"

#include <Rect.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkTextBlob.h>
#include <include/core/SkTypeface.h>
#include <log/log.h>
#include <src/core/SkTypefaceCache.h>

#include <algorithm>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>

class BFont::impl
{
   public:
	SkFont font;

	float  shear;
	uint32 flags;

	impl() : font(nullptr, 10.0f), shear{90.0f}, flags{0} {}
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
	font_family current_family;
	font_style	current_style;
	GetFamilyAndStyle(&current_family, &current_style);

	auto mgr = SkFontMgr::RefDefault();

	const char	   *familyName = family ? family : current_family;
	SkFontStyleSet *styleSet   = mgr->matchFamily(familyName);
	for (auto i = 0; i < styleSet->count(); ++i) {
		SkFontStyle fontStyle;
		SkString	styleName;
		styleSet->getStyle(i, &fontStyle, &styleName);
		if (styleName.equals(style ? style : current_style)) {
			SkTypeface *typeface = mgr->matchFamilyStyle(familyName, fontStyle);
			m->font.setTypeface(sk_sp<SkTypeface>(typeface));
			return B_OK;
		}
	}
	styleSet->unref();

	return B_BAD_VALUE;
}

static bool _cmp_face_uniqueId(SkTypeface *face, void *ctx)
{
	return face->uniqueID() == *static_cast<uint32 *>(ctx);
}

void BFont::SetFamilyAndStyle(uint32 code)
{
	m->font.setTypeface(SkTypefaceCache::FindByProcAndRef(_cmp_face_uniqueId, &code));
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
		if (face & B_ITALIC_FACE) {
			// TODO: find SkTypeface::kBold in current family
			// and replace typeface
			debugger(__PRETTY_FUNCTION__);
		}
		if (face & B_ITALIC_FACE && face & B_BOLD_FACE) {
			// TODO: find SkTypeface::kBoldItalic in current family
			// and replace typeface
			debugger(__PRETTY_FUNCTION__);
		}
		if (face & B_BOLD_FACE) {
			// TODO: find SkTypeface::kItalic in current family
			// and replace typeface
			debugger(__PRETTY_FUNCTION__);
		}
		if (face & B_REGULAR_FACE) {
			// TODO: find SkTypeface::kNormal style in current family
			// and replace typeface
			debugger(__PRETTY_FUNCTION__);
		}
	}
}

void BFont::SetFlags(uint32 flags)
{
	m->flags = flags;
	m->font.setEdging((flags & B_DISABLE_ANTIALIASING) ? SkFont::Edging::kAlias : SkFont::Edging::kAntiAlias);
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

			auto			mgr		 = SkFontMgr::RefDefault();
			SkFontStyleSet *styleSet = mgr->matchFamily(familyName.c_str());
			for (auto i = 0; i < styleSet->count(); ++i) {
				SkFontStyle setFontStyle;
				SkString	setStyleName;
				styleSet->getStyle(i, &setFontStyle, &setStyleName);
				if (setFontStyle == fontStyle) {
					strncpy(reinterpret_cast<char *>(style), setStyleName.c_str(), B_FONT_STYLE_LENGTH);
					(*style)[B_FONT_STYLE_LENGTH] = '\0';
					break;
				}
			}
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

void BFont::GetHeight(font_height *height) const
{
	if (!height) return;

	SkFontMetrics metrics;
	m->font.getMetrics(&metrics);
	*height = font_height{-metrics.fAscent, metrics.fDescent,
						  metrics.fLeading + metrics.fDescent + (-metrics.fAscent),
						  metrics.fXHeight, metrics.fCapHeight};
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

	os << std::format("BFont('{}', '{}' ({}) {:#x}/{:#b} {:.1f}deg/{:.1f}deg {:.1f}pt ({:.1f} {:.1f} {:.1f}/{:.1f} {:.1f}) {:#x})",
					  reinterpret_cast<const char *>(family), reinterpret_cast<const char *>(style), value.FamilyAndStyle(),
					  value.Face(), value.Flags(), value.Shear(), value.Rotation(), value.Size(),
					  height.ascent, height.descent, height.leading, height.x_height, height.cap_height, value.Encoding());
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

	SkFontStyleSet *styles = mgr->matchFamily(name);
	auto			count  = styles->count();

	styles->unref();
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
