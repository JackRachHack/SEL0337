#include <Arduino.h>

// Define pino do LED (troque para D4 se quiser usar o LED interno do NodeMCU), D5 é um LED externo.
#define LEDpin 2

/*********Definições do Wifi***********/
const int max_networks = 2; // Número máximo de redes disponíveis para uso no local, a fim de garantir redundância para conexão Wi-Fi
const char* wifi_ssids[max_networks] = {"jacksonhotspot", "LabMicros"}; // Adicione os dados das redes nestes dois vetores
const char* wifi_passwords[max_networks] = {"jack20wagner", "seluspeesc@"}; // Certifique-se de que tiver X redes, terão X IDs e X senhas
/*********************************************/

/**********Definições Google Sheets***********/
#include "TRIGGER_WIFI.h"               // Inclui ESP8266WiFi.h e WiFiClientSecure.h
#include "TRIGGER_GOOGLESHEETS.h"       // Arquivo da biblioteca para o Google Sheets

char column_name_in_sheets[][8] = {"value1", "value2", "value3", "value4"}; // Adicione mais "valueX" a medida que quiser modificar a quantidade de parâmetros (mudanças no Apps Script necessárias)
String Sheets_GAS_ID = "AKfycbwN3651-nx6dKNLBjllL98i62__oIwyJP6sFfJbLtI"; // Deployment ID da vesão 7 do Apps Script que recebe dados para a planilha (tirado na opção "manage deployments")
int No_of_Parameters = 4;                                                               
/*********************************************/

/*********Definições da comunicação***********/
//#include <SoftwareSerial.h>              // Biblioteca para usar conexão serial pelos pinos 2 e 3

//SoftwareSerial Esp8266(D2, D3);          // Configura uma nova porta serial usando os pinos D2 e D3 (RX, TX)

float vetRx[4];                          // Vetor para armazenar dados a serem enviados para a planilha
int i = 0;
int control = 1;                         // Variável para controlar o acesso ao loop de recepção e o if de envio

float val;                               // Variável para armazenar temporariamente o dado
/*********************************************/

void setup() 
{
    Serial.begin(9600);          // Inicializa a comunicação serial padrão
    while (!Serial);

 //   pinMode(D2, INPUT);          // Configura um pino para ser entrada
 //   pinMode(D3, OUTPUT);         // Configura um pino para ser saída
 //   pinMode(LEDpin, OUTPUT);       // Configura um pino para ser saída (usar um LED de indicação)

    WIFI_Connect();             // Função para conectar ao Wi-Fi por tentativas
    Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters); // Inicializa o Google Sheets

 //   Esp8266.begin(4800);        // Inicializa a conexão serial extra
}

void loop() 
{
    // Verifica a conexão do Wi-Fi
    if (WiFi.status() != WL_CONNECTED) { // Led piscando lentamente: o módulo está tentando se conectar a uma rede Wi-Fi listada no início
        WIFI_Connect();                  // Led apagado: Wi-Fi conectado
    }
    
    /*while (true && control == 0) {//while ((Esp8266.available() > 0) && control == 0) {  // Verificando se a porta está sendo usada e, se sim, coleta os dados
        float val = 23;//float val = Esp8266.parseFloat();
        char recebidoChar = Esp8266.read(); // Armazena o caractere lido
        
        if (recebidoChar == '\n' || recebidoChar == 'S') {

          if (recebidoChar == 'S') i = 0; // Reinicia o índice quando "S" é recebido

          Serial.print ("Dado:");
          Serial.println(val); // Mostra no monitor serial o dado recebido
          vetRx[i] = val;     // Armazena dado recebido de 4 em 4, sendo o último um identificador de dispositivo
          i++;

          if ((i == 4) && (vetRx[3] == 1.00 || vetRx[3] == 2.00 || vetRx[3] == 3.00)){ // Se é um dos identificadores válidos recebe permissão
            control = 1;  // Permite entrada à função de envio
          }
          if (i == 4) i = 0;  // Reinicia a variável que auxilia no uso do vetor de dados (vetRx) (garantia extra para sincronização)
        }
    }*/

    // Adding dumb numbers
    vetRx[0] = 23;
    vetRx[1] = 24;
    vetRx[2] = 25;
    vetRx[3] = 26;

    if (control == 1) { // Verifica, através do dado identificador, se é realmente um dado de um dos módulos                                        
        Data_to_Sheets(No_of_Parameters, vetRx[0], vetRx[1], vetRx[2], vetRx[3]); // Envia os dados para a planilha

        if (!falha){ // Se não falhou mantém Led pagado
          digitalWrite (LEDpin, LOW);
          delay(5000); // Espera 30 segundos para enviar o próximo dado
        } else {    // Se falhou, acende o Led 
          digitalWrite (LEDpin, HIGH);
        }
        //vetRx[3] = 0; // Restaura um valor neutro ao identificador de dispositivo para inibir reentrada desnecessária
    }
    control = 1;
}
