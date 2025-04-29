// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor: Jesús Manuel Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: main.c  Programa principal
// =======================================================================
#include "system.h"
#include "xpm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SCREEN_W   800
#define SCREEN_H   480
#define TILE       15
#define SCALE      2
#define TW         (TILE * SCALE)  // 30 px



#define WALL    '1'
#define COIN    'C'
#define EMPTY   '0'
#define PACMAN  'P'
#define GHOST   'G'

#define MAP_COLS   26
#define MAP_ROWS   14


// Márgenes para centrar (horizontal y vertical + HUD)
#define X_OFFSET  ((SCREEN_W - MAP_COLS*TW) / 2)  // = (800 - 780)/2 = 10 px
#define Y_OFFSET  60   

int GetAction();
// -----------------------------------------------------------------------------
  static const char originalMap[MAP_ROWS][MAP_COLS+1] = {
    "11111111111111111111111111",
    "1PCCCCCCCCCCCC1CCCCCCCCG11",
    "1CCCC11111CCCC1CCCC111CC11",
    "1CCCC1   1CCCC1CCCC1  CC11",  // Las espacios ' ' representan áreas de pared interna; ajustadas abajo.
    "1CCCC1   1CCCC1CCCC1   1C1",
    "1CCCC1   1CCCC1CCCC1   GC1",  // Suponemos 3 fantasmas G; reemplazar ' ' por '1' apropiadamente
    "1CCCC11111CCCC1CCCC111CCC1",
    "1CCCCCCCCCCCCCCCCCCCCCCCC1",
    "1CCCCCCCCCCCCCCCCCCCCCCCC1",
    "1CCCC1CCCC11111CCCC1CCCCC1",
    "1CCCC1CCCC1GGG1CCCC1CCCCC1",
    "1CCCC1CCCC1   1CCCC1CCCCC1",
    "1CCCC1CCCC1   1CCCC1CCCCC1",
    "11111111111111111111111111",
};

// Direcciones de movimiento
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NONE } Direction;

// Estructura para fantasmas
typedef struct {
    int x;            // Columna actual
    int y;            // Fila actual
    Direction dir;    // Dirección actual (para fantasma fijo o perseguidor, o última dirección válida)
    char under;       // Contenido de la celda debajo del fantasma (COIN o EMPTY)
    int type;         // Tipo de fantasma: 0=fijo, 1=aleatorio, 2=perseguidor
} Ghost;

static char map[MAP_ROWS][MAP_COLS+1];
static int lives;
static int score;
static int totalCoins;
static int pacmanX, pacmanY;
//static Direction pacmanDir = DIR_NONE;  // dirección actual de Pac-Man (opcional, aquí solo usamos input directo)

// Arreglo de fantasmas
#define GHOST_COUNT 3
static Ghost ghosts[GHOST_COUNT];

void initGame() {
    // Copiar la matriz de mapa original a la matriz de juego actual
   int i, j, g;
    for(i = 0; i < MAP_ROWS; ++i) {
        for(j = 0; j < MAP_COLS; ++j) {
            map[i][j] = originalMap[i][j];
        }
	map[i][MAP_COLS] = '\0';
    }
    // Inicializar vidas y puntaje
    lives = 3;
    score = 0;
    totalCoins = 0;
    // Encontrar la posición inicial de Pac-Man y contar monedas
    for(i = 0; i < MAP_ROWS; ++i) {
        for(j = 0; j < MAP_COLS; ++j) {
            if(map[i][j] == PACMAN) {
                pacmanY = i;
                pacmanX = j;
            } else if(map[i][j] == GHOST) {
                // Inicializar fantasmas: registrar sus posiciones iniciales
                // Encontrar primer slot libre en arreglo ghosts
                for(g = 0; g < GHOST_COUNT; ++g) {
                    if(ghosts[g].type == -1) { // usamos type==-1 como indicador no inicializado
                        ghosts[g].y = i;
                        ghosts[g].x = j;
                        ghosts[g].under = EMPTY; // asumimos que no hay moneda bajo fantasma inicial
                        // Asignar tipo según orden (por ejemplo, 0=fijo, 1=aleatorio, 2=perseguidor)
                        ghosts[g].type = g; 
                        // Dirección inicial: 
                        if(ghosts[g].type == 0) {
                            ghosts[g].dir = DIR_RIGHT; // fantasma fijo inicia moviendo derecha
                        } else {
                            ghosts[g].dir = DIR_NONE;  // otros pueden calcular dirección en su turno
                        }
                        break;
                    }
                }
            } else if(map[i][j] == COIN) {
                totalCoins++;
            }
        }
    }
    // Nota: arriba se asume que en originalMap colocamos exactamente GHOST_COUNT fantasmas 'G'. 
    // Alternativamente, podríamos enumerarlos si supiéramos sus posiciones de inicio.
    // Para simplificar, en este ejemplo usamos el truco de marcar ghosts[g].type = -1 inicialmente para encontrar posiciones.
}


void dibuja_hud(void) {
    // Ejemplo: fondo gris sobre la franja superior
    TFT_DrawFillSquareS(0, 0, SCREEN_W, Y_OFFSET, TFT_Color(50,50,50));
    FG_Color = WHITE; BG_Color = TFT_Color(50,50,50);
    CF = BigFont;
    TFT_print_xNyN( X_OFFSET, Y_OFFSET/2 - 8,
                    "VIDAS: 3  PUNTOS: 1234");
}

void dibuja_mapa(const char mapa[][MAP_COLS+1]) {
    int i, j;
    // Extraemos todos los sprites XPM
    XPM_Extract(wall_xpm);
    XPM_Extract(bg_xpm);
    XPM_Extract(coin_xpm);
    XPM_Extract(pacman_xpm);
    XPM_Extract(ghost_xpm);

    // Tamaño de tile y escala
    xpm_sx = TILE; 
    xpm_sy = TILE;
    XPM_SetxNyN(SCALE, SCALE);

    // Recorremos cada celda y pintamos el sprite adecuado
    for (i = 0; i < MAP_ROWS; i++) {
        for (j = 0; j < MAP_COLS; j++) {
            int x = X_OFFSET + j * TW;
            int y = Y_OFFSET + i * TW;
            switch (mapa[i][j]) {
                case WALL: // pared
                    XPM_PintaAtxNyN(x, y, 0, wall_xpm);
                    break;
                case EMPTY: // fondo sin moneda
                    XPM_PintaAtxNyN(x, y, 0, bg_xpm);
                    break;
                case COIN: // moneda
                    XPM_PintaAtxNyN(x, y, 0, coin_xpm);
                    break;
                case PACMAN: // Pac-Man
                    XPM_PintaAtxNyN(x, y, 0, pacman_xpm);
                    break;
                case GHOST: // fantasma
                    XPM_PintaAtxNyN(x, y, 0, ghost_xpm);
                    break;
                default:
                    // por seguridad, dibujamos fondo
                    XPM_PintaAtxNyN(x, y, 0, bg_xpm);
            }
        }
    }
}

#define LEFT	(1<< 0)
#define RIGHT   (1<< 1)
#define UP      (1<< 2)
#define DOWN    (1<< 3)

void movePacman(void)
{
    int action = GetAction();
    Direction newDir = DIR_NONE;

    if      (action & UP)    newDir = DIR_UP;
    else if (action & DOWN)  newDir = DIR_DOWN;
    else if (action & LEFT)  newDir = DIR_LEFT;
    else if (action & RIGHT) newDir = DIR_RIGHT;

    if (newDir == DIR_NONE) return;   // ninguna tecla: no se mueve

    int tx = pacmanX, ty = pacmanY;
    if (newDir == DIR_UP)    ty--;
    if (newDir == DIR_DOWN)  ty++;
    if (newDir == DIR_LEFT)  tx--;
    if (newDir == DIR_RIGHT) tx++;

    if (tx>=0 && tx<MAP_COLS &&
        ty>=0 && ty<MAP_ROWS &&
        map[ty][tx] != WALL)
    {
        map[pacmanY][pacmanX] = EMPTY;
        pacmanX = tx; pacmanY = ty;

        if (map[ty][tx] == COIN) {
            score++;
            totalCoins--;
        }
        map[pacmanY][pacmanX] = PACMAN;
    }
}




void checkCollisions() {
    // Revisar si Pac-Man comparte celda con algún fantasma
   int g, h, i, j;
    for(g = 0; g < GHOST_COUNT; ++g) {
        if(pacmanX == ghosts[g].x && pacmanY == ghosts[g].y) {
            // Colisión detectada
            lives--;
            if(lives > 0) {
                // Reset de posiciones (similar a lo hecho en moveGhosts colisión)
                // Remover fantasmas del mapa (restaurar debajo)
                for(h = 0; h < GHOST_COUNT; ++h) {
                    map[ghosts[h].y][ghosts[h].x] = ghosts[h].under;
                }
                // Remover Pac-Man del mapa
                map[pacmanY][pacmanX] = EMPTY;
                // Restaurar Pac-Man a posición inicial
                for(i = 0; i < MAP_ROWS; ++i) {
                    for(j = 0; j < MAP_COLS; ++j) {
                        if(originalMap[i][j] == PACMAN) {
                            pacmanY = i;
                            pacmanX = j;
                        }
                    }
                }
                map[pacmanY][pacmanX] = PACMAN;
                // Restaurar fantasmas a posiciones iniciales
                int foundGhosts = 0;
                for(i = 0; i < MAP_ROWS; ++i) {
                    for(j = 0; j < MAP_COLS; ++j) {
                        if(originalMap[i][j] == GHOST && foundGhosts < GHOST_COUNT) {
                            ghosts[foundGhosts].y = i;
                            ghosts[foundGhosts].x = j;
                            ghosts[foundGhosts].under = EMPTY;
                            ghosts[foundGhosts].type = foundGhosts;
                            ghosts[foundGhosts].dir = (foundGhosts == 0 ? DIR_RIGHT : DIR_NONE);
                            map[i][j] = GHOST;
                            foundGhosts++;
                        }
                    }
                }
            } else {
                // Game Over: todas las vidas perdidas (podríamos indicar fin del juego)
            }
            // Después de manejar la colisión, salimos de la función
            return;
        }
    }
}

int main (void)
{ 
 //int x,y,i,k,t;
   
 SCS = 1; // Muy importante para activar el FAST GPIO
 //          33222222222211111111110000000000
 //          10987654321098765432109876543210 
 FIODIR  = 0b11111111111111111111111111011101;  
 FIOSET  = 0xFFFFFFFF;  
 
 PINSEL0 = 0x00029505;
 PINSEL1 = 0x00000000;
 _delay_ms(100); // Espero a que el terminal del PC esté preparado

 _printf("Desarrollo Practico de Sistemas Electronicos\r\n"
         "Ingenieria de Sistemas Electronicos\r\n");

 AMPLIF_OFF;
 TFT_Init();    
 TP_Init();
 //TP_Calibrate();

 VibratorON(50);
 
 // Ejemplo de dibujo .....
 TFT_DrawFillSquareS(0,0,LENX,LENY,TFT_Color(0,0,0)); // Borro pantalla
 /*CF = (struct _current_font*) BigFont;				  // Selecciono fuente
 FG_Color = BLUE;									  // ForeGround Color
 BG_Color = TFT_Color(100,200,10);					  // BackGround Color
 xN = 4;											  // Tamaño fuente horizontal x2
 yN = 5;											  // Tamaño fuente vertical x3
 */
 TFT_print_xNyN(100,100,"Hola Mundo");				  // Pinta "Hola Mundo" 
				  // Linea
 
 /*XPM_Extract(bloques);
 xpm_sx = 15; xpm_sy = 15; // Tamaño de las teselas (tiles)
 XPM_SetxNyN( 5, 5);       // Amplificación
 i=0;
 
 //xpm_xmirror  =1;
 
 for(k=0;k<10;k++) XPM_PintaAtxNyN(k*15*xpm_xn, 300, k, bloques);
 for(k=0;k<10;k++) XPM_PintaAtxNyN(k*15*xpm_xn, 300+15*xpm_yn, k+10, bloques);*/
  
 /*Timer_On();
 t=Get_Time();
 for(k=0;k<1;k++)
 { 
  for(y=0; y<LENY/xpm_sy; y++)
   for(x=0; x<LENX/xpm_sx; x++)
    XPM_PintaAt(x*xpm_sx, y*xpm_sy, (i++)&0b1111, bloques);   
  i++;
 }*/
 

 
 ///////////////////// ///

 //_printf("%d\n", Get_Time()-t);
 
 // -------------------------------------------------------
 //TP_Calibrate();
 // Test de los botones y el panel táctil 
 CS_ON;
 FIOSET = SD_CS|TP_CS;
 _printf("\r\n""BT ""(X   ,Y   )""\r\n");
///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////




   initGame();
   dibuja_hud();
   dibuja_mapa(map);
   int cycle=0;
   
   while(1)
   {
     int act = GetAction();
    _printf("act=%02X\r\n", act);   // debería cambiar al pulsar
      if (totalCoins == 0)
      {
	 //meter msg victoria
	    break;
      }
      if (lives <= 0)
      {
	 //meter msg game over
	 break;
      }
      movePacman();
      checkCollisions();
      if(lives <= 0)
      {
            // Pac-Man murió, reiniciar ciclo sin mover fantasmas este turno
            // (El checkCollisions se encargó de resetear posiciones si vidas > 0)
            cycle++;
            dibuja_mapa(map);
            dibuja_hud();
            // Pequeño retardo para visualizar (ejemplo)
            // delay_ms(1000);
            continue;
       }
       //moveGhosts(cycle);
       
       checkCollisions();
       
       dibuja_mapa(map);
       dibuja_hud();
	cycle++;
      
   }
       if(totalCoins == 0) {
        //PrintText(100, 200, "¡Ganaste! Has recogido todas las monedas.");
    } else if(lives <= 0) {
        //PrintText(100, 200, "Game Over. Pac-Man se quedó sin vidas.");
    }
   return 0;
// -----------------------------------------
}   
