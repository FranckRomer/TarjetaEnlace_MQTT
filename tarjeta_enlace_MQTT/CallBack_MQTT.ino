/*
 * ********************************************************************
 *                              CallBack
 * ********************************************************************
*/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived MQTT [");
  Serial.print(topic);
  Serial.print("] ");
  
  char MQTT_to_CAN[length] ;
  String payload_string = "";
  String Lampara_Encender = "";
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Lampara_Encender = Lampara_Encender + (char)payload[i];
    
  }
  Serial.println();

  //Deserializacion de Json y envio a red CAN
  DeserializeObject(Lampara_Encender);

}
