#ifndef FB_H
#define FB_H

/*
 * Copyright 2021 - 2023 NSG650
 * Copyright 2021 - 2023 Neptune
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fs/devtmpfs.h>
#include <stddef.h>
#include <stdint.h>

struct flanterm_context {
	/* internal use */

	size_t tab_size;
	bool autoflush;
	bool cursor_enabled;
	bool scroll_enabled;
	bool control_sequence;
	bool csi;
	bool escape;
	bool osc;
	bool osc_escape;
	bool rrr;
	bool discard_next;
	bool bold;
	bool bg_bold;
	bool reverse_video;
	bool dec_private;
	bool insert_mode;
	uint64_t code_point;
	size_t unicode_remaining;
	uint8_t g_select;
	uint8_t charsets[2];
	size_t current_charset;
	size_t escape_offset;
	size_t esc_values_i;
	size_t saved_cursor_x;
	size_t saved_cursor_y;
	size_t current_primary;
	size_t current_bg;
	size_t scroll_top_margin;
	size_t scroll_bottom_margin;
	uint32_t esc_values[16];
	uint64_t oob_output;
	bool saved_state_bold;
	bool saved_state_bg_bold;
	bool saved_state_reverse_video;
	size_t saved_state_current_charset;
	size_t saved_state_current_primary;
	size_t saved_state_current_bg;

	/* to be set by backend */

	size_t rows, cols;

	void (*raw_putchar)(struct flanterm_context *, uint8_t c);
	void (*clear)(struct flanterm_context *, bool move);
	void (*set_cursor_pos)(struct flanterm_context *, size_t x, size_t y);
	void (*get_cursor_pos)(struct flanterm_context *, size_t *x, size_t *y);
	void (*set_text_fg)(struct flanterm_context *, size_t fg);
	void (*set_text_bg)(struct flanterm_context *, size_t bg);
	void (*set_text_fg_bright)(struct flanterm_context *, size_t fg);
	void (*set_text_bg_bright)(struct flanterm_context *, size_t bg);
	void (*set_text_fg_rgb)(struct flanterm_context *, uint32_t fg);
	void (*set_text_bg_rgb)(struct flanterm_context *, uint32_t bg);
	void (*set_text_fg_default)(struct flanterm_context *);
	void (*set_text_bg_default)(struct flanterm_context *);
	void (*set_text_fg_default_bright)(struct flanterm_context *);
	void (*set_text_bg_default_bright)(struct flanterm_context *);
	void (*move_character)(struct flanterm_context *, size_t new_x,
						   size_t new_y, size_t old_x, size_t old_y);
	void (*scroll)(struct flanterm_context *);
	void (*revscroll)(struct flanterm_context *);
	void (*swap_palette)(struct flanterm_context *);
	void (*save_state)(struct flanterm_context *);
	void (*restore_state)(struct flanterm_context *);
	void (*double_buffer_flush)(struct flanterm_context *);
	void (*full_refresh)(struct flanterm_context *);
	void (*deinit)(struct flanterm_context *, void (*)(void *, size_t));

	/* to be set by client */

	void (*callback)(struct flanterm_context *, uint64_t, uint64_t, uint64_t,
					 uint64_t);
};

// taken from linux/include/uapi/linux/fb.h
struct fb_bitfield {
	uint32_t offset;	/* beginning of bitfield	*/
	uint32_t length;	/* length of bitfield		*/
	uint32_t msb_right; /* != 0 : Most significant bit is */
						/* right */
};

struct fb_var_screeninfo {
	uint32_t xres; /* visible resolution		*/
	uint32_t yres;
	uint32_t xres_virtual; /* virtual resolution		*/
	uint32_t yres_virtual;
	uint32_t xoffset; /* offset from virtual to visible */
	uint32_t yoffset; /* resolution			*/

	uint32_t bits_per_pixel; /* guess what			*/
	uint32_t grayscale;		 /* 0 = color, 1 = grayscale,	*/
	/* >1 = FOURCC			*/
	struct fb_bitfield red;	  /* bitfield in fb mem if true color, */
	struct fb_bitfield green; /* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp; /* transparency			*/

	uint32_t nonstd; /* != 0 Non standard pixel format */

	uint32_t activate; /* see FB_ACTIVATE_*		*/

	uint32_t height; /* height of picture in mm    */
	uint32_t width;	 /* width of picture in mm     */

	uint32_t accel_flags; /* (OBSOLETE) see fb_info.flags */

	/* Timing: All values in pixclocks, except pixclock (of course) */
	uint32_t pixclock;	   /* pixel clock in ps (pico seconds) */
	uint32_t left_margin;  /* time from sync to picture	*/
	uint32_t right_margin; /* time from picture to sync	*/
	uint32_t upper_margin; /* time from sync to picture	*/
	uint32_t lower_margin;
	uint32_t hsync_len;	  /* length of horizontal sync	*/
	uint32_t vsync_len;	  /* length of vertical sync	*/
	uint32_t sync;		  /* see FB_SYNC_*		*/
	uint32_t vmode;		  /* see FB_VMODE_*		*/
	uint32_t rotate;	  /* angle we rotate counter clockwise */
	uint32_t colorspace;  /* colorspace for FOURCC-based modes */
	uint32_t reserved[4]; /* Reserved for future compatibility */
};

struct fb_fix_screeninfo {
	char id[16];			  /* identification string eg "TT Builtin" */
	unsigned long smem_start; /* Start of frame buffer mem */
	/* (physical address) */
	uint32_t smem_len;		  /* Length of frame buffer mem */
	uint32_t type;			  /* see FB_TYPE_*		*/
	uint32_t type_aux;		  /* Interleave for interleaved Planes */
	uint32_t visual;		  /* see FB_VISUAL_*		*/
	uint16_t xpanstep;		  /* zero if no hardware panning  */
	uint16_t ypanstep;		  /* zero if no hardware panning  */
	uint16_t ywrapstep;		  /* zero if no hardware ywrap    */
	uint32_t line_length;	  /* length of a line in bytes    */
	unsigned long mmio_start; /* Start of Memory Mapped I/O   */
	/* (physical address) */
	uint32_t mmio_len;	   /* Length of Memory Mapped I/O  */
	uint32_t accel;		   /* Indicate to driver which	*/
						   /*  specific chip/card we have	*/
	uint16_t capabilities; /* see FB_CAP_*			*/
	uint16_t reserved[2];  /* Reserved for future compatibility */
};

struct framebuffer {
	uint32_t *address;
	uint32_t *back_address;
	size_t pitch, bpp;
	uint16_t width, height;
	size_t tex_x, tex_y;
	uint16_t tex_width, tex_height;
	uint32_t tex_color;
	uint32_t bg_color;
	struct fb_bitfield color_masks[3];
};

void framebuffer_init(struct framebuffer *fb);
void framebuffer_set_callback(void (*callback)(struct flanterm_context *,
											   uint64_t, uint64_t, uint64_t,
											   uint64_t));
void framebuffer_clear(uint32_t foreground, uint32_t background);
void framebuffer_putchar(char c);
void framebuffer_puts(char *string);
struct framebuffer *framebuffer_info(void);

void fbdev_init(void);

#endif
