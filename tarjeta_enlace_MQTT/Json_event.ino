/*
 * ************************************************************************************************
 *                                          JSON
 * ************************************************************************************************
*/
//Serializacion
void SerializeObject(String json) {
    String dataSnd_string;
    char buffer[1024];
    dataSnd_string = String(dataSnd);
    StaticJsonDocument<1024> doc;
    doc["can"]        = (dataSnd_string.substring(3,5));
    doc["pin"]        = (dataSnd_string.substring(5,6));
    doc["percentage"] = (dataSnd_string.substring(6,9));
    doc["rgb"]        = (dataSnd_string.substring(11,14));

    size_t n = serializeJson(doc, buffer);
    client.publish(topic_pub, buffer, n);
}

// Deserializacion 
void DeserializeObject(String dato_json) {   
  //String envio_json_can = "{\"can\":\"09\",\"pin\":\"1\",\"stauts\":true,\"percentage\":100,\"rgb\":\"xxx\"}";
  String envio_json_can = dato_json;
    StaticJsonDocument<300> doc;
    
    DeserializationError error = deserializeJson(doc, envio_json_can);
    if (error) { return; }
    String can        = doc["can"];
    String pin        = doc["pin"];
    String percentage = doc["percentage"];
    String rgb        = doc["rgb"];

//    if(rgb == "000"){
//      rgb = "xxx";
//    }
    
    envio_json_can = "FF1"+ can + pin + percentage + "00" + rgb;
    //envio_json_can = "FF009210000xxx";

    Serial.println("------------------json desarmado------------------");
    Serial.println("CAN: " + can);
    Serial.println("PIN: " + pin);
    Serial.println("PORCENTAJE: " + percentage);
    Serial.println("RGB: " + rgb);
    Serial.println(envio_json_can);
    Serial.println("--------------------------------------------------\n");
    
    
    delay(100);

//    if ( reseteo >= 3){
//      reseteo = 0;
//      ESP.restart();
//    }
    
    CAN.envioActivacion(envio_json_can, &rx_frame);
    envioCAN(envio_json_can);
    
}
