#define test1 Serial3
String testst = "";
int relay1 = 0;
int relay2 = 0;
int relay3 = 0;
int relay4 = 0;
int relay5 = 0;
int relay6 = 0;
int relay7 = 0;
int relay8 = 0;
String Tes = "";
String resp = "";
int ActiveCounter = 0,  ActiveLight = 0;
bool stayon = false;

int pinRelaypwm1 = 77;
int pinRelayCS2 = 6;
int callPin = 9;
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay);
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  test1.begin(115200);
  Serial2.begin(9600);

  /////////
  pinMode( pinRelayCS2  , OUTPUT );
  pinMode( pinRelaypwm1  , OUTPUT );
  pinMode( callPin  , INPUT_PULLUP );

  /////////

  initialGSM();
  initaliSMS();
  initaliCalls();
}
void Readdatarelay()
{
  delay(300);
  while (test1.available())
  {
    String c = test1.readStringUntil(';');
    testst = c;
    Serial.println(c); // The M95 modem should reply OK
  }

  relay1 = testst.indexOf("Relay1-ON");
  relay2 = testst.indexOf("Relay2-ON");
  relay3 = testst.indexOf("Relay3-ON");
  relay4 = testst.indexOf("Relay4-ON");
  relay5 = testst.indexOf("Relay5-ON");
  relay6 = testst.indexOf("Relay6-ON");
  relay7 = testst.indexOf("Relay7-ON");
  relay8 = testst.indexOf("Relay8-ON");
  int callReceive = testst.indexOf("CRING: VOICE");

  if ((callReceive >= 3) && (digitalRead(callPin) == 0))
  {
    test1.println("ATA");
  }
  
  resp[resp.length() - 1] = '\0';
  //  50 >   52 is actual

  if (relay1 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    delay(500);
    processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    ActiveLight = 1;
  }
  if (relay2 > 50)
  {
    processdata("Relay2-ON", 0x02, 0x01, 0xAE, true);
    delay(500);
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF, false);
    ActiveLight = 1;
  }
  if (relay3 > 50)
  {
    processdata("Relay3-ON", 0x03, 0x01, 0xAF, true);
    delay(500);
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0, false);
    ActiveLight = 1;
  }
  if (relay4 > 50)
  {
    processdata("Relay4-ON", 0x04, 0x01, 0xB0, true);
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1, false);
    ActiveLight = 1;
  }
  if (relay5 > 50)
  { processdata("Relay5-ON", 0x05, 0x01, 0xB1, true);
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2, false);
    ActiveLight = 1;
  }
  if (relay6 > 50)
  {
    processdata("Relay6-ON", 0x06, 0x01, 0xB2, true);
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3, false);
    ActiveLight = 1;
  }
  if (relay7 > 50)
  {
    processdata("Relay7-ON", 0x07, 0x01, 0xB3, true);
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4, false);
    ActiveLight = 1;
  }
  if (relay8 > 50)
  {
    processdata("Relay8-ON", 0x08, 0x01, 0xB4, true);
    processdata("Relay8-OFF", 0x08, 0x02, 0xB5, false);
    ActiveLight = 1;
  }
  resp = "";
}
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay)
{
  byte tt = 0;
  delay(300);
  Serial2.write(0x55); Serial2.write(0x56); Serial2.write(tt); Serial2.write(tt); Serial2.write(tt); Serial2.write(rel); Serial2.write(rell); Serial2.write(data);
  if (offrelay == true)
  {
    test1.println("AT+CMGS=\"+4917676552098\"" );
    delay(300);
    test1.println(Text);
    gsmSendTextMessage(&test1, "017676552098", "OK");
  }
  delay(300);
  test1.println("AT+QMGDA=\"DEL READ\"");
}
void gsmSendTextMessage(HardwareSerial * gsm_uart, String phone_no, String  sms_text)
{
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 500);
  test1.write(26);
}

bool gsmSendCommandWithTimeout(HardwareSerial * gsm_uart, String at_command, unsigned int timeout)
{
  test1.println(at_command);
  test1.write(13);
  delay(timeout);
  return true;
}
///////////////////////////////////////////////////////////////////////////
void loop()
{
  int sensorVal = digitalRead(callPin);/// alwaya 1 HIGH
  if (sensorVal == 0)
  {
    test1.println("ATD017676552098;");
    Readdatarelay();
  }
  delay(300);
  Readdatarelay();
  Serial.println(". ");
  if (ActiveCounter > 45)
  {
    ActiveLight = 0;
    stayon = true;
    ActiveCounter = 0;
  }
  if (ActiveLight == 1)
  {
    stayon = true;
    ActiveCounter++;
  }
  if (stayon)
  {
    stayon = false;
    if (ActiveLight == 1)
    {
      digitalWrite( pinRelayCS2  , HIGH );
      digitalWrite( pinRelaypwm1  , HIGH );
    }
    else
    {
      digitalWrite( pinRelayCS2  , LOW );
      digitalWrite( pinRelaypwm1  , LOW );
    }

  }
}

void initialGSM()
{
  test1.println( "AT" ); // Sending the 1st AT command
  delay(500);
  test1.println("AT+IPR=115200");
  delay(500);
}

void initaliSMS()
{
  test1.println("AT+CSMS=1,1,1,1"); /// tried 1111 not good all 0 0 0 0 0  Test to see why many messages coming
  Readdatarelay();
  test1.println("AT+CPMS=\"SM\"");  /// tried MT not good
  Readdatarelay();
  test1.println("AT+CMGL=\"REC UNREAD\""); /// all also same
  Readdatarelay();
  test1.println("AT+QINDI=1");
  Readdatarelay();
  test1.println("AT+QIMODE=1");
  Readdatarelay();
  test1.println("AT+CMGF=1");
  Readdatarelay();
  test1.println( "AT+CNMI=2,2,0,0,0" );
}
void initaliCalls()
{
  test1.println("AT+CFUN=1");
  Readdatarelay();
  test1.println("AT+CMEE=1");
  Readdatarelay();
  test1.println("AT+CPIN?");
  Readdatarelay();
  test1.println("AT+QAUDCH=2");
  Readdatarelay();
  test1.println("AT*EREG=2");
  Readdatarelay();
  test1.println("AT+CLIP=1;+CRC=1");
  Readdatarelay();
  test1.println("AT+CLVL=50");
  Readdatarelay();
  test1.println("AT+QMIC=1,15");
  Readdatarelay();
}
