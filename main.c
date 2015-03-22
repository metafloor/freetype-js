#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library	library;
static FT_Face		faceA;		// OCR-A
static FT_Face		faceB;		// OCR-B
static FT_GlyphSlot	slot;

// Which is 0 == faceA, faceB otherwise
unsigned char* get_bitmap(int which, int size, int ch) {
	FT_Error	error;
	FT_Face		face = which == 0 ? faceA : faceB;

	/* 1pt == 1px == 72dpi */
	error = FT_Set_Char_Size(face, size * 64, 0, 72, 0 );
	if (error) {
		printf("Set_Char_Size Error! %d\n", error);
		return 0;
	}
	slot = face->glyph;

	// The OCR fonts we use have some quirks in their glyph maps.
	// ^ is mapped as U+02C6 circumflex
	// ~ is mapped as U+02DC tilde
	if (ch == '~')
		ch = 0x02dc;
	else if (ch == '^')
		ch = 0x02c6;

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
	FT_Error      error;

	error = FT_Init_FreeType(&library);
	if (error) {
		printf("Init Error! %d\n", error);
		return 1;
	}

	error = FT_New_Face(library, "OCRA.otf", 0, &faceA);
	if (error) {
		printf("New_Face(OCRA) Error! %d\n", error);
		return 1;
	}
	error = FT_New_Face(library, "OCRB.otf", 0, &faceB);
	if (error) {
		printf("New_Face(OCRB) Error! %d\n", error);
		return 1;
	}

	return 0;
}

