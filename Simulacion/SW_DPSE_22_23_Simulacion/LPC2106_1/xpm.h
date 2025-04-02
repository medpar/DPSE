// =======================================================================
// Proyecto VIDEOCONSOLA Curso 2022-2023
// Fecha: 2023/03/21
// Autor: Jes�s M. Hern�ndez Mangas
// Asignatura: Desarrollo Pr�ctico de Sistemas Electr�nicos
// ----------------------------------------------------------------------
// mapa de bits de los sprites en formato gr�fico xpm
extern char * bloques[]; //
extern unsigned short paleta[96];
extern int xpm_ancho, xpm_alto, xpm_ncol, xpm_cpp;
extern int xpm_sx, xpm_sy; // Tama�o en pixel de los sprites
extern int xpm_xn;
extern int xpm_yn;
extern int xpm_xmirror;
// ----------------------------------------------------------------------
void XPM_Extract(char ** xpm);
void XPM_PintaAll(int x, int y, char** xpm);
void XPM_PintaAt(int x, int y, int sprite, char** xpm);
void XPM_PintaAtxNyN(int x, int y, int sprite, char** xpm);
void XPM_SetxNyN(int xn, int yn);
