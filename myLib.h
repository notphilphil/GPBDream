//////////////////////////////////////////////////////////////////////
//
//  INDEX
// 
//////////////////////////////////////////////////////////////////////
/* 
        General
        Video
        DMA
        Input

====================================================================*/

// *** General =======================================================

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int s32;

#define TRUE 1
#define FALSE 0


// *** Video =======================================================

/* Mode 3 */
extern u16* videoBuffer;               // 0x6000000
// Need: unsigned short *videoBuffer = (unsigned short *)0x6000000;
#define REG_DISPCNT  *(u16*) 0x4000000
#define SCANLINECOUNTER *(volatile u16*)0x4000006

#define COLOR(r,g,b) (((b) << 10) | ((g) << 5) | (r))
#define OFFSET(r, c, row_length) (((r) * (row_length)) + (c))

//Background Enables
#define BG0_ENABLE  (1<<8)
#define BG1_ENABLE  (1<<9)
#define BG2_ENABLE  (1<<10)
#define BG3_ENABLE  (1<<11)

#define BLACK   COLOR(0,0,0)
#define WHITE   COLOR(31, 31, 31)
#define RED     COLOR(31,0,0)
#define GREEN   COLOR(0, 31, 0)
#define BLUE    COLOR(0,0,31)
#define YELLOW  COLOR(31,31,0)
#define CYAN    COLOR(0,31,31)
#define MAGENTA COLOR(31,0,31)
#define GRAY    COLOR(25, 25, 25)

#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4
#define MODE_5 5
#define MODE_6 6

/* DMA */

typedef struct
{
    const volatile void *src;
    const volatile void *dst;
    u32                  cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
// DMA channels
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

// Mostly just used for sound.
#define DMA_REPEAT (1 << 25)

// DMA copies 16 bits at a time normally (1 pixel), 
// but can be set to copy 32 bits at a time to be 
// even faster for multiples of 32 bits.
#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

// Don't worry about these, the default DMA_NOW is good enough
// for 2110, if you want to go beyond the scope of the class you
// can ask a TA or the internet about the other options.
#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)


// *** Input =========================================================

// Buttons

#define BUTTON_A        (1<<0)
#define BUTTON_B        (1<<1)
#define BUTTON_SELECT   (1<<2)
#define BUTTON_START    (1<<3)
#define BUTTON_RIGHT    (1<<4)
#define BUTTON_LEFT     (1<<5)
#define BUTTON_UP       (1<<6)
#define BUTTON_DOWN     (1<<7)
#define BUTTON_R        (1<<8)
#define BUTTON_L        (1<<9)

#define BUTTONS (*(unsigned int *)0x4000130)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

// My stuff

#define UNUSED(x) (void)(x)

// Different stats for player and pieces
#define PLAYER_SPEED 2
#define JACKET_SPEED 1
#define BULLDOG_SPEED 1

#define NUM_JACKETS_1 3
#define NUM_JACKETS_2 4
#define NUM_JACKETS_3 6
#define MAX_JACKETS 6

#define NUM_BULLDOGS_1 3
#define NUM_BULLDOGS_2 3
#define NUM_BULLDOGS_3 5
#define MAX_BULLDOGS 5

#define PLAYER_HP 10
#define NUM_LEVELS 3

// Enum for state of game
typedef enum State {
    START,
    INTRO,
    INSTRUCTIONS,
    LEVEL_CHANGE,
    PLAY,
    VICTORY,
    DEFEAT
} GameState;

// Structures
typedef struct {
    int x;
    int y;
} Vector;

typedef struct {
    Vector pos;
    Vector size;
    int hp;
    int score;
} Player;

typedef struct {
    Vector pos;
    Vector size;
    Vector direction;
    int strength;
} Bulldog;

typedef struct {
    Vector pos;
    Vector size;
    Vector direction;
    int reward;
} Jacket;

// Prototypes

// Utility
void waitForVBlank();
void initializePieces(Player* hero, int new_hp, int new_score, int new_level, int* current_level,
    int new_jacket_count, int new_bulldog_count, int* jacket_count, int* bulldog_count,
    Jacket jackets[], Bulldog bulldogs[]);
// Drawing
void drawFullscreenImage(int width, int height, const u16* image);
void drawPiece(Vector pos, int width, int height, const u16* image);
void drawPlayerStats(Player* hero);
void drawScore(int score);
void drawColoredRectangle(Vector pos, int width, int height, volatile unsigned short* color);
// Moving
void movePlayer(Player* hero, int up, int down, int right, int left);
int moveBulldog(Bulldog* curr, Player* hero, int speed);
int moveJacket(Jacket* curr, Player* hero, int speed);
int detectCollisions(Vector hero_pos, Vector hero_size, Vector piece_pos, Vector piece_size);