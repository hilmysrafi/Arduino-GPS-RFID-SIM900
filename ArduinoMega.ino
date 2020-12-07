#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <TinyGPS.h>

SoftwareSerial Gsm(10, 11);
TinyGPS gps;  //Creates a new instance of the TinyGPS object

#define SS_PIN 53
#define RST_PIN 49
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
const int buzzer = 13;
String content;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); //GPS
  Serial2.begin(9600); //BLE
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 
  Gsm.begin(9600);
  delay(6000);
  pinMode(9,OUTPUT);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(2000);
  digitalWrite(9, LOW);
  delay(3000);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(2000);
  digitalWrite(9, LOW);
  delay(3000);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(2000);
//  digitalWrite(9, LOW);
//  delay(3000);
  pinMode(buzzer, OUTPUT);
}

void loop()
{
      Scanrfid();  
      Sendgps();
  
      if(Serial2.available())
      Serial.write(Serial2.read());
      if(Serial.available())
      Serial2.write(Serial.read());

}

void Scanrfid(){
      if ( ! mfrc522.PICC_IsNewCardPresent())
      {
        return;
      }
      if ( ! mfrc522.PICC_ReadCardSerial())
      {
        return;
      }
      Serial.println();
      Serial.print("UID tag = ");
      content = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      Serial.println();
      tone(buzzer, 1000); // Send 1KHz sound signal...
      delay(2000);        // ...for 2 sec
      noTone(buzzer);     // Stop sound...   
      Konfigurasi();
      Serial.println("AT+HTTPPARA=\"URL\",\"http://codemantul.my.id/TugasAkhir/rfidpay.php\"\\r\\n");
      Gsm.print("AT+HTTPPARA=\"URL\",\"http://codemantul.my.id/TugasAkhir/rfidpay.php?id=" + String(content));
      Gsm.println("\"");
      delay(1000);
      ShowSerialData();
      Serial.print("AT+HTTPACTION=0\\r\\n");
      Gsm.println("AT+HTTPACTION=0");//submit the request 
      delay(2000);//the delay is very important, the delay time is base on the return from the website, if the return datas are very large, the time required longer.
      ShowSerialData();
      Serial.print("AT+HTTPREAD\\r\\n");
      Gsm.println("AT+HTTPREAD");// read the data from the website you access
      delay(500);
      ShowSerialData();
      Gsm.println("");
      delay(100);
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
  }
void Sendgps(){
    bool newData = false;
    for (unsigned long start = millis(); millis() - start < 500;)
  
    {
  
      while (Serial1.available())
  
      {
        char c = Serial1.read();
        //Serial.print(c);
        if (gps.encode(c))
          newData = true;  
  
      }
  
    }
  
    if (newData)      //If newData is true
    {
      float flat, flon;
      String output="";
      unsigned long age;
      gps.f_get_position(&flat, &flon, &age);
      Serial.print("LAT=");
      Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
      Serial.print(" LON=");
      Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
      Serial.println("");
      output="http://codemantul.my.id/TugasAkhir/gps.php?lti="+String(flat,6)+"&lon="+String(flon,6);
      Serial.println(output);
      Konfigurasi();
      Serial.println("AT+HTTPPARA=\"URL\",\"http://codemantul.my.id/TugasAkhir/gps.php\"\\r\\n");
      Gsm.print("AT+HTTPPARA=\"URL\",\"http://codemantul.my.id/TugasAkhir/gps.php?");
      Gsm.print("&lti=");
      Gsm.print(flat,6);
      Gsm.print("&lon=");
      Gsm.print(flon,6);
      Gsm.print("\"\r\n");
      delay(800);
      ShowSerialData();
//      delay(800);
      Serial.print("AT+HTTPACTION=0\\r\\n");
      Gsm.println("AT+HTTPACTION=0");  /* Start GET session */
      delay(2000);
      ShowSerialData();
//      delay(800);
      Serial.print("AT+HTTPREAD\\r\\n");
      Gsm.println("AT+HTTPREAD=0,100000");  /* Read data from HTTP server */
      delay(500);
      ShowSerialData();
//      delay(500);
      Serial.print("AT+HTTPTERM\\r\\n");  
      Gsm.println("AT+HTTPTERM");  /* Terminate HTTP service */
      delay(500);
      ShowSerialData();
//      delay(500);
      Serial.print("AT+SAPBR=0,1\\r\\n");
      Gsm.println("AT+SAPBR=0,1"); /* Close GPRS context */
      delay(500);
      ShowSerialData();
//      delay(10000);
     
    }
  }
void Konfigurasi(){
//    Serial.println("HTTP get method :");
//    Serial.print("AT\\r\\n");
//    Gsm.println("AT");  /* Check Communication */
//    delay(500);
//    ShowSerialData(); /* Print response on the serial monitor */
//    delay(500);
    /* Configure bearer profile 1 */
    Serial.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\\r\\n");    
    Gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  /* Connection type GPRS */
    delay(500);
    ShowSerialData();
//    delay(800);
    Serial.print("AT+SAPBR=3,1,\"APN\",\"Telkomsel\"\\r\\n");  
    Gsm.println("AT+SAPBR=3,1,\"APN\",\"Telkomsel\"");  /* APN of the provider */
    delay(500);
    ShowSerialData();
//    delay(800);
    Serial.print("AT+SAPBR=3,1,\"PWD\",\"wap123\"\\r\\n");  
    Gsm.println("AT+SAPBR=3,1,\"PWD\",\"wap123\"");  /* APN of the provider */
    delay(500);
    ShowSerialData();
//    delay(800);
    Serial.print("AT+SAPBR=1,1\\r\\n");
    Gsm.println("AT+SAPBR=1,1"); /* Open GPRS context */
    delay(1000);
    ShowSerialData();
//    delay(800);
    Serial.println();
    Serial.print("AT+SAPBR=2,1\\r\\n");
    Gsm.println("AT+SAPBR=2,1"); /* Query the GPRS context */
    delay(500);
    ShowSerialData();
//    delay(1000);
    Serial.print("AT+HTTPINIT\\r\\n");
    Gsm.println("AT+HTTPINIT"); /* Initialize HTTP service */
    delay(500); 
    ShowSerialData();
//    delay(800);
//    Serial.print("AT+HTTPINIT=?\\r\\n");
//    Gsm.println("AT+HTTPINIT=?"); /* Initialize HTTP service */
//    delay(500); 
//    ShowSerialData();
//    delay(800);
    Serial.print("AT+HTTPPARA=\"CID\",1\\r\\n");
    Gsm.println("AT+HTTPPARA=\"CID\",1");  /* Set parameters for HTTP session */
    delay(800);
    ShowSerialData();
//    delay(800);
}
void ShowSerialData()
{
  while(Gsm.available()!=0)  
  Serial.write(char (Gsm.read()));
}
