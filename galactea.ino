#include <Arduboy2.h>
#include <ArduboyTones.h>
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

#define PANTALLA_ANCHO 128
#define PANTALLA_ALTO 64
#define NAVE_ALTO 12
#define NAVE_ANCHO 16
#define INTERFAZ_ALTO 11
#define INTERFAZ_ANCHO 4+NAVE_ANCHO+40+1
#define VIDAS_ANCHO 1+9*6
#define BALA_ALTO 4
#define BALA_ANCHO 6
#define N_BALAS 40
#define N_NAVES 10
#define JEFE_ANCHO 40
#define JEFE_ALTO 25
#define PISTOLA_ANCHO 11
#define PISTOLA_ALTO 16
#define LANZAMIS_ANCHO 12
#define LANZAMIS_ALTO 12
#define HANGAR_ANCHO 19
#define HANGAR_ALTO 12

#define JUEGO 0
#define MUERTO 1
#define TIENDA 2
#define MENU 3
#define FIN_NIVEL 4
#define JEFE 5
#define CREDITOS 6
short escenaActual = TIENDA;

short protaX = 0;
short protaY = 0;
boolean disparo = false;
short enfriamiento = 50;
short calor = 50;
short estadoCreditos = 0;
short naveXCreditos = -NAVE_ANCHO;
boolean naveDetenida = true;
boolean inicioMuerte = true;

struct explosion {
  short explosionMuerte = 0;
  short tiempoExplosion = -20;
};
explosion explosiones[3];

short posXjefe = PANTALLA_ANCHO + 100;
short posYjefe = 30;
short despJefe = 1;
short ataqueActual = 0;
short vidaJefe = 1000;
short vidaPistola = 700;
short disparoX = 999;
short tamanoDisparo = 1;
short vidaLanzaMisiles = 700;
short posXcohete = 999;
short posYcohete = 999;
short ataque = false;
boolean atacaAbajo = false;
short vidaHangar = 700;
short hangarFunciona = true;
boolean hangarMuerto = false;
boolean lanzamisilesMuerto = false;
boolean pistolaMuerto = false;

short vida = 1;
short vidaMax = 1;
short dano = 1;
short dinero = 8000;

struct bala {
  short posX = 0;
  short posY = 0;
  boolean viva = false;
  boolean enemigo = false;
};
bala *balas[N_BALAS];

struct nave {
  short posX = 130;
  short posY = 0;
  short vida = 25;
  short tipo = 0;
  short des = 0;
  short enfriamiento = 30;
  short calor = 0;
  short dinero = 0;
  short posXdetenida = 64;
  float verdaderaY = 0;
  float despY = 0;
  short procesando = 0;
  boolean bajando = true;
  boolean embestida = false;
  boolean viva = false;
};
nave *naves[N_NAVES];
int siguienteNave = 40;
int tiempoActNave = 50;

short seleccionTienda = 0;
boolean teclaPresionada = false;
short estadoVida = 0;
short estadoFuego = 0;
short estadoDano = 0;
short estadoMulti = 0;
short dineroVida[] = {30, 50, 100, 300, 500};
short dineroFuego[] = {100, 200, 350, 600, 900};
short dineroMulti[] = {500, 999};

short nivelActual = 1;
short nivelDesbloqueado = 1;
short progresoActual = 0;

//------------------------------------------------------------------------------Estructura musical--------------------------------------------------------------------------
const uint16_t victoria[] PROGMEM = {NOTE_G4, 300, NOTE_D4, 200, NOTE_F4, 200, NOTE_G4, 300, TONES_END};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const unsigned char PROGMEM protaImg[] = {
  0x40, 0x48, 0xe8, 0xec, 0x56, 0xb7, 0xf7, 0x0b, 0x8a, 0xdc, 0x88, 0x88, 0x90, 0xd0, 0x60, 0x60,
  0x00, 0x02, 0x02, 0x06, 0x0d, 0x0c, 0x0d, 0x0d, 0x05, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char PROGMEM vidaImg[] = {
  0x0a, 0x0d, 0x06, 0x1b, 0x28, 0x50, 0xa0, 0xc0,
};

const unsigned char PROGMEM balaProtaImg[] = {
  0x04, 0x09, 0x04, 0x09, 0x0f, 0x06,
};

const unsigned char PROGMEM enemigo0Img[] = {
  0x60, 0xf0, 0x68, 0x64, 0xf2, 0x9c, 0x0f, 0x97, 0x92, 0xf2, 0x6a, 0x65, 0x02, 0xf4, 0x68, 0x98,
  0x00, 0x00, 0x01, 0x02, 0x04, 0x03, 0x0f, 0x0e, 0x04, 0x04, 0x05, 0x0a, 0x04, 0x02, 0x01, 0x01,
};

const unsigned char PROGMEM enemigo1Img[] = {
  0x62, 0x95, 0x65, 0x95, 0x65, 0x95, 0x0d, 0x65, 0x61, 0xf1, 0x69, 0x05, 0x62, 0x94, 0x68, 0x68,
  0x04, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x09, 0x08, 0x08, 0x08, 0x0b, 0x05, 0x04, 0x02, 0x01, 0x01,
};

const unsigned char PROGMEM enemigo2Img[] = {
  0x60, 0x90, 0x48, 0x94, 0x24, 0x22, 0x41, 0xd1, 0xcd, 0x45, 0x22, 0x24, 0x94, 0x4a, 0x91, 0x6e,
  0x00, 0x00, 0x01, 0x02, 0x02, 0x05, 0x06, 0x07, 0x07, 0x06, 0x05, 0x02, 0x02, 0x05, 0x08, 0x07,
};

const unsigned char PROGMEM balaEnemigoImg[] = {
  0x06, 0x0f, 0x09, 0x02, 0x09, 0x02,
};

const unsigned char PROGMEM dineroImg[] = {
  0x99, 0x24, 0x5a, 0xb5, 0xad, 0x5a, 0x24, 0x99,
};

const unsigned char PROGMEM multishotImg[] = {
  0x08, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x20, 0x18, 0x00, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x20, 0x24, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x98, 0xb0, 0x80, 0x00, 0x01, 0x82, 0x80, 0x06, 0x3d, 0x7f, 0x1e, 0x4c, 0x7c, 0x38, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0d, 0x31, 0xf9, 0xfe, 0x6c, 0x78, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x38, 0x7c, 0x40, 0x85, 0x10, 0xc0, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x02, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0b, 0x1f, 0x3b, 0x29, 0x3f, 0x1c, 0x00, 0x00, 0x00,
};

const unsigned char PROGMEM fireImg[] = {
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xf4, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x90, 0xe0, 0xea, 0xc0, 0xe7, 0xf0, 0xc4, 0x90, 0xc0, 0x00, 0x00, 0xc0, 0x60, 0xf0, 0x90, 0x80, 0x80, 0xc4, 0xc6, 0xfe, 0xfe, 0xd6, 0xcc, 0x88, 0xd0, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x60, 0xf8, 0x9e, 0x07, 0x41, 0x40, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x41, 0x41, 0x07, 0x9e, 0xf8, 0x60, 0x00,
  0x00, 0x00, 0x01, 0x03, 0x06, 0x04, 0x09, 0x09, 0x05, 0x06, 0x03, 0x01, 0x00, 0x00, 0x01, 0x07, 0x0e, 0x18, 0x18, 0x30, 0x30, 0x60, 0x66, 0x30, 0x30, 0x18, 0x18, 0x0e, 0x07, 0x01, 0x00, 0x00,
};

const unsigned char PROGMEM vidaTiendaImg[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf1, 0xc0, 0x00, 0x3e, 0xff, 0xfd, 0xf9, 0xf3, 0x67, 0x0f, 0x9e, 0x9e, 0x04, 0x64, 0xf8, 0xf8, 0xf0, 0xc0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x20, 0x20, 0xbc, 0x84, 0x84, 0x77, 0xf0, 0xf0, 0xee, 0xdc, 0xb9, 0x71, 0x62, 0x66, 0x07, 0x0f, 0x0f, 0x0f, 0x1e, 0x1c, 0x1d, 0x1f, 0x1f, 0x0e, 0x00,
  0x40, 0x78, 0x08, 0xef, 0xe1, 0xe1, 0xdd, 0xbc, 0x7c, 0xfb, 0xf7, 0xef, 0x1f, 0x9e, 0x9d, 0x13, 0x03, 0xc3, 0xc4, 0xc0, 0xc0, 0xfc, 0xfc, 0xfc, 0xc0, 0xc0, 0xc0, 0xc2, 0x01, 0x10, 0x88, 0x80,
  0x78, 0x17, 0x2f, 0x3e, 0x1d, 0x1b, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00, 0x21, 0x11, 0x01, 0x01, 0x1f, 0xdf, 0x1f, 0x01, 0x01, 0x11, 0x21, 0x00, 0x04, 0x08, 0x00,
};

const unsigned char PROGMEM danoImg[] = {
  0x00, 0x3c, 0x66, 0xc2, 0x80, 0x08, 0x14, 0x12, 0x14, 0x08, 0xe0, 0xf0, 0x02, 0xf3, 0xf9, 0x09, 0x0c, 0x0c, 0x06, 0x05, 0x85, 0xc4, 0xfb, 0x2c, 0x84, 0x8e, 0xfa, 0xf8, 0xc0, 0x04, 0x1c, 0x18,
  0x00, 0x00, 0x3c, 0x27, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x10, 0x28, 0x24, 0x94, 0xc8, 0xe3, 0xff, 0xf3, 0xfb, 0xfb, 0xfd, 0xfd, 0xfb, 0xf2, 0x82, 0xbe, 0xf9, 0x01, 0x01, 0xc1, 0x37, 0x00, 0x80,
  0x00, 0x00, 0x00, 0x80, 0x01, 0x41, 0xa3, 0xd0, 0xe8, 0x2c, 0x5e, 0xff, 0xad, 0xf7, 0xdb, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x08, 0x0c, 0x35, 0x49, 0x31, 0x01, 0x00, 0x00, 0x43, 0x61,
  0x50, 0x00, 0x94, 0xc2, 0x04, 0xb5, 0x82, 0x00, 0x25, 0x1b, 0x09, 0x0f, 0x05, 0x07, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x34, 0x54, 0x64, 0x10, 0x30, 0x20, 0x20, 0x24, 0x2a, 0x2a, 0x26,
};

const unsigned char PROGMEM jefeImg[] = {
  0x00, 0xa0, 0x50, 0x08, 0xc4, 0x02, 0x31, 0x2f, 0x44, 0x8b, 0x9a, 0x94, 0x98, 0x10, 0x38, 0x24, 0x28, 0x34, 0x22, 0x38, 0x14, 0x94, 0x9a, 0x90, 0x8e, 0x4b, 0x23, 0x1e, 0x0e, 0x8e, 0x46, 0x26, 0x12, 0x8c, 0x46, 0x61, 0x91, 0x0e, 0x90, 0x60,
  0x01, 0x04, 0x15, 0xa2, 0x74, 0x11, 0x12, 0x12, 0x10, 0x10, 0x90, 0x50, 0x10, 0x11, 0x95, 0xb9, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0x1c, 0x50, 0x10, 0x10, 0x10, 0x10, 0x5a, 0x81, 0x18, 0x24, 0x5a, 0x81, 0x81, 0x5a, 0x24, 0x42, 0xa5, 0x18, 0x18,
  0x0c, 0x18, 0x24, 0x12, 0x09, 0x0a, 0x1c, 0x30, 0xd0, 0x09, 0x04, 0xf2, 0x09, 0x29, 0x14, 0x04, 0x08, 0x38, 0x44, 0x14, 0x48, 0x29, 0x19, 0x22, 0x24, 0x28, 0x30, 0x20, 0x20, 0x21, 0x12, 0x14, 0x18, 0x21, 0x42, 0x86, 0x89, 0x70, 0x09, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char PROGMEM pistolaImg[] = {
  0x1f, 0x2e, 0x4c, 0x4c, 0x8c, 0x4c, 0x52, 0x21, 0x40, 0x80, 0x86,
  0xfc, 0x7a, 0x31, 0x21, 0x20, 0x21, 0x69, 0xc6, 0x21, 0x90, 0x48,
};
const unsigned char PROGMEM pistolaPla[] = {
  0x1f, 0x2e, 0x4c, 0x4c, 0x8c, 0x4c, 0x5e, 0x3f, 0x7f, 0xff, 0xff,
  0xfc, 0x7a, 0x31, 0x21, 0x20, 0x21, 0x69, 0xfe, 0xff, 0xff, 0xff,
};

const unsigned char PROGMEM lanzaMisilesImg[] = {
  0xfc, 0x02, 0x02, 0xfd, 0x07, 0x0e, 0x0e, 0x0d, 0x1d, 0x9a, 0x9a, 0xf6,
  0x03, 0x04, 0x04, 0x0b, 0x0c, 0x07, 0x07, 0x0b, 0x0b, 0x05, 0x05, 0x06,
};
const unsigned char PROGMEM coheteHImg[] = {
  0x0c, 0x18, 0x1a, 0x1e, 0x16, 0x2d, 0x04, 0x12,
};
const unsigned char PROGMEM hangarImg[] = {
  0xc0, 0xa1, 0x16, 0x0a, 0x14, 0xa4, 0x42, 0xa2, 0x15, 0x09, 0x15, 0xa2, 0x42, 0xa4, 0x14, 0x0a, 0x16, 0xa1, 0xc0,
  0x0b, 0x04, 0x0b, 0x0a, 0x05, 0x08, 0x08, 0x04, 0x09, 0x0a, 0x05, 0x08, 0x08, 0x04, 0x09, 0x0a, 0x05, 0x0c, 0x0b,
};
const unsigned char PROGMEM hangarAbiertoImg[] = {
  0xc0, 0xa1, 0x16, 0x0a, 0x14, 0xa4, 0x42, 0x02, 0x05, 0x09, 0x05, 0x02, 0x42, 0xa4, 0x14, 0x0a, 0x16, 0xa1, 0xc0,
  0x0b, 0x04, 0x0b, 0x0a, 0x05, 0x08, 0x08, 0x04, 0x08, 0x08, 0x04, 0x08, 0x08, 0x04, 0x09, 0x0a, 0x05, 0x0c, 0x0b,
};
const unsigned char PROGMEM explosionesImg[5][32] = {
  {0x00, 0x00, 0x10, 0x08, 0x40, 0xa0, 0x20, 0x50, 0x10, 0x20, 0xc8, 0x14, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x0a, 0x05, 0x08, 0x0a, 0x05, 0x08, 0x14, 0x03, 0x08, 0x04, 0x00, 0x00},
  {0x00, 0x00, 0x60, 0x88, 0x74, 0x02, 0xa4, 0x16, 0x1a, 0x44, 0x98, 0x24, 0x68, 0x90, 0x04, 0x00, 0x00, 0x08, 0x22, 0x85, 0x1a, 0x25, 0x28, 0x28, 0x41, 0x48, 0x20, 0x23, 0x18, 0x24, 0x13, 0x00},
  {0x00, 0x92, 0x6c, 0x02, 0x77, 0x09, 0x12, 0x02, 0x05, 0x33, 0x66, 0x8c, 0x30, 0xc8, 0x01, 0x00, 0x02, 0x25, 0x1a, 0x24, 0x48, 0x48, 0x41, 0x22, 0x21, 0x18, 0x94, 0x22, 0x20, 0x19, 0x06, 0x20},
  {0x08, 0xf1, 0xdc, 0x06, 0x26, 0xe7, 0x33, 0x37, 0x27, 0x3f, 0x0e, 0x06, 0x1c, 0x72, 0xc0, 0x88, 0x07, 0x75, 0x58, 0xcc, 0x98, 0x90, 0x90, 0xc2, 0x42, 0xc3, 0x60, 0x3c, 0x47, 0x40, 0x32, 0x8f},
  {0x00, 0x00, 0x10, 0x08, 0x40, 0xa0, 0x20, 0x50, 0x10, 0x20, 0xc8, 0x14, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x0a, 0x05, 0x08, 0x0a, 0x05, 0x08, 0x14, 0x03, 0x08, 0x04, 0x00, 0x00}
};
const unsigned char PROGMEM explosionesPla[5][32] = {
  {0x00, 0x00, 0x10, 0x08, 0x40, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xc8, 0x1c, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x0a, 0x07, 0x0f, 0x0f, 0x07, 0x0f, 0x17, 0x03, 0x08, 0x04, 0x00, 0x00},
  {0x00, 0x00, 0x60, 0x88, 0xfc, 0xfe, 0xfc, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xf8, 0x90, 0x04, 0x00, 0x00, 0x08, 0x22, 0x87, 0x1f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x27, 0x13, 0x00},
  {0x00, 0x92, 0xfc, 0xfe, 0xff, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xfe, 0xfc, 0xf0, 0xc8, 0x01, 0x00, 0x02, 0x27, 0x1f, 0x3f, 0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x9f, 0x3f, 0x3f, 0x1f, 0x06, 0x20},
  {0x08, 0xf1, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf2, 0xc0, 0x88, 0x07, 0x77, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0x7f, 0x3f, 0x7f, 0x7f, 0x3f, 0x8f},
  {0x00, 0x00, 0x10, 0x08, 0x40, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xc8, 0x1c, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x0a, 0x07, 0x0f, 0x0f, 0x07, 0x0f, 0x17, 0x03, 0x08, 0x04, 0x00, 0x00}
};
const unsigned char PROGMEM laser1Img[] = {
  0x22, 0x95, 0x4a, 0x93,
  0x11, 0x2a, 0x14, 0x32,
};
const unsigned char PROGMEM laser2Img[] = {
  0x53, 0x8a, 0x55, 0x22,
  0x32, 0x14, 0x2a, 0x11,
};

void resetearJefe() {
  posXjefe = PANTALLA_ANCHO + 100;
  posYjefe = 30;
  despJefe = 1;
  ataqueActual = 0;
  vidaJefe = 1000;
  vidaPistola = 700;
  disparoX = 999;
  tamanoDisparo = 1;
  vidaLanzaMisiles = 700;
  posXcohete = 999;
  posYcohete = 999;
  ataque = false;
  atacaAbajo = false;
  vidaHangar = 700;
  hangarFunciona = true;
  hangarMuerto = false;
  lanzamisilesMuerto = false;
  pistolaMuerto = false;
  explosiones[0].explosionMuerte = 0;
  explosiones[0].tiempoExplosion = -20;
  explosiones[1].explosionMuerte = 0;
  explosiones[1].tiempoExplosion = -20;
  explosiones[2].explosionMuerte = 0;
  explosiones[2].tiempoExplosion = -20;
}

void gestorMuerte() {
  vida = 0;
  gestorInterfaz();
  if (inicioMuerte) {
    resetearJefe();
    inicioMuerte = false;
  }
  if (explosiones[0].explosionMuerte < 4) {
    if (explosiones[0].explosionMuerte == 1) {
      sound.tone(NOTE_E1, 100, NOTE_F1, 100, NOTE_G1, 150);
    }
    arduboy.drawBitmap(protaX + 1, protaY + INTERFAZ_ALTO, protaImg, NAVE_ANCHO, NAVE_ALTO, WHITE);
  }
  if (explosiones[0].tiempoExplosion >= 0 && explosiones[0].explosionMuerte < 5) {
    arduboy.drawBitmap(protaX + 1, protaY + INTERFAZ_ALTO - 3, explosionesPla[explosiones[0].explosionMuerte], 16, 16, BLACK);
    arduboy.drawBitmap(protaX + 1, protaY + INTERFAZ_ALTO - 3, explosionesImg[explosiones[0].explosionMuerte], 16, 16, WHITE);
  }
  for (int i = 0; i < N_NAVES; i++) {
    naves[i]->viva = false;
  }
  for (int i = 0; i < N_BALAS; i++) {
    balas[i]->viva = false;
  }
  if (explosiones[0].explosionMuerte < 5 && explosiones[0].tiempoExplosion < 3) {
    explosiones[0].tiempoExplosion++;
  } else if (explosiones[0].explosionMuerte < 5 && explosiones[0].tiempoExplosion >= 3) {
    explosiones[0].explosionMuerte++;
    explosiones[0].tiempoExplosion = 0;
  } else if (explosiones[0].explosionMuerte >= 5 && explosiones[0].tiempoExplosion < 20) {
    explosiones[0].tiempoExplosion++;
  } else {
    explosiones[0].tiempoExplosion = -20;
    explosiones[0].explosionMuerte = 0;
    vida = vidaMax;
    escenaActual = TIENDA;
    progresoActual = 0;
    calor = 50;
    teclaPresionada = true;
    inicioMuerte = true;
  }
}

void gestorInterfaz() {
  arduboy.drawLine(0, INTERFAZ_ALTO - 1, 128, INTERFAZ_ALTO - 1);
  arduboy.drawLine(VIDAS_ANCHO, 0, VIDAS_ANCHO, INTERFAZ_ALTO - 1);
  arduboy.drawLine(PANTALLA_ANCHO - 1 - 6 * 4 - 8 - 2, 0, PANTALLA_ANCHO - 1 - 6 * 4 - 8 - 2, INTERFAZ_ALTO - 1);
  arduboy.drawBitmap(PANTALLA_ANCHO - 1 - 6 * 4 - 8, 1, dineroImg, 8, 8, WHITE);
  arduboy.setCursor(PANTALLA_ANCHO - 6 * 4, 2);
  arduboy.print(dinero);
  switch (escenaActual) {
    case TIENDA:
      arduboy.setCursor(VIDAS_ANCHO + 9, 1);
      arduboy.print("SHOP");
      break;
    case FIN_NIVEL:
    case MUERTO:
    case JUEGO:
      arduboy.drawRect(VIDAS_ANCHO +4, 2, 31,INTERFAZ_ALTO - 5 );
      if (escenaActual!=FIN_NIVEL){
        arduboy.fillRect(VIDAS_ANCHO +4, 3, progresoActual,INTERFAZ_ALTO - 7 );
      }else{
        arduboy.fillRect(VIDAS_ANCHO +4, 3, 30,INTERFAZ_ALTO - 7 );
      }
      break;
    case MENU:
      arduboy.setCursor(VIDAS_ANCHO + 9, 1);
      arduboy.print("MENU");
      break;
    case JEFE:
      arduboy.drawRect(VIDAS_ANCHO +4, 2, 31,INTERFAZ_ALTO - 5 );
      arduboy.fillRect(VIDAS_ANCHO +4, 3, (vidaJefe+vidaLanzaMisiles+vidaHangar+vidaPistola)/103,INTERFAZ_ALTO - 7 );
      
      break;
  }
  for (int i = 0; i < vida; i++) {
    arduboy.drawBitmap(1 + 9 * i, 1, vidaImg, 8, 8, WHITE);
  }
}

void gestorTienda() {
  gestorInterfaz();
  arduboy.drawLine(INTERFAZ_ANCHO, INTERFAZ_ALTO, INTERFAZ_ANCHO, PANTALLA_ALTO);
  protaX = 1;
  protaY = 1 + seleccionTienda * (1 + NAVE_ALTO);
  arduboy.drawBitmap(protaX + 1, protaY + INTERFAZ_ALTO, protaImg, NAVE_ANCHO, NAVE_ALTO, WHITE);
  //arduboy.setCursor(2,INTERFAZ_ALTO+2);
  arduboy.setCursor(6 + NAVE_ANCHO, INTERFAZ_ALTO + 4);
  arduboy.print("Health");
  arduboy.setCursor(6 + NAVE_ANCHO, INTERFAZ_ALTO + (NAVE_ALTO + 2) * 1 + 2);
  arduboy.print("Damage");
  arduboy.setCursor(6 + NAVE_ANCHO, INTERFAZ_ALTO + (NAVE_ALTO + 2) * 2 + 1);
  arduboy.print("Fire");
  arduboy.setCursor(6 + NAVE_ANCHO, INTERFAZ_ALTO + (NAVE_ALTO + 2) * 3);
  arduboy.print("Multi");
  for (int i = 0; i < 4; i++) {
    if (i == seleccionTienda) {
      arduboy.drawRect(4 + NAVE_ANCHO, 1 + INTERFAZ_ALTO + (1 + NAVE_ALTO)*i, 40, NAVE_ALTO);
    }
  }
  if (arduboy.pressed(DOWN_BUTTON) && seleccionTienda != 3 && !teclaPresionada) {
    seleccionTienda++;
    teclaPresionada = true;
  } else if (arduboy.pressed(UP_BUTTON) && seleccionTienda != 0 && !teclaPresionada) {
    seleccionTienda--;
    teclaPresionada = true;
  } else if (arduboy.pressed(A_BUTTON) && !teclaPresionada) {
    switch (seleccionTienda) {
      case 0:
        if (estadoVida < 5 && dinero >= dineroVida[estadoVida]) {
          sound.tone(NOTE_F3, 50);
          dinero -= dineroVida[estadoVida];
          estadoVida++;
          vida++;
          vidaMax++;
        } else {
          sound.tone(NOTE_G1, 50);
        }
        break;
      case 1:
        if (estadoDano < 4 && dinero >= dineroVida[estadoDano + 1]) {
          sound.tone(NOTE_F3, 50);
          dinero -= dineroVida[estadoDano + 1];
          estadoDano++;
          dano++;
        } else {
          sound.tone(NOTE_G1, 50);
        }
        break;
      case 2:
        if (estadoFuego < 5 && dinero >= dineroFuego[estadoFuego]) {
          sound.tone(NOTE_F3, 50);
          dinero -= dineroFuego[estadoFuego];
          if (estadoFuego < 4) {
            enfriamiento -= 10;
          } else {
            enfriamiento = 5;
          }
          estadoFuego++;
        } else {
          sound.tone(NOTE_G1, 50);
        }
        break;
      case 3:
        if (estadoMulti < 2 && dinero >= dineroMulti[estadoMulti]) {
          sound.tone(NOTE_F3, 50);
          dinero -= dineroMulti[estadoMulti];
          estadoMulti++;
        } else {
          sound.tone(NOTE_G1, 50);
        }
        break;
    }
    teclaPresionada = true;
  } else if (arduboy.pressed(B_BUTTON) && !teclaPresionada) {
    escenaActual = MENU;
    for (int i = 0; i < N_BALAS; i++) {
      balas[i]->viva = false;
    }
  } else if (teclaPresionada && !arduboy.pressed(B_BUTTON) && !arduboy.pressed(A_BUTTON) && !arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(UP_BUTTON)) {
    teclaPresionada = false;
  }
  boolean maximo = false;
  switch (seleccionTienda) {
    case 0:
      arduboy.drawBitmap(PANTALLA_ANCHO - 33, PANTALLA_ALTO - 32, vidaTiendaImg, 32, 32, WHITE);
      arduboy.setCursor(INTERFAZ_ANCHO + 15, INTERFAZ_ALTO + 6);
      arduboy.print("HEALTH");
      if (estadoVida < 5) {
        arduboy.setCursor(INTERFAZ_ANCHO + 12, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print(dineroVida[estadoVida]);
      } else {
        arduboy.setCursor(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print("MAX");
        maximo = true;
      }
      break;
    case 1:
      arduboy.drawBitmap(PANTALLA_ANCHO - 33, PANTALLA_ALTO - 32, danoImg, 32, 32, WHITE);
      arduboy.setCursor(INTERFAZ_ANCHO + 15, INTERFAZ_ALTO + 6);
      arduboy.print("DAMAGE");
      if (estadoDano < 4) {
        arduboy.setCursor(INTERFAZ_ANCHO + 12, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print(dineroVida[estadoDano + 1]);
      } else {
        arduboy.setCursor(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print("MAX");
        maximo = true;
      }
      break;
    case 2:
      arduboy.drawBitmap(PANTALLA_ANCHO - 33, PANTALLA_ALTO - 32, fireImg, 32, 32, WHITE);
      arduboy.setCursor(INTERFAZ_ANCHO + 10, INTERFAZ_ALTO + 6);
      arduboy.print("FIRE RATE");
      if (estadoFuego < 5) {
        arduboy.setCursor(INTERFAZ_ANCHO + 12, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print(dineroFuego[estadoFuego]);
      } else {
        arduboy.setCursor(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print("MAX");
        maximo = true;
      }
      break;
    case 3:
      arduboy.drawBitmap(PANTALLA_ANCHO - 33, PANTALLA_ALTO - 32, multishotImg, 32, 32, WHITE);
      arduboy.setCursor(INTERFAZ_ANCHO + 10, INTERFAZ_ALTO + 6);
      arduboy.print("MULTISHOT");
      if (estadoMulti < 2) {
        arduboy.setCursor(INTERFAZ_ANCHO + 12, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print(dineroMulti[estadoMulti]);
      } else {
        arduboy.setCursor(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 9 - 9 + 1);
        arduboy.print("MAX");
        maximo = true;
      }
      break;
  }
  arduboy.setCursor(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 19 - 9);
  arduboy.print("Buy:");
  if (!maximo) {
    arduboy.drawBitmap(INTERFAZ_ANCHO + 3, PANTALLA_ALTO - 9 - 9, dineroImg, 8, 8, WHITE);
  }
}

void gestorNavesEnemigas() {
  if (tiempoActNave >= siguienteNave && escenaActual != JEFE) {
    tiempoActNave = 0;
    siguienteNave = 10 + rand() % 15;
    for (int i = 0; i < N_NAVES; i++) {
      if (!naves[i]->viva) {
        naves[i]->viva = true;
        naves[i]->posX = 130;
        naves[i]->posY = rand() % (PANTALLA_ALTO - INTERFAZ_ALTO - NAVE_ALTO);
        switch (nivelActual) {
          case 1:
            if (progresoActual >= 15) {
              naves[i]->tipo = rand() % 2;
            } else {
              naves[i]->tipo = 0;
            }
            if (naves[i]->tipo == 0) {
              naves[i]->vida = 1;
              naves[i]->dinero = 2;
            } else {
              naves[i]->vida = 2;
              naves[i]->dinero = 4;
            }
            break;
          case 2:
            if (progresoActual >= 15) {
              naves[i]->tipo = rand() % 3;
            } else {
              naves[i]->tipo = rand() % 2;
            }
            if (naves[i]->tipo == 0) {
              naves[i]->vida = 2;
              naves[i]->dinero = 4;
            } else if (naves[i]->tipo == 1) {
              naves[i]->vida = 4;
              naves[i]->dinero = 6;
            } else {
              naves[i]->vida = 10;
              naves[i]->dinero = 9;
            }
            break;
          case 3:
            naves[i]->tipo = rand() % 3;
            if (naves[i]->tipo == 0) {
              naves[i]->vida = 10;
              naves[i]->dinero = 10;
            } else if (naves[i]->tipo == 1) {
              naves[i]->vida = 15;
              naves[i]->dinero = 15;
            } else {
              naves[i]->vida = 25;
              naves[i]->dinero = 20;
            }
            break;
        }

        //nave 1
        naves[i]->des = 0;
        naves[i]->enfriamiento = 30 + rand() % 15;
        naves[i]->calor = 0;
        if (naves[i]->tipo == 1 && naves[i]->posY < (PANTALLA_ALTO - NAVE_ALTO - INTERFAZ_ALTO) / 2) {
          naves[i]->bajando = true;
        } else {
          naves[i]->bajando = false;
        }

        //nave 2
        naves[i]->posXdetenida = 50 + rand() % 20;
        naves[i]->embestida = false;
        naves[i]->despY = 0;
        naves[i]->verdaderaY = naves[i]->posY;
        naves[i]->procesando = 0;

        i = N_NAVES;
      }
    }
  } else if (escenaActual != JEFE) {
    tiempoActNave++;
  }
  for (int i = 0; i < N_NAVES; i++) {
    if (naves[i]->viva) {
      switch (naves[i]->tipo) {
        case 0:
          naves[i]->posX -= 2;
          arduboy.drawBitmap(naves[i]->posX + 1, naves[i]->posY + INTERFAZ_ALTO + 1, enemigo0Img, NAVE_ANCHO, NAVE_ALTO, WHITE);
          break;
        case 1:
          naves[i]->posX--;
          arduboy.drawBitmap(naves[i]->posX + 1, naves[i]->posY + INTERFAZ_ALTO + 1, enemigo1Img, NAVE_ANCHO, NAVE_ALTO, WHITE);
          if (naves[i]->des < 20) {
            naves[i]->des++;
            if (naves[i]->bajando) {
              naves[i]->posY++;
            } else {
              naves[i]->posY--;
            }
          } else {
            naves[i]->bajando = !naves[i]->bajando;
            naves[i]->des = 0;
          }
          if (naves[i]->calor < naves[i]->enfriamiento) {
            naves[i]->calor++;
          } else {
            for (int j = 0; j < N_BALAS; j++) {
              if (!balas[j]->viva) {
                balas[j]->viva = true;
                balas[j]->enemigo = true;
                balas[j]->posX = naves[i]->posX + 8;
                balas[j]->posY = naves[i]->posY + 15;
                j = N_BALAS;
              }
            }
            naves[i]->enfriamiento = 30 + rand() % 15;
            naves[i]->calor = 0;
          }
          break;
        case 2:
          arduboy.drawBitmap(naves[i]->posX + 1, naves[i]->posY + INTERFAZ_ALTO + 1, enemigo2Img, NAVE_ANCHO, NAVE_ALTO, WHITE);
          if (naves[i]->posX > naves[i]->posXdetenida) {
            naves[i]->posX--;
          } else if (naves[i]->procesando < 1) {
            if (naves[i]->procesando == 0) {
              float magnitud = sqrt(pow(naves[i]->posX - protaX, 2) + pow(naves[i]->posY - protaY, 2));
              float seno = (naves[i]->posY - protaY) / magnitud;
              float coseno = (naves[i]->posX - protaX) / magnitud;
              naves[i]->despY = -(coseno * seno);
            }
            naves[i]->procesando++;
          } else {
            naves[i]->posX -= 2;
            naves[i]->verdaderaY += naves[i]->despY * 2;
            naves[i]->posY = naves[i]->verdaderaY;
          }
          break;
      }
      if (naves[i]->posX + NAVE_ANCHO < 0) {
        naves[i]->viva = false;
      }
    }
  }
}

void gestorBalas() {
  for (int i = 0; i < N_BALAS; i++) {
    if (balas[i]->viva) {
      if (balas[i]->enemigo) {
        balas[i]->posX -= 3;
        arduboy.drawBitmap(balas[i]->posX, balas[i]->posY, balaEnemigoImg, BALA_ANCHO, BALA_ALTO, WHITE);
      } else {
        balas[i]->posX += 3;
        arduboy.drawBitmap(balas[i]->posX, balas[i]->posY, balaProtaImg, BALA_ANCHO, BALA_ALTO, WHITE);
      }
      if (balas[i]->posX > PANTALLA_ANCHO || balas[i]->posX < -20) {
        balas[i]->viva = false;
      }
      for (int j = 0; j < N_NAVES; j++) {
        if (!balas[i]->enemigo && naves[j]->viva && balas[i]->posX + BALA_ANCHO > naves[j]->posX && balas[i]->posX < naves[j]->posX + NAVE_ANCHO && balas[i]->posY + BALA_ALTO > naves[j]->posY + INTERFAZ_ALTO && balas[i]->posY < naves[j]->posY + NAVE_ALTO + INTERFAZ_ALTO) {
          balas[i]->viva = false;
          if (naves[j]->vida > dano) {
            naves[j]->vida -= dano;
          } else {
            naves[j]->viva = false;
            sound.tone(NOTE_G1, 75);
            progresoActual++;
            dinero += naves[j]->dinero;
            //cambiar la duracion de los niveles a 30
            if (progresoActual >= 30) {
              if (nivelActual == 1 && nivelDesbloqueado == 1) {
                nivelDesbloqueado = 2;
              } else if (nivelActual == 2 && nivelDesbloqueado == 2) {
                nivelDesbloqueado = 3;
              }
              if (nivelActual == 1) {
                dinero += 50;
              } else if (nivelActual == 2) {
                dinero += 100;
              }
              progresoActual = 0;
              teclaPresionada = true;
              if (nivelActual != 3) {
                sound.tones(victoria);
                escenaActual = FIN_NIVEL ;
                calor=50;
                tiempoActNave = 0;
              } else {
                escenaActual = JEFE ;
              }
              for (int i = 0; i < N_NAVES; i++) {
                naves[i]->viva = false;
              }
              for (int i = 0; i < N_BALAS; i++) {
                balas[i]->viva = false;
              }
            }
          }
        }
      }
      if (balas[i]->enemigo && balas[i]->posX + BALA_ANCHO > protaX && balas[i]->posX < protaX + NAVE_ANCHO && balas[i]->posY + BALA_ALTO > protaY + INTERFAZ_ALTO && balas[i]->posY < protaY + NAVE_ALTO + INTERFAZ_ALTO) {
        balas[i]->viva = false;
        if (vida > 1) {
          vida--;
          sound.tone(NOTE_E1, 75);
        } else {
          escenaActual = MUERTO;
        }
      }
      //gestorVidaJefe
      if (escenaActual == JEFE && vidaPistola <= 0 && vidaLanzaMisiles <= 0 && vidaHangar <= 0 && balas[i]->posX + BALA_ANCHO > posXjefe && balas[i]->posX < posXjefe + JEFE_ANCHO && balas[i]->posY + BALA_ALTO > posYjefe && balas[i]->posY < posYjefe + JEFE_ALTO) {
        balas[i]->viva = false;
        if (vidaJefe > 0) {
          vidaJefe -= dano;
        }
      }
      if (escenaActual == JEFE && vidaPistola > 0 && balas[i]->posX + BALA_ANCHO > posXjefe - 7 && balas[i]->posX < posXjefe - 7 + PISTOLA_ANCHO && balas[i]->posY + BALA_ALTO > posYjefe + 6 && balas[i]->posY < posYjefe + 6 + PISTOLA_ALTO) {
        balas[i]->viva = false;
        if (vidaPistola > 0) {
          vidaPistola -= dano;
        }
      }
      if (escenaActual == JEFE && vidaLanzaMisiles > 0 && balas[i]->posX + BALA_ANCHO > posXjefe + 12 && balas[i]->posX < posXjefe + 12 + LANZAMIS_ANCHO && balas[i]->posY + BALA_ALTO > posYjefe + 19 && balas[i]->posY < posYjefe + 19 + LANZAMIS_ALTO) {
        balas[i]->viva = false;
        if (vidaLanzaMisiles > 0) {
          vidaLanzaMisiles -= dano;
        }
      }
      if (escenaActual == JEFE && vidaHangar > 0 && balas[i]->posX + BALA_ANCHO > posXjefe + 8 && balas[i]->posX < posXjefe + 8 + HANGAR_ANCHO && balas[i]->posY + BALA_ALTO > posYjefe - 9 && balas[i]->posY < posYjefe - 9 + HANGAR_ALTO) {
        balas[i]->viva = false;
        if (vidaHangar > 0) {
          vidaHangar -= dano;
        }
      }
      //Comprueba menu
      if (escenaActual == MENU) {
        if (balas[i]->posX + BALA_ANCHO >= PANTALLA_ANCHO / 2 + 2) {
          if (balas[i]->posY > INTERFAZ_ALTO + 2 && balas[i]->posY < INTERFAZ_ALTO + 2 + 15) {
            escenaActual = JUEGO;
            nivelActual = 1;
            balas[i]->viva = false;
          } else if (nivelDesbloqueado > 1 && balas[i]->posY > INTERFAZ_ALTO + 2 + 17 && balas[i]->posY < INTERFAZ_ALTO + 2 + 17 + 15) {
            escenaActual = JUEGO;
            nivelActual = 2;
            balas[i]->viva = false;
          } else if (nivelDesbloqueado > 2 && balas[i]->posY > INTERFAZ_ALTO + 2 + 17 * 2 && balas[i]->posY < INTERFAZ_ALTO + 2 + 17 * 2 + 15) {
            escenaActual = JUEGO;
            nivelActual = 3;
            balas[i]->viva = false;
          }
        }
      }
    }
  }
}

void gestorNaveProta() {
  arduboy.drawBitmap(protaX + 1, protaY + INTERFAZ_ALTO, protaImg, NAVE_ANCHO, NAVE_ALTO, WHITE);
  //Movimiento
  if (arduboy.pressed(DOWN_BUTTON) && (escenaActual != JEFE || posYjefe < PANTALLA_ALTO + 30)) {
    protaY += 2;
    if (protaY + NAVE_ALTO + 1 >= PANTALLA_ALTO - INTERFAZ_ALTO) {
      protaY = PANTALLA_ALTO - NAVE_ALTO - 1 - INTERFAZ_ALTO;
    }
  } else if (arduboy.pressed(UP_BUTTON) && (escenaActual != JEFE || posYjefe < PANTALLA_ALTO + 30)) {
    protaY -= 2;
    if (protaY <= 0) {
      protaY = 1;
    }
  }
  if (arduboy.pressed(RIGHT_BUTTON) && (escenaActual != JEFE || posYjefe < PANTALLA_ALTO + 30)) {
    protaX += 2;
    if (protaX + NAVE_ANCHO + 1 >= PANTALLA_ANCHO / 2) {
      protaX = PANTALLA_ANCHO / 2 - NAVE_ANCHO - 1;
    }
  } else if (arduboy.pressed(LEFT_BUTTON) && (escenaActual != JEFE || posYjefe < PANTALLA_ALTO + 30)) {
    protaX -= 2;
    if (protaX <= 0) {
      protaX = 1;
    }
  }
  //Disparo
  short enfriamientoGuardado = enfriamiento;
  if (escenaActual == MENU) {
    enfriamiento = 50;
  }
  if (arduboy.pressed(A_BUTTON) && calor >= enfriamiento) {
    calor = 0;
    short balasActuales = 0;
    for (int i = 0; i < N_BALAS; i++) {
      if (!balas[i]->viva) {
        balas[i]->viva = true;
        balas[i]->enemigo = false;
        balas[i]->posX = protaX + 8;
        //implementacion del multishot
        if (escenaActual == MENU) {
          balas[i]->posY = protaY + 15;
          i = N_BALAS;
        } else {
          switch (estadoMulti) {
            case 0:
              balas[i]->posY = protaY + 15;
              break;
            case 1:
              if (balasActuales == 0) {
                balas[i]->posY = protaY + 15 - BALA_ALTO / 2 - 1;
              } else {
                balas[i]->posY = protaY + 15 + BALA_ALTO / 2 + 1;
              }
              break;
            case 2:
              switch (balasActuales) {
                case 0:
                  balas[i]->posY = protaY + 15 - BALA_ALTO - 1;
                  break;
                case 1:
                  balas[i]->posY = protaY + 15;
                  break;
                case 2:
                  balas[i]->posY = protaY + 15 + BALA_ALTO + 1;
                  break;
              }
              break;
          }
          if (balasActuales == estadoMulti) {
            i = N_BALAS;
          } else {
            balasActuales++;
          }
        }
      }
    }
  } else {
    calor++;
  }
  enfriamiento = enfriamientoGuardado;
  //colision con nave enemiga
  for (int i = 0; i < N_NAVES; i++) {
    if (naves[i]->viva && protaX + NAVE_ANCHO > naves[i]->posX && protaX < naves[i]->posX + NAVE_ANCHO && protaY + NAVE_ALTO > naves[i]->posY && protaY < naves[i]->posY + NAVE_ALTO) {
      naves[i]->viva = false;
      if (vida > 1) {
        vida--;
        sound.tone(NOTE_E1, 75);
      } else {
        escenaActual = MUERTO;
      }
    }
  }
  if (escenaActual == JEFE && protaX + NAVE_ANCHO > disparoX && protaX < disparoX + 30 && protaY + NAVE_ALTO + INTERFAZ_ALTO > posYjefe + 7 && protaY + INTERFAZ_ALTO < posYjefe + 7 + PISTOLA_ALTO - 2) {
    if (vida > 1) {
      vida--;
      sound.tone(NOTE_E1, 75);
    } else {
      escenaActual = MUERTO;
    }
    disparoX = 999;
    ataqueActual = 0;
  }
  if (escenaActual == JEFE && protaX + NAVE_ANCHO > posXcohete && protaX < posXcohete + 8 && protaY + NAVE_ALTO + INTERFAZ_ALTO > posYcohete && protaY + INTERFAZ_ALTO < posYcohete + 6) {
    if (vida > 1) {
      vida--;
      sound.tone(NOTE_E1, 75);
    } else {
      escenaActual = MUERTO;
    }
    posXcohete = 999;
    posYcohete = 999;
    ataque = false;
    ataqueActual = 0;
    atacaAbajo = false;
  }
}

void gestorExplosion(int idExplosion, boolean resetear) {
  if (explosiones[idExplosion].explosionMuerte == 1) {
    sound.tone(NOTE_E1, 100, NOTE_F1, 100, NOTE_G1, 150);
  }
  if (explosiones[idExplosion].tiempoExplosion == -20 && resetear) {
    explosiones[idExplosion].tiempoExplosion = 0;
  }
  if (explosiones[idExplosion].explosionMuerte < 5 && explosiones[idExplosion].tiempoExplosion < 3) {
    explosiones[idExplosion].tiempoExplosion++;
  } else if (explosiones[idExplosion].explosionMuerte < 5 && explosiones[idExplosion].tiempoExplosion >= 3) {
    explosiones[idExplosion].explosionMuerte++;
    explosiones[idExplosion].tiempoExplosion = 0;
  } else {
    explosiones[idExplosion].explosionMuerte = 0;
    explosiones[idExplosion].tiempoExplosion = -20;
    switch (idExplosion) {
      case 0:
        pistolaMuerto = true;
        break;
      case 1:
        lanzamisilesMuerto = true;
        break;
      case 2:
        hangarMuerto = true;
        break;
    }
  }
}

void gestorJefe() {
  gestorInterfaz();
  gestorBalas();
  gestorNaveProta();
  gestorNavesEnemigas();
  arduboy.drawBitmap(posXjefe, posYjefe, jefeImg, JEFE_ANCHO, JEFE_ALTO, WHITE);
  if (vidaPistola > 0) {
    arduboy.drawBitmap(posXjefe - 7, posYjefe + 6, pistolaPla, PISTOLA_ANCHO, PISTOLA_ALTO, BLACK);
    arduboy.drawBitmap(posXjefe - 7, posYjefe + 6, pistolaImg, PISTOLA_ANCHO, PISTOLA_ALTO, WHITE);
  }
  if (vidaLanzaMisiles > 0) {
    arduboy.drawBitmap(posXjefe + 12, posYjefe + 19, lanzaMisilesImg, LANZAMIS_ANCHO, LANZAMIS_ALTO, WHITE);
  }
  if (vidaHangar > 0) {
    if (ataqueActual == 3) {
      arduboy.drawBitmap(posXjefe + 8, posYjefe - 9, hangarAbiertoImg, HANGAR_ANCHO, HANGAR_ALTO, WHITE);
    } else {
      arduboy.drawBitmap(posXjefe + 8, posYjefe - 9, hangarImg, HANGAR_ANCHO, HANGAR_ALTO, WHITE);
    }
  }
  //comprobar explosiones
  if (vidaPistola <= 0 && !pistolaMuerto) {
    arduboy.drawBitmap(posXjefe - 9, posYjefe + 6, explosionesPla[explosiones[0].explosionMuerte], 16, 16, BLACK);
    arduboy.drawBitmap(posXjefe - 9, posYjefe + 6, explosionesImg[explosiones[0].explosionMuerte], 16, 16, WHITE);
    gestorExplosion(0, true);
  }
  if (vidaLanzaMisiles <= 0 && !lanzamisilesMuerto) {
    arduboy.drawBitmap(posXjefe + 12 - 2, posYjefe + 19 - 2, explosionesPla[explosiones[1].explosionMuerte], 16, 16, BLACK);
    arduboy.drawBitmap(posXjefe + 12 - 2, posYjefe + 19 - 2, explosionesImg[explosiones[1].explosionMuerte], 16, 16, WHITE);
    gestorExplosion(1, true);
  }
  if (vidaHangar <= 0 && !hangarMuerto) {
    arduboy.drawBitmap(posXjefe + 9, posYjefe - 11, explosionesPla[explosiones[2].explosionMuerte], 16, 16, BLACK);
    arduboy.drawBitmap(posXjefe + 9, posYjefe - 11, explosionesImg[explosiones[2].explosionMuerte], 16, 16, WHITE);
    gestorExplosion(2, true);
  }
  if (vidaJefe < 900 && posYjefe < PANTALLA_ALTO + 30) {
    if (explosiones[0].tiempoExplosion > 0) {
      arduboy.drawBitmap(posXjefe + 5, posYjefe - 3, explosionesPla[explosiones[0].explosionMuerte], 16, 16, BLACK);
      arduboy.drawBitmap(posXjefe + 5, posYjefe - 3, explosionesImg[explosiones[0].explosionMuerte], 16, 16, WHITE);
    }
    gestorExplosion(0, false);
  }
  if (vidaJefe < 500 && posYjefe < PANTALLA_ALTO + 30) {
    if (explosiones[1].tiempoExplosion > 0) {
      arduboy.drawBitmap(posXjefe + 23, posYjefe + 5, explosionesPla[explosiones[1].explosionMuerte], 16, 16, BLACK);
      arduboy.drawBitmap(posXjefe + 23, posYjefe + 5, explosionesImg[explosiones[1].explosionMuerte], 16, 16, WHITE);
    }
    gestorExplosion(1, false);
  }
  if (vidaJefe < 200 && posYjefe < PANTALLA_ALTO + 30) {
    if (explosiones[2].tiempoExplosion > 0) {
      arduboy.drawBitmap(posXjefe, posYjefe + 15, explosionesPla[explosiones[2].explosionMuerte], 16, 16, BLACK);
      arduboy.drawBitmap(posXjefe, posYjefe + 15, explosionesImg[explosiones[2].explosionMuerte], 16, 16, WHITE);
    }
    gestorExplosion(2, false);
  }
  //ataques
  if (posXjefe > 75) {
    posXjefe--;
  } else {
    if (vidaJefe > 0) {
      if (despJefe < 0 && posYjefe <= INTERFAZ_ALTO + 12) {
        if (vidaJefe < 500) {
          despJefe = rand() % 2 + 1;
        } else {
          despJefe = 1;
        }
      } else if (despJefe > 0 && posYjefe >= PANTALLA_ALTO - JEFE_ALTO - 7) {
        if (vidaJefe < 500) {
          despJefe = -(rand() % 2 + 1);
        } else {
          despJefe = -1;
        }
      }
    } else {
      despJefe = 1;
    }
    if (posYjefe > PANTALLA_ALTO + 30) {
      protaX++;
    }
    if (protaX >= PANTALLA_ANCHO + 20) {
      escenaActual = CREDITOS;
    }
    if (ataqueActual != 1) {
      posYjefe += despJefe;
    }
    if (tiempoActNave >= siguienteNave && ataqueActual == 0) {
      tiempoActNave = 0;
      siguienteNave = 60 + rand() % 30;
      ataqueActual = rand() % 3 + 1;
      if ((vidaPistola <= 0 && ataqueActual == 1) || (vidaLanzaMisiles <= 0 && ataqueActual == 2) || (vidaHangar <= 0 && ataqueActual == 3)) {
        ataqueActual = 0;
        tiempoActNave = siguienteNave + 10;
      }
      if (vidaPistola <= 0 && vidaLanzaMisiles <= 0 && vidaHangar <= 0) {
        ataqueActual = 4;
      }
      switch(ataqueActual){
        case 1:
          sound.tone(NOTE_E5, 75,NOTE_F5, 50);
          break;
        case 2:
          sound.tone(NOTE_F2, 75);
          break;
        case 3:
          sound.tone(NOTE_E3, 75,NOTE_F3, 75,NOTE_E3, 75);
          break;
      }
    } else {
      tiempoActNave++;
    }
  }
  switch (ataqueActual) {
    case 1:
      if (disparoX == 999) {
        disparoX = posXjefe - 7;
      }
      if (disparoX > 0 - 30) {
        disparoX -= 4;
      } else {
        disparoX = 999;
        ataqueActual = 0;
        tamanoDisparo=1;
      }
      if (tamanoDisparo < 7) {
        tamanoDisparo++;
      }
      for (int i = 1; i <= tamanoDisparo; i++) {
        if (i % 2 == 0) {
          arduboy.drawBitmap(disparoX-PISTOLA_ANCHO + 4 * i, posYjefe + 7, laser1Img, 4, 14);
        } else {
          arduboy.drawBitmap(disparoX-PISTOLA_ANCHO + 4 * i, posYjefe + 7, laser2Img, 4, 14);
        }
      }
      break;
    case 2:
      if (posXcohete == 999) {
        posXcohete = posXjefe + 4;
        posYcohete = posYjefe + 24;
      }
      if (!ataque) {
        if (posXcohete <= protaX + 7) {
          ataque = true;
          if (posYcohete < protaY) {
            atacaAbajo = true;
          } else {
            atacaAbajo = false;
          }
        } else {
          posXcohete -= 2;
        }
      } else {
        if (atacaAbajo) {
          posYcohete += 5;
        } else {
          posYcohete -= 5;
        }
        if (posYcohete <= INTERFAZ_ALTO || posYcohete > PANTALLA_ALTO) {
          posXcohete = 999;
          posYcohete = 999;
          ataque = false;
          ataqueActual = 0;
          atacaAbajo = false;
        }
      }
      arduboy.drawBitmap(posXcohete, posYcohete, coheteHImg, 8, 6);
      break;
    case 3:
      if (hangarFunciona) {
        hangarFunciona = false;
        naves[0]->viva = true;
        naves[0]->posX = posXjefe + 8;
        naves[0]->posY = posYjefe - 9;
        naves[0]->vida = 1000;
        naves[0]->tipo = rand() % 3;
        naves[0]->dinero = 0;

        //nave 1
        naves[0]->des = 0;
        naves[0]->enfriamiento = 30 + rand() % 15;
        naves[0]->calor = 0;
        if (naves[0]->tipo == 1 && naves[0]->posY < (PANTALLA_ALTO - NAVE_ALTO - INTERFAZ_ALTO) / 2) {
          naves[0]->bajando = true;
        } else {
          naves[0]->bajando = false;
        }

        //nave 2
        naves[0]->posXdetenida = 50 + rand() % 20;
        naves[0]->embestida = false;
        naves[0]->despY = 0;
        naves[0]->verdaderaY = naves[0]->posY;
        naves[0]->procesando = 0;
      }
      if (!naves[0]->viva) {
        hangarFunciona = true;
        ataqueActual = 0;
      }

      break;
  }
}

void creditosNave(short naveYCreditos) {
  arduboy.drawBitmap(naveXCreditos, naveYCreditos, protaImg, NAVE_ANCHO, NAVE_ALTO, WHITE);
  naveXCreditos++;
  if (naveDetenida && naveXCreditos > PANTALLA_ANCHO / 2 - NAVE_ANCHO / 2) {
    naveXCreditos--;
    if (arduboy.pressed(A_BUTTON)) {
      naveDetenida = false;
    }
  }
  if (naveXCreditos > PANTALLA_ANCHO) {
    naveXCreditos = -NAVE_ANCHO;
    estadoCreditos++;
    naveDetenida = true;
  }
}

void gestorCreditos() {
  switch (estadoCreditos) {
    case 0:
      creditosNave(10);
      arduboy.setCursor(43, 35);
      arduboy.print("THE END");
      break;
    case 1:
      creditosNave(40);
      arduboy.setCursor(40, 10);
      arduboy.println("CODE BY:");
      arduboy.setCursor(22, 18);
      arduboy.print("Ivan Villacampa");
      break;
    case 2:
      creditosNave(45);
      arduboy.setCursor(35, 15);
      arduboy.println("ARTWORK BY:");
      arduboy.setCursor(35, 23);
      arduboy.print("Marta Perez");
      break;
    case 3:
      creditosNave(5);
      arduboy.setCursor(10, 40);
      arduboy.print("THANKS FOR PLAYING");
      break;
    case 4:
      resetearJefe();
      estadoCreditos = 0;
      escenaActual = TIENDA;
      vida = vidaMax;
      progresoActual = 0;
      calor = 50;
      break;
  }
}

void gestorNivelAcabado() {
  gestorInterfaz();
  gestorNaveProta();
  arduboy.setCursor(65, 30);
  arduboy.print("LEVEL");
  arduboy.setCursor(65, 40);
  arduboy.print("COMPLETE");
  arduboy.setCursor(65, 52);
  if (nivelActual == 1) {
    arduboy.print("+   50");
  } else if (nivelActual == 2) {
    arduboy.print("+   100");
  }
  arduboy.drawBitmap(78, 51, dineroImg, 8, 8, WHITE);
  if (arduboy.pressed(A_BUTTON) && !teclaPresionada) {
    escenaActual = TIENDA;
    teclaPresionada = true;
  } else if (!arduboy.pressed(A_BUTTON)) {
    teclaPresionada = false;
  }
}

void gestorJuego() {
  gestorInterfaz();
  gestorBalas();
  gestorNaveProta();
  gestorNavesEnemigas();
}

void gestorMenu() {
  gestorInterfaz();
  gestorNaveProta();
  gestorBalas();
  arduboy.drawRect(PANTALLA_ANCHO / 2 + 2, INTERFAZ_ALTO + 2, PANTALLA_ANCHO / 2 - 4, 15);
  arduboy.setCursor(PANTALLA_ANCHO / 2 + 13, INTERFAZ_ALTO + 6);
  arduboy.print("LEVEL 1");
  if (nivelDesbloqueado > 1) {
    arduboy.drawRect(PANTALLA_ANCHO / 2 + 2, INTERFAZ_ALTO + 2 + 17, PANTALLA_ANCHO / 2 - 4, 15);
    arduboy.setCursor(PANTALLA_ANCHO / 2 + 13, INTERFAZ_ALTO + 6 + 17);
    arduboy.print("LEVEL 2");
  }
  if (nivelDesbloqueado > 2) {
    arduboy.drawRect(PANTALLA_ANCHO / 2 + 2, INTERFAZ_ALTO + 2 + 17 * 2, PANTALLA_ANCHO / 2 - 4, 15);
    arduboy.setCursor(PANTALLA_ANCHO / 2 + 13, INTERFAZ_ALTO + 6 + 17 * 2);
    arduboy.print("LEVEL 3");
  }
}

void gestorEscenas() {
  switch (escenaActual) {
    case JUEGO:
      gestorJuego();
      break;
    case TIENDA:
      gestorTienda();
      break;
    case MENU:
      gestorMenu();
      break;
    case FIN_NIVEL:
      gestorNivelAcabado();
      break;
    case JEFE:
      gestorJefe();
      break;
    case MUERTO:
      gestorMuerte();
      break;
    case CREDITOS:
      gestorCreditos();
      break;
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.initRandomSeed();
  for (int i = 0; i < N_BALAS; i++) {
    balas[i] = new bala();
  }
  for (int i = 0; i < N_NAVES; i++) {
    naves[i] = new nave();
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }
  arduboy.pollButtons();
  if (arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(RIGHT_BUTTON) || arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
    rand();
  }
  arduboy.clear();
  arduboy.setCursor(0, 0);
  gestorEscenas();
  arduboy.display();
}
