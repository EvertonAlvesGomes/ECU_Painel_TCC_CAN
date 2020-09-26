/* ECU_Painel.ino
 *  Firmware da ECU Painel do projeto CAN desenvolvido para a equipe CEFAST Baja.
 *  A ECU Painel se comunica via serial com o painel virtual desenvolvido em LabVIEW.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/



//********************************************************************************
//                             #includes
//********************************************************************************

#include "ECU_Painel_Defines.h"
#include "ECU_Painel_CAN.h"
#include "ECU_Painel_Timer.h"


//********************************************************************************
//                             Variáveis
//********************************************************************************

uint8_t tick = 0;       //temporizador de inicialização do CAN
int size_serial;

void setup(){

  tc0config();
  rc_compare_config(500); //tempo de envio das mensagens na serial
  //*pTC_RC0 = 42000*500;
  
  #ifdef ENABLE_SERIAL_COMMUNICATION
    Serial.begin(9600);
    Serial1.begin(9600);
  #endif

  #ifdef ENABLE_CAN_COMMUNICATION
    startCan();   //inicializa interface CAN
   
    canConfigMailboxRx(&can_mailbox_rx_a, MB_RX_A_INDEX, ID_MB_RX_A);   //configura mailbox rx A
    canConfigMailboxRx(&can_mailbox_rx_b, MB_RX_B_INDEX, ID_MB_RX_B);   //configura mailbox rx B
    canConfigMailboxRx(&can_mailbox_rx_c, MB_RX_C_INDEX, ID_MB_RX_C);   //configura mailbox rx C

    //Aguarda inicialização do CAN
    while(!(*pCAN_SR & (1 << 21))){
      tick++;
      if(tick >= TIMEOUT_CAN){
        #ifdef ENABLE_SERIAL_COMMUNICATION
          //Serial.println("Falha na inicialização do CAN.\n");
        #endif
        return;
      }
    }
    #ifdef ENABLE_SERIAL_COMMUNICATION
      //Serial.println("CAN Inicializado.\n\n");
    #endif
    
    can_enable_interrupts(ENABLE_MB0_INTERRUPT | ENABLE_MB1_INTERRUPT | ENABLE_MB2_INTERRUPT);

    
  #endif
  
}

void loop(){

  
  
  #ifdef ENABLE_CAN_COMMUNICATION

    //Se a comunicação serial estiver habilitada, os dados são escritos na serial
    #ifdef ENABLE_SERIAL_COMMUNICATION
      //can_monitoring();
      //enviar_dados_painel_virtual();
      //Serial.println(*pCAN_SR, HEX);
      //data_monitoring();
      //Serial.println(*pTC_CV0, HEX);
    #endif
    
  #endif
  
}



//********************************************************************************
//                             ISRs
//********************************************************************************

void TC0_Handler(void){
  enviar_dados_painel_virtual_2();
  //formatar_dados_painel_virtual();
  tc_sr = *pTC_SR0; //leitura do registrador de status para permitir o processador retornara pra thread mode
}

//CAN_ISR:
void CAN0_Handler(void){

  //RX mailbox A
  if((*pCAN_MSR_0 & 0xFFFFFF) >> 23){
    //se o flag MDRY estiver setado
    rec_status_1 = can_mailbox_read(CAN0, &can_mailbox_rx_a); //pressD-tempD-velD
  }
  
  //RX mailbox B
  if((*pCAN_MSR_1 & 0xFFFFFF) >> 23){
    rec_status_2 = can_mailbox_read(CAN0, &can_mailbox_rx_b); //pressT-tempT-rpm-velT
  }

  //RX mailbox C
  if((*pCAN_MSR_2 & 0xFFFFFF) >> 23){
    rec_status_3 = can_mailbox_read(CAN0, &can_mailbox_rx_c); //comb
  }
  
  can_sr = *pCAN_SR; //leitura do registrador de status (libera thread mode)
}
