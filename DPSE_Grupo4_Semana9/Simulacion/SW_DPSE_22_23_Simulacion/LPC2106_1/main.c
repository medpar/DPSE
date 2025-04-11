// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Autor:  Jesús Manuel Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: main.c  Programa principal
// =======================================================================

#include "system.h"
#include "xpm.h"   // Para manejar los gráficos XPM

// -----------------------------------------------------------------------------
// Definición de XPMs (wall, back, coin, pacman_xpm, ghost_red) omitidos
// por brevedad, asumiendo que ya los has definido.
// -----------------------------------------------------------------------------

// Tamaño base de los tiles (definido en tus XPM)
#define TILE_BASE_W  16
#define TILE_BASE_H  16

// Factor de escala deseado
#define TILE_SCALE   1

// Calculamos el tamaño resultante de cada tile en pantalla
#define TILE_W   (TILE_BASE_W * TILE_SCALE)  // = 48
#define TILE_H   (TILE_BASE_H * TILE_SCALE)  // = 48

// Tamaño del "mapa" en tiles
#define MAP_W  10
#define MAP_H  8

// Valores para el array de mapa
#define TILE_WALL    'W'
#define TILE_BACK    'B'
#define TILE_COIN    'C'
#define TILE_PACMAN  'P'
#define TILE_GHOST   'G'

// Mapa en texto, 10 columnas × 8 filas
static char level_map[MAP_H][MAP_W+1] = {
  "WWWWWWWWWW",
  "W   C    W",
  "W   C    W",
  "W  PC    W",
  "W   C G  W",
  "W   C    W",
  "W        W",
  "WWWWWWWWWW"
};

// Posiciones de Pac-Man
static int pacman_x = 3;  // col
static int pacman_y = 3;  // row

// -----------------------------------------------------------------------------
// Declaración de funciones
// -----------------------------------------------------------------------------
int GetAction(); // Lectura de botones

// Dibuja un tile en la pantalla, dada su posición [row,col]
void DrawTile(int row, int col, char tile)
{
    // Calcula coordenadas en píxeles, escaladas
    int px = col * TILE_W;  // col * 48
    int py = row * TILE_H;  // row * 48

    switch(tile)
    {
      case TILE_WALL:
        XPM_PintaAt(px, py, 0, wall);
        break;
      case TILE_COIN:
        XPM_PintaAt(px, py, 0, coin);
        break;
      case TILE_PACMAN:
        XPM_PintaAt(px, py, 0, pacman_xpm);
        break;
      case TILE_GHOST:
        XPM_PintaAt(px, py, 0, ghost_red);
        break;
      case TILE_BACK:
      default:
        XPM_PintaAt(px, py, 0, back);
        break;
    }
}

// Dibuja todo el mapa en pantalla
void DrawMap(void)
{
  int r, c;
  for(r=0; r<MAP_H; r++)
  {
    for(c=0; c<MAP_W; c++)
    {
       char tile = level_map[r][c];
       if(tile==' ')
         tile = TILE_BACK;
       DrawTile(r, c, tile);
    }
  }
}

// Verifica si la siguiente posición es pared
int IsWall(int row, int col)
{
  if(row<0 || row>=MAP_H || col<0 || col>=MAP_W) return 1; // fuera de límites => pared
  if(level_map[row][col] == 'W') return 1;
  return 0;
}

// Mueve a Pac-Man según la acción de los botones
void MovePacman(int action)
{
  int new_row = pacman_y;
  int new_col = pacman_x;

  // Ajusta estos bits a tu conveniencia
  // p.ej. asumiendo (1<<0)->UP, (1<<1)->LEFT, (1<<2)->RIGHT, (1<<3)->DOWN
  if(action & (1<<0)) new_row--;  // UP
  if(action & (1<<1)) new_col--;  // LEFT
  if(action & (1<<2)) new_col++;  // RIGHT
  if(action & (1<<3)) new_row++;  // DOWN

  // Comprueba si no es pared
  if(!IsWall(new_row,new_col))
  {
    // Si había moneda
    if(level_map[new_row][new_col] == 'C')
    {
      // puntuación++;
    }
    // Elimina el pacman de su posición anterior
    level_map[pacman_y][pacman_x] = ' ';
    // Actualiza a la nueva
    pacman_y = new_row;
    pacman_x = new_col;
    level_map[pacman_y][pacman_x] = 'P';
  }
}

// -----------------------------------------------------------------------------
// main()
// -----------------------------------------------------------------------------

int main(void)
{
   int x, y;

   // Config inicial
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

   // Borra la pantalla
   TFT_DrawFillSquareS(0, 0, LENX, LENY, TFT_Color(0,0,0));

   // Carga los XPM en la paleta
   XPM_Extract(wall);
   XPM_Extract(back);
   XPM_Extract(coin);
   XPM_Extract(pacman_xpm);
   XPM_Extract(ghost_red);

   // Ajusta la escala en tu driver XPM para expandir de 16×16 a 48×48
   xpm_sx = 16; xpm_sy = 16;
   XPM_SetxNyN(TILE_SCALE, TILE_SCALE);

   // Dibuja el mapa inicial
   DrawMap();

   // Activa chip select para la SD / panel táctil si procede
   CS_ON;
   FIOSET = SD_CS | TP_CS;

   _printf("\r\nReady. Use buttons to move Pac-Man.\n");

   while(1)
   {
       // Mueve Pac-Man
       int action = GetAction();
       MovePacman(action);

       // Re-dibuja mapa completo
       DrawMap();

       // Por si quieres uso del táctil
       if(TP_Get_XY(&x, &y))
       {
           TFT_FillCircle(x, y, 5, TFT_Color(255,255,0));
       }

       _delay_ms(100);
   }
   return 0;
}
