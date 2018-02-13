
#include "nopsys.h"
#include "console.h"
#include "fonttex.h"
#include "string.h"

/**
 * Activate this if you wan't to directly draw to the framebuffer, like
 * when the VM is unexpectedly crashing and you want to know why.
 * Most of the time using
 * Transcript show: Computer current primPullDebugString is better.
**/

console_t console;

#define CON_COLOR_RED     0x00FF0000
#define CON_COLOR_GREEN   0x0000FF00
#define CON_COLOR_BLUE    0x000000FF
#define CON_COLOR_WHITE   0x00FFFFFF
#define CON_COLOR_BLACK   0x00000000

void text_pen_initialize( text_pen_t *pen, console_t *console)
{
	pen->positionX         = 0;
	pen->positionY         = console->glyph_height;
	
	pen->char_separation_x = console->char_separation_x;
	pen->char_separation_y = console->char_separation_y;
	
	pen->glyph_width       = console->glyph_width;
	pen->glyph_height      = console->glyph_height;

	pen->line_width        = console->width;

	pen->display           = &current_computer()->video_info;
}

void text_pen_carriage_return(text_pen_t *pen)
{
	pen->positionX = 0;
	pen->positionY += pen->glyph_height;
}

int  text_pen_line_height(text_pen_t *pen)
{
	return pen->glyph_height + pen->char_separation_y;
}

void text_pen_advance_one_char(text_pen_t *pen)
{
	pen->positionX += (pen->glyph_width + pen->char_separation_x);
}

void text_pen_advance_n_chars(text_pen_t *pen, int n)
{
	pen->positionX = (pen->glyph_width + pen->char_separation_x) * n;
}

int text_pen_tab_space(text_pen_t *pen)
{
	return 4 - ((pen->positionX/pen->glyph_width) % 4);
}


void text_pen_correct_margin_if_necessary(text_pen_t *pen)
{
	if (pen->positionX + pen->glyph_width > pen->line_width)
		text_pen_carriage_return(pen);
}

void text_pen_advance_char(text_pen_t *pen, unsigned char nextChar)
{
	int tab_space;
	switch (nextChar)
	{
		case '\n':
			text_pen_carriage_return(pen);
			return;
		case '\t':
			tab_space = text_pen_tab_space(pen);
			text_pen_advance_n_chars(pen, tab_space);
			break;
		default:
			text_pen_advance_one_char(pen);
	}
	
	text_pen_correct_margin_if_necessary(pen);
}

int text_pen_collected_height(text_pen_t *pen)
{
	return pen->positionY + pen->char_separation_y;
}


void text_pen_draw_char(text_pen_t *pen, char character)
{
	if (character == '\t')
	{
		int tab_space = text_pen_tab_space(pen);
		for (int i = 0; i < tab_space; i++)
			text_pen_draw_char(pen, ' ');
		return;
	}
	else if (character < 32)
	{	
		return;
	}
	else if (character > 126)
	{
		breakpoint();
	}

	bitblt_32bit_to_fb(pen->display, rasters[character-32], pen->glyph_width, pen->glyph_height, pen->positionX, pen->positionY, CON_COLOR_GREEN);
	
	// draw separation between next character
	fill_rectangle(pen->display, pen->char_separation_x, pen->glyph_height, pen->positionX+pen->glyph_width, pen->positionY, CON_COLOR_BLACK); 
}


void putc_at_pos(char character, int x, int y)
{
	text_pen_t pen;
	text_pen_initialize(&pen, &console);

	pen.positionX = x;
	pen.positionY = y;
 
	text_pen_draw_char(&pen, character); 
}

void putc_debug(char character)
{
	int x = current_computer()->video_info.width - console.glyph_width;
	int y = current_computer()->video_info.height; // height is subtracted automatically

	putc_at_pos(character, x, y);
}

void console_initialize(console_t *console, int width, int height)
{
	console->glyph_width = FONT_GLYPH_WIDTH;
	console->glyph_height = FONT_GLYPH_HEIGHT;
	console->char_separation_x = 1;
	console->char_separation_y = 3;
	
	console->width = width;
	console->height = height;
	
	console->text[0] = 0;
	console->text_size = 0;
	
	console->debugging_now = 1;
}

void console_clear(console_t *console)
{
	console->text[0] = 0;
	console->text_size = 0;
}

void console_initialize_stdout()
{
	console_initialize(&console, 1000, 600);
}

void console_set_debugging(bool debugging)
{
	console.debugging_now = debugging;
}

void console_fill_remaining_line_with_background(console_t *console, int left, int top)
{
	display_info_t *display = &current_computer()->video_info;
	fill_rectangle(display, console->width - left, console->glyph_height, left, top, 0x00000000);
//	fill_rectangle(display, console->width, console->glyph_height - FONT_GLYPH_HEIGHT, 0 , top + console->glyph_height - FONT_GLYPH_HEIGHT, 0);

}

void console_draw_string(console_t *console, char *string)
{
	if (!console->debugging_now)
		return;
		
	text_pen_t pen;
	text_pen_initialize(&pen, console);
	
	while (*string != 0)
	{
		text_pen_draw_char(&pen, *string);
		if (*string == '\n')
			console_fill_remaining_line_with_background(console, pen.positionX, pen.positionY);

		text_pen_advance_char(&pen, *string);
		string++;
	}
}

// remember that one text line can span over many lines in the display
int console_calc_height_of_line(console_t *console, char *line_start)
{
	text_pen_t pen;
	text_pen_initialize(&pen, console);
			
	while (*line_start != 0 && *line_start != '\n')
	{
		text_pen_advance_char(&pen, *line_start);
		line_start++;
	}

	return text_pen_collected_height(&pen);
}

/*
void console_first_char_shown_with(console_t *console, char *line_start, int remainingHeight)
{
	int fitting_lines = fitting_lines_in(console, remainingHeight);
	int characters_in_line = fitting_characters_in_line(console);
	char *first_shown = line_start;
	char
	while (actualChar != '\n' && actualChar != 0 )
	{
		actualChar = next_console_line_from(actualChar);
		remainingHeight -= console->charHeight;
		if (remainingHeight < 0)
			firstShown = actualChar;
	} 
}
*/

char* first_char_of_line_ending_at(char *line_end, char *string_start)
{
	//printf("string starts at...%p ends at: %p\n", string_start, line_end);
	
	char *current = --line_end; // first char is end of string or end of line.
	
	while (current >= string_start && *current != '\n')
	{
		current--;
	}
	
	return ++current;
}

/**
 * Do it by iterating lines backwards until reaching beginning of string or it's height
 * surpases display height.
**/
char* console_calc_fitting_text(console_t *console)
{
	int text_height = 0;
	int height_of_previous_line;
	
	char *end_of_current_line   = console->text + console->text_size;
	char *start_of_current_line = console->text, *start_of_previous_line;
	
	while (end_of_current_line > console->text)
	{
		start_of_previous_line  = first_char_of_line_ending_at(end_of_current_line, console->text);
		
		//printf("previous...%p\n", start_of_previous_line);
		
		height_of_previous_line = console_calc_height_of_line(console, start_of_previous_line);
		//printf("console height...%d\n", console->height);
		
		if (text_height + height_of_previous_line > console->height)
			break;

		text_height += height_of_previous_line;

		//printf("got here\n");
		start_of_current_line = start_of_previous_line;
		end_of_current_line = start_of_current_line - 1;
	}
	
	return start_of_current_line;
	//return console_first_char_shown_with(console, start_of_current_line, displayHeight - text_height);
	
}

void console_draw(console_t *console)
{
	char *text_start = console_calc_fitting_text(console);
	//printf("calculated... %p start: %p, string:\n%s\n", text_start, console->text, text_start);
	
	console_draw_string(console, text_start);
}

void console_append_string(console_t *console, const char string[])
{
	//printf("viewing len...\n");
	int len = strlen(string);
	//printf("len is %d...\n", len);
	if (console->text_size + len > CONSOLE_TEXT_BUFFER_SIZE)
		return;
	
	//printf("start %p...\n", console->text + console->text_size);
	strcpy(console->text + console->text_size, string);
	//printf("copied...\n");
	
	console->text_size += len;
}

void console_std_put_string(const char string[])
{
	if (*string == 'W') 
		breakpoint();

	console_append_string(&console, string);
	console_draw(&console);
	
}

void console_std_put_char(char c)
{
	//breakpoint();
	char str[2] = { c, 0 };
	console_std_put_string(str);
	
}
