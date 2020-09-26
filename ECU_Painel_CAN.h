/* ECU_Painel_CAN.h
 *  Implementação do CAN para ECU Painel.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/

#ifndef _ECU_PAINEL_CAN_H
#define _ECU_PAINEL_CAN_H

//********************************************************************************
//                             #includes
//********************************************************************************

#include "D:\Documentos\CEFET-MG\Disciplinas\9 Periodo\TCC2\Projeto\Firmware\Final\CanCefast\CanCefast.h"
#include "ECU_Painel_Timer.h"

//********************************************************************************
//                             #defines
//********************************************************************************

//IDs das mensagens:
/*#define ID_MSG_1          0x2702        //pressD | tempD | velD
#define ID_MSG_2            0x3066        //pressT | tempT | rpm | velT
#define ID_MSG_3            0xEA27        //com*/

//IDs das mailboxes:
#define ID_MB_RX_A          ID_MSG_1      //ID mailbox rx A
#define ID_MB_RX_B          ID_MSG_2      //ID mailbox rx B
#define ID_MB_RX_C          ID_MSG_3      //ID mailbox rx C

//Índices das mailboxes: 0x00 a 0x07 para microcontroladores SAM3X8E
#define MB_RX_A_INDEX       0x00        //Índice Mailbox rx A
#define MB_RX_B_INDEX       0x01        //Indice mailbox rx B
#define MB_RX_C_INDEX       0x02        //Indice mailbox rx C

//Masks para habilitar interrupção das mailboxes
#define ENABLE_MB0_INTERRUPT  (1 << 0)      //Habilita interrupção da mailbox 0
#define ENABLE_MB1_INTERRUPT  (1 << 1)      //Habilita interrupção da mailbox 1
#define ENABLE_MB2_INTERRUPT  (1 << 2)      //Habilita interrupção da mailbox 2
#define ENABLE_MB3_INTERRUPT  (1 << 3)      //Habilita interrupção da mailbox 3
#define ENABLE_MB4_INTERRUPT  (1 << 4)      //Habilita interrupção da mailbox 4
#define ENABLE_MB5_INTERRUPT  (1 << 5)      //Habilita interrupção da mailbox 5
#define ENABLE_MB7_INTERRUPT  (1 << 6)      //Habilita interrupção da mailbox 6
#define ENABLE_MB8_INTERRUPT  (1 << 7)      //Habilita interrupção da mailbox 7


//********************************************************************************
//                             Variáveis
//********************************************************************************

//Variáveis de monitoramento
uint8_t rec_status_1, rec_status_2, rec_status_3;
uint32_t can_sr=0;

byte packet[30];


//Mailboxes:
can_mb_conf_t can_mailbox_rx_a;                //mailbox rx_a
can_mb_conf_t can_mailbox_rx_b;                //mailbox rx_b
can_mb_conf_t can_mailbox_rx_c;                //mailbox rx_c

//********************************************************************************
//                             Funções
//********************************************************************************

void configura_envio_painel(uint16_t delayTime){
  tc0config();
  rc_compare_config(delayTime);
}

/* uint16_t splitToRead
 *  Seleciona o dado nos registradores de dados das mailboxes de recepcao.
 *  uint8_t pos = varia de 0 a 7 e indica qual dado deve ser selecionado
*/
uint16_t splitToRead(uint8_t pos){
  uint16_t retval;

  switch(pos) {
    //Mailbox rx_a:
    //case 0 -> velocidade dianteira
    case 0 : {
      retval = ((can_mailbox_rx_a.ul_datal) & (0x0000FFFF));
      break;
    }
    //case 1 -> temperatura dianteira
    case 1 : {
      retval = (uint16_t)((can_mailbox_rx_a.ul_datal & 0xFFFF0000) >> 16);
      break;
    }
    //case 2 -> pressão dianteira
    case 2 : {
      retval = ((can_mailbox_rx_a.ul_datah) & (0x0000FFFF));
      break;
    }

    //Mailbox rx_b:
    //case 3 -> velocidade traseira
    case 3 : {
      retval = ((can_mailbox_rx_b.ul_datal) & (0x0000FFFF));
      break;
    }
    //case 4 -> rpm
    case 4 : {
      retval = (uint16_t)(((can_mailbox_rx_b.ul_datal) & (0xFFFF0000)) >> 16);
      break;
    }
    //case 5 -> temperatura traseira
    case 5 : {
      retval = ((can_mailbox_rx_b.ul_datah) & (0x0000FFFF));
      break;
    }
    //case 6 -> pressão traseira
    case 6 : {
      retval = (uint16_t)(((can_mailbox_rx_b.ul_datah) & (0xFFFF0000)) >> 16);
      break;
    }

    //Mailbox rx_c
    //case 7 -> combustível
    case 7: {
      retval = ((can_mailbox_rx_c.ul_datal) & (0x0000FFFF));
      break;
    }
  }
  return retval;
  
}


void data_monitoring(){

  //Mailboxes de recepção:
  Serial.print("VD: ");
  Serial.println(splitToRead(0));
  Serial.print("TEMPD: ");
  Serial.println(splitToRead(1));
  Serial.print("PRESSD: ");
  Serial.println(splitToRead(2));
  Serial.print("VT: ");
  Serial.println(splitToRead(3));
  Serial.print("RPM: ");
  Serial.println(splitToRead(4));
  Serial.print("TEMPT: ");
  Serial.println(splitToRead(5));
  Serial.print("PRESST: ");
  Serial.println(splitToRead(6));
  Serial.print("COMB: ");
  Serial.println(splitToRead(7));
  Serial.println("");

}

void can_monitoring(){
  
  //Mailboxes de recepção:
  Serial.print("RX_A: ");
  Serial.print(can_mailbox_rx_a.ul_datah, HEX);
  Serial.print(" ");
  Serial.println(can_mailbox_rx_a.ul_datal, HEX);

  Serial.print("RX_B: ");
  Serial.print(can_mailbox_rx_b.ul_datah, HEX);
  Serial.print(" ");
  Serial.println(can_mailbox_rx_b.ul_datal, HEX);

  Serial.print("RX_C: ");
  Serial.print(can_mailbox_rx_c.ul_datah, HEX);
  Serial.print(" ");
  Serial.println(can_mailbox_rx_c.ul_datal, HEX);

  Serial.println("");
}


void enviar_dados_painel_virtual(){
  Serial.print(can_mailbox_rx_c.ul_datah, HEX);
  Serial.print(",");
  Serial.print(can_mailbox_rx_c.ul_datal, HEX);
  Serial.print(",");
  Serial.print(can_mailbox_rx_b.ul_datah, HEX);
  Serial.print(",");
  Serial.print(can_mailbox_rx_b.ul_datal, HEX); 
  Serial.print(",");
  Serial.print(can_mailbox_rx_a.ul_datah, HEX);
  Serial.print(",");
  Serial.print(can_mailbox_rx_a.ul_datal, HEX);
  Serial.print("\n");
}

void enviar_dados_painel_virtual_2(){
  for(int i=0; i<8; i++){
    Serial.print(splitToRead(i));
    Serial.print(",");
  }
  Serial.print("\n");
}


void formatar_dados_painel_virtual(){
  //packet = "";
  int i = 0;
  for(i=0; i<5; i++){
    if(i == 0){
        packet[5*i] = (byte)can_mailbox_rx_a.ul_datal & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_a.ul_datal & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_a.ul_datal & 0x00FF0000) >> 10);
        packet[5*i+3] = (byte)((can_mailbox_rx_a.ul_datal & 0xFF000000) >> 18);
        packet[5*i+4] = (byte)'t';
    }
    if(i == 1){
        packet[5*i] = (byte)can_mailbox_rx_a.ul_datah & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_a.ul_datah & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_a.ul_datah & 0x00FF0000) >> 10);
        packet[5*i+3] = (byte)((can_mailbox_rx_a.ul_datah & 0xFF000000) >> 18);
        packet[5*i+4] = (byte)'t';
    }
    if(i == 2){
        packet[5*i] = (byte)can_mailbox_rx_b.ul_datal & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_b.ul_datal & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_b.ul_datal & 0x00FF0000) >> 10);
        packet[5*i+3] = (byte)((can_mailbox_rx_b.ul_datal & 0xFF000000) >> 18);
        packet[5*i+4] = (byte)'t';
    }
    if(i == 3){
        packet[5*i] = (byte)can_mailbox_rx_b.ul_datah & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_b.ul_datah & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_b.ul_datah & 0x00FF0000) >> 10);
        packet[5*i+3] = (byte)((can_mailbox_rx_b.ul_datah & 0xFF000000) >> 18);
        packet[5*i+4] = (byte)'t';
    }
    if(i == 4){
        packet[5*i] = (byte)can_mailbox_rx_c.ul_datal & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_c.ul_datal & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_c.ul_datal & 0x00FF0000) >> 16);
        packet[5*i+3] = (byte)((can_mailbox_rx_c.ul_datal & 0xFF000000) >> 24);
        packet[5*i+4] = (byte)'t';
    }
    if(i == 5){
        packet[5*i] = (byte)can_mailbox_rx_c.ul_datah & 0x000000FF;
        packet[5*i+1] = (byte)((can_mailbox_rx_c.ul_datah & 0x0000FF00) >> 8);
        packet[5*i+2] = (byte)((can_mailbox_rx_c.ul_datah & 0x00FF0000) >> 16);
        packet[5*i+3] = (byte)((can_mailbox_rx_c.ul_datah & 0xFF000000) >> 24);
        packet[5*i+4] = (byte)'t';
    }
    
  }
  /*
   * packet[0] = (byte)can_mailbox_rx_a.ul_datal & 0x000000FF;
   * packet[1] = (byte)((can_mailbox_rx_a.ul_datal & 0x0000FF00) >> 8);
   * packet[2] = (byte)((can_mailbox_rx_a.ul_datal & 0x00FF0000) >> 16);
   * packet[3] = (byte)((can_mailbox_rx_a.ul_datal & 0xFF000000) >> 24);
   * packet[4] = '\t';
   * packet[5] = (byte)can_mailbox_rx_a.ul_datah & 0x000000FF;
   * packet[6] = (byte)((can_mailbox_rx_a.ul_datah & 0x0000FF00) >> 8);
   * packet[7] = (byte)((can_mailbox_rx_a.ul_datah & 0x00FF0000) >> 16);
   * packet[8] = (byte)((can_mailbox_rx_a.ul_datah & 0xFF000000) >> 24);
   * packet[9] = '\t';
   * packet[10] = (byte)can_mailbox_rx_b.ul_datal & 0x000000FF;
   * packet[11] = (byte)((can_mailbox_rx_b.ul_datal & 0x0000FF00) >> 8);
   * packet[12] = (byte)((can_mailbox_rx_b.ul_datal & 0x00FF0000) >> 16);
   * packet[13] = (byte)((can_mailbox_rx_b.ul_datal & 0xFF000000) >> 24);
   * packet[14] = '\t';
   * packet[15] = (byte)can_mailbox_rx_b.ul_datah & 0x000000FF;
   * packet[16] = (byte)((can_mailbox_rx_b.ul_datah & 0x0000FF00) >> 8);
   * packet[17] = (byte)((can_mailbox_rx_b.ul_datah & 0x00FF0000) >> 16);
   * packet[18] = (byte)((can_mailbox_rx_b.ul_datah & 0xFF000000) >> 24);
   * packet[19] = '\t';
   * packet[20] = (byte)can_mailbox_rx_c.ul_datal & 0x000000FF;
   * packet[21] = (byte)((can_mailbox_rx_c.ul_datal & 0x0000FF00) >> 8);
   * packet[22] = (byte)((can_mailbox_rx_c.ul_datal & 0x00FF0000) >> 16);
   * packet[23] = (byte)((can_mailbox_rx_c.ul_datal & 0xFF000000) >> 24);
   * packet[24] = '\t';
   * packet[25] = (byte)can_mailbox_rx_c.ul_datah & 0x000000FF;
   * packet[26] = (byte)((can_mailbox_rx_c.ul_datah & 0x0000FF00) >> 8);
   * packet[27] = (byte)((can_mailbox_rx_c.ul_datah & 0x00FF0000) >> 16);
   * packet[28] = (byte)((can_mailbox_rx_c.ul_datah & 0xFF000000) >> 24);
   * packet[29] = '\t';
   */

   for(i=29; i > -1; i--){
    Serial.print((char)packet[i], HEX);
   }
   //Serial.print(packet);
   Serial.print("\n");
 
}

void formatar_dados_struct_painel_virtual(){
  
}

void canStatus(){
  Serial.println(*pCAN_SR, HEX);
}


#endif
