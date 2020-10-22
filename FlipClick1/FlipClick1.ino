#include <SPI.h>
#include <SD.h>

const int chipSelect = 78;
// make a string for assembling the data to log:
byte leds[4] = { 38, 37, 39, 40 };
#define GSM2ClickSE Serial3
String testst = "";
int relay1 = 0, relay2 = 0, relay3 = 0, relay4 = 0, relay5 = 0, relay6 = 0, relay7 = 0, relay8 = 0;
String resp = "";
int countertext = 0;
int ActiveCounter = 0,  ActiveLight = 0;
bool stayon = false;
int SMSNotify = 1, SMSCheckON = 0, SMSCheckOFF = 0, SetControl = 0;
int Relaydelay = 500, RelayCheck500 = 0, RelayCheck1000 = 0, RelayCheck6000 = 0;
int pinRelaypwm1 = 77;
int pinRelayCS2 = 6;
int callPin = 9;
int numberfound = 0;
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay);
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
void SendSMSNotify(String Text);

//////////////////////////
String dataString1 = "SMS-ON";
String dataString2 = "RD-500";
String dataString3 = "+4917676552098";
char  array[100] = "";
String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
//////////////////////////

void setup() {
  for ( int i = 0; i < 4; i++ )
  {
    pinMode( leds[i], OUTPUT );
    delay(1000);
    digitalWrite( leds[i], HIGH );
    pinMode( 35, OUTPUT );
    pinMode( 8, OUTPUT );
    digitalWrite( 35, HIGH );
    pinMode( 8, OUTPUT );
    digitalWrite( 8, HIGH );
  }
  // initialize both serial ports:
  Serial.begin(115200);
  GSM2ClickSE.begin(115200);
  Serial2.begin(9600);
  //////////////////////////////////////////////////////

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    // while (1);
  }
  Serial.println("card initialized.");
  //////////////////////////////////////////////////

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  /* //SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
     dataFile.println(dataString5);
     dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
     Serial.println("error opening datalog.txt");
    }*/

  ///////////////////////////////////////
  // re-open the file for reading:
  String text1 = "";
  File dataFile = SD.open("datalog1.txt", FILE_READ);
  if (dataFile) {
    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      dataFile.read(array, 50);
      Serial.println(array);

      for (int i = 0; i <= 100; i++)
      {
        if (array[i] == '+')
        {
          numberfound = 1;
        }
        if (array[i] != ',')
        {
          text1.concat(array[i]);
        }
        else
        {
          countertext++;
          Serial.println(text1);
          if (text1 == "SMS-OFF")
          {
            SMSNotify = 0;
            dataString1 = "SMS-OFF";
          }
          if (text1 == "SMS-ON")
          {
            SMSNotify = 1;
            dataString1 = "SMS-ON";
          }
          if (text1 == "RD-500")
          {
            Relaydelay = 500;
            dataString2 = "RD-500";
          }
          if (text1 == "RD-1000")
          {
            Relaydelay = 1000;
            dataString2 = "RD-1000";
          }
          if (text1 == "RD-6000")
          {
            Relaydelay = 6000;
            dataString2 = "RD-6000";
          }
          if (countertext == 3)
          {
            if (numberfound == 1)
            {
              dataString3 = text1;
            }
          }
          text1 = "";
        }
      }
    }
    // close the file:
    dataFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog1.txt");
  }
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  pinMode( pinRelayCS2  , OUTPUT );
  pinMode( pinRelaypwm1  , OUTPUT );
  pinMode( callPin  , INPUT_PULLUP );
  pinMode(35, INPUT);
  /////////

  initialGSM();
  initaliSMS();
  initaliCalls();

  //////////////
}
void Readdatarelay()
{
  delay(300);
  while (GSM2ClickSE.available())
  {
    String c = GSM2ClickSE.readStringUntil(';');
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
  relay8 = -testst.indexOf("Relay8-ON");
  int callReceive = testst.indexOf("CRING: VOICE");
  SMSCheckON = testst.indexOf("SMS-ON");
  SMSCheckOFF = testst.indexOf("SMS-OFF");
  SetControl = testst.indexOf("SET CONTROL +49");

  if (SetControl > 50)
  {
    String num;
    for (int i = 64; i < 80; i++)
    {
      num = num + testst[i];
      Serial.print(testst[i]);
    }
    SendSMSNotify("SET CONTROL " + num);
    dataString3 = num;
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }

  }

  /////////////////////////////////////////
  if (SMSCheckON > 50)
  {
    SMSNotify = 1;
    SendSMSNotify("SMS-ON");
    dataString1 = "SMS-ON";
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  /////////////////////////////////////////
  //////////////////////////////
  if (SMSCheckOFF > 50)
  {
    SendSMSNotify("SMS-OFF");
    dataString1 = "SMS-OFF";
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
        SMSNotify = 0;
  }
  ///////////////////////////////////////
  RelayCheck500 = testst.indexOf("RD-500");
  /////////////////////////////////////////
  if (RelayCheck500 > 50)
  {
    SendSMSNotify("Relay Delay 500");
    Relaydelay = 500;
    dataString2 = "RD-500";
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  ////////////////////////////////////////
  RelayCheck1000 = testst.indexOf("RD-1000");
  /////////////////////////////////////////
  if (RelayCheck1000 > 50)
  {
    SendSMSNotify("Relay Delay 1000");
    Relaydelay = 1000;
    dataString2 = "RD-1000";
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  ////////////////////////////////////////
  RelayCheck6000 = testst.indexOf("RD-6000");
  /////////////////////////////////////////
  if (RelayCheck6000 > 50)
  {
    SendSMSNotify("Relay Delay 6000");
    Relaydelay = 6000;
    dataString2 = "RD-6000";
    String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";
    SD.remove("datalog1.txt");
    File dataFile = SD.open("datalog1.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString5);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  /////////////////////////////////////////////
  if ((callReceive >= 3) && (digitalRead(callPin) == 0))
  {
    GSM2ClickSE.println("ATA");
  }

  resp[resp.length() - 1] = '\0';
  //  50 >   52 is actual

  if (relay1 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    delay(Relaydelay);
    processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    ActiveLight = 1;
  }
  if (relay2 > 50)
  {
    processdata("Relay2-ON", 0x02, 0x01, 0xAE, true);
    delay(Relaydelay);
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF, false);
    ActiveLight = 1;
  }
  if (relay3 > 50)
  {
    processdata("Relay3-ON", 0x03, 0x01, 0xAF, true);
    delay(Relaydelay);
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0, false);
    ActiveLight = 1;
  }
  if (relay4 > 50)
  {
    processdata("Relay4-ON", 0x04, 0x01, 0xB0, true);
    delay(Relaydelay);
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1, false);
    ActiveLight = 1;
  }
  if (relay5 > 50)
  { processdata("Relay5-ON", 0x05, 0x01, 0xB1, true);
    delay(Relaydelay);
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2, false);
    ActiveLight = 1;
  }
  if (relay6 > 50)
  {
    processdata("Relay6-ON", 0x06, 0x01, 0xB2, true);
    delay(Relaydelay);
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3, false);
    ActiveLight = 1;
  }
  if (relay7 > 50)
  {
    processdata("Relay7-ON", 0x07, 0x01, 0xB3, true);
    delay(Relaydelay);
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4, false);
    ActiveLight = 1;
  }
  if (relay8 > 50)
  {
    processdata("Relay8-ON", 0x08, 0x01, 0xB4, true);
    delay(Relaydelay);
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
    if (SMSNotify == 1)
    {
      GSM2ClickSE.println("AT+CMGS=\"+4917676552098\"" );
      delay(300);
      GSM2ClickSE.println(Text);
      gsmSendTextMessage(&GSM2ClickSE, "017676552098", "OK");
    }
  }
  delay(300);
  GSM2ClickSE.println("AT+QMGDA=\"DEL READ\"");
}
void gsmSendTextMessage(HardwareSerial * gsm_uart, String phone_no, String  sms_text)
{
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 500);
  GSM2ClickSE.write(26);
}

bool gsmSendCommandWithTimeout(HardwareSerial * gsm_uart, String at_command, unsigned int timeout)
{
  GSM2ClickSE.println(at_command);
  GSM2ClickSE.write(13);
  delay(timeout);
  return true;
}
///////////////////////////////////////////////////////////////////////////
void loop()
{
  ////////////////////////////////////////
  Serial.println(dataString1);
  Serial.println(dataString2);
  Serial.println(dataString3);
  Serial.println(numberfound);

  ////////////////////////////////////////
  ///////////////////////////////////////////////
  int sensorVal = digitalRead(callPin);/// alwaya 1 HIGH
  if (sensorVal == 0)
  {
    GSM2ClickSE.println("ATD017676552098;");
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

void SendSMSNotify(String Text)
{
  if (SMSNotify == 1)
  {
    GSM2ClickSE.println("AT+CMGS=\"+4917676552098\"" );
    delay(300);
    GSM2ClickSE.println(Text);
    gsmSendTextMessage(&GSM2ClickSE, "017676552098", "OK");
    GSM2ClickSE.println("AT+CMGD=1,4");
    GSM2ClickSE.println("AT+QMGDA=\"DEL READ\"");
  }
}

////////////////////////
void initialGSM()
{
  for (int i = 0; i < 500; i++)
  {
    // digitalWrite( 35, HIGH );
    //digitalWrite( 8, HIGH );
  }

  GSM2ClickSE.println( "AT" ); // Sending the 1st AT command
  delay(500);
  GSM2ClickSE.println("AT+IPR=115200");
  delay(500);
}

void initaliSMS()
{
  GSM2ClickSE.println("AT+CSMS=1,1,1,1"); /// tried 1111 not good all 0 0 0 0 0  Test to see why many messages coming
  Readdatarelay();
  GSM2ClickSE.println("AT+CPMS=\"SM\"");  /// tried MT not good
  Readdatarelay();
  GSM2ClickSE.println("AT+CMGL=\"REC UNREAD\""); /// all also same
  Readdatarelay();
  GSM2ClickSE.println("AT+QINDI=1");
  Readdatarelay();
  GSM2ClickSE.println("AT+QIMODE=1");
  Readdatarelay();
  GSM2ClickSE.println("AT+CMGF=1");
  Readdatarelay();
  GSM2ClickSE.println( "AT+CNMI=2,2,0,0,0" );
}
void initaliCalls()
{
  GSM2ClickSE.println("AT+CFUN=1");
  Readdatarelay();
  GSM2ClickSE.println("AT+CMEE=1");
  Readdatarelay();
  GSM2ClickSE.println("AT+CPIN?");
  Readdatarelay();
  GSM2ClickSE.println("AT+QAUDCH=2");
  Readdatarelay();
  GSM2ClickSE.println("AT*EREG=2");
  Readdatarelay();
  GSM2ClickSE.println("AT+CLIP=1;+CRC=1");
  Readdatarelay();
  GSM2ClickSE.println("AT+CLVL=50");
  Readdatarelay();
  GSM2ClickSE.println("AT+QMIC=1,15");
  Readdatarelay();
}
