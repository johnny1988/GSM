//int stat = 0;
String resp = "";
int gt = 4;
#include "HardwareSerial.h"
#define SSerial2 Serial2
#define relaySerial Serial
int Pin8Dail = 8;
int Pin9Answer = 9;

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
int StatusRelay = 0;
unsigned char _rx_buffer[2048];
int relaystatus[8];
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout);
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay);

void Readdata()
{
  delay(500);
  while (SSerial2.available())
  {
    char c = SSerial2.read();
    resp += c;
  }
  resp[resp.length() - 1] = '\0';
  // Serial.println(resp); // The M95 modem should reply OK
  resp = "";
}

void Readdatarelay()
{
  delay(100);
  while (Serial3.available())
  {
    byte c1 = Serial3.read();
    String c = Serial3.readStringUntil("");
    resp += c;
    // relaySerial.println(c1, HEX);
  }
  resp[resp.length() - 1] = '\0';
  relaySerial.println(resp); // The M95 modem should reply OK
  resp = "";
}
void setup()
{
  pinMode(Pin8Dail, INPUT);
  pinMode(Pin9Answer, INPUT);

  Serial.println(digitalRead(Pin8Dail));
  Serial.println(digitalRead(Pin9Answer));

  Serial3.begin(9600);

  relaySerial.begin(9600);
  Serial.begin(115200);
  Serial.println("Starting MCU UART...");

  Serial.println("Starting GSM2 UART...");
  SSerial2.begin(115200);

  SSerial2.println( "AT+IPR=115200" );
  SSerial2.println( "AT" ); // Sending the 1st AT command
  delay(1000);
  SSerial2.println( "AT" );// Sending the 2nd AT command
  delay(500);

  SSerial2.println("AT+CSMS=1,1,1,1"); /// tried 1111 not good all 0 0 0 0 0  Test to see why many messages coming
  Readdata();
  SSerial2.println("AT+CPMS=\"SM\"");  /// tried MT not good
  Readdata();
  SSerial2.println("AT+CMGL=\"REC UNREAD\""); /// all also same
  Readdata();
  SSerial2.println("AT+QINDI=1");
  Readdata();
  SSerial2.println("AT+QIMODE=1");
  Readdata();
  SSerial2.println("AT+CMGF=1");
  Readdata();
  SSerial2.println( "AT+CNMI=2,2,0,0,0" );  // TEST1  0,2-S  1.2 S  3.2S->Test3 2.0 N.G
  Readdata();
  SSerial2.println("AT+QMIC=1,15");
  Readdata();
  SSerial2.println("ATS0=0");
  SSerial2.println(" AT&W0");
  Readdata();

  relaystatus[0] = 0; relaystatus[1] = 0; relaystatus[2] = 0; relaystatus[3] = 0; relaystatus[4] = 0; relaystatus[5] = 0; relaystatus[6] = 0; relaystatus[7] = 0;
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

void processdata(String Text, byte rel, byte rell, byte data, bool offrelay)
{
  delay(300);
  Serial3.write(0x55); Serial3.write(0x56); Serial3.write(0x00); Serial3.write(0x00); Serial3.write(0x00); Serial3.write(rel); Serial3.write(rell); Serial3.write(data);
  Readdatarelay();
  if (offrelay == true)
  {
    SSerial2.println("AT+CMGS=\"+4917676552098\"" );
    //SSerial2.println("AT+CMGS=\"+4915122918640\"" );
    delay(300);
    SSerial2.println(Text);
    gsmSendTextMessage(&SSerial2, "017676552098", "OK");
    //gsmSendTextMessage(&SSerial2, "015122918640", "OK");
  }

  relay1on = 0;
}
//////////////////////////////////////////////////
void loop()
{
  if (digitalRead(Pin8Dail) == 1)
  {
    SSerial2.println("AT+QAUDCH=2");
    SSerial2.println("AT+QMIC=1,15");
    // SSerial2.println("ATD017676552098;");
    //SSerial2.println("ATD015122918640;");
  }
  if (digitalRead(Pin9Answer) == 1)
  {
    SSerial2.println("AT+QAUDCH=2");
    SSerial2.println("AT+QMIC=1,15");
    SSerial2.println("ATA");
  }
  gt++;
  String resp1 = "";
  if (gt > 9) gt = 4;
  delay(600);
  SSerial2.println( "AT+CMGR=" + String(gt));
  //delay(1000);
  SSerial2.setTimeout(3000);
  byte byteRead;
  int rec = 0;
  String testc;
  while (SSerial2.available() > 0)
  {
    String c;
    char c2;
    c = SSerial2.readStringUntil('');
    c2 = SSerial2.read();
    resp += c;
    rec++;
    _rx_buffer[rec] = c2;
    testc = c;
    resp[resp.length() - 1] = '\0';

    // Serial.print(testc);
  }
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
  StatusRelay = testc.indexOf("Status-Relay");

  if (relay1on > 80)  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    delay(10000);
    processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    relaystatus[0] = 1;
  }
  if (relay1off > 80)  {
    processdata("Relay1-OFF", 0x01, 0x02, 0xAE,false);
    relaystatus[0] = 0;
  }
  if (relay2on > 80)  {
    processdata("Relay2-ON", 0x02, 0x01, 0xAE, true);
    delay(1000);
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF, false);
    relaystatus[1] = 1;
  }
  if (relay2off > 80)  {
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF,false);
    relaystatus[1] = 0;
  }
  if (relay3on > 80)  {
    processdata("Relay3-ON", 0x03, 0x01, 0xAF, true);
    delay(1500);
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0, false);
    relaystatus[2] = 1;
  }
  if (relay3off > 80)  {
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0,false);
    relaystatus[2] = 0;
  }
  if (relay4on > 80)  {
    processdata("Relay4-ON", 0x04, 0x01, 0xB0, true);
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1, false);
    relaystatus[3] = 1;
  }
  if (relay4off > 80)  {
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1,false);
    relaystatus[3] = 0;
  }
  if (relay5on > 80)  {
    processdata("Relay5-ON", 0x05, 0x01, 0xB1, true);
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2, false);
    relaystatus[4] = 1;
  }
  if (relay5off > 80)  {
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2,false);
    relaystatus[4] = 0;
  }
  if (relay6on > 80)  {
    processdata("Relay6-ON", 0x06, 0x01, 0xB2, true);
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3, false);

    relaystatus[5] = 1;
  }
  if (relay6off > 80)  {
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3,false);
    relaystatus[5] = 0;
  }
  if (relay7on > 80)  {
    processdata("Relay7-ON", 0x07, 0x01, 0xB3, true);
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4, false);

    relaystatus[6] = 1;
  }
  if (relay7off > 80)  {
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4,false);
    relaystatus[6] = 0;
  }
  if (relay8on > 80)  {
    processdata("Relay8-ON", 0x08, 0x01, 0xB4, true);
    processdata("Relay8-OFF", 0x08, 0x02, 0xB5, false);

    relaystatus[7] = 1;
  }
  if (relay8off > 80)  {
    processdata("Relay8-OFF", 0x08, 0x02, 0xB5,false);
    relaystatus[7] = 0;
  }
  if (StatusRelay > 80)
  {
    delay(300);
    SSerial2.println("AT+CMGS=\"+4917676552098\"" );
    //SSerial2.println("AT+CMGS=\"+4915122918640\"" );

    if (relaystatus[0] == 1)
    {
      SSerial2.println("Relay1-ON");
    }
    else
    {
      SSerial2.println("Relay1-OFF");
    }
    if (relaystatus[1] == 1)
    {
      SSerial2.println("Relay2-ON");
    }
    else
    {
      SSerial2.println("Relay2-OFF");
    }
    if (relaystatus[2] == 1)
    {
      SSerial2.println("Relay3-ON");
    }
    else
    {
      SSerial2.println("Relay3-OFF");
    }
    if (relaystatus[3] == 1)
    {
      SSerial2.println("Relay4-ON");
    }
    else
    {
      SSerial2.println("Relay4-OFF");
    }
    if (relaystatus[4] == 1)
    {
      SSerial2.println("Relay5-ON");
    }
    else
    {
      SSerial2.println("Relay5-OFF");
    }
    if (relaystatus[5] == 1)
    {
      SSerial2.println("Relay6-ON");
    }
    else
    {
      SSerial2.println("Relay6-OFF");
    }
    if (relaystatus[6] == 1)
    {
      SSerial2.println("Relay7-ON");
    }
    else
    {
      SSerial2.println("Relay7-OFF");
    }
    if (relaystatus[7] == 1)
    {
      SSerial2.println("Relay8-ON");
    }
    else
    {
      SSerial2.println("Relay8-OFF");
    }
     gsmSendTextMessage(&SSerial2, "017676552098", "OK");
    //gsmSendTextMessage(&SSerial2, "015122918640", "OK");
    delay(300);
  }

  int numbermatch = resp.indexOf("+4917676552098");

  if (numbermatch == 25)
  {
    //Serial.println(resp); // The M95 modem should reply OK
  }

  if (relay1on == 25)
  {
    // Serial.println(resp); // The M95 modem should reply OK
  }
  resp = "";
  Serial.print(". ");
  SSerial2.println("AT+QMGDA=\"DEL READ\"");
}
