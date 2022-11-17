#include "Font.h"

#include <Rect.h>
#include <fontconfig/fontconfig.h>

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

BFont::BFont() {}

BFont::BFont(const BFont &font) {}

BFont::BFont(const BFont *font) {}

status_t BFont::SetFamilyAndStyle(const font_family family, const font_style style)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BFont::SetFamilyAndStyle(uint32 code)
{
	debugger(__PRETTY_FUNCTION__);
}

status_t BFont::SetFamilyAndFace(const font_family family, uint16 face)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BFont::SetSize(float size)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetShear(float shear)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetRotation(float rotation)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetSpacing(uint8 spacing)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetEncoding(uint8 encoding)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetFace(uint16 face)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::SetFlags(uint32 flags)
{
	debugger(__PRETTY_FUNCTION__);
}

void BFont::GetFamilyAndStyle(font_family *family, font_style *style) const
{
	debugger(__PRETTY_FUNCTION__);
}

uint32 BFont::FamilyAndStyle() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
}

float BFont::Size() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

float BFont::Shear() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

float BFont::Rotation() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

uint8 BFont::Spacing() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
}

uint8 BFont::Encoding() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
}

uint16 BFont::Face() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
}

uint32 BFont::Flags() const
{
	debugger(__PRETTY_FUNCTION__);
	return 0;
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
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

float BFont::StringWidth(const char *string, int32 length) const
{
	debugger(__PRETTY_FUNCTION__);
	return 0.0;
}

void BFont::GetHeight(font_height *height) const
{
	debugger(__PRETTY_FUNCTION__);
}

BFont &BFont::operator=(const BFont &font)
{
	debugger(__PRETTY_FUNCTION__);
	return *this;
}

bool BFont::operator==(const BFont &font) const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

bool BFont::operator!=(const BFont &font) const
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}

void BFont::PrintToStream() const
{
	std::cout << *this << std::endl;
}

#pragma mark globals

std::ostream &operator<<(std::ostream &os, const BFont &value)
{
	// font_family family;
	// font_style	style;
	// GetFamilyAndStyle(&family, &style);

	// printf("BFont { %s (%d), %s (%d) 0x%x %f/%f %fpt (%f %f %f), %d }\n",
	// 	   family, fFamilyID, style, fStyleID, fFace, fShear, fRotation, fSize,
	// 	   fHeight.ascent, fHeight.descent, fHeight.leading, fEncoding);

	os << "BFont { ";
	// family
	// style
	// size (in points)
	// shear (in degrees)
	// rotation (in degrees)
	// ascent
	// descent
	// leading
	os << " }";
	return os;
}

static BFont sPlainFont;
static BFont sBoldFont;
static BFont sFixedFont;

const BFont *be_plain_font = &sPlainFont;
const BFont *be_bold_font  = &sBoldFont;
const BFont *be_fixed_font = &sFixedFont;

namespace {
static std::unordered_map<std::string, std::unordered_set<std::string>> sInstalledFonts;

static void _update_installed_fonts()
{
	if (sInstalledFonts.empty()) {
		FcPattern	  *pat = FcPatternCreate();
		FcObjectSet *os	 = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, nullptr);
		FcFontSet	  *fs	 = FcFontList(nullptr, pat, os);
		FcFontSetPrint(fs);

		if (fs) {
			for (int i = 0; i < fs->nfont; ++i) {
				FcPattern *font = fs->fonts[i];
				FcChar8	*file, *style, *family;  // FIXME: is file needed?
				if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch
					&& FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch
					&& FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch) {
					// dprintf(2, "Filename: %s (family %s, style %s)\n", file, family, style);
					auto s_family = std::string(reinterpret_cast<const char *>(family),
												strnlen(reinterpret_cast<const char *>(family), B_FONT_FAMILY_LENGTH));
					auto s_style  = std::string(reinterpret_cast<const char *>(style),
												strnlen(reinterpret_cast<const char *>(style), B_FONT_STYLE_LENGTH));
					sInstalledFonts[s_family].insert(s_style);
				}
			}

			FcFontSetDestroy(fs);
		}

		if (os) FcObjectSetDestroy(os);
		if (pat) FcPatternDestroy(pat);
	}
}
}  // namespace

int32 count_font_families()
{
	_update_installed_fonts();

	return sInstalledFonts.size();
}

status_t get_font_family(int32 index, font_family *name, uint32 *flags)
{
	_update_installed_fonts();

	for (auto &font : sInstalledFonts) {
		if (index == 0) {
			strncpy(name[0], font.first.c_str(), B_FONT_FAMILY_LENGTH);
			*(name[B_FONT_FAMILY_LENGTH]) = '\0';
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
		for (auto &style : sInstalledFonts.at(s_family)) {
			if (index == 0) {
				strncpy(name[0], style.c_str(), B_FONT_STYLE_LENGTH);
				*(name[B_FONT_STYLE_LENGTH]) = '\0';
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
