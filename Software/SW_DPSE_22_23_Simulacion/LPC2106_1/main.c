// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor : Jesús Manuel Hernández Mangas
// File  : main.c – PAC-MAN suave, rápido y sin parpadeos
// =======================================================================
#include "system.h"
#include "xpm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stddef.h>

/* ------------------- Constantes generales -------------------------- */
#define SCREEN_W   800
#define SCREEN_H   480
#define TILE       24
#define SCALE      1
#define TW         (TILE * SCALE)          /* 32 px */

#define MAP_COLS   33
#define MAP_ROWS   18
#define X_OFFSET   ((SCREEN_W - MAP_COLS * TW)/2)
#define Y_OFFSET   48

/* Mapa */
#define WALL_UP   	'0'
#define WALL_DOWN   	'1'
#define WALL_RIGHT   	'2'
#define WALL_LEFT   	'3'
#define CORNER_UPR   	'4'
#define CORNER_UPL   	'5'
#define CORNER_DOWNR   	'6'
#define CORNER_DOWNL   	'7'
#define WALL_HOR	'8'
#define WALL_VER	'9'
#define WALL_HOR_LIM1	'A'
#define WALL_HOR_LIM2	'B'
#define WALL_VER_LIM1	'Y'
#define WALL_VER_LIM2	'Z'
#define COIN   		'C'
#define EMPTY  		'E'
#define PACMAN 		'P'
#define POWER_UP 	'V'
#define GHOST  		'G'

/* GPIO bits (no tocar) */
#define LEFT   (1 << 0)
#define RIGHT  (1 << 1)
#define UP     (1 << 2)
#define DOWN   (1 << 3)

/* Velocidad / suavidad */
#define STEP_PX            2   /* píxeles por iteración (1 = extra-suave) */
#define PAC_DELAY_MS       6   /* retardo interno Pac-Man                 */
#define GHOST_DELAY_MS     0   /* retardo interno fantasmas               */

/* ------------------- Laberinto base ------------------------------- */
static const char originalMap[MAP_ROWS][MAP_COLS+1] = {
    "500000000000000000000000000000004",  //  0: borde superior
    "3VPCCCCCCCCCCCCCCCCCCCCCCCCCCCCV2",  //  1: corredor superior exterior con power-ups
    "3CA88BCA88BCA88BCA88BCA88BCA88BC2",  //  2: pasillos verticales con monedas
    "3CEEEECEEEECEEEECEEEECEEEECEEEEC2",  //  3: pasillos verticales con espacios
    "3CYCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  //  4: corredor con esquinas internas
    "3C9CCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  //  5: corredor interior lleno de monedas
    "3C9EEECEEEECEEEECEEEECEEEECEEEEC2",  //  6: repetición de pasillos verticales con espacios
    "3C9CCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  //  7: corredor interior lleno de monedas
    "3C9CCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  //  8: repetición del corredor interior
    "3E9EEEEEGGGEEEEEEEEEEEEEEEEEEEEE2",  //  9: zona central de fantasmas
    "3C9CCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  // 10: corredor interior lleno de monedas
    "3C9EEECEEEECEEEECEEEECEEEECEEEEC2",  // 11: pasillos verticales con espacios
    "3C9CCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  // 12: corredor interior lleno de monedas
    "3CZEEECEEEECEEEECEEEECEEEECEEEEC2",  // 13: repetición de pasillos verticales
    "3CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2",  // 14: corredor interior lleno de monedas
    "3CA88BCA88BCA88BCA88BCA88BCA88BC2",  // 15: Pac-Man en corredor central
    "3VCCCCCCCCCCCCCCCCCCCCCCCCCCCCCV2",  // 16: corredor inferior exterior con power-ups
    "711111111111111111111111111111116"   // 17: borde inferior
};


/* ------------------- Tipos y estructuras -------------------------- */
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NONE } Direction;

typedef struct {
    int  x, y;                 /* casillas                  */
    Direction dir;
    char under;
    int  type;                 /* 0 rebote, 1 random, 2 AI  */
} Ghost;

/* ------------------- Estado global -------------------------------- */
#define GHOST_COUNT 3
static Ghost ghosts[GHOST_COUNT];
static char  map[MAP_ROWS][MAP_COLS + 1];

static int pacPixX, pacPixY;          /* Pac-Man en píxeles */
static Direction desiredDir = DIR_NONE;
static Direction currentDir = DIR_NONE;
static Direction printdir = DIR_NONE;

static int lives, score, totalCoins;
static bool hudDirty = true;

/* ------------------- Prototipos externos -------------------------- */
int GetAction(void);

/* ------------------- Prototipos internos -------------------------- */
static bool      wallAt(int r, int c);
static bool      canMoveDir(int px, int py, Direction d);
static void      restoreBackground(int px, int py);
static void      drawPacman(int px, int py);
static void      animatePacmanMove(int nx, int ny);
static void      movePacman(void);

static void      animateGhostMove(Ghost *g, int nx, int ny);
static void      moveGhosts(int tick);
static void      checkCollisions(void);

static void      initGame(void);
static void      dibuja_hud(void);
static void      dibuja_mapa(const char m[][MAP_COLS + 1]);
static void      drawTile(int r, int c);

/* =================================================================== */
/*                     FUNCIONES AUXILIARES                            */
/* =================================================================== */
static bool wallAt(int r, int c)
{
    if (c < 0 || c >= MAP_COLS || r < 0 || r >= MAP_ROWS) return true;
    return map[r][c] == (WALL_UP || WALL_DOWN || WALL_RIGHT || WALL_LEFT || WALL_HOR || WALL_VER || WALL_HOR_LIM1 || WALL_HOR_LIM2 || WALL_VER_LIM1 || WALL_VER_LIM2);
}

static bool canMoveDir(int px, int py, Direction d)
{
    int cx = (px - X_OFFSET) / TW;
    int cy = (py - Y_OFFSET) / TW;
    if (d == DIR_UP)    cy--;
    if (d == DIR_DOWN)  cy++;
    if (d == DIR_LEFT)  cx--;
    if (d == DIR_RIGHT) cx++;
    return !wallAt(cy, cx);
}

/* Restaura todos los tiles tocados por el sprite */
static void restoreBackground(int px, int py)
{
    int r0 = (py - Y_OFFSET) / TW;
    int c0 = (px - X_OFFSET) / TW;
    int offx = (px - X_OFFSET) % TW;
    int offy = (py - Y_OFFSET) % TW;

    drawTile(r0, c0);
    if (offx && c0 + 1 < MAP_COLS) drawTile(r0, c0 + 1);
    if (offy && r0 + 1 < MAP_ROWS) drawTile(r0 + 1, c0);
    if (offx && offy && r0 + 1 < MAP_ROWS && c0 + 1 < MAP_COLS)
        drawTile(r0 + 1, c0 + 1);
}

static void drawPacman(int px, int py)
{
    XPM_PintaAtxNyN(px, py, 11, pacman);
}

/* =================================================================== */
/*                         INICIALIZACIÓN                              */
/* =================================================================== */
static void initGame(void)
{
    int r, c, g;

    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) map[r][c] = originalMap[r][c];
        map[r][MAP_COLS] = '\0';
    }

    for (g = 0; g < GHOST_COUNT; g++) ghosts[g].type = -1;
    totalCoins = 0;

    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) {
            if (map[r][c] == PACMAN) {
                pacPixX = X_OFFSET + c * TW;
                pacPixY = Y_OFFSET + r * TW;
                map[r][c] = EMPTY;
            } else if (map[r][c] == GHOST) {
                for (g = 0; g < GHOST_COUNT; g++)
                    if (ghosts[g].type == -1) {
                        ghosts[g].x = c; ghosts[g].y = r;
                        ghosts[g].under = EMPTY;
                        ghosts[g].type  = g;
                        ghosts[g].dir   = (g == 0) ? DIR_RIGHT : DIR_NONE;
                        break;
                    }
            } else if (map[r][c] == COIN) totalCoins++;
        }
    }

    lives = 3; score = 0;
    desiredDir = currentDir = DIR_NONE;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned)tv.tv_usec);
}

/* =================================================================== */
/*                         DIBUJO MAPA & HUD                           */
/* =================================================================== */
static void drawTile(int r, int c)
{
    int px = X_OFFSET + c * TW;
    int py = Y_OFFSET + r * TW;
    switch (map[r][c]) {
        case WALL_UP:   	XPM_PintaAtxNyN(px, py, 49, pacman);  break;
	case WALL_RIGHT:	XPM_PintaAtxNyN(px, py, 62, pacman);  break;
	case WALL_LEFT:   	XPM_PintaAtxNyN(px, py, 60, pacman);  break;
	case WALL_DOWN:   	XPM_PintaAtxNyN(px, py, 73, pacman);  break;
	case CORNER_UPR:   	XPM_PintaAtxNyN(px, py, 50, pacman);  break;
	case CORNER_UPL:   	XPM_PintaAtxNyN(px, py, 48, pacman);  break;
	case CORNER_DOWNR:   	XPM_PintaAtxNyN(px, py, 74, pacman);  break;
	case CORNER_DOWNL:   	XPM_PintaAtxNyN(px, py, 72, pacman);  break;
	case WALL_HOR:   	XPM_PintaAtxNyN(px, py, 56, pacman);  break;
	case WALL_HOR_LIM1:   	XPM_PintaAtxNyN(px, py, 55, pacman);  break;
	case WALL_HOR_LIM2:   	XPM_PintaAtxNyN(px, py, 57, pacman);  break;
	case WALL_VER:   	XPM_PintaAtxNyN(px, py, 66, pacman);  break;
	case WALL_VER_LIM1:   	XPM_PintaAtxNyN(px, py, 54, pacman);  break;
	case WALL_VER_LIM2:   	XPM_PintaAtxNyN(px, py, 78, pacman);  break;
        case COIN:   		XPM_PintaAtxNyN(px, py, 81, pacman);  break;
        case EMPTY:  		XPM_PintaAtxNyN(px, py, 61, pacman);  break;
	case POWER_UP:  	XPM_PintaAtxNyN(px, py, 80, pacman);  break;
        case GHOST:  		XPM_PintaAtxNyN(px, py, 0, pacman);   break;
        default:     		XPM_PintaAtxNyN(px, py, 11, pacman);
    }
}

static void dibuja_mapa(const char m[][MAP_COLS + 1])
{
    XPM_Extract(pacman);
    xpm_sx = TILE; xpm_sy = TILE; XPM_SetxNyN(SCALE, SCALE);

    int r, c;
    for (r = 0; r < MAP_ROWS; r++){
        for (c = 0; c < MAP_COLS; c++){
            drawTile(r, c);}
   }
    drawPacman(pacPixX, pacPixY);
}

static void dibuja_hud(void)
{
    char buf[40];
    TFT_DrawFillSquareS(0, 0, SCREEN_W, Y_OFFSET, TFT_Color(50, 50, 50));
    FG_Color = WHITE; BG_Color = TFT_Color(50, 50, 50);
    CF = BigFont;
    //_sprintf(printdir, "VIDAS: %d  PUNTOS: %d", lives, score);
    TFT_print_xNyN(X_OFFSET, Y_OFFSET / 2 - 8, buf);
}

/* =================================================================== */
/*                   ANIMACIÓN PAC-MAN (suave)                         */
/* =================================================================== */
static void animatePacmanMove(int nx, int ny)
{
    int dx = (nx > pacPixX) ? STEP_PX : (nx < pacPixX ? -STEP_PX : 0);
    int dy = (ny > pacPixY) ? STEP_PX : (ny < pacPixY ? -STEP_PX : 0);

    while (pacPixX != nx || pacPixY != ny) {
        XPM_PintaAtxNyN(pacPixX, pacPixY,10,pacman);
        if (pacPixX != nx) pacPixX += dx;
        if (pacPixY != ny) pacPixY += dy;
        drawPacman(pacPixX, pacPixY);
        if (PAC_DELAY_MS) _delay_ms(PAC_DELAY_MS);
    }
}

/* --------------------- Movimiento de Pac-Man ----------------------- */
static void movePacman(void)
{
    int keys = GetAction();
    if (keys & UP)      desiredDir = DIR_UP;
    else if (keys & DOWN)  desiredDir = DIR_DOWN;
    else if (keys & LEFT)  desiredDir = DIR_LEFT;
    else if (keys & RIGHT) desiredDir = DIR_RIGHT;

    bool ax = ((pacPixX - X_OFFSET) % TW) == 0;
    bool ay = ((pacPixY - Y_OFFSET) % TW) == 0;

    if (ax && ay) {
        int cx = (pacPixX - X_OFFSET) / TW;
        int cy = (pacPixY - Y_OFFSET) / TW;

        if (map[cy][cx] == COIN) {
            map[cy][cx] = EMPTY; drawTile(cy, cx);
            score++; totalCoins--; hudDirty = true;
        }
        if (desiredDir != DIR_NONE && canMoveDir(pacPixX, pacPixY, desiredDir))
            currentDir = desiredDir;
        if (!canMoveDir(pacPixX, pacPixY, currentDir))
            currentDir = DIR_NONE;
    }

    if (currentDir == DIR_NONE) {
        /* Si está parado, asegúrate de que sigue visible */
        drawPacman(pacPixX, pacPixY);
        return;
    }

    int tx = pacPixX, ty = pacPixY;
    if (currentDir == DIR_UP)    ty -= TW;
    if (currentDir == DIR_DOWN)  ty += TW;
    if (currentDir == DIR_LEFT)  tx -= TW;
    if (currentDir == DIR_RIGHT) tx += TW;

    animatePacmanMove(tx, ty);
}

/* =================================================================== */
/*                      ANIMACIÓN FANTASMAS                            */
/* =================================================================== */
static void animateGhostMove(Ghost *g, int nx, int ny)
{
    map[g->y][g->x] = g->under; drawTile(g->y, g->x);

    int px = X_OFFSET + g->x * TW;
    int py = Y_OFFSET + g->y * TW;
    int tx = X_OFFSET + nx * TW;
    int ty = Y_OFFSET + ny * TW;
    int dx = (tx > px) ? STEP_PX : (tx < px ? -STEP_PX : 0);
    int dy = (ty > py) ? STEP_PX : (ty < py ? -STEP_PX : 0);

    while (px != tx || py != ty) {
        restoreBackground(px, py);
        if (px != tx) px += dx;
        if (py != ty) py += dy;
        XPM_PintaAtxNyN(px, py, 1, pacman);
        if (GHOST_DELAY_MS) _delay_ms(GHOST_DELAY_MS);
    }

    g->x = nx; g->y = ny;
    g->under = map[ny][nx];
    map[ny][nx] = GHOST;
    XPM_PintaAtxNyN(tx, ty, 0, pacman); 
}

/* ------------------ Movimientos lógicos fantasmas ------------------ */
static void moveGhosts(int tick)
{
    int i, nx, ny, t, k;
    int dirs[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

    for (i = 0; i < GHOST_COUNT; i++) {
        Ghost *g = &ghosts[i];
        nx = g->x; ny = g->y;

        if (g->type == 0) {
            if (g->dir == DIR_RIGHT) { nx++; if (wallAt(ny, nx)) { g->dir = DIR_LEFT;  nx = g->x - 1; } }
            else                     { nx--; if (wallAt(ny, nx)) { g->dir = DIR_RIGHT; nx = g->x + 1; } }
        }
        else if (g->type == 1) {
            if ((tick & 3) == 0) {
                for (t = 0; t < 10; t++) {
                    g->dir = dirs[rand() % 4];
                    nx = g->x; ny = g->y;
                    if (g->dir == DIR_UP)    ny--;
                    if (g->dir == DIR_DOWN)  ny++;
                    if (g->dir == DIR_LEFT)  nx--;
                    if (g->dir == DIR_RIGHT) nx++;
                    if (!wallAt(ny, nx)) break;
                }
            } else {
                if (g->dir == DIR_UP)    ny--;
                if (g->dir == DIR_DOWN)  ny++;
                if (g->dir == DIR_LEFT)  nx--;
                if (g->dir == DIR_RIGHT) nx++;
                if (wallAt(ny, nx)) { g->dir = DIR_NONE; nx = g->x; ny = g->y; }
            }
        }
        else {
            int dx = ((pacPixX - X_OFFSET) / TW) - g->x;
            int dy = ((pacPixY - Y_OFFSET) / TW) - g->y;
            g->dir = (abs(dx) > abs(dy))
                        ? (dx > 0 ? DIR_RIGHT : DIR_LEFT)
                        : (dy > 0 ? DIR_DOWN  : DIR_UP);

            nx = g->x; ny = g->y;
            if (g->dir == DIR_UP)    ny--;
            if (g->dir == DIR_DOWN)  ny++;
            if (g->dir == DIR_LEFT)  nx--;
            if (g->dir == DIR_RIGHT) nx++;
            if (wallAt(ny, nx)) {
                for (k = 0; k < 4; k++) {
                    g->dir = dirs[rand() % 4];
                    nx = g->x; ny = g->y;
                    if (g->dir == DIR_UP)    ny--;
                    if (g->dir == DIR_DOWN)  ny++;
                    if (g->dir == DIR_LEFT)  nx--;
                    if (g->dir == DIR_RIGHT) nx++;
                    if (!wallAt(ny, nx)) break;
                }
            }
        }

        if (nx != g->x || ny != g->y) animateGhostMove(g, nx, ny);
    }
}

/* =================================================================== */
/*                           COLISIONES                                */
/* =================================================================== */
static void checkCollisions(void)
{
    int tx = (pacPixX - X_OFFSET + TW / 2) / TW;
    int ty = (pacPixY - Y_OFFSET + TW / 2) / TW;
    int g;
    for (g = 0; g < GHOST_COUNT; g++) {
        if (tx == ghosts[g].x && ty == ghosts[g].y) {
            lives--; hudDirty = true;
            if (lives > 0) {
                restoreBackground(pacPixX, pacPixY);
                initGame(); dibuja_mapa(map); dibuja_hud();
            }
            return;
        }
    }
}

/* =================================================================== */
/*                                MAIN                                 */
/* =================================================================== */
int main(void)
{
    /* -------- NO TOCAR CONFIGURACIÓN DE PINES ---------------------- */
    SCS = 1;
    FIODIR  = 0b11111111111111111111111111011101;
    FIOSET  = 0xFFFFFFFF;
    PINSEL0 = 0x00029505;
    PINSEL1 = 0x00000000;
    _delay_ms(100);

    _printf("Desarrollo Practico de Sistemas Electronicos\r\n"
            "Ingenieria de Sistemas Electronicos\r\n");

    AMPLIF_OFF;
    TFT_Init();
    TP_Init();
    VibratorON(50);
   
    TFT_DrawFillSquareS(0, 0, LENX, LENY, TFT_Color(0, 0, 0));

    initGame();
    dibuja_mapa(map);
    dibuja_hud();

    int tick = 0;
    while (1) {
        if (totalCoins == 0) break;
        if (lives     <= 0) break;

        movePacman();
        //moveGhosts(tick);
        checkCollisions();
	

        if (hudDirty) { dibuja_hud(); hudDirty = false; }
        tick++;
    }
    return 0;
}

/* =================================================================== */
/*                        STUBS NEWLIB                                 */
/* =================================================================== */
int _gettimeofday(struct timeval *tv, void *tz)
{
    (void)tz;
    if (tv) { tv->tv_sec = 0; tv->tv_usec = 0; }
    return 0;
}

void *_sbrk(ptrdiff_t incr)
{
    extern char _end;
    static char *heap_end = NULL;
    char *prev_heap_end;

    if (heap_end == NULL) heap_end = &_end;
    prev_heap_end = heap_end;
    heap_end += incr;
    return (void *)prev_heap_end;
}
