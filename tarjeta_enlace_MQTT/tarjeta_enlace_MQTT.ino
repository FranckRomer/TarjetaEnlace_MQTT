#include <creacionEnviosMQTTyCAN.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <iostream>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

using namespace std;


// Update these with values suitable for your network.
const char* ssid = "RED ACCESA";
//const char* ssid = "DOMOTICA";
const char* password = "037E32E7";
//const char* password = "de la llave 2021 domotica";

const char* mqtt_server = "143.110.229.128";   // Servidor Remoto de DANA
//const char* mqtt_server = "164.92.75.200";   // Servidor Remoto de pruebas
//const char* mqtt_server = "192.168.1.70";   // Servidor Remoto de pruebas

// topic_basic = Accesa/project/ratify  // confirmar
// topic_basic = Accesa/project/admit   //  recibir
String rootTopic = "Accesa";
//String project = "joya";
String project = "laboratory";

String topicPubString = rootTopic + "/"+ project +"/ratify";
String topicSubString = rootTopic + "/"+ project +"/admit";
String clienteIDString = rootTopic + "_"+ project +"_admit" + random(1,100);

char path[] = "/";                    //no tiene otras direcciones 
const char* topic_sub = topicSubString.c_str();
const char* topic_pub = topicPubString.c_str();
const char* clienteID = clienteIDString.c_str();
/*
 *  OBJETOS
*/
EnvioCAN         CAN;
CAN_frame_t      rx_frame;
CAN_device_t     CAN_cfg;
String json;

//WiFiClient       client;

EnvioMqtt   cadenaMqtt;

/*
 * FUNCIONES PROTOTIPO
*/
void envioCAN(String cadenaTCP);

/*
 * VARIABLES Mqtt
*/


WiFiClient espClient;
PubSubClient client(espClient);

/*
 * VARIABLES GLOBALES
*/
unsigned long previousMillis = 0;
const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 10;       // Receive Queue size
 
byte    limiteEnvioInst = 0;
bool    banderaEscInst = false, banderaEscDia = false, banderaEscFecha = false;
bool    banderaFechaFin = false, configurarEntrada = false;
String  dataEscenario, dataConfiguracion;
char    dataSnd[15];

long lastMsg = 0;
char msg[50];
int value = 0;

int reseteo = 0;

/*
 * ********************************************************************
 *                                 Setup
 * ********************************************************************
*/

void setup() {

  Serial.begin(115200);
  //Serial.begin(92600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.publish(topic_pub,"Mensaje Recibido");
  
  /*
   * Conexion CAN
  */
  /* Seleccionar pines TX, RX y baudios */
  CAN_cfg.speed=CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_35;
  
  /* Crear una cola de 10 mensajes en el buffer del CAN */
  CAN_cfg.rx_queue = xQueueCreate(300,sizeof(CAN_frame_t));
  
  //INICIALIZAR MODULO CAN
  ESP32Can.CANInit();
  reseteo = 0;
}


/*
 * ********************************************************************
 * Programa principal
 * ********************************************************************
*/

void loop() {
  String data;
  String json_ouput;
  
  /*
   * MENSAJES RECIBIDOS POR EL BUS CAN
   */
  if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){
      
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("New standard frame");
      else
        printf("NUEVO MENSAJE CAN     ");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%.3x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf("ID CAN: 0x%.3x, No. BYTES: %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        Serial.print(char(rx_frame.data.u8[0]));Serial.print("|");
        Serial.print(rx_frame.data.u8[1]);Serial.print("|");
        Serial.print(rx_frame.data.u8[2]);Serial.print("|");
        Serial.print(rx_frame.data.u8[3]);Serial.print("|");
        Serial.print(rx_frame.data.u8[4]);Serial.print("|");
        Serial.print(rx_frame.data.u8[5]);Serial.print("|");
        Serial.print(rx_frame.data.u8[6]);Serial.print("|");
        Serial.println(rx_frame.data.u8[7]);
        if(rx_frame.MsgID == 255 || 1){
          switch(char(rx_frame.data.u8[0])){
            case '0': 
                      Serial.println("ACTIVACIÓN/DESACTIVACIÓN CONFIRMADA");
                      cadenaMqtt.envioActivacion(&rx_frame, dataSnd);
                      client.publish(topic_pub, dataSnd);
                      reseteo = 0;
                      // Transformacion y envio de Json
                      SerializeObject(dataSnd);

                      
                      break;
            
            default:  break;       
          }
        memset(dataSnd, 'x', 15);
        printf("\n");
        }
      }
  }

/*
 * ********************************************************************
 * Envio  MQTT
 * ********************************************************************
*/

  if (!client.connected()) {  
    reconnect();  
  }
  client.loop();
  long now = millis(); 
}
