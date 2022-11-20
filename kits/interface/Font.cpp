#include "Font.h"

#include <Rect.h>
#include <fontconfig/fontconfig.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkTextBlob.h>
#include <log/log.h>

#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <unordered_map>

class BFont::impl
{
   public:
	sk_sp<SkTypeface> typeface;
};

BFont::BFont() : BFont(be_plain_font) {}

BFont::BFont(const BFont &font) : BFont(&font) {}

BFont::BFont(const BFont *font)
	: fFamilyID{0},
	  fStyleID{0},
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
	fFamilyID = font.fFamilyID;
	fStyleID  = font.fStyleID;
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
	return fFamilyID == font.fFamilyID
		   && fStyleID == font.fStyleID
		   && fSize == font.fSize
		   && fShear == font.fShear
		   && fRotation == font.fRotation
		   && fSpacing == font.fSpacing
		   && fEncoding == font.fEncoding
		   && fFace == font.fFace;
}

bool BFont::operator!=(const BFont &font) const
{
	return fFamilyID != font.fFamilyID
		   || fStyleID != font.fStyleID
		   || fSize != font.fSize
		   || fShear != font.fShear
		   || fRotation != font.fRotation
		   || fSpacing != font.fSpacing
		   || fEncoding != font.fEncoding
		   || fFace != font.fFace;
}

namespace {
inline uint16 hash16(uint64 val)
{
	uint16 hash = (uint16)(val & 0xFFFF);
	hash ^= (uint16)((val >> 16) & 0xFFFF);
	hash ^= (uint16)((val >> 32) & 0xFFFF);
	hash ^= (uint16)((val >> 48) & 0xFFFF);

	return hash;
}

static std::unordered_map<std::string,										// family
						  std::unordered_map<std::string,					// style
											 std::pair<std::string, int>>>	// file_path, file_index
	sInstalledFonts;

static void _update_installed_fonts()
{
	if (sInstalledFonts.empty()) {
		FcPattern	  *pat = FcPatternCreate();
		FcObjectSet	*os	 = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, FC_INDEX, nullptr);
		FcFontSet	  *fs	 = FcFontList(nullptr, pat, os);
		FcFontSetPrint(fs);

		if (fs) {
			for (int i = 0; i < fs->nfont; ++i) {
				FcPattern *font = fs->fonts[i];
				FcChar8	*file, *family, *style;
				int		   index;
				if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch
					&& FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch
					&& FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch
					&& FcPatternGetInteger(font, FC_INDEX, 0, &index) == FcResultMatch) {
					auto s_family					   = std::string(reinterpret_cast<char *>(family),
																	 strnlen(reinterpret_cast<char *>(family), B_FONT_FAMILY_LENGTH));
					auto s_style					   = std::string(reinterpret_cast<char *>(style),
																	 strnlen(reinterpret_cast<char *>(style), B_FONT_STYLE_LENGTH));
					sInstalledFonts[s_family][s_style] = std::pair(std::string(reinterpret_cast<char *>(file)), index);
				}
			}

			FcFontSetDestroy(fs);
		}

		if (os) FcObjectSetDestroy(os);
		if (pat) FcPatternDestroy(pat);
	}
}
}  // namespace

status_t BFont::SetFamilyAndStyle(const font_family family, const font_style style)
{
	_update_installed_fonts();

	font_family current_family;
	font_style	current_style;
	GetFamilyAndStyle(&current_family, &current_style);

	auto s_family = std::string(reinterpret_cast<const char *>(family ? family : current_family));
	auto s_style  = std::string(reinterpret_cast<const char *>(style ? style : current_style));

	if (sInstalledFonts.contains(s_family) && sInstalledFonts[s_family].contains(s_style)) {
		const std::hash<std::string> hash;
		fFamilyID = hash16(hash(s_family));
		fStyleID  = hash16(hash(s_style));

		return B_OK;
	}

	return B_BAD_VALUE;
}

void BFont::SetFamilyAndStyle(uint32 code)
{
	fStyleID  = code & 0xFFFF;
	fFamilyID = (code >> 16) & 0xFFFF;
}

status_t BFont::SetFamilyAndFace(const font_family family, uint16 face)
{
	_update_installed_fonts();

	status_t ret = SetFamilyAndStyle(family, nullptr);
	if (ret != B_OK) return ret;
	SetFace(face);
	return B_OK;
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

	_update_installed_fonts();
	_get_font_info(family, style, nullptr, nullptr);
}

uint32 BFont::FamilyAndStyle() const
{
	return (fFamilyID << 16) & fStyleID;
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
	*height = font_height{-metrics.fAscent, metrics.fDescent, metrics.fLeading + metrics.fDescent + (-metrics.fAscent)};
}

void BFont::PrintToStream() const
{
	std::cout << *this << std::endl;
}

void BFont::_get_font_info(font_family *family, font_style *style, const char **filename, int *index) const
{
	const std::hash<std::string> hash;

	for (const auto &font_family : sInstalledFonts) {
		const uint16 familyId = hash16(hash(font_family.first));
		if (familyId == fFamilyID) {
			if (family) {
				strncpy(reinterpret_cast<char *>(family), font_family.first.c_str(), B_FONT_FAMILY_LENGTH);
				(*family)[B_FONT_FAMILY_LENGTH] = '\0';
			}

			for (const auto &font_style : font_family.second) {
				const uint16 styleId = hash16(hash(font_style.first));
				if (styleId == fStyleID) {
					if (style) {
						strncpy(reinterpret_cast<char *>(style), font_style.first.c_str(), B_FONT_STYLE_LENGTH);
						(*style)[B_FONT_STYLE_LENGTH] = '\0';
					}
					if (filename) {
						*filename = font_style.second.first.c_str();
					}
					if (index) {
						*index = font_style.second.second;
					}

					return;
				}
			}

			return;
		}
	}
}

void BFont::_get_font(SkFont *font) const
{
	LOG_FATAL_IF(!font, "_getFont called with nullptr");

	if (!m->typeface) {
		const char *filename;
		int			index;
		_update_installed_fonts();
		_get_font_info(nullptr, nullptr, &filename, &index);

		m->typeface = SkTypeface::MakeFromFile(filename, index);
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
	_update_installed_fonts();

	font_family family;
	font_style	style;
	value.GetFamilyAndStyle(&family, &style);
	font_height height;
	value.GetHeight(&height);

	os << std::format("BFont('{}' ({}), '{}' ({}) {:#x} {:.1f}deg/{:.1f}deg {:.1f}pt ({:.1f} {:.1f} {:.1f}) {:#x})",
					  reinterpret_cast<const char *>(family), value.fFamilyID, reinterpret_cast<const char *>(style), value.fStyleID,
					  value.fFace, value.fShear, value.fRotation, value.fSize,
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
	_update_installed_fonts();

	return sInstalledFonts.size();
}

status_t get_font_family(int32 index, font_family *name, uint32 *flags)
{
	_update_installed_fonts();

	for (auto &font_family : sInstalledFonts) {
		if (index == 0) {
			strncpy(reinterpret_cast<char *>(name), font_family.first.c_str(), B_FONT_FAMILY_LENGTH);
			(*name)[B_FONT_FAMILY_LENGTH] = '\0';
			*flags						  = 0;	// FIXME: compute real flags

			return B_OK;
		}
		index -= 1;
	}

	return B_BAD_INDEX;
}

int32 count_font_styles(font_family name)
{
	_update_installed_fonts();

	auto s_family = std::string(reinterpret_cast<const char *>(name),
								strnlen(reinterpret_cast<const char *>(name), B_FONT_FAMILY_LENGTH));
	if (sInstalledFonts.contains(s_family)) {
		return sInstalledFonts.at(s_family).size();
	}

	return B_BAD_VALUE;
}

status_t get_font_style(font_family family, int32 index, font_style *name, uint32 *flags)
{
	_update_installed_fonts();

	auto s_family = std::string(reinterpret_cast<const char *>(family),
								strnlen(reinterpret_cast<const char *>(family), B_FONT_FAMILY_LENGTH));
	if (sInstalledFonts.contains(s_family)) {
		for (auto &font_style : sInstalledFonts.at(s_family)) {
			if (index == 0) {
				strncpy(reinterpret_cast<char *>(name), font_style.first.c_str(), B_FONT_STYLE_LENGTH);
				(*name)[B_FONT_STYLE_LENGTH] = '\0';
				*flags						 = 0;  // FIXME: compute real flags

				return B_OK;
			}
			index -= 1;
		}

		return B_BAD_INDEX;
	}

	return B_BAD_VALUE;
}

status_t get_font_style(font_family family, int32 index, font_style *name, uint16 *face, uint32 *flags)
{
	_update_installed_fonts();

	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool update_font_families(bool check_only)
{
	const auto size = sInstalledFonts.size();
	sInstalledFonts.clear();
	FcInitBringUptoDate();
	_update_installed_fonts();
	return sInstalledFonts.size() != size;
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
