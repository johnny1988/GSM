#include <SoftwareSerial.h>

SoftwareSerial mySerial(18, 19);

void setup()
{
  mySerial.begin(115200);   // Setting the baud rate of GSM Module
  Serial.begin(115200);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
  Serial.println("Setup done");
}

void loop()
{
  if (Serial.available() > 0)
    switch (Serial.read())
    {
      case 's':
        SendMessage();
        break;
      case 'r':
        RecieveMessage();
        break;
    }
  if (mySerial.isListening())
  {
    Serial.println("Port One is listening!");
  }
  while (mySerial.available() > 0)
  {
    Serial.println(mySerial.read());
    Serial.println("available ");
  }
  if (Serial.available() > 0)
  {
    Serial.println(mySerial.readString());
    Serial.println(Serial.readString());
  }
}

void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"017676552098\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println("Send message done");
}

void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  mySerial.println("AT+CFUN=1");
  mySerial.println("AT+CMGF=1");
  mySerial.println("AT+CMGF?");
  mySerial.println("AT+CMGR=1");
  mySerial.println("AT+CMGL=\"REC UNREAD\"");
  mySerial.println(" AT+CMGL=\"ALL\"");
  Serial.println("receive message done");
}
