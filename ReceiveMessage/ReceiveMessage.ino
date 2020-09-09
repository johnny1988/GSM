int stat = 0;
String resp = "";
int gt = 4;
#include "HardwareSerial.h"
#define SSerial2 Serial2
int relay1off = 0;
int relay1on = 0;
int relay2off = 0;
int relay2on = 0;
int relay3off = 0;
int relay3on = 0;
int relay4off = 0;
int relay4on = 0;
int relay5off = 0;
int relay5on = 0;
int relay6off = 0;
int relay6on = 0;
int relay7off = 0;
int relay7on = 0;
int relay8off = 0;
int relay8on = 0;
unsigned char _rx_buffer[2048];
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout);
void processdata(String Text, byte rel, byte data);

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

void processdata(String Text, byte rel, byte data)
{
  delay(300);
  Serial3.write(0x55); Serial3.write(0x56); Serial3.write(0x00); Serial3.write(0x00); Serial3.write(0x00); Serial3.write(rel); Serial3.write(0x03); Serial3.write(data);
  SSerial2.println("AT+CMGS=\"+4917676552098\"" );
  delay(300);
  SSerial2.println(Text);
  gsmSendTextMessage(&SSerial2, "017676552098", "OK");
  relay1on = 0;
}
//////////////////////////////////////////////////
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
    relay1on = testc.indexOf("Relay1-ON");
    relay1off = testc.indexOf("Relay1-OFF");
    relay2on = testc.indexOf("Relay2-ON");
    relay2off = testc.indexOf("Relay2-OFF");
    relay3on = testc.indexOf("Relay3-ON");
    relay3off = testc.indexOf("Relay3-OFF");
    relay4on = testc.indexOf("Relay4-ON");
    relay4off = testc.indexOf("Relay4-OFF");
    relay5on = testc.indexOf("Relay5-ON");
    relay5off = testc.indexOf("Relay5-OFF");
    relay6on = testc.indexOf("Relay6-ON");
    relay6off = testc.indexOf("Relay6-OFF");
    relay7on = testc.indexOf("Relay7-ON");
    relay7off = testc.indexOf("Relay7-OFF");
    relay8on = testc.indexOf("Relay8-ON");
    relay8off = testc.indexOf("Relay8-OFF");
  }


  if (relay1on > 80)  {
    processdata("Relay1-ON", 0x01, 0xAF);
  }
  if (relay1off > 80)  {
    processdata("Relay1-OFF", 0x01, 0xAF);
  }
  if (relay2on > 80)  {
    processdata("Relay2-ON", 0x02, 0xB0);
  }
  if (relay2off > 80)  {
    processdata("Relay2-OFF", 0x02, 0xB0);
  }
  if (relay3on > 80)  {
    processdata("Relay3-ON", 0x03, 0xB1);
  }
  if (relay3off > 80)  {
    processdata("Relay3-OFF", 0x03, 0xB1);
  }
  if (relay4on > 80)  {
    processdata("Relay4-ON", 0x04, 0xB2);
  }
  if (relay4off > 80)  {
    processdata("Relay4-OFF", 0x04, 0xB2);
  }
  if (relay5on > 80)  {
    processdata("Relay5-ON", 0x05, 0xB3);
  }
  if (relay5off > 80)  {
    processdata("Relay5-OFF", 0x05, 0xB3);
  }
  if (relay6on > 80)  {
    processdata("Relay6-ON", 0x06, 0xB4);
  }
  if (relay6off > 80)  {
    processdata("Relay6-OFF", 0x06, 0xB4);
  }
  if (relay7on > 80)  {
    processdata("Relay7-ON", 0x07, 0xB5);
  }
  if (relay7off > 80)  {
    processdata("Relay7-OFF", 0x07, 0xB5);
  }
  if (relay8on > 80)  {
    processdata("Relay8-ON", 0x08, 0xB6);
  }
  if (relay8off > 80)  {
    processdata("Relay8-OFF", 0x08, 0xB6);
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
