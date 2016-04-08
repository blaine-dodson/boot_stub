/*
 * A Text mode VGA driver
 */

/******************************************************************************/
//                                 INCLUDES
/******************************************************************************/

#include "kernel.h"

/******************************************************************************/
//                         DEFINITIONS & CONSTANTS
/******************************************************************************/


// locations
//static const uint16_t* const VGA_BUF      = (uint16_t*) 0xB8000;
//static const uint8_t*  const VGA_ADDR_REG = (uint8_t*)  0x3D4;
//static const uint8_t*  const VGA_DATA_REG = (uint8_t*)  0x3D5;
#define VGA_BUF      ((uint16_t*) 0xB8000)
#define VGA_ADDR_REG ((uint8_t*)  0x3D4)
#define VGA_DATA_REG ((uint8_t*)  0x3D5)

// sizes and limits
//static const uint16_t  VGA_WIDTH  = 80;
//static const uint16_t  VGA_HEIGHT = 25;
//static const uint16_t* const VGA_LIM  = VGA_BUF+VGA_HEIGHT*VGA_WIDTH*sizeof(uint16_t);
#define VGA_WIDTH  ((uint) 80)
#define VGA_HEIGHT ((uint) 25)
#define VGA_SIZE   (VGA_HEIGHT*VGA_WIDTH*sizeof(uint16_t))

// Cursor Commands
//static const uint8_t VGA_CUR_HB = 14;
//static const uint8_t VGA_CUR_LB = 15;
#define VGA_CUR_HB ((uint8_t) 14)
#define VGA_CUR_LB ((uint8_t) 15)


/******************************************************************************/
//                             PRIVATE PROTOTYPES
/******************************************************************************/


static void     _scroll    (void);
static void     _set_cursor(uint pos);
//static void     _inc_cursor(void);
static uint16_t _mk_vgacell(char c);


/******************************************************************************/
//                              GLOBAL VARIABLES
/******************************************************************************/


static struct {
	uint16_t pos;	// linear position in the buffer
	uint8_t  color;
} cursor;


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


// Set the background and foreground color of characters to be written after
void set_color(enum vga_color fg, enum vga_color bg) {
	cursor.color = fg | (bg << 4);
}

// Clear the screen
void clear_vga(){
	uint16_t  cell;
	uint16_t* pos;
	
	set_color(BLACK, WHITE);
	cell = _mk_vgacell(' ');
	
	for(pos=VGA_BUF; pos < VGA_BUF+VGA_SIZE; pos++){
		*pos = cell;
	}
	
	_set_cursor(0);
}

// Print a character at the cursor position
void kputc(char c){
	uint16_t* pos;
	
	pos = VGA_BUF+cursor.pos;
	
	*pos = _mk_vgacell(c);
	_set_cursor(cursor.pos+1);
}

// Print a string on the next line after the cursor position
uint kputs(const char *buf, uint len){
	uint i;
	uint16_t* pos;
	
	if (len > VGA_WIDTH) return 0;
	
//	if (start => VGA_SIZE) {
//		_scroll();
//		start =- VGA_WIDTH;
//	}
	pos = VGA_BUF+cursor.pos;
	
	for(i=0; i<len; i++)
		pos[i] = _mk_vgacell(buf[i]);
	
	_set_cursor(cursor.pos+VGA_WIDTH);
	
	return i;
}


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


static void _set_cursor(uint pos) {
	cursor.pos=pos;
	outb(VGA_ADDR_REG, VGA_CUR_HB           );
	outb(VGA_DATA_REG, ((pos >> 8) & 0x00FF)); // & for masking
	outb(VGA_ADDR_REG, VGA_CUR_LB           );
	outb(VGA_DATA_REG, pos         & 0x00FF );
}

//static void _inc_cursor(void){
//	cursor_pos.l_pos++;
//	cursor_pos.y =cursor_pos.l_pos / VGA_WIDTH;
//	cursor_pos.x =cursor_pos.l_pos % VGA_WIDTH;
//	
//	outb(VGA_CUR_CMD, FB_HIGH_BYTE_CMD );
//	outb(FB_DATA_PORT   , ((cursor_pos.l_pos >> 8) & 0x00FF)); // & for masking
//	outb(VGA_CUR_CMD, FB_LOW_BYTE_CMD  );
//	outb(FB_DATA_PORT   , cursor_pos.l_pos & 0x00FF         );
//}

static uint16_t _mk_vgacell(char c) {
	uint16_t c16 = c;
	uint16_t color16 = cursor.color << 8;
	return color16 | c16;
}

/* Scroll the contents of the framebuffer by one line */
/*static void _scroll(){*/
/*	uint16_t* fb;*/
/*	*/
/*	for(fb = (uint16_t*)FB; fb <= (uint16_t*)((MAX_ROW-1)* MAX_COL); fb++) {*/
/*		*fb = *(fb +ROW);*/
/*	}*/
/*	*/
/*	_move_cursor((cursor_pos - ROW) / MAX_ROW);*/
/*}*/

