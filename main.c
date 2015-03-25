#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library	library;
static FT_Face		faces[10];	// 0 & 1 reserved for OCR-A/B
static const char*	names[10] = { "OCR-A", "OCR-B" };
static int			mults[10] = { 90, 90 };
static FT_GlyphSlot	slot;

// The multiplier allows globally adjusting the font size by mult-%.
int load_font(const char* path, const char* name, int mult) {
	FT_Error error;
	for (int i = 2; i < 10; i++) {
		if (!names[i]) {
			error = FT_New_Face(library, path, 0, &faces[i]);
			if (error) {
				printf("New_Face(%s,%s) Error! %d\n", path, name, error);
				return -1;
			}
			names[i] = strdup(name);
			mults[i] = mult;
			return 0;
		}
	}
	printf("load_font(%s,%s): too many fonts!\n", path, name);
	return -1;
}

int find_font(const char* name) {
	for (int i = 0; i < 10 && names[i]; i++) {
		if (strcasecmp(name, names[i]) == 0) {
			return i;
		}
	}
	// If not found, default is OCR-B
	return 1;
}

unsigned char* get_bitmap(int font, int size, int ch) {
	FT_Error	error;
	FT_Face		face;

	if (font < 0 || font >= 10 || !names[font])
		font = 1;	// Default OCR-B

	face = faces[font];
	size = (size * mults[font] * 64) / 100;

	int height = 0;		// Default the height of the glyph
	if (face == faces[1]) {
		// The A - Z characters in OCR-B are rendered a tad short, by design.
		// But it looks like hell when used in mixed alpha-numeric strings.
		// Make them the same size as the digits.
		if (ch >= 'A' && ch <= 'Z')
			height = (size * 108) / 100;
	}

	/* 1pt == 1px == 72dpi */
	error = FT_Set_Char_Size(face, size, height, 72, 0 );
	if (error) {
		printf("Set_Char_Size Error! %d\n", error);
		return 0;
	}
	slot = face->glyph;

	if (face == faces[0] || face == faces[1]) {
		// The OCR fonts we use have some quirks in their glyph maps.
		// ^ is mapped as U+02C6 circumflex
		// ~ is mapped as U+02DC tilde
		if (ch == '~')
			ch = 0x02dc;
		else if (ch == '^')
			ch = 0x02c6;
	}

	error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
	if (error) {
		printf("Load_Char Error! %d\n", error);
		return 0;
	}

	return slot->bitmap.buffer;
}

unsigned get_left() {
	return slot->bitmap_left;
}
unsigned get_top() {
	return slot->bitmap_top;
}
unsigned get_width() {
	return slot->bitmap.width;
}
unsigned get_height() {
	return slot->bitmap.rows;
}
unsigned get_pitch() {
	return slot->bitmap.pitch;
}
unsigned get_advance() {
	return slot->advance.x >> 6;
}

int main() {
	FT_Error	  error;

	error = FT_Init_FreeType(&library);
	if (error) {
		printf("Init Error! %d\n", error);
		return 1;
	}
	error = FT_New_Face(library, "OCRA.otf", 0, &faces[0]);
	if (error) {
		printf("New_Face(OCR-A) Error! %d\n", error);
		return 1;
	}
	error = FT_New_Face(library, "OCRB.otf", 0, &faces[1]);
	if (error) {
		printf("New_Face(OCR-B) Error! %d\n", error);
		return 1;
	}
	return 0;
}

