#include <stdbool.h>

// From task 0 copy all functions
#include <stdint.h>
#include <stdio.h>
// Start of task 0
	
// FUNCTION PROTOTYPES:
// Reads a byte from a specific memory address
char read_byte(unsigned int address);

// Writes a byte to a specific memory address
void write_byte(unsigned int address, char value);

// Reads a halfword (2 bytes) from a specific memory address
short read_halfword(unsigned int address);

// Writes a halfword (2 bytes) to a specific memory address
void write_halfword(unsigned int address, short value);

// Reads a word (4 bytes) from a specific memory address
int read_word(unsigned int address);

// Writes a word (4 bytes) to a specific memory address
void write_word(unsigned int address, int value);

// FUNCTION IMPLEMENTATIONS:

// Reads a byte from a specific memory address
char read_byte(unsigned int address) {
	char result;
	__asm__ __volatile__(
		"ldrb %0, [%1]" // load byte from memory addr %1 into output operand %0
		: "=r"(result) // output
		: "r" (address) // input
		);
	return result; // return output
	}
		
// Writes a byte to a specific memory address
void write_byte(unsigned int address, char value) {
	__asm__ __volatile__ (
		"strb %0, [%1]" // store the input "value" into the input "address"
		: // signifies no outputs
		: "r"(value), "r"(address) // inputs
		);
	}

// Reads a halfword (2 bytes) from a specific memory address
short read_halfword(unsigned int address) {
	short result;
	__asm__ __volatile__(
		"ldrh %0, [%1]" // load halfword from addr %1 into output operand %0
		: "=r"(result) // output
		: "r" (address) // input
		);
	return result; // return output
	}

// Writes a halfword (2 bytes) to a specific memory address
void write_halfword(unsigned int address, short value) {
	__asm__ __volatile__ (
		"strh %0, [%1]" // store the input "value" into the input "address"
		: // signifies no outputs
		: "r"(value), "r"(address) // inputs
		);
	}

// Reads a word (4 bytes) from a specific memory address
int read_word(unsigned int address) {
	int result;
	__asm__ __volatile__(
		"ldr %0, [%1]" // load word from memory addr %1 into output operand %0
		: "=r"(result) // output (= tells us it's output)
		: "r" (address) // input
		);
	return result; // return output
	}

// Writes a word (4 bytes) to a specific memory address
void write_word(unsigned int address, int value) {
	__asm__ __volatile__ (
		"str %0, [%1]" // store the input "value" into the input "address"
		: // signifies no outputs
		: "r"(value), "r"(address) // inputs
		);
	}
// End of task 0
// FUNCTION PROTOTYPES:
void VGA_clear_pixelbuff(); 
void VGA_write_char(int x, int y, char c); 
void VGA_clear_charbuff();
void VGA_draw_point(int x, int y, short color);
int read_PS2_data(char *data);

// FUNCTION IMPLEMENTATIONS:
void VGA_write_char(int x, int y, char c) {
	if (x>=0 && x<=80 && y>=0 && y<= 60) {
	int char_address = 0xc9000000 | (y << 7) | x;
	write_byte(char_address,c); 
	}
}
// draws a point on the screen with the color as indicated in third argument
void VGA_draw_point(int x, int y, short c) {
	if (x>=0 && x<=320){
		int pixel_address = 0xc8000000 | (y << 10) | (x << 1);
		write_halfword(pixel_address, c);
	}
}
void VGA_clear_pixelbuff() {
	for (int x=0; x<=320; x++) {
		for (int y=0; y<=240; y++) {
			VGA_draw_point(x,y,0);
		}
	}
}

 void VGA_clear_charbuff() {
	for (int x=0; x<=80; x++) {
		for (int y=0; y<=60; y++) {
			VGA_write_char(x,y,0);
		}
	}
}

int read_PS2_data(char *data) {
	int RVALID = ((*(volatile int *)0xff200100) >> 15) & 0x1;
	if (RVALID) {
		*data = (*(volatile int *)0xff200100) & 0xff;
		return 1;
	}
	return 0;
}

void write_hex_digit(unsigned int x,unsigned int y, char c) {
    if (c > 9) {
        c += 55;
    } else {
        c += 48;
    }
    c &= 255;
    VGA_write_char(x,y,c);
}
void write_byte_kbrd(unsigned int x,unsigned int y, unsigned int c) {
   char lower=c>>4 &0x0F;
   write_hex_digit(x,y,lower);
   char upper=c&0x0F;
   write_hex_digit(x+1,y,upper);
   return;
}

void input_loop_fun() {
    unsigned int x = 0;
    unsigned int y = 0;
	VGA_clear_pixelbuff();
    VGA_clear_charbuff();

    while (y<=59) {
    
			char data;
            char r2 = read_PS2_data(&data);

            if (r2 != 0) {  // Check if data is available

				write_byte_kbrd(x,y,data); 
                x += 3;
                if (x > 79) {
                    y++;
                    x = 0;
                }

                if (y > 59) {  // Check if loop should exit
                    return;  // End of input loop
                }
            }
    }
}

// ----------------------- //
// Psuedo Number Generator //
// ----------------------- //

unsigned int seed = 12345;  // You can set this to any starting value

// Function to generate a pseudo-random number
unsigned int pseudo_random() {
    // LCG parameters (from Numerical Recipes)
    seed = (1103515245 * seed + 12345) & 0x7fffffff;
    return seed;
}

// Function to get a pseudo-random number within a specific range [min, max]
unsigned int random_in_range(int min, int max) {
    return (pseudo_random() % (max - min + 1)) + min;
}

// constants & variables

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define ROWS 5
#define COLS 5
#define CELL_WIDTH (SCREEN_WIDTH / COLS)
#define CELL_HEIGHT (SCREEN_HEIGHT / ROWS)
#define PLAYER_Y (SCREEN_HEIGHT - CELL_HEIGHT)
#define MAX_OBJECTS 3
#define TIMER_LOAD_VALUE 25000000   
#define MPCORE_PRIV_TIMER 0xFFFEC600

int player_position;   
int score;
bool game_over_flag;
int object_positions[MAX_OBJECTS][2];  
int object_speeds[MAX_OBJECTS];     
bool object_active[MAX_OBJECTS];   

// task 3 game functions below:

void VGA_fill(short color) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            VGA_draw_point(x, y, color);
        }
    }
}

void draw_character(int col, short color) {
    int x_start = col * CELL_WIDTH;
    for (int x = x_start; x < x_start + CELL_WIDTH; x++) {
        for (int y = PLAYER_Y; y < PLAYER_Y + CELL_HEIGHT; y++) {
            VGA_draw_point(x, y, color);
        }
    }
}

void draw_object(int x, int y, short color) {
    for (int i = 0; i < CELL_WIDTH; i++) {
        for (int j = 0; j < CELL_HEIGHT; j++) {
            VGA_draw_point(x + i, y + j, color);
        }
    }
}

void erase_object(int x, int y) {
    draw_object(x, y, 0); // Black color for erasing
}

void init_game() {
	VGA_clear_pixelbuff();
	VGA_clear_charbuff();
    player_position = COLS / 2;
    score = 0;
    game_over_flag = false;

    for (int i = 0; i < MAX_OBJECTS; i++) {
        object_active[i] = false;
    }
    VGA_fill(0); 
    draw_character(player_position, 0xFFFF); 
}

void spawn_object() {
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!object_active[i]) {
            int column = random_in_range(0, COLS - 1);
            int speed = random_in_range(3, 7);
            object_positions[i][0] = column;
            object_positions[i][1] = 0;
            object_speeds[i] = speed;
            object_active[i] = true;
            draw_object(column * CELL_WIDTH, 0, 0xFFC0CB); 
            break;
        }
    }
}

// Updates positions of falling objects
void update_objects() {
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (object_active[i]) {
            int x = object_positions[i][0] * CELL_WIDTH;
            int y = object_positions[i][1];
            erase_object(x, y);

            y += object_speeds[i]; // Update position by speed in pixels

            // if it hits the bottom, game over
            if (y + CELL_HEIGHT >= SCREEN_HEIGHT) {
                game_over_flag = true;
            } else if (y + CELL_HEIGHT >= PLAYER_Y && x == player_position * CELL_WIDTH) {
                // if it hits the player, increase score
                if (object_positions[i][0] == player_position) {
                    score++;
                    printf("Score: %d\n", score); // print updated score
                    object_active[i] = false;
                }
            } else {
                if (y < SCREEN_HEIGHT) { // if it's still on the screen, update its position
                    object_positions[i][1] = y;
                    draw_object(x, y, 0xFFC0CB); // redraw it in pink
                }
            }
        }
    }
}

// updates player position based on keyboard input
void update_character_position(char keycode) {
    erase_object(player_position * CELL_WIDTH, PLAYER_Y);

    if (keycode == 0x6B) { // move left
        if (player_position > 0) player_position--;
    } else if (keycode == 0x74) { // move right
        if (player_position < COLS - 1) player_position++;
    }

    draw_character(player_position, 0xFFFF); // redraw player in white
}

// ends game and displays score
void game_over() {
    VGA_fill(0); // clear screen
    VGA_write_char(20, 10, 'g');
    VGA_write_char(21, 10, 'a');
    VGA_write_char(22, 10, 'm');
    VGA_write_char(23, 10, 'e');
    VGA_write_char(24, 10, ' ');
    VGA_write_char(25, 10, 'o');
    VGA_write_char(26, 10, 'v');
    VGA_write_char(27, 10, 'e');
    VGA_write_char(28, 10, 'r');

    char buffer[10];
    sprintf(buffer, "score: %d", score);
    for (int i = 0; buffer[i] != '\0'; i++) {
        VGA_write_char(30 + i, 12, buffer[i]);
    }
}

// time to check if timer has expired
int timer_expired() {
    volatile int *MPcore_private_timer_ptr = (int *)MPCORE_PRIV_TIMER;

    // check if timer's done
    if (*(MPcore_private_timer_ptr + 3) & 0x1) {
        *(MPcore_private_timer_ptr + 3) = 0x1; // clear the flag
        return 1;
    }
    return 0;
}

// initialize timer in game
void init_timer() {
    volatile int *MPcore_private_timer_ptr = (int *)MPCORE_PRIV_TIMER;
    *(MPcore_private_timer_ptr) = TIMER_LOAD_VALUE; // set the timer value
    *(MPcore_private_timer_ptr + 2) = 0b011; // enable auto-reload and start it
}

int main() {
    init_game();
    init_timer();

    char data;
	
    while (!game_over_flag) {
        if (read_PS2_data(&data)) {
            // handle the movement keys directly
            if (data == 0x6B || data == 0x74) { // left or right arrow
                update_character_position(data);
            }
        }
        
        // update the objects but only if the timer has expired
        if (timer_expired()) {
            update_objects();
        }

        if (random_in_range(0, 100) < 10) { // spawn new objects occasionally
            spawn_object();
        }
    }

    game_over(); 
    return 0;
}
