/*  ----------------------------------------------------------------
    SETISAEDU
    Codigo de prueba de transmision a larga distancia con NRF2401+.
    Codigo del Receptor
  --------------------------------------------------------------------
*/
#include <SPI.h>      //Libreria para usar el modulo SPI 
#include "nRF24L01.h" //Librerias del modulos nRF24L01+
#include "RF24.h"

RF24 radio(9, 10);//Declaracion de los pines de control CE y CSN para el modulo, se define "radio"

// Definicion de las direcciones de comunicacion
const uint64_t direccion1 = 0x7878787878LL;
const uint64_t direccion2 = 0xB3B4B5B6F1LL;
//Declaracion de variables y pines a usar
const char enviando = 4; //Pin indicador de envio
const char recibiendo = 5; //Pin indicador de recepcion

unsigned int RX[128];
//unsigned char TX[128];

bool TX_state = 0;

void setup(void)
{
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(115200);//Inicializacion del puerto serial en el caso de tener depuracion por PC

  radio.begin(); //Inicio del modulo nRF24L01+
  radio.setPALevel(RF24_PA_MAX);  // Configuracion en modo de baja potencia
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100); // Configuracion del canal de comunicacion
  //Apertura de los canales de comunicacion
  radio.openWritingPipe(direccion2);
  radio.openReadingPipe(1, direccion1);

  radio.startListening(); //Iniciamos en modo de escucha

  //Configuracion de pines de salida de los indicadores
  pinMode(enviando, OUTPUT);
  pinMode(recibiendo, OUTPUT);
  digitalWrite(enviando, LOW);
  digitalWrite(recibiendo, LOW);
  //Mensaje inicial
  Serial.println("Sistema de verificacion de coherencia en");
  Serial.println("envio de datos con el modulo  nRF24L01+");
  for (int x = 0; x < 128; x++)
  {
    RX[x] = 0;
  }

}

int j = 0, error_cont = 0;

void loop(void)
{

  if (radio.available())      // Comprobacion de datos recibidos
  {
    //Empezando recepcion
    digitalWrite(recibiendo, HIGH);
    bool done = false;
    unsigned long inicio = millis();

    radio.read( &RX[j], sizeof(RX[j])); //Funcion de lectura de datos
    Serial.print("Dato ");
    Serial.print(j);
    Serial.print(" recibido: ");
    Serial.println(RX[j]);
    digitalWrite(recibiendo, LOW);

    //Empezando el envio
    digitalWrite(enviando, HIGH);
    radio.stopListening(); // Paramos el modo de escucha para poder escribir
    bool ok = radio.write(&RX[j], sizeof(RX[j])); //Envio de datos
    if (!ok)
    {
      Serial.print("fallo el envio en el dato ");
      Serial.println(j);
      //return;
    } else 
    {
      Serial.print("Dato ");
      Serial.print(j);
      Serial.print(" enviado: ");
      Serial.println(RX[j]);
    }
    radio.startListening(); //Volvemos a la escucha
    digitalWrite(enviando, LOW);
    // delay(100);
    j++;
    if (j >= 128)
    {
      Serial.println("Fin de la transmision");
      for (int x = 0; x < 128; x++)
      {
        RX[x] = 0;
      }
      j = 0;
    }
  }
}
