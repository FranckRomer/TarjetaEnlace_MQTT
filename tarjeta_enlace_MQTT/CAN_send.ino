/*
 * ********************************************************************
 *                                   Envio CAN
 * ********************************************************************
*/

void envioCAN(String cadenaTCP){

  
  char destinatario[3];
  byte destinatarioByte;
  
  for(byte i=0; i<2; i++) destinatario[i] = cadenaTCP[3+i];
  destinatarioByte = CAN.x2i(destinatario);
  rx_frame.FIR.B.FF = CAN_frame_ext;
  rx_frame.MsgID = destinatarioByte;
  rx_frame.FIR.B.DLC = 8;  
  
  //ENVIO CAN
  ESP32Can.CANWriteFrame(&rx_frame);
  delay(200);
  ESP32Can.CANStop(); 
  ESP32Can.CANInit();  
}
