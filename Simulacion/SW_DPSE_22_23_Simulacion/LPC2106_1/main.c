// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor : Jes�s Manuel Hern�ndez Mangas
// File  : main.c � PAC-MAN suave, r�pido y sin parpadeos
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
#define ABAJO_IZQ_X2 'T'
#define ARRIBA_IZQ_X2 'M'
#define ARRIBA_DER_X2 'U'
#define ABAJO_DER_X2 'I'
#define DOT 'D'
#define FDOT 'O'
#define cross1 'F'
#define cross2 'L'
#define cross3 'H'
#define cross4 'J'
#define cross 'S'


/* GPIO bits (no tocar) */
#define LEFT   (1 << 0)
#define RIGHT  (1 << 1)
#define UP     (1 << 2)
#define DOWN   (1 << 3)

/* Velocidad / suavidad */
#define STEP_PX            2   /* p�xeles por iteraci�n (1 = extra-suave) */
#define PAC_DELAY_MS       7   /* retardo interno Pac-Man                 */  /* retardo interno fantasmas (en ms)       */
#define GHOST_MOVE_INTERVAL_MS 1 /* ms entre cada movimiento de los fantasmas */        
#define GHOST_MOVE_INTERVAL_STEPS  1
#define POWERUP_DURATION_MS       500
#define POWERUP_DURATION_STEPS    (POWERUP_DURATION_MS / PAC_DELAY_MS)

#define PAC_MOUTH_TOGGLE_STEPS 20
#define PACMAN_OPEN_FRAME    11
#define PACMAN_CLOSED_FRAME  10
#define GHOST_COUNT 4




/* --- Parpadeo de ojos de los fantasmas --- */
static bool ghostsEyeOpen    = true;
static int  ghostsEyeCounter = 0;

static bool pacMouthOpen = true;
static int  pacMouthCounter = 0;

/* Estado power-up */
static bool powerMode         = false;
static int  powerTickCounter  = 0;

/* Orígenes de fantasmas para respawn */
static int origGhostX[GHOST_COUNT];
static int origGhostY[GHOST_COUNT];
/* -- Teletransporte: almacena las dos casillas 'O' -- */
static int teleR[2], teleC[2];
static int teleCount = 0;
static bool justTeleported = false;


/* --- Liberación escalonada de fantasmas --- */
#define GHOST_RELEASE_INTERVAL_MS    1600
#define GHOST_RELEASE_INTERVAL_STEPS \
    ((GHOST_RELEASE_INTERVAL_MS + PAC_DELAY_MS - 1) / PAC_DELAY_MS)
static bool ghostReleased[GHOST_COUNT] = { false, false, false, false };
static int  ghostReleaseStepCounter     = 0;
static int  ghostsReleasedCount        = 0;


/* ------------------- Laberinto base ------------------------------- */
static const char originalMap[MAP_ROWS][MAP_COLS+1] = {
    "M888888UOM888UEEEM8888888888F888U",  //  0: borde superior
    "9VPCCCC9E9CCCT888ICCCCCCCCCC9CCV9",  //  1: corredor superior exterior con power-ups
    "9CA88BC9E9CYCCCCCCCM88F888UC9CYC9",  //  2: pasillos verticales con monedas
    "9CCCCCCZCZCJ888BCCALVCZCCCZCZC9C9",  //  3: pasillos verticales con espacios
    "J88UCCCCCCC9CCCCCCC9CCCCYCCCCC9C9",  //  4: corredor con esquinas internas
    "9CCZCA888BCZCA888BCZCA88HF8888IC9",  //  5: corredor interior lleno de monedas
    "9CCCCCCCCCGCCCCCCCCCCCCCC9CCCCCC9",  //  6: repetici�n de pasillos verticales con espacios
    "9CM88UCCM888UCM888BCM88BC9CYCM88I",  //  7: corredor interior lleno de monedas
    "9C9EETUC9GGG9C9CCCCC9CCCCZC9C9EEE",  //  8: repetici�n del corredor interior
    "9C9EEE9C9EEE9CT888UCJ88BCCC9CT888",  //  9: zona central de fantasmas
    "9C9EEE9CJ888ICCCCC9C9CCCCA8LCEEEO",  // 10: corredor interior lleno de monedas
    "9C9EEE9C9CCCCCA888ICT88BCCC9CM888",  // 11: pasillos verticales con espacios
    "9C9EEMIC9CYCYCCCCCCCCCCCCYC9C9EEE",  // 12: corredor interior lleno de monedas
    "9CT88ICCZC9CT88888BCM88BC9CZCT88U",  // 13: repetici�n de pasillos verticales
    "9CCCCCCCCC9CCCCCCCCCZCCCC9CCCCCC9",  // 14: corredor interior lleno de monedas
    "9CA8BCYCA8H8888BCCYCCCYCAH8888BC9",  // 15: Pac-Man en corredor central
    "9VCCCC9CCCCCCCCCCMHUCC9CCCCCCCCV9",  // 16: corredor inferior exterior con power-ups
    "T88888H8888888888IET88H888888888I"   // 17: borde inferior
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

static Ghost ghosts[GHOST_COUNT];
static char  map[MAP_ROWS][MAP_COLS + 1];

static int pacPixX, pacPixY;          /* Pac-Man en p�xeles */
static Direction desiredDir = DIR_NONE;
static Direction currentDir = DIR_NONE;
static struct timeval lastGhostMoveTime;  /* para temporizar fantasmas */

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
static int getPacmanFrame(Direction dir);

static int       getGhostFrame(Ghost *g);
static void      animateGhostMove(Ghost *g, int nx, int ny);

static void      moveGhosts();
static void      checkCollisions(void);

static void      initGame(void);
static void      dibuja_hud(void);
static void      dibuja_mapa(const char m[][MAP_COLS + 1]);
static void      drawTile(int r, int c);
static char 		*ft_itoa(int n);
/* =================================================================== */
/*                     FUNCIONES AUXILIARES                            */
/* =================================================================== */
static int	get_digits(int n)
{
	size_t	i;

	i = 1;
	while (n /= 10)
		i++;
	return (i);
}

static char  *ft_itoa(int n)
{
	char		*str_num;
	int		digits;
	int	num;

	num = n;
	digits = get_digits(n);
	if (n < 0)
	{
		num *= -1;
		digits++;
	}
	if (!(str_num = (char *)malloc(sizeof(char) * (digits + 1))))
		return (NULL);
	*(str_num + digits) = 0;
	while (digits--)
	{
		*(str_num + digits) = num % 10 + '0';
		num = num / 10;
	}
	if (n < 0)
		*(str_num + 0) = '-';
	return (str_num);
}

static bool wallAt(int r, int c)
{
    if (c < 0 || c >= MAP_COLS || r < 0 || r >= MAP_ROWS) {
        return true;
    }

    char tile = map[r][c];
    return tile == WALL_UP   || tile == WALL_DOWN  ||
           tile == WALL_RIGHT|| tile == WALL_LEFT  ||
           tile == CORNER_UPR || tile == CORNER_UPL ||
           tile == CORNER_DOWNR|| tile == CORNER_DOWNL||
           tile == WALL_HOR   || tile == WALL_VER   ||
           tile == WALL_HOR_LIM1 || tile == WALL_HOR_LIM2 ||
           tile == WALL_VER_LIM1 || tile == WALL_VER_LIM2 ||
           tile == ABAJO_IZQ_X2  || tile == ABAJO_DER_X2  ||
           tile == ARRIBA_IZQ_X2|| tile == ARRIBA_DER_X2||
           tile == DOT         ||
           tile == cross1      || tile == cross2      ||
           tile == cross3      || tile == cross4      ||
           tile == cross;
}

#define DOT 'D'
#define FDOT 'O'
#define cross1 'F'
#define cross2 'L'
#define cross3 'H'
#define cross4 'J'
#define cross 'S'
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
    /* usa currentDir (o derecha si NONE) para orientar */
    Direction dir = currentDir;
    int tile= getPacmanFrame(dir);
    XPM_PintaAtxNyN(px, py, tile, pacman);
}

/* =================================================================== */
/*                         INICIALIZACI�N                              */
/* =================================================================== */
static void initGame(void)
{
    int r, c, g;

    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) map[r][c] = originalMap[r][c];
        map[r][MAP_COLS] = '\0';
    }
    /* Detecta las dos casillas teleport (FDOT = 'O') */
    teleCount = 0;
    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) {
            if (map[r][c] == FDOT && teleCount < 2) {
                teleR[teleCount] = r;
                teleC[teleCount] = c;
                teleCount++;
            }
        }
    }
    justTeleported = false;
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
                        origGhostX[g] = c; origGhostY[g] = r;
                        ghosts[g].under = EMPTY;
                        ghosts[g].type  = g;
                        ghosts[g].dir   = (g == 0) ? DIR_RIGHT : DIR_NONE;
                        break;
                    }
            } else if (map[r][c] == COIN) totalCoins++;
        }
    }
        /* Ajusta posiciones iniciales de fantasmas:
       solo el primero se moverá; los demás quedan en home hasta su turno */
    {
        int homeX = ghosts[0].x, homeY = ghosts[0].y;
        origGhostX[0] = homeX; origGhostY[0] = homeY;
        for (g = 1; g < GHOST_COUNT; g++) {
            ghosts[g].x     = homeX;
            ghosts[g].y     = homeY;
            ghosts[g].under = EMPTY;
            ghosts[g].dir   = DIR_NONE;
            origGhostX[g]   = homeX;
            origGhostY[g]   = homeY;
        }
    }
    /* Reinicia estado power-up y flags de liberación */
    powerMode        = false;
    powerTickCounter = 0;
    ghostReleaseStepCounter = 0;
    ghostsReleasedCount     = 0;
    for (g = 0; g < GHOST_COUNT; g++)
        ghostReleased[g] = false;
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
        case GHOST:  		XPM_PintaAtxNyN(px, py, 61, pacman);   break;
        case ARRIBA_IZQ_X2: XPM_PintaAtxNyN(px, py, 51, pacman);   break;
        case ABAJO_IZQ_X2: XPM_PintaAtxNyN(px, py, 75, pacman);   break;
        case ARRIBA_DER_X2: XPM_PintaAtxNyN(px, py, 53, pacman);   break;
        case ABAJO_DER_X2: XPM_PintaAtxNyN(px, py, 77, pacman);   break;
        case DOT: XPM_PintaAtxNyN(px, py, 67, pacman);   break;
        case FDOT: XPM_PintaAtxNyN(px, py, 61, pacman);   break;
        case cross1: XPM_PintaAtxNyN(px, py, 52, pacman);   break;
        case cross2: XPM_PintaAtxNyN(px, py, 65, pacman);   break;
        case cross3: XPM_PintaAtxNyN(px, py, 76, pacman);   break;
        case cross4: XPM_PintaAtxNyN(px, py, 63, pacman);   break;
        case cross: XPM_PintaAtxNyN(px, py, 64, pacman);   break;
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
    // char buf[40]; // This variable is declared but not used. Can be removed if not needed.
    TFT_DrawFillSquareS(0, 0, SCREEN_W, Y_OFFSET, TFT_Color(50, 50, 50));
    FG_Color = WHITE; BG_Color = TFT_Color(50, 50, 50); // Ensure WHITE is defined
    CF = BigFont; // Ensure BigFont is defined

    TFT_print_xNyN(X_OFFSET, Y_OFFSET / 2 - 8, "VIDAS:");
    char *tmp_lives_str = ft_itoa(lives); // Declare a local variable for the string
    if (tmp_lives_str) {
        TFT_print_xNyN(X_OFFSET + 170, Y_OFFSET / 2 - 8, tmp_lives_str); // Use original offset
        free(tmp_lives_str); // Free memory allocated by ft_itoa
    }

    TFT_print_xNyN(X_OFFSET + 250, Y_OFFSET / 2 - 8, "PUNTOS:"); // Use original offset
    char *tmp_score_str = ft_itoa(score); // Use global 'score' and declare local string variable
    if (tmp_score_str) {
       TFT_print_xNyN(X_OFFSET + 500, Y_OFFSET / 2 - 8, tmp_score_str); // Print score string, use original offset
       free(tmp_score_str); // Free memory allocated by ft_itoa
    }
    // hudDirty should be set to false in the main loop after this function is called.
    // The original code already does this: if (hudDirty) { dibuja_hud(); hudDirty = false; }
}

/* =================================================================== */
/*                   ANIMACI�N PAC-MAN (suave)                         */
/* =================================================================== */

static int getPacmanFrame(Direction dir) {
    /* si no hay dirección, apunta a la derecha por defecto */
    if (dir == DIR_NONE) dir = DIR_RIGHT;
    if (pacMouthOpen) {
        switch (dir) {
            case DIR_RIGHT: return 11;
            case DIR_DOWN:  return 23;
            case DIR_LEFT:  return 35;
            case DIR_UP:    return 47;
        }
    } else {
        switch (dir) {
            case DIR_RIGHT: return 10;
            case DIR_DOWN:  return 22;
            case DIR_LEFT:  return 34;
            case DIR_UP:    return 46;
        }
    }
    return 11; /* fallback */
}

static void animatePacmanMove(int nx, int ny)
{
    int dx    = (nx > pacPixX) ? STEP_PX : (nx < pacPixX ? -STEP_PX : 0);
    int dy    = (ny > pacPixY) ? STEP_PX : (ny < pacPixY ? -STEP_PX : 0);
    int steps = (abs(nx - pacPixX) + abs(ny - pacPixY)) / STEP_PX;
    int i;
    for (i = 0; i < steps; i++) {
        restoreBackground(pacPixX, pacPixY);
        pacPixX += dx;
        pacPixY += dy;
        /* alterna boca */
        if (++pacMouthCounter >= PAC_MOUTH_TOGGLE_STEPS) {
            pacMouthOpen    = !pacMouthOpen;
            pacMouthCounter = 0;
        }
        /* dibuja según dirección actual */
        int frame = getPacmanFrame(currentDir);
        XPM_PintaAtxNyN(pacPixX, pacPixY, frame, pacman);
        if (PAC_DELAY_MS) _delay_ms(PAC_DELAY_MS);
    }
}

/* ------------ Lógica de movimiento de Pac-Man ------------ */
static void movePacman(void)
{
    int keys = GetAction();
    if      (keys & UP)    desiredDir = DIR_UP;
    else if (keys & DOWN)  desiredDir = DIR_DOWN;
    else if (keys & LEFT)  desiredDir = DIR_LEFT;
    else if (keys & RIGHT) desiredDir = DIR_RIGHT;

    /* Sólo en cruces chequea monedas y posible cambio de dirección */
    bool onTileX = ((pacPixX - X_OFFSET) % TW) == 0;
    bool onTileY = ((pacPixY - Y_OFFSET) % TW) == 0;

    if (onTileX && onTileY) {
        int cx = (pacPixX - X_OFFSET) / TW;
        int cy = (pacPixY - Y_OFFSET) / TW;

    /* Power-up: si come 'V' */
    if (map[cy][cx] == POWER_UP) {
            map[cy][cx] = EMPTY;
            drawTile(cy, cx);
            powerMode        = true;
            powerTickCounter = 0;
            /* quizá sumar puntos aquí */
        }
        /* Si acabamos de teleportar, reseteamos la flag y seguimos movimiento */
        if (justTeleported) {
                justTeleported = false;
            }
            /* Teletransporte: si estamos en 'O' y no aún teleportando */
            else if (map[cy][cx] == FDOT) {
                /* índice de origen y destino */
                int srcIdx  = (cy == teleR[0] && cx == teleC[0]) ? 0 : 1;
                int destIdx = 1 - srcIdx;
                /* Limpia el sprite en la casilla actual */
                restoreBackground(pacPixX, pacPixY);
                /* Mueve a la otra salida */
                pacPixX = X_OFFSET + teleC[destIdx] * TW;
                pacPixY = Y_OFFSET + teleR[destIdx] * TW;
                /* Cuando venga del primer O (srcIdx=0) gira a izquierda */
                /* Cuando venga del segundo O (srcIdx=1) gira abajo */
                if (srcIdx == 0) {
                    desiredDir = currentDir = DIR_LEFT;
                } else {
                    desiredDir = currentDir = DIR_DOWN;
                }
                /* Marcamos para no retrigger */
                justTeleported = true;
                /* Dibuja Pac-Man en la nueva posición */
                drawPacman(pacPixX, pacPixY);
                return;
            }
        /* Come moneda */
        if (map[cy][cx] == COIN) {
            map[cy][cx] = EMPTY;
            drawTile(cy, cx);
            score++; totalCoins--; hudDirty = true;
        }

        /* Cambia dirección si es posible */
        if (desiredDir != DIR_NONE && canMoveDir(pacPixX, pacPixY, desiredDir))
            currentDir = desiredDir;
        if (!canMoveDir(pacPixX, pacPixY, currentDir))
            currentDir = DIR_NONE;
    }

    /* Si no puede moverse, simplemente repinta su sprite actual */
    if (currentDir == DIR_NONE) {
        int frame = getPacmanFrame(desiredDir);
        XPM_PintaAtxNyN(pacPixX, pacPixY, frame, pacman);
        return;
    }

    /* cálculo de destino y animación */
    int tx = pacPixX, ty = pacPixY;
    if (currentDir == DIR_UP)    ty -= TW;
    if (currentDir == DIR_DOWN)  ty += TW;
    if (currentDir == DIR_LEFT)  tx -= TW;
    if (currentDir == DIR_RIGHT) tx += TW;
    animatePacmanMove(tx, ty);
}

static int getGhostFrame(Ghost *g) {
        /* Calcula dirección del fantasma hacia Pac-Man */
        int gx = X_OFFSET + g->x * TW;
        int gy = Y_OFFSET + g->y * TW;
        int dx = pacPixX - gx;
        int dy = pacPixY - gy;
        Direction dir;
        if (abs(dx) > abs(dy))
            dir = (dx >= 0) ? DIR_RIGHT : DIR_LEFT;
        else
            dir = (dy >= 0) ? DIR_DOWN  : DIR_UP;
    
        bool open = ghostsEyeOpen;
        switch (g->type) {
          case 0: /* Rojo */
            switch (dir) {
              case DIR_RIGHT: return open?  0:  1;
              case DIR_DOWN:  return open? 12: 13;
              case DIR_LEFT:  return open? 24: 25;
              case DIR_UP:    return open? 36: 37;
            }
            break;
          case 1: /* Amarillo */
            switch (dir) {
              case DIR_RIGHT: return open?  2:  3;
              case DIR_DOWN:  return open? 14: 15;
              case DIR_LEFT:  return open? 26: 27;
              case DIR_UP:    return open? 38: 39;
            }
            break;
          case 2: /* Azul */
            switch (dir) {
              case DIR_RIGHT: return open?  6:  7;
              case DIR_DOWN:  return open? 18: 19;
              case DIR_LEFT:  return open? 30: 31;
              case DIR_UP:    return open? 42: 43;
            }
            break;
          case 3: /* Morado */
            switch (dir) {
              case DIR_RIGHT: return open?  8:  9;
              case DIR_DOWN:  return open? 20: 21;
              case DIR_LEFT:  return open? 32: 33;
              case DIR_UP:    return open? 44: 45;
            }
            break;
        }
        return 0;
    }

/* =================================================================== */
/*                      ANIMACI�N FANTASMAS                            */
/* =================================================================== */
static void animateGhostMove(Ghost *g, int nx, int ny)
{
    /* Restaura celda anterior */
    map[g->y][g->x] = g->under;
    drawTile(g->y, g->x);

    int px    = X_OFFSET + g->x * TW;
    int py    = Y_OFFSET + g->y * TW;
    int tx    = X_OFFSET + nx    * TW;
    int ty    = Y_OFFSET + ny    * TW;
    int dx    = (tx > px) ? STEP_PX : (tx < px ? -STEP_PX : 0);
    int dy    = (ty > py) ? STEP_PX : (ty < py ? -STEP_PX : 0);
    int steps = (abs(tx - px) + abs(ty - py)) / STEP_PX;
    int i;
    for (i = 0; i < steps; i++) {
                restoreBackground(px, py);
                px += dx;
                py += dy;
                /* Parpadeo de ojos */
                if (++ghostsEyeCounter >= PAC_MOUTH_TOGGLE_STEPS) {
                    ghostsEyeOpen    = !ghostsEyeOpen;
                    ghostsEyeCounter = 0;
                }
                /* Dibuja fantasma orientado y animado */
                int frame = getGhostFrame(g);
                frame = powerMode ? 59 : getGhostFrame(g);
                XPM_PintaAtxNyN(px, py, frame, pacman);
                //if (GHOST_DELAY_MS) _delay_ms(GHOST_DELAY_MS);
            }

    /* Actualiza lógica */
    g->x     = nx;
    g->y     = ny;
    g->under = map[ny][nx];
    map[ny][nx] = GHOST;
}

/* ------------ Movimiento aleatorio de Fantasmas ------------ */
static void moveGhosts(void)
{
    static const Direction dirs[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
    int i, t;
    for (i = 0; i < GHOST_COUNT; i++) {
        if (!ghostReleased[i]) continue;
        Ghost *g = &ghosts[i];

        /* A veces elige nueva dirección al azar */
        if (g->dir == DIR_NONE || (rand() & 7) == 0) {
            for (t = 0; t < 8; t++) {
                Direction d = dirs[rand() % 4];
                int nx = g->x + (d==DIR_RIGHT) - (d==DIR_LEFT);
                int ny = g->y + (d==DIR_DOWN)  - (d==DIR_UP);
                if (!wallAt(ny, nx)) {
                    g->dir = d;
                    break;
                }
            }
        }

        /* Intenta mover en esa dirección */
        int nx = g->x + (g->dir==DIR_RIGHT) - (g->dir==DIR_LEFT);
        int ny = g->y + (g->dir==DIR_DOWN)  - (g->dir==DIR_UP);

        if (!wallAt(ny, nx) && (nx!=g->x || ny!=g->y))
            animateGhostMove(g, nx, ny);
        else
            g->dir = DIR_NONE;  /* colisión: reinicia dir */
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
        if (!ghostReleased[g]) continue;
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
#define PACMANIA_TEMPO 150
void start_pacmania_music(void) {
    // El array PARTITURE se define en "pacmania.mod.txt.c"
    // Calculamos N, el n�mero de eventos (cada evento son 4 bytes)
   
    int numero_de_eventos = 131;

    // Llamamos a la funci�n principal para iniciar la partitura
    PARTITURE_On(PARTITURE, numero_de_eventos, 90);
}

int main(void)
{
    /* -------- NO TOCAR CONFIGURACI�N DE PINES ---------------------- */
    SCS = 1;
    FIODIR  = 0b11111111111111111111111111011101;
    FIOSET  = 0xFFFFFFFF;
    PINSEL0 = 0x00029505;
    PINSEL1 = 0x00000000;
    _delay_ms(100);

    _printf("Desarrollo Practico de Sistemas Electronicos\r\n"
            "Ingenieria de Sistemas Electronicos\r\n");

    AMPLIF_ON;
   
       start_pacmania_music();
   
    TFT_Init();
    TP_Init();

    TFT_DrawFillSquareS(0, 0, LENX, LENY, TFT_Color(0, 0, 0));

    initGame();
    dibuja_mapa(map);
    dibuja_hud();
    start_pacmania_music();

    xpm_sx = TILE; xpm_sy = TILE;
    XPM_SetxNyN(SCALE, SCALE);

    int i;
    for (i = 0; i < GHOST_COUNT; i++) {
            int px    = X_OFFSET + ghosts[i].x * TW;
            int py    = Y_OFFSET + ghosts[i].y * TW;
            int frame = powerMode ? 60 : getGhostFrame(&ghosts[i]);
            XPM_PintaAtxNyN(px, py, frame, pacman);
        }
    int ghostStepCounter=0;
    //AUDIO_Play_Pacmania();
    while (1) {
        if (totalCoins == 0) break;
        if (lives     <= 0) break;

        movePacman();
        if (powerMode) {
            if (++powerTickCounter >= POWERUP_DURATION_STEPS) {
            powerMode = false;
            }
            }
        if (++ghostStepCounter >= GHOST_MOVE_INTERVAL_STEPS) {
            moveGhosts();
            ghostStepCounter = 0;
        }
            if (++ghostReleaseStepCounter >= GHOST_RELEASE_INTERVAL_STEPS
                    && ghostsReleasedCount < GHOST_COUNT) {
                    ghostReleased[ghostsReleasedCount++] = true;
		    VibratorON(200);
                    ghostReleaseStepCounter = 0;
                }
            {
                    int tx = (pacPixX - X_OFFSET + TW/2) / TW;
                    int ty = (pacPixY - Y_OFFSET + TW/2) / TW;
                    int g;
                    for (g = 0; g < GHOST_COUNT; g++) {
                        if (tx == ghosts[g].x && ty == ghosts[g].y) {
                            if (powerMode) {
                                /* come fantasma: respawn */
                                restoreBackground(pacPixX, pacPixY);
                                ghosts[g].x = origGhostX[g];
                                ghosts[g].y = origGhostY[g];
                                ghosts[g].dir   = DIR_NONE;
                                ghosts[g].under = EMPTY;
                                map[origGhostY[g]][origGhostX[g]] = EMPTY;
                                /* dibuja fantasma en casa */
                                drawTile(origGhostY[g], origGhostX[g]);
                            } else {
                                lives--; hudDirty = true;
                                if (lives > 0) {
                                    restoreBackground(pacPixX, pacPixY);
                                    initGame(); dibuja_mapa(map); dibuja_hud();
                                }
                            }
                        }
                    }
                }
            checkCollisions();    
        if (hudDirty) { dibuja_hud(); hudDirty = false; }
        
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
