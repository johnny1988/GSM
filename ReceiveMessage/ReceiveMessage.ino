int stat = 0;
String resp = "";
int gt = 4;
#include "HardwareSerial.h"
#define SSerial2 Serial2
int relay1off;
int relay1on;
unsigned char _rx_buffer[2048];
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout);

void Readdata()
{
  delay(500);
  while (SSerial2.available())
  {
    char c = SSerial2.read();
    resp += c;
  }
  resp[resp.length() - 1] = '\0';
  Serial.println(resp); // The M95 modem should reply OK
  resp = "";
}

void setup()
{
  Serial3.begin(9600);

  Serial.begin(115200);
  Serial.println("Starting MCU UART...");

  Serial.println("Starting GSM2 UART...");
  SSerial2.begin(115200);

  SSerial2.println( "AT+IPR=115200" );
  SSerial2.println( "AT" ); // Sending the 1st AT command
  delay(1000);
  SSerial2.println( "AT" );// Sending the 2nd AT command
  delay(500);

  SSerial2.println("AT+CSMS=1,1,1,1");
  Readdata();
  SSerial2.println("AT+CPMS=\"SM\"");
  Readdata();
  SSerial2.println("AT+CMGL=\"ALL\"");
  Readdata();

  //  SSerial2.println("AT+QIMODE=0");
  //  Readdata();
  SSerial2.println("AT+QINDI=1");
  Readdata();
  //  SSerial2.println("AT+QIMODE=1");
  //  Readdata();
  //  SSerial2.println("AT+QIMODE=1");
  //  Readdata();
  SSerial2.println("AT+QIMODE=1");
  Readdata();
  SSerial2.println("AT+CMGF=1");
  Readdata();
  SSerial2.println( "AT+CNMI=2,2,0,0,0" );
  Readdata();
  SSerial2.println("AT+QMIC=2,15");
  Readdata();
  SSerial2.println(" AT+CMUT=0");
  Readdata();
  SSerial2.println(" AT+CMUT?");
  Readdata();
  SSerial2.println(" AT&W0");
  Readdata();
}

void gsmSendTextMessage(HardwareSerial * gsm_uart, String phone_no, String  sms_text)
{
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 500);
  SSerial2.write(26);
}

bool gsmSendCommandWithTimeout(HardwareSerial * gsm_uart, String at_command, unsigned int timeout)
{
  SSerial2.println(at_command);
  SSerial2.write(13);
  delay(timeout);
  return true;
}

void loop()
{
  gt++;
  String resp1 = "";
  if (gt > 9) gt = 4;
  delay(500);
  SSerial2.println( "AT+CMGR=" + String(gt));
  //delay(1000);
  SSerial2.setTimeout(4000);
  byte byteRead;
  int rec = 0;
  String testc;
  while (SSerial2.available() > 0)
  {
    String c;
    char c2;
    c = SSerial2.readStringUntil(';');
    c2 = SSerial2.read();
    resp += c;
    rec++;
    _rx_buffer[rec] = c2;
    testc = c;
    resp[resp.length() - 1] = '\0';

    Serial.print(testc);
    relay1on = testc.indexOf("Relay1-ON;");
    Serial.println(relay1on);
    relay1off = testc.indexOf("Relay1-OFF;");
    Serial.println(relay1off);
  }


    if ((relay1on > 80))
    {
      delay(300);
      Serial3.write(0x55);
      Serial3.write(0x56);
      Serial3.write(0x00);
      Serial3.write(0x00);
      Serial3.write(0x00);
      Serial3.write(0x01);
      Serial3.write(0x03);
      Serial3.write(0xAF);

      //delay(1000);
      SSerial2.println("AT+CMGS=\"+4917676552098\"" );
      delay(300);
      SSerial2.println("Relay1-ON");
      gsmSendTextMessage(&SSerial2, "017676552098", "OK");
      Readdata();

      relay1on = 0;
    }

    // bool checknumer = Serial.find();
    if (relay1off > 80)
    {
      delay(300);
      Serial3.write(0x55);
      Serial3.write(0x56);
      Serial3.write(0x00);
      Serial3.write(0x00);
      Serial3.write(0x00);
      Serial3.write(0x01);
      Serial3.write(0x03);
      Serial3.write(0xAF);

      //delay(1000);
      SSerial2.println("AT+CMGS=\"+4917676552098\"" );
      SSerial2.println("Relay1-OFF");
      delay(300);
      gsmSendTextMessage(&SSerial2, "017676552098", "OK");
      Readdata();

      relay1off = 0;
    }

  
  resp[resp.length() - 1] = '\0';
  int numbermatch = resp.indexOf("+4917676552098");

  if (numbermatch == 25)
  {
    Serial.println(resp); // The M95 modem should reply OK
  }

  if (relay1on == 25)
  {
    Serial.println(resp); // The M95 modem should reply OK
  }
  resp = "";
  Serial.print(". ");
}
