// =======================================================================
// Proyecto 
// Fecha: 2023/03/21
// Autor: Jesús M. Hernández Mangas
// Asignatura: Ingeniería de Sistemas Electrónicos
// File: NT35510.c Rutinas para manejar el controlador del TFT
// =======================================================================
#include <string.h>
#include "system.h"
int LENX, LENY;
// ---------------------------------------------------
inline void NT35510_Cmd( unsigned short cmd)
{
 //RS_OFF; // Comando
 FIOCLR = (0xFFFF <<NT35510_DT) | NT35510_RS;
 FIOSET = (cmd    <<NT35510_DT);
 WR_OFF;

 WR_ON;
}
// ---------------------------------------------------
//inline
void NT35510_Data( unsigned short data)
{
 //RS_ON; // Data
 FIOCLR = (0xFFFF<<NT35510_DT);
 FIOSET = (data  <<NT35510_DT) | NT35510_RS;
 WR_OFF;

 WR_ON; 
}

// ---------------------------------------------------
//inline 
void NT35510_Data_( unsigned short data)
{// Faster version
 //RS_ON; // Data
 FIOCLR = (0xFFFF<<NT35510_DT);
 FIOSET = (data  <<NT35510_DT);
 WR_OFF;

 WR_ON;
}
// ---------------------------------------------------
void NT35510_Reg( unsigned short cmd, unsigned short data)
{
 RS_OFF; // Comando
 FIOCLR = (0xFFFF <<NT35510_DT);
 FIOSET = (cmd    <<NT35510_DT);
 WR_OFF;

 WR_ON;
 RS_ON; // Data
 FIOCLR = (0xFFFF<<NT35510_DT);
 FIOSET = (data  <<NT35510_DT);
 WR_OFF;

 WR_ON; 
}

// ---------------------------------------------------
inline void TFT_SetArea(int x0, int y0, int x1, int y1 )
{
 NT35510_Cmd( CMD_SET_COLUMN  );  NT35510_Data(  x0>>8 ); // Start Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+1);  NT35510_Data(  x0    ); // Start Column Low  byte
 NT35510_Cmd( CMD_SET_COLUMN+2);  NT35510_Data(  x1>>8 ); // End   Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+3);  NT35510_Data(  x1    ); // End   Column Low  byte
 NT35510_Cmd( CMD_SET_PAGE   );   NT35510_Data( y0>>8 ); // Start Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+1 );   NT35510_Data( y0    ); // Start Row    Low  byte
 NT35510_Cmd( CMD_SET_PAGE+2 );   NT35510_Data( y1>>8 ); // End   Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+3 );   NT35510_Data( y1    ); // End   Row    Low  byte
 NT35510_Cmd( CMD_WR_MEMSTART );
}
// ---------------------------------------------------
inline void TFT_SetAreaR(int x0, int y0, int x1, int y1 )
{
 NT35510_Cmd( CMD_SET_COLUMN  );  NT35510_Data(  x0>>8 ); // Start Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+1);  NT35510_Data(  x0    ); // Start Column Low  byte
 NT35510_Cmd( CMD_SET_COLUMN+2);  NT35510_Data(  x1>>8 ); // End   Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+3);  NT35510_Data(  x1    ); // End   Column Low  byte
 NT35510_Cmd( CMD_SET_PAGE   );   NT35510_Data( y0>>8 ); // Start Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+1 );   NT35510_Data( y0    ); // Start Row    Low  byte
 NT35510_Cmd( CMD_SET_PAGE+2 );   NT35510_Data( y1>>8 ); // End   Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+3 );   NT35510_Data( y1    ); // End   Row    Low  byte
 NT35510_Cmd( CMD_RD_MEMSTART );
}
// ---------------------------------------------------
inline unsigned short NT35510_ReadCMD( unsigned short cmd )
{
 unsigned short data;
 CS_OFF;
 RS_OFF; // CMD
 FIOCLR = (0xFFFF << NT35510_DT);
 FIOSET = (cmd    << NT35510_DT);
 WR_OFF;
   asm("nop");
 WR_ON;

 RS_ON; // DATA
 DT_INPUT;
 _delay_us(10);
 RD_OFF;
   asm("nop");
   data = (FIOPIN >> NT35510_DT) & 0xFFFF;
 RD_ON;

 DT_OUTPUT;
 CS_ON;
 return data;
}
// ---------------------------------------------------
void TFT_Init()
{
 FIODIR = FIODIR | NT35510_CS | NT35510_RS | NT35510_WR | NT35510_RD |
	 (0xFFFF << NT35510_DT);
 CS_OFF;
 
 //************* NT35510**********//	
 NT35510_Reg(0xF000, 0x55);  //#Enable Page1
 NT35510_Reg(0xF001, 0xAA);
 NT35510_Reg(0xF002, 0x52);
 NT35510_Reg(0xF003, 0x08);
 NT35510_Reg(0xF004, 0x01);
 //# AVDD: manual
 NT35510_Reg(0xB600, 0x34);
 NT35510_Reg(0xB601, 0x34);
 NT35510_Reg(0xB602, 0x34);
 
 NT35510_Reg(0xB000, 0x0D);//09
 NT35510_Reg(0xB001, 0x0D);
 NT35510_Reg(0xB002, 0x0D);
 //# AVEE: manual -6V
 NT35510_Reg(0xB700, 0x24);
 NT35510_Reg(0xB701, 0x24);
 NT35510_Reg(0xB702, 0x24);
                    
 NT35510_Reg(0xB100, 0x0D);
 NT35510_Reg(0xB101, 0x0D);
 NT35510_Reg(0xB102, 0x0D);
 //#Power Control for
 //VCL
 NT35510_Reg(0xB800, 0x24);
 NT35510_Reg(0xB801, 0x24);
 NT35510_Reg(0xB802, 0x24);
                   
 NT35510_Reg(0xB200, 0x00);
 
 //# VGH: Clamp Enable
 NT35510_Reg(0xB900, 0x24);
 NT35510_Reg(0xB901, 0x24);
 NT35510_Reg(0xB902, 0x24);
                   
 NT35510_Reg(0xB300, 0x05);
 NT35510_Reg(0xB301, 0x05);
 NT35510_Reg(0xB302, 0x05);
 
 ///NT35510_Reg(0xBF00, 0x01);
 
 //# VGL(LVGL):
 NT35510_Reg(0xBA00, 0x34);
 NT35510_Reg(0xBA01, 0x34);
 NT35510_Reg(0xBA02, 0x34);
 //# VGL_REG(VGLO)
 NT35510_Reg(0xB500, 0x0B);
 NT35510_Reg(0xB501, 0x0B);
 NT35510_Reg(0xB502, 0x0B);
 //# VGMP/VGSP:
 NT35510_Reg(0xBC00, 0X00);
 NT35510_Reg(0xBC01, 0xA3);
 NT35510_Reg(0xBC02, 0X00);
 //# VGMN/VGSN
 NT35510_Reg(0xBD00, 0x00);
 NT35510_Reg(0xBD01, 0xA3);
 NT35510_Reg(0xBD02, 0x00);
 //# VCOM=-0.1
 NT35510_Reg(0xBE00, 0x00);
 NT35510_Reg(0xBE01, 0x63);//4f
 //  VCOMH+0x01;
 //#R+
 NT35510_Reg(0xD100, 0x00);
 NT35510_Reg(0xD101, 0x37);
 NT35510_Reg(0xD102, 0x00);
 NT35510_Reg(0xD103, 0x52);
 NT35510_Reg(0xD104, 0x00);
 NT35510_Reg(0xD105, 0x7B);
 NT35510_Reg(0xD106, 0x00);
 NT35510_Reg(0xD107, 0x99);
 NT35510_Reg(0xD108, 0x00);
 NT35510_Reg(0xD109, 0xB1);
 NT35510_Reg(0xD10A, 0x00);
 NT35510_Reg(0xD10B, 0xD2);
 NT35510_Reg(0xD10C, 0x00);
 NT35510_Reg(0xD10D, 0xF6);
 NT35510_Reg(0xD10E, 0x01);
 NT35510_Reg(0xD10F, 0x27);
 NT35510_Reg(0xD110, 0x01);
 NT35510_Reg(0xD111, 0x4E);
 NT35510_Reg(0xD112, 0x01);
 NT35510_Reg(0xD113, 0x8C);
 NT35510_Reg(0xD114, 0x01);
 NT35510_Reg(0xD115, 0xBE);
 NT35510_Reg(0xD116, 0x02);
 NT35510_Reg(0xD117, 0x0B);
 NT35510_Reg(0xD118, 0x02);
 NT35510_Reg(0xD119, 0x48);
 NT35510_Reg(0xD11A, 0x02);
 NT35510_Reg(0xD11B, 0x4A);
 NT35510_Reg(0xD11C, 0x02);
 NT35510_Reg(0xD11D, 0x7E);
 NT35510_Reg(0xD11E, 0x02);
 NT35510_Reg(0xD11F, 0xBC);
 NT35510_Reg(0xD120, 0x02);
 NT35510_Reg(0xD121, 0xE1);
 NT35510_Reg(0xD122, 0x03);
 NT35510_Reg(0xD123, 0x10);
 NT35510_Reg(0xD124, 0x03);
 NT35510_Reg(0xD125, 0x31);
 NT35510_Reg(0xD126, 0x03);
 NT35510_Reg(0xD127, 0x5A);
 NT35510_Reg(0xD128, 0x03);
 NT35510_Reg(0xD129, 0x73);
 NT35510_Reg(0xD12A, 0x03);
 NT35510_Reg(0xD12B, 0x94);
 NT35510_Reg(0xD12C, 0x03);
 NT35510_Reg(0xD12D, 0x9F);
 NT35510_Reg(0xD12E, 0x03);
 NT35510_Reg(0xD12F, 0xB3);
 NT35510_Reg(0xD130, 0x03);
 NT35510_Reg(0xD131, 0xB9);
 NT35510_Reg(0xD132, 0x03);
 NT35510_Reg(0xD133, 0xC1);
 //#G+   
 NT35510_Reg(0xD200, 0x00);
 NT35510_Reg(0xD201, 0x37);
 NT35510_Reg(0xD202, 0x00);
 NT35510_Reg(0xD203, 0x52);
 NT35510_Reg(0xD204, 0x00);
 NT35510_Reg(0xD205, 0x7B);
 NT35510_Reg(0xD206, 0x00);
 NT35510_Reg(0xD207, 0x99);
 NT35510_Reg(0xD208, 0x00);
 NT35510_Reg(0xD209, 0xB1);
 NT35510_Reg(0xD20A, 0x00);
 NT35510_Reg(0xD20B, 0xD2);
 NT35510_Reg(0xD20C, 0x00);
 NT35510_Reg(0xD20D, 0xF6);
 NT35510_Reg(0xD20E, 0x01);
 NT35510_Reg(0xD20F, 0x27);
 NT35510_Reg(0xD210, 0x01);
 NT35510_Reg(0xD211, 0x4E);
 NT35510_Reg(0xD212, 0x01);
 NT35510_Reg(0xD213, 0x8C);
 NT35510_Reg(0xD214, 0x01);
 NT35510_Reg(0xD215, 0xBE);
 NT35510_Reg(0xD216, 0x02);
 NT35510_Reg(0xD217, 0x0B);
 NT35510_Reg(0xD218, 0x02);
 NT35510_Reg(0xD219, 0x48);
 NT35510_Reg(0xD21A, 0x02);
 NT35510_Reg(0xD21B, 0x4A);
 NT35510_Reg(0xD21C, 0x02);
 NT35510_Reg(0xD21D, 0x7E);
 NT35510_Reg(0xD21E, 0x02);
 NT35510_Reg(0xD21F, 0xBC);
 NT35510_Reg(0xD220, 0x02);
 NT35510_Reg(0xD221, 0xE1);
 NT35510_Reg(0xD222, 0x03);
 NT35510_Reg(0xD223, 0x10);
 NT35510_Reg(0xD224, 0x03);
 NT35510_Reg(0xD225, 0x31);
 NT35510_Reg(0xD226, 0x03);
 NT35510_Reg(0xD227, 0x5A);
 NT35510_Reg(0xD228, 0x03);
 NT35510_Reg(0xD229, 0x73);
 NT35510_Reg(0xD22A, 0x03);
 NT35510_Reg(0xD22B, 0x94);
 NT35510_Reg(0xD22C, 0x03);
 NT35510_Reg(0xD22D, 0x9F);
 NT35510_Reg(0xD22E, 0x03);
 NT35510_Reg(0xD22F, 0xB3);
 NT35510_Reg(0xD230, 0x03);
 NT35510_Reg(0xD231, 0xB9);
 NT35510_Reg(0xD232, 0x03);
 NT35510_Reg(0xD233, 0xC1);
 //#B+   
 NT35510_Reg(0xD300, 0x00);
 NT35510_Reg(0xD301, 0x37);
 NT35510_Reg(0xD302, 0x00);
 NT35510_Reg(0xD303, 0x52);
 NT35510_Reg(0xD304, 0x00);
 NT35510_Reg(0xD305, 0x7B);
 NT35510_Reg(0xD306, 0x00);
 NT35510_Reg(0xD307, 0x99);
 NT35510_Reg(0xD308, 0x00);
 NT35510_Reg(0xD309, 0xB1);
 NT35510_Reg(0xD30A, 0x00);
 NT35510_Reg(0xD30B, 0xD2);
 NT35510_Reg(0xD30C, 0x00);
 NT35510_Reg(0xD30D, 0xF6);
 NT35510_Reg(0xD30E, 0x01);
 NT35510_Reg(0xD30F, 0x27);
 NT35510_Reg(0xD310, 0x01);
 NT35510_Reg(0xD311, 0x4E);
 NT35510_Reg(0xD312, 0x01);
 NT35510_Reg(0xD313, 0x8C);
 NT35510_Reg(0xD314, 0x01);
 NT35510_Reg(0xD315, 0xBE);
 NT35510_Reg(0xD316, 0x02);
 NT35510_Reg(0xD317, 0x0B);
 NT35510_Reg(0xD318, 0x02);
 NT35510_Reg(0xD319, 0x48);
 NT35510_Reg(0xD31A, 0x02);
 NT35510_Reg(0xD31B, 0x4A);
 NT35510_Reg(0xD31C, 0x02);
 NT35510_Reg(0xD31D, 0x7E);
 NT35510_Reg(0xD31E, 0x02);
 NT35510_Reg(0xD31F, 0xBC);
 NT35510_Reg(0xD320, 0x02);
 NT35510_Reg(0xD321, 0xE1);
 NT35510_Reg(0xD322, 0x03);
 NT35510_Reg(0xD323, 0x10);
 NT35510_Reg(0xD324, 0x03);
 NT35510_Reg(0xD325, 0x31);
 NT35510_Reg(0xD326, 0x03);
 NT35510_Reg(0xD327, 0x5A);
 NT35510_Reg(0xD328, 0x03);
 NT35510_Reg(0xD329, 0x73);
 NT35510_Reg(0xD32A, 0x03);
 NT35510_Reg(0xD32B, 0x94);
 NT35510_Reg(0xD32C, 0x03);
 NT35510_Reg(0xD32D, 0x9F);
 NT35510_Reg(0xD32E, 0x03);
 NT35510_Reg(0xD32F, 0xB3);
 NT35510_Reg(0xD330, 0x03);
 NT35510_Reg(0xD331, 0xB9);
 NT35510_Reg(0xD332, 0x03);
 NT35510_Reg(0xD333, 0xC1);
 
 //#R-///////////////////////////////////////////
 NT35510_Reg(0xD400, 0x00);
 NT35510_Reg(0xD401, 0x37);
 NT35510_Reg(0xD402, 0x00);
 NT35510_Reg(0xD403, 0x52);
 NT35510_Reg(0xD404, 0x00);
 NT35510_Reg(0xD405, 0x7B);
 NT35510_Reg(0xD406, 0x00);
 NT35510_Reg(0xD407, 0x99);
 NT35510_Reg(0xD408, 0x00);
 NT35510_Reg(0xD409, 0xB1);
 NT35510_Reg(0xD40A, 0x00);
 NT35510_Reg(0xD40B, 0xD2);
 NT35510_Reg(0xD40C, 0x00);
 NT35510_Reg(0xD40D, 0xF6);
 NT35510_Reg(0xD40E, 0x01);
 NT35510_Reg(0xD40F, 0x27);
 NT35510_Reg(0xD410, 0x01);
 NT35510_Reg(0xD411, 0x4E);
 NT35510_Reg(0xD412, 0x01);
 NT35510_Reg(0xD413, 0x8C);
 NT35510_Reg(0xD414, 0x01);
 NT35510_Reg(0xD415, 0xBE);
 NT35510_Reg(0xD416, 0x02);
 NT35510_Reg(0xD417, 0x0B);
 NT35510_Reg(0xD418, 0x02);
 NT35510_Reg(0xD419, 0x48);
 NT35510_Reg(0xD41A, 0x02);
 NT35510_Reg(0xD41B, 0x4A);
 NT35510_Reg(0xD41C, 0x02);
 NT35510_Reg(0xD41D, 0x7E);
 NT35510_Reg(0xD41E, 0x02);
 NT35510_Reg(0xD41F, 0xBC);
 NT35510_Reg(0xD420, 0x02);
 NT35510_Reg(0xD421, 0xE1);
 NT35510_Reg(0xD422, 0x03);
 NT35510_Reg(0xD423, 0x10);
 NT35510_Reg(0xD424, 0x03);
 NT35510_Reg(0xD425, 0x31);
 NT35510_Reg(0xD426, 0x03);
 NT35510_Reg(0xD427, 0x5A);
 NT35510_Reg(0xD428, 0x03);
 NT35510_Reg(0xD429, 0x73);
 NT35510_Reg(0xD42A, 0x03);
 NT35510_Reg(0xD42B, 0x94);
 NT35510_Reg(0xD42C, 0x03);
 NT35510_Reg(0xD42D, 0x9F);
 NT35510_Reg(0xD42E, 0x03);
 NT35510_Reg(0xD42F, 0xB3);
 NT35510_Reg(0xD430, 0x03);
 NT35510_Reg(0xD431, 0xB9);
 NT35510_Reg(0xD432, 0x03);
 NT35510_Reg(0xD433, 0xC1);
 
 //#G-//////////////////////////////////////////////
 NT35510_Reg(0xD500, 0x00);
 NT35510_Reg(0xD501, 0x37);
 NT35510_Reg(0xD502, 0x00);
 NT35510_Reg(0xD503, 0x52);
 NT35510_Reg(0xD504, 0x00);
 NT35510_Reg(0xD505, 0x7B);
 NT35510_Reg(0xD506, 0x00);
 NT35510_Reg(0xD507, 0x99);
 NT35510_Reg(0xD508, 0x00);
 NT35510_Reg(0xD509, 0xB1);
 NT35510_Reg(0xD50A, 0x00);
 NT35510_Reg(0xD50B, 0xD2);
 NT35510_Reg(0xD50C, 0x00);
 NT35510_Reg(0xD50D, 0xF6);
 NT35510_Reg(0xD50E, 0x01);
 NT35510_Reg(0xD50F, 0x27);
 NT35510_Reg(0xD510, 0x01);
 NT35510_Reg(0xD511, 0x4E);
 NT35510_Reg(0xD512, 0x01);
 NT35510_Reg(0xD513, 0x8C);
 NT35510_Reg(0xD514, 0x01);
 NT35510_Reg(0xD515, 0xBE);
 NT35510_Reg(0xD516, 0x02);
 NT35510_Reg(0xD517, 0x0B);
 NT35510_Reg(0xD518, 0x02);
 NT35510_Reg(0xD519, 0x48);
 NT35510_Reg(0xD51A, 0x02);
 NT35510_Reg(0xD51B, 0x4A);
 NT35510_Reg(0xD51C, 0x02);
 NT35510_Reg(0xD51D, 0x7E);
 NT35510_Reg(0xD51E, 0x02);
 NT35510_Reg(0xD51F, 0xBC);
 NT35510_Reg(0xD520, 0x02);
 NT35510_Reg(0xD521, 0xE1);
 NT35510_Reg(0xD522, 0x03);
 NT35510_Reg(0xD523, 0x10);
 NT35510_Reg(0xD524, 0x03);
 NT35510_Reg(0xD525, 0x31);
 NT35510_Reg(0xD526, 0x03);
 NT35510_Reg(0xD527, 0x5A);
 NT35510_Reg(0xD528, 0x03);
 NT35510_Reg(0xD529, 0x73);
 NT35510_Reg(0xD52A, 0x03);
 NT35510_Reg(0xD52B, 0x94);
 NT35510_Reg(0xD52C, 0x03);
 NT35510_Reg(0xD52D, 0x9F);
 NT35510_Reg(0xD52E, 0x03);
 NT35510_Reg(0xD52F, 0xB3);
 NT35510_Reg(0xD530, 0x03);
 NT35510_Reg(0xD531, 0xB9);
 NT35510_Reg(0xD532, 0x03);
 NT35510_Reg(0xD533, 0xC1);
 //#B-///////////////////////////////
 NT35510_Reg(0xD600, 0x00);
 NT35510_Reg(0xD601, 0x37);
 NT35510_Reg(0xD602, 0x00);
 NT35510_Reg(0xD603, 0x52);
 NT35510_Reg(0xD604, 0x00);
 NT35510_Reg(0xD605, 0x7B);
 NT35510_Reg(0xD606, 0x00);
 NT35510_Reg(0xD607, 0x99);
 NT35510_Reg(0xD608, 0x00);
 NT35510_Reg(0xD609, 0xB1);
 NT35510_Reg(0xD60A, 0x00);
 NT35510_Reg(0xD60B, 0xD2);
 NT35510_Reg(0xD60C, 0x00);
 NT35510_Reg(0xD60D, 0xF6);
 NT35510_Reg(0xD60E, 0x01);
 NT35510_Reg(0xD60F, 0x27);
 NT35510_Reg(0xD610, 0x01);
 NT35510_Reg(0xD611, 0x4E);
 NT35510_Reg(0xD612, 0x01);
 NT35510_Reg(0xD613, 0x8C);
 NT35510_Reg(0xD614, 0x01);
 NT35510_Reg(0xD615, 0xBE);
 NT35510_Reg(0xD616, 0x02);
 NT35510_Reg(0xD617, 0x0B);
 NT35510_Reg(0xD618, 0x02);
 NT35510_Reg(0xD619, 0x48);
 NT35510_Reg(0xD61A, 0x02);
 NT35510_Reg(0xD61B, 0x4A);
 NT35510_Reg(0xD61C, 0x02);
 NT35510_Reg(0xD61D, 0x7E);
 NT35510_Reg(0xD61E, 0x02);
 NT35510_Reg(0xD61F, 0xBC);
 NT35510_Reg(0xD620, 0x02);
 NT35510_Reg(0xD621, 0xE1);
 NT35510_Reg(0xD622, 0x03);
 NT35510_Reg(0xD623, 0x10);
 NT35510_Reg(0xD624, 0x03);
 NT35510_Reg(0xD625, 0x31);
 NT35510_Reg(0xD626, 0x03);
 NT35510_Reg(0xD627, 0x5A);
 NT35510_Reg(0xD628, 0x03);
 NT35510_Reg(0xD629, 0x73);
 NT35510_Reg(0xD62A, 0x03);
 NT35510_Reg(0xD62B, 0x94);
 NT35510_Reg(0xD62C, 0x03);
 NT35510_Reg(0xD62D, 0x9F);
 NT35510_Reg(0xD62E, 0x03);
 NT35510_Reg(0xD62F, 0xB3);
 NT35510_Reg(0xD630, 0x03);
 NT35510_Reg(0xD631, 0xB9);
 NT35510_Reg(0xD632, 0x03);
 NT35510_Reg(0xD633, 0xC1);
 
 //#Enable Page0
 NT35510_Reg(0xF000, 0x55); // Enable access CMD2 (CMD_ENA1)
 NT35510_Reg(0xF001, 0xAA);
 NT35510_Reg(0xF002, 0x52);
 NT35510_Reg(0xF003, 0x08);
 NT35510_Reg(0xF004, 0x00);
 //# RGB I/F Setting
 NT35510_Reg(0xB000, 0x08); 
 NT35510_Reg(0xB001, 0x05);
 NT35510_Reg(0xB002, 0x02);
 NT35510_Reg(0xB003, 0x05);
 NT35510_Reg(0xB004, 0x02);
 //## SDT:
 NT35510_Reg(0xB600, 0x08);
 NT35510_Reg(0xB500, 0x50);//0x6b resolution 480x854, 0x50 resolution 480x800
 
 //## Gate EQ:
 NT35510_Reg(0xB700, 0x00);
 NT35510_Reg(0xB701, 0x00);
 
 //## Source EQ:
 NT35510_Reg(0xB800, 0x01);
 NT35510_Reg(0xB801, 0x05);
 NT35510_Reg(0xB802, 0x05);
 NT35510_Reg(0xB803, 0x05);
 
 //# Inversion: Column inversion (NVT)
 NT35510_Reg(0xBC00, 0x00);
 NT35510_Reg(0xBC01, 0x00);
 NT35510_Reg(0xBC02, 0x00);
 
 //# BOE's Setting(default)
 NT35510_Reg(0xCC00, 0x03);
 NT35510_Reg(0xCC01, 0x00);
 NT35510_Reg(0xCC02, 0x00);
 
 //# Display Timing:
 NT35510_Reg(0xBD00, 0x01); // ??
 NT35510_Reg(0xBD01, 0x84);
 NT35510_Reg(0xBD02, 0x07);
 NT35510_Reg(0xBD03, 0x31);
 NT35510_Reg(0xBD04, 0x00);
 
 NT35510_Reg(0xBA00, 0x01);
 
 NT35510_Reg(0xFF00, 0xAA); // Enable access CMD2 (CMD_ENA1)
 NT35510_Reg(0xFF01, 0x55);
 NT35510_Reg(0xFF02, 0x25);
 NT35510_Reg(0xFF03, 0x01);
 
 NT35510_Reg(0x3500, 0x00); // Tearing effect mode off
 NT35510_Reg(0x3a00, 0x55); // Interface pixel format (0x55, 16bit) (0x66, 18bit)
 NT35510_Cmd(0x1100); // Sleep out and booster on
 _delay_ms(120); 
 NT35510_Cmd(0x2900 ); // Display ON
 int direction = 3; // To match our console orientation
 int LCD_W = 480;
 int LCD_H = 800;
 switch(direction)
 {// Memory Data Access Control 0x3600
  case 0: LENX = LCD_W; LENY = LCD_H; NT35510_Reg(0x3600,          0x00);	break;
  case 1: LENX = LCD_H; LENY = LCD_W; NT35510_Reg(0x3600, (1<<5)|(1<<6));	break;
  case 2: LENX = LCD_W; LENY = LCD_H; NT35510_Reg(0x3600, (1<<7)|(1<<6));	break;
  case 3: LENX = LCD_H; LENY = LCD_W; NT35510_Reg(0x3600, (1<<7)|(1<<5));	break;
 }	
/*
 unsigned short val;
 NT35510_Reg(0xF000, 0x55); // Enable access CMD2 (CMD_ENA1)  //#Enable Page1
 NT35510_Reg(0xF001, 0xAA);
 NT35510_Reg(0xF002, 0x52);
 NT35510_Reg(0xF003, 0x08);
 NT35510_Reg(0xF004, 0x01);
 val  = NT35510_ReadCMD(0xC500)<<8;
 val |= NT35510_ReadCMD(0xC501);
 _printf("TFT ID = 0x%04x\n", val);
*/
 CS_ON;
}
// ---------------------------------------------------
unsigned short FG_Color = 0xFFFF;
unsigned short BG_Color = 0x0000;
// ---------------------------------------------------
void TFT_FillArea(unsigned short x0, unsigned short y0,
		       unsigned short x1, unsigned short y1 )
{int i;
 CS_OFF;
 TFT_SetArea( x0, y0, x1, y1 );
 for(i=0;i<(x1-x0+1)*(y1-y0+1);i++) NT35510_Data(FG_Color);
 CS_ON;
}
// ---------------------------------------------------
inline void TFT_PutPixel(int x0, int y0)
{    
 //if(x0<0) return;
 if(x0<0 || x0>LENX-1 || y0<0 || y0>LENY-1) return;
 CS_OFF;
 //TFT_SetArea(x0, y0, LENX-1, LENY-1 );
 //TFT_SetArea(x0, y0, x0+1, y0+1);
 
 NT35510_Cmd( CMD_SET_COLUMN  );  NT35510_Data(  x0>>8 ); // Start Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+1);  NT35510_Data(  x0    ); // Start Column Low  byte
// NT35510_Cmd( CMD_SET_COLUMN+2);  NT35510_Data(  (x0+1)>>8 ); // End   Column High byte
// NT35510_Cmd( CMD_SET_COLUMN+3);  NT35510_Data(  x0+1    ); // End   Column Low  byte
 NT35510_Cmd( CMD_SET_PAGE   );   NT35510_Data( y0>>8 ); // Start Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+1 );   NT35510_Data( y0    ); // Start Row    Low  byte
// NT35510_Cmd( CMD_SET_PAGE+2 );   NT35510_Data( (y0+1)>>8 ); // End   Row    High byte
// NT35510_Cmd( CMD_SET_PAGE+3 );   NT35510_Data( y0+1    ); // End   Row    Low  byte
 NT35510_Cmd( CMD_WR_MEMSTART );
 
 NT35510_Data(FG_Color);
 CS_ON;
}
// ---------------------------------------------------
inline void TFT_PutPixelColor(unsigned short x0, unsigned short y0, unsigned short color)
{
 //if(x0<0) return;
 if(x0<0 || x0>LENX-1 || y0<0 || y0>LENY-1) return;
 CS_OFF;
 //TFT_SetArea(x0, y0, LENX-1, LENY-1 );
 //TFT_SetArea(x0, y0, x0+1, y0+1);
 
 NT35510_Cmd( CMD_SET_COLUMN  );  NT35510_Data(  x0>>8 ); // Start Column High byte
 NT35510_Cmd( CMD_SET_COLUMN+1);  NT35510_Data(  x0    ); // Start Column Low  byte
// NT35510_Cmd( CMD_SET_COLUMN+2);  NT35510_Data(  (x0+1)>>8 ); // End   Column High byte
// NT35510_Cmd( CMD_SET_COLUMN+3);  NT35510_Data(  x0+1    ); // End   Column Low  byte
 NT35510_Cmd( CMD_SET_PAGE   );   NT35510_Data( y0>>8 ); // Start Row    High byte
 NT35510_Cmd( CMD_SET_PAGE+1 );   NT35510_Data( y0    ); // Start Row    Low  byte
// NT35510_Cmd( CMD_SET_PAGE+2 );   NT35510_Data( (y0+1)>>8 ); // End   Row    High byte
// NT35510_Cmd( CMD_SET_PAGE+3 );   NT35510_Data( y0+1    ); // End   Row    Low  byte
 NT35510_Cmd( CMD_WR_MEMSTART );
 
 NT35510_Data(color);
 CS_ON;
}
// ---------------------------------------------------
void TFT_DrawLine(int x1, int y1, int x2, int y2, int color)
{ // Bresenham's algorithm
 unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
 short			xstep =  x2 > x1 ? 1 : -1;
 unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
 short			ystep =  y2 > y1 ? 1 : -1;
 int			col = x1, row = y1;

 FG_Color = color;
 if (dx < dy)
 {
  int t = - (dy >> 1);
  while(1)
  {

   TFT_PutPixel(col,row);
   if (row == y2) return;
   row += ystep;
   t += dx;
   if (t >= 0){ col += xstep; t -= dy; }
  }
 }
 else
 {
  int t = - (dx >> 1);
  while(1)
  {
   TFT_PutPixel(col,row);
   if (col == x2) return;
   col += xstep;
   t += dy;
   if (t >= 0){	row += ystep; t -= dx; }
  }
 }
}
//------------------------------------------------------------------
void TFT_HorizontalLine(int x1, int x2, int y, int color)
{
 int i, n;
 if(y>LENY-1) return;
 CS_OFF;
 TFT_SetArea( x1, y, x2-1, y );
 RS_ON;
 if(x1<x2) n = (x2-x1); else n = (x1-x2);
 for(i=0;i<n;i++) NT35510_Data_(color);
 CS_ON;
}
//------------------------------------------------------------------
void TFT_VerticalLine(int x, int y1, int y2, int color)
{
 int i, n;
 CS_OFF;
 TFT_SetArea( x, y1, x, y2-1 );
 RS_ON;
 if(y1<y2) n = (y2-y1); else n = (y1-y2);
 for(i=0;i<n;i++) NT35510_Data_(color);
 CS_ON;
}
//------------------------------------------------------------------
void TFT_DrawSquare(int x1, int y1, int x2, int y2, int color)
{
 TFT_DrawLine(x1,y1,x1,y2,color);
 TFT_DrawLine(x1,y2,x2,y2,color);
 TFT_DrawLine(x2,y2,x2,y1,color);
 TFT_DrawLine(x2,y1,x1,y1,color);
}
// -----------------------------------------------------------------------
void TFT_DrawCircle(int x, int y, int radius, int color)
{ // Bresenham's algorithm
 int f = 1 - radius;
 int ddF_x = 1;
 int ddF_y = -2 * radius;
 int x1 = 0;
 int y1 = radius;

 FG_Color = color;
 TFT_PutPixel(x, y+radius);
 TFT_PutPixel(x, y-radius);
 TFT_PutPixel(x+radius, y);
 TFT_PutPixel(x-radius, y);
 while(x1 < y1)
 {
  if(f >= 0)
  {
	y1--;
	ddF_y += 2;
	f += ddF_y;
  }
  x1++;
  ddF_x += 2;
  f += ddF_x;

  TFT_PutPixel(x+x1, y+y1);
  TFT_PutPixel(x-x1, y+y1);
  TFT_PutPixel(x+x1, y-y1);
  TFT_PutPixel(x-x1, y-y1);

  TFT_PutPixel(x+y1, y+x1);
  TFT_PutPixel(x-y1, y+x1);
  TFT_PutPixel(x+y1, y-x1);
  TFT_PutPixel(x-y1, y-x1);
 }
}
// -----------------------------------------------------------------------
void TFT_FillCircle(int x, int y, int radius, int color)
{
 int y1,x1;

 for(y1=-radius; y1<=0; y1++)
	for(x1=-radius; x1<=0; x1++)
		if(x1*x1+y1*y1 <= radius*radius)
		{
			TFT_HorizontalLine(x+x1, x-x1, y+y1, color);
			TFT_HorizontalLine(x+x1, x-x1, y-y1, color);
			break;
		}
}
// -----------------------------------------------------------------------
void TFT_DrawFillSquare(int x1, int y1, int x2, int y2, int color)
{
 int i,n;
 CS_OFF;
 TFT_SetArea( x1, y1, x2, y2 );
 RS_ON;
 n = (x2-x1+1)*(y2-y1+1);
 for(i=0;i<n;i++) NT35510_Data_(color);
 CS_ON;
}
// -----------------------------------------------------------------------
void TFT_DrawFillSquareS(int x1, int y1, int sx, int sy, int color)
{
 int i,n;
 CS_OFF;
 TFT_SetArea( x1, y1, x1+sx-1, y1+sy-1 );
 RS_ON;
 n = (sx)*(sy);
 for(i=0;i<n;i++) NT35510_Data_(color);
 CS_ON;
}
// -----------------------------------------------------------------------
struct _current_font *CF = (struct _current_font*) BigFont;
// -----------------------------------------------------------------------
void TFT_DumpBinaryFont()
{
 int i, j, k, b, d;	
 _printf("//------- \n");
 _printf("const unsigned char Font[] ={\n");
 _printf(" %d,%d, // sx, sy\n", CF->x_size, CF->y_size); 
 _printf(" %d,    // x_step\n", CF->x_step);
 _printf(" '%c',   // first_char\n", CF->offset);
 _printf(" %3d,  // n_chars\n", CF->numchars);
 for(i=0; i<CF->numchars; i++)
 {
  _printf("// Character %3d '%c'\n", i+CF->offset, i+CF->offset);	 
  for(j=0;j<CF->y_size;j++)
  {
	//_printf(" 0b");
	
	for(k=0;k<CF->x_size>>3;k++)
	{
	 _printf(" 0b");
	 d = CF->font[((i*CF->x_size*CF->y_size)>>3) +((j*CF->x_size)>>3) + k];
	 for(b=0;b<8;b++)
	 {		
	  if(d&0x80) _printf("1");
	  else       _printf("0");
	  d = d << 1;
	 }	 
	 _printf(",");
	}
	_printf("\n");
  }	  
  
 }
 _printf("};\n//------- \n");
}

/*
void TFT_putch_old(int x1, int y1, unsigned char c)
{
 int x, y, j, d, org, xx, yy;

 //y1 = y1 + CF->y_size;
 org = (c - CF->offset)*CF->x_size*CF->y_size>>3;
 for(y=0;y<CF->y_size;y++)
 {
  yy = y1+y;
  for(x=0;x<CF->x_size>>3;x++)
  {
   xx = x1 + (x<<3);
   d = CF->font[ org + ((y*CF->x_size)>>3) + x ];
   for(j=0;j<8;j++)
   {
    if( d&0x80 ) TFT_PutPixelColor( xx+j, yy, FG_Color); // Color primer plano
    else         TFT_PutPixelColor( xx+j, yy, BG_Color); // Color fondo
    d = (d<<1);
   }
  }
 }
}
*/
// -----------------------------------------------------------------------
void TFT_putch(int x1, int y1, unsigned char c)
{ // Faster version (now 197 ut, before 2312 ut)
 int j, d, org, i, n;

 n = CF->x_size*CF->y_size>>3;
 org = (c - CF->offset)*n;
CS_OFF;
 TFT_SetArea(x1,y1,x1+CF->x_size-1,y1+CF->y_size-1);
 RS_ON; // Data

 for(i=0;i<n;i++)
 {
   d = CF->font[ org++ ];
   for(j=0;j<8;j++)
   {
    if(d&0x80) NT35510_Data_(FG_Color);
    else       NT35510_Data_(BG_Color);
    d = (d<<1);
   }
 }
CS_ON; 
}
// -----------------------------------------------------------------------
void TFT_print(int x1, int y1, char * s)
{
 int i=0;
 while(s[i]){ TFT_putch(x1+i*CF->x_step, y1,s[i]);i++;}
}
// -----------------------------------------------------------------------
void TFT_print_centered_x(int y1, char * s)
{
 int i=0, x1;
 x1 = LENX/2 - (strlen(s)*CF->x_step)/2;
 while(s[i]){ TFT_putch(x1+i*CF->x_step, y1,s[i]);i++;}
}
// -----------------------------------------------------------------------
int tft_x, tft_y;
void TFT_putchar(unsigned char c)
{
 TFT_putch(tft_x, tft_y, c);	
 tft_x += CF->x_step;
}
// -----------------------------------------------------------------------
/* // Definido en ssd1963.h MACRO variadica
#define TFT_printf(...) \
   tft_x = x;\
   tft_y = y;\
   _vputch =(void*)TFT_putchar;\
   _printf(__VA_ARGS__);\
   _vputch =(void*)_U0putch;
*/
// -----------------------------------------------------------------------
#ifdef TFT_PRINT_xN_yN
int yN = 2;
int xN = 2;

void TFT_putch_xNyN(int x1, int y1, unsigned char c)
{
 int x, y, j, d, org, xx, yy, ix, iy;

 org = ((c - CF->offset)*CF->x_size*CF->y_size)>>3;
 for(y=0,iy=0;y<(CF->y_size*yN);y+=yN,iy++)
 {
  yy = y1+y;
  for(x=0,ix=0;x<(CF->x_size>>3)*xN;x+=xN,ix++)
  {
   xx = x1 + (x<<3);
   d = CF->font[ org + ((iy*CF->x_size)>>3) + ix ];
   for(j=0;j<8;j++)
   {
    if( d&0x80 ) 
       TFT_DrawFillSquareS( xx+j*xN, yy, xN, yN, FG_Color); // Color primer plano
    else
       TFT_DrawFillSquareS( xx+j*xN, yy, xN, yN, BG_Color); // Color fondo
    d = (d<<1);
   }
  }
 }
}
// -----------------------------------------------------------------------
void TFT_putchar_xNyN(unsigned char c)
{
 TFT_putch_xNyN(tft_x, tft_y, c);
 tft_x += CF->x_step*xN;
}
// -----------------------------------------------------------------------
void TFT_print_xNyN(int x1, int y1, char * s)
{
 int i=0;
 while(s[i]){ TFT_putch_xNyN(x1+(i*CF->x_step)*xN, y1,s[i]);i++;}
}
// -----------------------------------------------------------------------
int TFT_print_centered_x_xNyN(int y1, char * s)
{
 int i=0, x1;
 x1 = LENX/2 - (strlen(s)*CF->x_step*xN)/2;
 while(s[i]){ TFT_putch_xNyN(x1+(i*CF->x_step)*xN, y1,s[i]);i++;}
 return x1;
}
#endif
// -----------------------------------------------------------------------
void TFT_Off()
{
CS_OFF;    
 NT35510_Cmd( CMD_BLANK_DISPLAY ); // 0x28
 NT35510_Cmd( CMD_SET_PWM_CONF );  // 0xBE
   NT35510_Data( 0x01 ); // PWMF
						 // PWM_f = 120e6/(65536*PWMF) = 1831 Hz
   NT35510_Data( 0x00 ); // PWM duty cycle = 0 (OFF)
   NT35510_Data( 0x01 ); // PWM enabled and controlled by host
   NT35510_Data( 0x00 ); //
   NT35510_Data( 0x00 ); //
   NT35510_Data( 0x00 ); //
 // Espero a que se desactive el PWM, si no podría quedarse en alto
 // y encendido
 _delay_ms(10);  
 
 NT35510_Cmd( CMD_ENT_DEEP_SLEEP );  // 0xE5  , 2 dummy read exits drom deep sleep
 _delay_ms(10);
 CS_ON;
}
// -----------------------------------------------------------------------
void TFT_On()
{
 TFT_Init();
}
// -----------------------------------------------------------------------
// 2018/10/26 Aprovechamiento de la RAM del NT35510. Prueba de concepto
void NT35510_WritePage(int page, unsigned short * data)
{
 // El tamaño de los bloques es de 16 kB
 #define NP16K ((8192/480)+1)
 #define NLI (272)
 int i;
CS_OFF;
 TFT_SetArea(0,NLI+page*NP16K, 479, NLI+(page+1)*NP16K);
 RS_ON; // DATA
 for(i=0;i<8192;i++) NT35510_Data_(data[i]);
 CS_ON;
}
// -----------------------------------------------------------------------
// 2018/10/26 Aprovechamiento de la RAM del NT35510. Prueba de concepto
void NT35510_ReadPage(int page, unsigned short * data)
{
  // El tamaño de los bloques es de 16 kB
 int i;
CS_OFF;
 TFT_SetAreaR(0,NLI+page*NP16K, 479, NLI+(page+1)*NP16K);
 RS_ON; // DATA
 DT_INPUT;
 _delay_us(10);
 for(i=0;i<8192;i++)
 {
  RD_OFF;
   asm("nop");
   data[i] = (FIOPIN >> NT35510_DT);
  RD_ON;
 }
 DT_OUTPUT;
 CS_ON;
}
/*
// -----------------------------------------------------------------------
extern unsigned char ZX_RAM[48*1024];

void NT35510_TestPages()
{
 // Prueba de concepto 2018/10/26 ===============================
 int i;
 int chk;
 _delay_ms(100);
 #define NMM (480)

 _printf("Relleno y guardo ----------------\n");
 chk =0;
 for(i=0;i<8192;i++)
 {
  ((unsigned short*)ZX_RAM)[i] = i;
  chk+=((unsigned short*)ZX_RAM)[i];
 }
 //for(i=0;i<10;i++) _printf("[%6d] = %3d\n",i,((unsigned short*)ZX_RAM)[i*NMM]);
 _printf("chk=%6d\n",chk);
 NT35510_WritePage(10,(unsigned short*) ZX_RAM);

 _printf("Borro            ----------------\n");
 for(i=0;i<8192;i++) ((unsigned short*)ZX_RAM)[i] = 0;
 //for(i=0;i<10;i++) _printf("[%6d] = %3d\n",i,((unsigned short*)ZX_RAM)[i*NMM]);

 _printf("Leo y muestro    ----------------\n");
 NT35510_ReadPage(10,(unsigned short*) ZX_RAM);
 chk=0;
 for(i=0;i<8192;i++)
 {
  chk+=((unsigned short*)ZX_RAM)[i];
 }

 //for(i=0;i<10;i++) _printf("[%6d] = %3d\n",i,((unsigned short*)ZX_RAM)[i*NMM]);
 _printf("chk=%6d\n",chk);
}
*/