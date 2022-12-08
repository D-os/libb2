#include "Font.h"

#define LOG_TAG "BFont"

#include <Rect.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkTextBlob.h>
#include <include/core/SkTypeface.h>
#include <log/log.h>

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
	sk_sp<SkTypeface> typeface;
};

static void _get_font_info(uint32 id, font_family *family, font_style *style, sk_sp<SkTypeface> *face, uint32 *flags)
{
	auto mgr = SkFontMgr::RefDefault();

	for (auto i = 0; i < mgr->countFamilies(); ++i) {
		SkString familyName;
		mgr->getFamilyName(i, &familyName);
		SkFontStyleSet *styleSet = mgr->matchFamily(familyName.c_str());
		for (auto j = 0; j < styleSet->count(); ++j) {
			SkFontStyle fontStyle;
			SkString	styleName;
			styleSet->getStyle(j, &fontStyle, &styleName);
			SkTypeface *typeface = mgr->matchFamilyStyle(familyName.c_str(), fontStyle);
			if (SkTypeface::UniqueID(typeface) == id) {
				if (family) {
					strncpy(reinterpret_cast<char *>(family), familyName.c_str(), B_FONT_FAMILY_LENGTH);
					(*family)[B_FONT_FAMILY_LENGTH] = '\0';
				}
				if (style) {
					strncpy(reinterpret_cast<char *>(style), styleName.c_str(), B_FONT_STYLE_LENGTH);
					(*style)[B_FONT_STYLE_LENGTH] = '\0';
				}
				if (face) {
					face->reset(typeface);
					typeface = nullptr;
				}
				if (flags) {
					*flags = 0;
					if (typeface && typeface->isFixedPitch()) {
						*flags |= B_IS_FIXED;
					}
				}
			}
			if (typeface) {
				typeface->unref();
			}
		}
		styleSet->unref();
	}
}

BFont::BFont() : BFont(be_plain_font) {}

BFont::BFont(const BFont &font) : BFont(&font) {}

BFont::BFont(const BFont *font)
	: fID{0},
	  fSize{10.0},
	  fShear{90.0},
	  fRotation{0.0},
	  fSpacing{B_BITMAP_SPACING},
	  fEncoding{B_UNICODE_UTF8},
	  fFace{0},
	  fFlags{0},
	  m{new BFont::impl()}
{
	if (font)
		*this = *font;
	else
		*this = *be_plain_font;
}

BFont::~BFont()
{
	delete m;
}

BFont &BFont::operator=(const BFont &font)
{
	fID		  = font.fID;
	fSize	  = font.fSize;
	fShear	  = font.fShear;
	fRotation = font.fRotation;
	fSpacing  = font.fSpacing;
	fEncoding = font.fEncoding;
	fFace	  = font.fFace;
	fFlags	  = font.fFlags;
	// fExtraFlags = font.fExtraFlags;

	return *this;
}

bool BFont::operator==(const BFont &font) const
{
	return fID == font.fID
		   && fSize == font.fSize
		   && fShear == font.fShear
		   && fRotation == font.fRotation
		   && fSpacing == font.fSpacing
		   && fEncoding == font.fEncoding
		   && fFace == font.fFace;
}

bool BFont::operator!=(const BFont &font) const
{
	return fID != font.fID
		   || fSize != font.fSize
		   || fShear != font.fShear
		   || fRotation != font.fRotation
		   || fSpacing != font.fSpacing
		   || fEncoding != font.fEncoding
		   || fFace != font.fFace;
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
			SetFamilyAndStyle(SkTypeface::UniqueID(typeface));
			if (typeface) typeface->unref();
			return B_OK;
		}
	}
	styleSet->unref();

	return B_BAD_VALUE;
}

void BFont::SetFamilyAndStyle(uint32 code)
{
	fID = code;
}

void BFont::SetSize(float size)
{
	fSize = size;
}

void BFont::SetShear(float shear)
{
	fShear = shear;
}

void BFont::SetRotation(float rotation)
{
	fRotation = rotation;
}

void BFont::SetSpacing(uint8 spacing)
{
	fSpacing = spacing;
}

void BFont::SetEncoding(uint8 encoding)
{
	fEncoding = encoding;
}

void BFont::SetFace(uint16 face)
{
	fFace = face;
}

void BFont::SetFlags(uint32 flags)
{
	fFlags = flags;
}

void BFont::GetFamilyAndStyle(font_family *family, font_style *style) const
{
	if (!family || !style) return;

	*family[0] = '\0';
	*style[0]  = '\0';
	_get_font_info(FamilyAndStyle(), family, style, nullptr, nullptr);
}

uint32 BFont::FamilyAndStyle() const
{
	return fID;
}

float BFont::Size() const
{
	return fSize;
}

float BFont::Shear() const
{
	return fShear;
}

float BFont::Rotation() const
{
	return fRotation;
}

uint8 BFont::Spacing() const
{
	return fSpacing;
}

uint8 BFont::Encoding() const
{
	return fEncoding;
}

uint16 BFont::Face() const
{
	return fFace;
}

uint32 BFont::Flags() const
{
	return fFlags;
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

	SkFont font;
	_get_font(&font);
	return font.measureText(string, length, SkTextEncoding::kUTF8);
}

void BFont::GetHeight(font_height *height) const
{
	if (!height) return;
	SkFont font;
	_get_font(&font);

	SkFontMetrics metrics;
	font.getMetrics(&metrics);
	*height = font_height{-metrics.fAscent, metrics.fDescent,
						  metrics.fLeading + metrics.fDescent + (-metrics.fAscent),
						  metrics.fXHeight, metrics.fCapHeight};
}

void BFont::PrintToStream() const
{
	std::cout << *this << std::endl;
}

void BFont::_get_font(SkFont *font) const
{
	LOG_FATAL_IF(!font, "_getFont called with nullptr");

	if (!m->typeface) {
		_get_font_info(FamilyAndStyle(), nullptr, nullptr, &m->typeface, nullptr);
	}

	font->setTypeface(m->typeface);
	font->setSize(Size());
	font->setBaselineSnap(true);
	font->setEdging((fFlags & B_DISABLE_ANTIALIASING) ? SkFont::Edging::kAlias : SkFont::Edging::kAntiAlias);
	// SkewX is computed as x' = x + xSkew · y (positive value skews to the left)
	// BFont Shear is 45.0° (slanted to the right) through 135.0°, with 90.0° default.
	// We round to 2 decimal places to have straight up as 0.00, not 0.000000000000something.
	font->setSkewX(std::round(((Shear() - 90.0f) * 100.0f) / 45.0f) / 100.0f);
}

#pragma mark globals

std::ostream &operator<<(std::ostream &os, const BFont &value)
{
	font_family family;
	font_style	style;
	value.GetFamilyAndStyle(&family, &style);
	font_height height;
	value.GetHeight(&height);

	os << std::format("BFont('{}', '{}' ({}) {:#x}/{:#b} {:.1f}deg/{:.1f}deg {:.1f}pt ({:.1f} {:.1f} {:.1f}) {:#x})",
					  reinterpret_cast<const char *>(family), reinterpret_cast<const char *>(style), value.fID,
					  value.fFace, value.fFlags, value.fShear, value.fRotation, value.fSize,
					  height.ascent, height.descent, height.leading, value.fEncoding);
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
