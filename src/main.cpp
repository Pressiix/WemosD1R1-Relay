#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#include <MicroGear.h>

#define APPID   "NETPIE2RELAY"
#define KEY     "0xoSHpipDcGlkZT"
#define SECRET  "ygfA2idGUqYtU1lJkd6xrRp9c"
#define ALIAS   "Relay" 

#define RELAY_PIN1 D2
#define RELAY_PIN2 D8

WiFiClient client;
MicroGear microgear(client);
const char* ssid = "TOTWIFI_2.4G"; 
const char* password = "036389356"; 
const char* wifi_Check_status = "";
long timer = 0;
const char* check_relay_status = "4";
bool reconnect = false;
const char* reconnected = "0";

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    String iMsg = "";
      for (int i = 0; i < msglen; i++) {
        iMsg += (char)msg[i];
      }
  if(iMsg == "ON1" || iMsg == "ON2" || iMsg == "ONALL" || iMsg == "OFFALL"){
    if (iMsg == "ON1") 
    {
        digitalWrite(RELAY_PIN1, HIGH);
        digitalWrite(RELAY_PIN2, LOW);
    } 
      if (iMsg == "ON2") {
        digitalWrite(RELAY_PIN1, LOW);
        digitalWrite(RELAY_PIN2, HIGH);
      }
      if (iMsg == "ONALL") {
        digitalWrite(RELAY_PIN1, HIGH);
        digitalWrite(RELAY_PIN2, HIGH);
      }
      if (iMsg == "OFFALL") {
        digitalWrite(RELAY_PIN1, LOW);
        digitalWrite(RELAY_PIN2, LOW);
      }
  }
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    microgear.setAlias(ALIAS);
}

void setup() {
      pinMode(RELAY_PIN1, OUTPUT);
      pinMode(RELAY_PIN2, OUTPUT);
      digitalWrite(RELAY_PIN1, LOW);
      digitalWrite(RELAY_PIN2, LOW);
      //check_relay_status = "4";

      lcd.init();                      // initialize the lcd 
      lcd.backlight();
      lcd.print("Switch 1 : OFF");
      lcd.setCursor(0, 1);
      lcd.print("Switch 2 : OFF");
      lcd.setCursor(0, 2);

      WiFi.disconnect(); // Disconnect AP
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);   //Connect WIFI

      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
      }
      microgear.on(MESSAGE,onMsghandler);
      microgear.on(CONNECTED,onConnected);
      microgear.init(KEY,SECRET,ALIAS);
      microgear.connect(APPID);
      microgear.subscribe("/relaystat");
      microgear.publish("/relaystat","4",true);
      wifi_Check_status = "connect";
}

void loop() {
          if(check_relay_status){ 
              if(digitalRead(RELAY_PIN1)&& !digitalRead(RELAY_PIN2)){  
                if(check_relay_status != "1"){  //if relay1 status = OFF and if relay2 status = OFF
                  check_relay_status = "1";   //set relay1 status = ON and relay2 status = OFF
                  microgear.subscribe("/relaystat");
                  microgear.publish("/relaystat","1",true);
                  lcd.clear();
                  lcd.print("Switch 1 : ON");
                  lcd.setCursor(0, 1);
                  lcd.print("Switch 2 : OFF");
                  lcd.setCursor(0, 2); 
                }
              }

              if(!digitalRead(RELAY_PIN1)&& digitalRead(RELAY_PIN2)){  
                if(check_relay_status != "2"){  //if relay1 status = OFF and if relay2 status = OFF
                  check_relay_status = "2";   //set relay1 status = ON and relay2 status = OFF
                  microgear.subscribe("/relaystat");
                  microgear.publish("/relaystat","2",true);
                  lcd.clear();
                  lcd.print("Switch 1 : OFF");
                  lcd.setCursor(0, 1);
                  lcd.print("Switch 2 : ON");
                  lcd.setCursor(0, 2);
                }
              }

              if(digitalRead(RELAY_PIN1)&& digitalRead(RELAY_PIN2)){  
                if(check_relay_status != "3"){  //if relay1 status = OFF and if relay2 status = OFF
                  check_relay_status = "3";   //set relay1 status = ON and relay2 status = OFF
                    microgear.subscribe("/relaystat");
                    microgear.publish("/relaystat","3",true);
                    lcd.clear();
                    lcd.print("Switch 1 : ON");
                    lcd.setCursor(0, 1);
                    lcd.print("Switch 2 : ON");
                    lcd.setCursor(0, 2);
                } 
              }

              if(!digitalRead(RELAY_PIN1)&& !digitalRead(RELAY_PIN2)){  
                if(check_relay_status != "4"){  //if relay1 status = OFF and if relay2 status = OFF
                  check_relay_status = "4";   //set relay1 status = ON and relay2 status = OFF
                    microgear.subscribe("/relaystat");
                     microgear.publish("/relaystat","4",true);
                     lcd.clear();
                     lcd.print("Switch 1 : OFF");
                     lcd.setCursor(0, 1);
                     lcd.print("Switch 2 : OFF");
                     lcd.setCursor(0, 2);
                }
                else if(reconnected == "1")
                {
                  microgear.subscribe("/relaystat");
                  microgear.publish("/relaystat","4",true);
                  reconnected = "0";
                }    
              }   
         }
      if(WiFi.status() != WL_CONNECTED)
      {
        if(!wifi_Check_status || wifi_Check_status == "connect")
        {
          wifi_Check_status = "lost";
          digitalWrite(RELAY_PIN1, LOW);
          digitalWrite(RELAY_PIN2, LOW);
          check_relay_status = "4";
          lcd.clear();
          lcd.print("Switch 1 : OFF");
          lcd.setCursor(0, 1);
          lcd.print("Switch 2 : OFF");
          lcd.setCursor(0, 2);
          reconnected = "1"; 
        }
      }
      if(WiFi.status() == WL_CONNECTED)
      {
        if(!wifi_Check_status || wifi_Check_status == "lost")
        {
          wifi_Check_status = "connect";
        }
      }
    /* To check if the microgear is still connected */
    if (microgear.connected()) 
    {
        microgear.loop();
    }
    if (microgear.connected() && !reconnect) 
    {
        reconnect = true;
    }
    else if (!microgear.connected() && reconnect) 
    {
        digitalWrite(RELAY_PIN1, LOW);
        digitalWrite(RELAY_PIN2, LOW);
        check_relay_status = "4";
        lcd.clear();
        lcd.print("Switch 1 : OFF");
        lcd.setCursor(0, 1);
        lcd.print("Switch 2 : OFF");
        lcd.setCursor(0, 2);
        reconnected = "1"; 
        //if ((millis() - timer) % 500 == 0){}
        if ((millis() - timer) > 5000)
        {
          reconnect = false;
        }
    }
    else if (!microgear.connected() && !reconnect) 
    {
        digitalWrite(RELAY_PIN1, LOW);
        digitalWrite(RELAY_PIN2, LOW);
        check_relay_status = "4";
        lcd.clear();
        lcd.print("Switch 1 : OFF");
        lcd.setCursor(0, 1);
        lcd.print("Switch 2 : OFF");
        lcd.setCursor(0, 2);
        reconnected = "1"; 
        microgear.connect(APPID);
        timer = millis();
    }
}