// =======================================================================
// Proyecto 
// Fecha: 2024/03/21
// Autor: Jesús M. Hernández Mangas
// Asignatura: Desarrollo Práctico de Sistemas Electrónicos
// File: xpt2046.h  Rutinas para manejar el controlador del TouchPannel
// =======================================================================
#ifndef __XPT2046_H__
#define __XPT2046_H__
void TP_Init();
unsigned char TP_EnviaRecibe(unsigned char d);
int TP_Get_XY(int *x, int *y);
int TP_Get_XY_Cal(int *x, int *y);
void TP_Calibrate();
#endif