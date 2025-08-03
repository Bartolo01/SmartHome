#include <esp_now.h>
#include <WiFi.h>

#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>




//uint8_t slaveX_address[] = {0x,0x,0x,0x,0x,0x};
uint8_t master_address[] = {0xC4,0x4F,0x33,0x39,0x5A,0xFD};
esp_now_peer_info_t master;




/** COMMUNICATION WITH MASTER **/
typedef struct struct_data {

  byte id = 0;
  byte code = 101;
  byte mode[4];   //MODES (0-Off : 1-ON/OFF : 2-PWM : 3-Sensor : 4-Sprinkler)

  bool state[4];
  byte duty[4];
  int power[4];
  int runtime[4];

  bool automation[4];
  int starttime[4];
  int duration[4];
  uint8_t days[4];
  
} struct_data;

struct_data dataincoming;    //Data received
struct_data databuffer;      //Data buffer for checking origin

struct_data dataonboard;     //Data it's own




/** INPUTS AND OUTPUTS **/
  #define button 9  //Encoder button
  bool last_buttonstate = LOW;
  unsigned long last_buttonpress;
  #define enc1 8    //Encoder terminal 1
  #define enc2 10   //Encoder terminal 2
  byte encoderduty;
  byte last_encoderduty;
  bool last_encoderstate;
  

  #define switch0 4
  #define switch1 5
  #define switch3 7
  #define switch2 6
  byte switchPin[4] = {switch0, switch1, switch2, switch3};

  #define current0_Pin 0
  #define current1_Pin 1
  #define current2_Pin 2
  #define current3_Pin 3
  byte currentPin[4] = {current0_Pin, current1_Pin, current2_Pin, current3_Pin};
  
  #define led 3

  
  int current0;             int current1;             int current2;             int current3;
  float current0_adj = 1;   float current1_adj = 1;   float current2_adj = 1;   float current3_adj = 1;
  int current0_count;       int current1_count;       int current2_count;       int current3_count;

  int current[4] = {current0, current1, current2, current3};
  float current_adj[4] = {current0_adj, current1_adj, current2_adj, current3_adj};
  int current_count[4] = {current0_count, current1_count, current2_count, current3_count};
//


void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);



/** SETTING UP ESP-NOW **/
  esp_now_init();
  if(esp_now_init() != ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
  // Register peer
  memcpy(master.peer_addr, master_address, 6);
  master.channel = 1;  
  master.encrypt = false;

  // Add peer  
  if(esp_now_add_peer(&master) != ESP_OK){ 
    Serial.println("Failed to add peer");    
    return;
  }
//



/** CONFIG OF PINS **/
  ledcSetup(0, 2000, 8);    //Frequency of 2kHz and 8bit duty

  pinMode(switch0, OUTPUT);
  //ledcAttachPin(switch0, 0);
  pinMode(switch1, OUTPUT);
  //ledcAttachPin(switch1, 1);
  pinMode(switch2, OUTPUT);
  //ledcAttachPin(switch2, 2);
  pinMode(switch3, OUTPUT);
  //ledcAttachPin(switch3, 3);

  pinMode(led, OUTPUT);

  pinMode(button, INPUT);
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);
//


  /** MODES (0-Off : 1-Relay : 2-Transistor : 3-Sensor : 4-Sprinkler) **/
  dataonboard.mode[0] = 1; dataonboard.mode[1] = 2; dataonboard.mode[2] = 3; dataonboard.mode[3] = 0;   

}



unsigned long lastupdate = millis();

void loop() {


  
  if(millis()-lastupdate>=1000){
    lastupdate = millis();

    /** TRACKING RUNTIME OF CHANNELS **/
    for(int channel=0; channel<4; channel++){
      if(dataonboard.state[channel]==1){
        dataonboard.runtime[channel]++;
      }
      else if(dataonboard.state[channel]==0){
        dataonboard.runtime[channel]=0;
      }
    }

  }


  
  /** CHANGING BUTTON INTO ON/OFF SWITCH **/
  byte channel=0;
  if(digitalRead(button) == LOW && last_buttonstate==LOW){
    if(millis()-last_buttonpress >= 200){
      last_buttonpress=millis();
      last_buttonstate = HIGH;

      dataonboard.state[channel]=!dataonboard.state[channel];
    }
  }
  else if(digitalRead(button) == HIGH && last_buttonstate == HIGH){
    last_buttonstate=LOW;
  }
  


  
  /** TRACKING ENCODER POSITION **//*
  byte channel = 0;
  if(dataonboard.state[channel] == HIGH){         //Optional
    if(digitalRead(enc1) != last_encoderstate){
      if(digitalRead(enc2) != digitalRead(enc1)){
        if(encodercount==255){
          encodercount = encodercount-5;
        }
        encodercount = encodercount+5;
      }
      else{
        if(encodercount==0){
          encodercount = encodercount+5;
        }
        encodercount = encodercount-5;
      }

      dataonboard.duty[channel]=encodercount;

    }
    last_encoderstate=digitalRead(enc1);
  }
  /*




  /** USING OUTPUTS FOR STUFF **/
  digitalWrite(led ,dataonboard.state[0]);



  /** MEASURING CURRENT OF ACTIVE CHANNELS **/
  for(int channel=0; channel<4; channel++){
    if(dataonboard.mode[channel] != 0){
      if(dataonboard.state[channel]==HIGH || dataonboard.mode[channel]==3){
        handleCurrent(channel);
      }
      else{
        current[channel]=0;
      }
    }
  }


}




/** CURRENT MEASURMENT FOR ANY CHANNEL **/
void handleCurrent(byte channel){

  current[channel] = current[channel] + analogRead(currentPin[channel]);
  current_count[channel]++;
  if(current_count[channel]==500){
    current[channel] = current[channel]/500;
    current[channel] = current[channel]/1.241;      //Conversion to mV

    //if relay and acs712
    if(dataonboard.mode[channel]==1){         
      current[channel] = current[channel]*2;          //Voltage divider
      current[channel] = current[channel]*current_adj[channel];
      current[channel] = (current[channel]-2500);     //Conversion from V to mA
    }
    //if transistor and 0.1R shunt
    else if(dataonboard.mode[channel]==2){    
      current[channel] = current[channel]/0.1*current_adj[channel];
    }
    //if thermocuple 10k with 10k voltage divider
    else if(dataonboard.mode[channel]==3){    
      current[channel] = current[channel]*10000 / (3300-current[channel]);          //Conversion to R of thermocouple
      current[channel] = 1 / ((1/298) + (1/3950) * log(current[channel] / 10000));  //Conversion from R to K
      current[channel] = current[channel] - 273;
    }

    if(current[channel]<0){
      current[channel]=0;
    }
    dataonboard.power[channel] = current[channel];
    current[channel]=0;
    current_count[channel]=0;
  }

}




void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){

  Serial.print("Send Status: "); Serial.println(status == 0 ? "Succeeded" : "Failed");

}


void OnDataRecv(const uint8_t * mac, const uint8_t *dataincoming, int len){

  memcpy(&databuffer, dataincoming, sizeof(dataincoming));
  Serial.print("Data received, Code: ");  Serial.println(databuffer.code);

  if(databuffer.code == 100){   //Giving statistics
    
    dataonboard.code = 100;
    esp_now_send(master_address, (uint8_t *) &dataonboard, sizeof(dataonboard));

  }
  else if(databuffer.code >= 101){  //Changing something
    
    memcpy(&dataonboard, dataincoming, sizeof(dataonboard));

  }

}

