#include <SPI.h>
#include <SD.h>

const int chipSelect = 78;
// make a string for assembling the data to log:
byte leds[4] = { 38, 37, 39, 40 };
#define GSM2ClickSE Serial3
String GSM2SMS = "";
int relay1 = 0, relay2 = 0, relay3 = 0, relay4 = 0, relay5 = 0, relay6 = 0, relay7 = 0, relay8 = 0, relay9 = 0, relay10 = 0;
char Numbrelay1 = '0', Numbrelay2 = '0', Numbrelay3 = '0', Numbrelay4 = '0', Numbrelay5 = '0', Numbrelay6 = '0', Numbrelay7 = '0', Numbrelay8 = '0', Numbrelay9 = '0', Numbrelay10 = '0';
int relay11 = 0, relay12 = 0, relay13 = 0, relay14 = 0, relay15 = 0, relay16 = 0;
int MultRelay1 = 0, MultRelay2 = 0;
String resp = "", SMSMobile, ComplMess = "";
int ActiveCounter = 0,  ActiveLight = 0,  countertext = 0;
bool stayon = false, Runonce = false, SendonceRelayComm = false;
int SMSNotify = 1, SMSCheckON = 0, SMSCheckOFF = 0, SetControl = 0, SMSControl = 0, SMSdelete = 0, numberfound = 0;
int Relaydelay = 0, RelayCheck500 = 0, RelayCheck1000 = 0, RelayCheck6000 = 0;
int pinRelaypwm1 = 77;
int pinRelayCS2 = 6;
int callPin = 9;

//////////////////////////////////////////////////////
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay);
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
void SendSMSNotify(String Text);
void SendRelayNotify(String Text);
void MultiRelaySMS(char Number);
//////////////////////////////////////////////////////

String dataString1 = "SMS-ON";
String dataString2 = "RD-500";
String dataString3 =  "+4915122918640";//"+4917673882064";// "+4917676552098"; // "+4915122918640";
char  array[100] = "";
String dataString5 = dataString1 + "," + dataString2 + "," + dataString3 + ",";

//////////////////////////////////////////////////////
void setup() {
  for ( int i = 0; i < 4; i++ )
  {
    pinMode( leds[i], OUTPUT );
    delay(500);
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
  }
  Serial.println("card initialized.");
  //////////////////////////////////////////////////////
  // re-open the file for reading:
  String text1;
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
  /////////
  delay(1000);
  initialGSM();
  delay(1000);
  initaliSMS();
  initaliCalls();
  ///////////////////////////////////////////
}
/////////////////////////////////////////////
void Readdatarelay()
{
  delay(200);
  while (GSM2ClickSE.available())
  {
    String c = GSM2ClickSE.readStringUntil(';');
    GSM2SMS = c;
    Serial.println(c); // The M95 modem should reply OK
  }

  relay1 = GSM2SMS.indexOf("Relay1-ON");
  relay2 = GSM2SMS.indexOf("Relay2-ON");
  relay3 = GSM2SMS.indexOf("Relay3-ON");
  relay4 = GSM2SMS.indexOf("Relay4-ON");
  relay5 = GSM2SMS.indexOf("Relay5-ON");
  relay6 = GSM2SMS.indexOf("Relay6-ON");
  relay7 = GSM2SMS.indexOf("Relay7-ON");
  relay8 = GSM2SMS.indexOf("Relay8-ON");
  relay9 = GSM2SMS.indexOf("Relay9-ON");
  relay10 = GSM2SMS.indexOf("Relay10-ON");
  relay11 = GSM2SMS.indexOf("Relay11-ON");
  relay12 = GSM2SMS.indexOf("Relay12-ON");
  relay13 = GSM2SMS.indexOf("Relay13-ON");
  relay14 = GSM2SMS.indexOf("Relay14-ON");
  relay15 = GSM2SMS.indexOf("Relay15-ON");
  relay16 = GSM2SMS.indexOf("Relay16-ON");

  MultRelay2 = GSM2SMS.indexOf("end");
  MultRelay1 = GSM2SMS.indexOf("Relay-");
  // Serial.print(MultRelay1); Serial.println('-');
  // Serial.print(Numbrelay1); Serial.println('-');

  if (Runonce == false)
  {
    if ((MultRelay1 > 10) && (MultRelay2 > 10))
    {
      if ((GSM2SMS[52] == 'R') && (GSM2SMS[53] == 'e'))
      {
        if (GSM2SMS[57] == '-')
        {
          Numbrelay1 = GSM2SMS[58];   Serial.print(Numbrelay1); Serial.print('-');
        }
        if (GSM2SMS[59] == '-')
        {
          Numbrelay2 = GSM2SMS[60];   Serial.print(Numbrelay2); Serial.print('-');
        }
        if (GSM2SMS[61] == '-')
        {
          Numbrelay3 = GSM2SMS[62];   Serial.print(Numbrelay3); Serial.print('-');
        }
        if (GSM2SMS[63] == '-')
        {
          Numbrelay4 = GSM2SMS[64];   Serial.print(Numbrelay4); Serial.print('-');
        }
        if (GSM2SMS[65] == '-')
        {
          Numbrelay5 = GSM2SMS[66];   Serial.print(Numbrelay5); Serial.print('-');
        }
        if (GSM2SMS[67] == '-')
        {
          Numbrelay6 = GSM2SMS[68];   Serial.print(Numbrelay6); Serial.print('-');
        }
        if (GSM2SMS[69] == '-')
        {
          Numbrelay7 = GSM2SMS[70];   Serial.print(Numbrelay7); Serial.print('-');
        }
        if (GSM2SMS[71] == '-')
        {
          Numbrelay8 = GSM2SMS[72];   Serial.print(Numbrelay8); Serial.print('-');
        }
        if (GSM2SMS[73] == '-')
        {
          Numbrelay9 = GSM2SMS[74];   Serial.print(Numbrelay9); Serial.print('-');
        }
        if (GSM2SMS[75] == '-')
        {
          Numbrelay10 = GSM2SMS[76];  Serial.print(Numbrelay10); Serial.print('-');
        }
        Runonce = true;
      }

      String num;
      for (int i = 51; i < 86; i++)
      {
        //delay(500);
        Serial.print(i);
        Serial.print('-');
        Serial.print(GSM2SMS[i]);
        num = num + GSM2SMS[i];
        Serial.print(";   ");
      }
      ComplMess = num;
      GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
      GSM2ClickSE.println("AT+CMGD=1,4");
    }
  }

  if (Runonce)
  {
    MultiRelaySMS(Numbrelay1);
    MultiRelaySMS(Numbrelay2);
    MultiRelaySMS(Numbrelay3);
    MultiRelaySMS(Numbrelay4);
    MultiRelaySMS(Numbrelay5);
    MultiRelaySMS(Numbrelay6);
    MultiRelaySMS(Numbrelay7);
    MultiRelaySMS(Numbrelay8);
    MultiRelaySMS(Numbrelay9);
    MultiRelaySMS(Numbrelay10);
    Serial.print("ENDDDDDDDDDDD");
    Serial.print('-');
    Runonce = false;
    ComplMess = "";
    MultRelay1 = 0;
    MultRelay2 = 0;
    Numbrelay1 = '0';
    Numbrelay2 = '0';
    Numbrelay3 = '0';
    Numbrelay4 = '0';
    Numbrelay5 = '0';
    Numbrelay6 = '0';
    Numbrelay7 = '0';
    Numbrelay8 = '0';
    Numbrelay9 = '0';
    Numbrelay10 = '0';

    GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
    GSM2ClickSE.println("AT+CMGD=1,4");
    SendSMSNotify("");

  }

  int callReceive = GSM2SMS.indexOf("CRING: VOICE");
  SMSCheckON = GSM2SMS.indexOf("SMS-ON");
  SMSCheckOFF = GSM2SMS.indexOf("SMS-OFF");
  SetControl = GSM2SMS.indexOf("SET CONTROL +49");
  SMSControl = GSM2SMS.indexOf("+CMT:");
  SMSdelete = GSM2SMS.indexOf("+CMTI:");
  RelayCheck500 = GSM2SMS.indexOf("RD-500");
  RelayCheck1000 = GSM2SMS.indexOf("RD-1000");
  RelayCheck6000 = GSM2SMS.indexOf("RD-6000");

  if (SMSdelete > 1)
  {
    GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
    GSM2ClickSE.println("AT+CMGD=1,4");
  }

  ////////////////////////////////////
  if ((SMSControl > 1) && (SetControl == -1) && (SMSCheckON == -1) & (SMSCheckOFF == -1) && (RelayCheck500 == -1) && (RelayCheck1000 == -1) & (RelayCheck6000 == -1))
  {
    for (int i = 9; i < 23; i++)
    {
      SMSMobile = SMSMobile + GSM2SMS[i];
      Serial.print(GSM2SMS[i]);
      //  delay(500);0
      //  Serial.print("-");
      //  Serial.print(i);
    }
    dataString3.trim();
    SMSMobile.trim();
    if (SMSMobile != dataString3)
    {
      relay1 = 0; relay2 = 0; relay3 = 0; relay4 = 0; relay5 = 0; relay6 = 0; relay7 = 0; relay8 = 0;
      SendSMSNotify("Unauthorized");
      SMSMobile = "";
    }
    else
    {
      Serial.print("Authorized");
      SMSMobile = "";
    }
    // GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");/// test for multi sms
  }

  ////////////////////////////////
  if (SetControl > 50)
  {
    String num;
    for (int i = 64; i < 78; i++)
    {
      num = num + GSM2SMS[i];
      Serial.print(GSM2SMS[i]);
      //      delay(500);
      //      Serial.print("-");
      //      Serial.print(i);
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
  ////////////////////////////////////////
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
  RelayCheck500 = GSM2SMS.indexOf("RD-500");
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
  RelayCheck1000 = GSM2SMS.indexOf("RD-1000");
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
  RelayCheck6000 = GSM2SMS.indexOf("RD-6000");
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
  if ((callReceive >= 3))// && (digitalRead(callPin) == 0))
  {
    GSM2ClickSE.println("ATA");
  }
  resp[resp.length() - 1] = '\0';

  //Relaydelay = 500;

  if (relay1 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    //processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    //  delay(Relaydelay);
    // processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    ActiveLight = 1;
    SendRelayNotify("Light ON");
  }
  if (relay2 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay2-ON", 0x02, 0x01, 0xAE, true);
    delay(Relaydelay);
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF, false);
    ActiveLight = 1;
    SendRelayNotify("Relay2-ON");
  }
  if (relay3 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay3-ON", 0x03, 0x01, 0xAF, true);
    delay(Relaydelay);
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0, false);
    ActiveLight = 1;
    SendRelayNotify("Relay3-ON");
  }
  if (relay4 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay4-ON", 0x04, 0x01, 0xB0, true);
    delay(Relaydelay);
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1, false);
    ActiveLight = 1;
    SendRelayNotify("Relay4-ON");
  }
  if (relay5 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay5-ON", 0x05, 0x01, 0xB1, true);
    delay(Relaydelay);
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2, false);
    ActiveLight = 1;
    SendRelayNotify("Relay5-ON");
  }
  if (relay6 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay6-ON", 0x06, 0x01, 0xB2, true);
    delay(Relaydelay);
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3, false);
    ActiveLight = 1;
    SendRelayNotify("Relay6-ON");
  }
  if (relay7 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay7-ON", 0x07, 0x01, 0xB3, true);
    delay(Relaydelay);
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4, false);
    ActiveLight = 1;
    SendRelayNotify("Relay7-ON");
  }
  if (relay8 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay8-ON", 0x08, 0x01, 0xB4, true);
    delay(Relaydelay);
    processdata("Relay8-OFF", 0x08, 0x02, 0xB5, false);
    ActiveLight = 1;
    SendRelayNotify("Relay8-ON");
  }
  if (relay9 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay9-ON", 0x09, 0x01, 0xB5, true);
    delay(Relaydelay);
    processdata("Relay9-OFF", 0x09, 0x02, 0xB6, false);
    ActiveLight = 1;
    SendRelayNotify("Relay9-ON");
  }
  if (relay10 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay10-ON", 0x0A, 0x01, 0xB6, true);
    delay(Relaydelay);
    processdata("Relay10-OFF", 0x0A, 0x02, 0xB7, false);
    ActiveLight = 1;
    SendRelayNotify("Relay10-ON");
  }
  if (relay11 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay11-ON", 0x0B, 0x01, 0xB7, true);
    delay(Relaydelay);
    processdata("Relay11-OFF", 0x0B, 0x02, 0xB8, false);
    ActiveLight = 1;
    SendRelayNotify("Relay11-ON");
  }
  if (relay12 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay12-ON", 0x0C, 0x01, 0xB8, true);
    delay(Relaydelay);
    processdata("Relay12-OFF", 0x0C, 0x02, 0xB9, false);
    ActiveLight = 1;
    SendRelayNotify("Relay12-ON");
  }
  if (relay13 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay13-ON", 0x0D, 0x01, 0xB9, true);
    delay(Relaydelay);
    processdata("Relay13-OFF", 0x0D, 0x02, 0xBA, false);
    ActiveLight = 1;
    SendRelayNotify("Relay13-ON");
  }
  if (relay14 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay14-ON", 0x0E, 0x01, 0xBA, true);
    delay(Relaydelay);
    processdata("Relay14-OFF", 0x0E, 0x02, 0xBB, false);
    ActiveLight = 1;
    SendRelayNotify("Relay14-ON");
  }
  if (relay15 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay15-ON", 0x0F, 0x01, 0xBB, true);
    delay(Relaydelay);
    processdata("Relay15-OFF", 0x0F, 0x02, 0xBC, false);
    ActiveLight = 1;
    SendRelayNotify("Relay15-ON");
  }
  if (relay16 > 50)
  {
    processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    processdata("Relay16-ON", 0x10, 0x01, 0xBC, true);
    delay(Relaydelay);
    processdata("Relay16-OFF", 0x10, 0x02, 0xBD, false);
    ActiveLight = 1;
    SendRelayNotify("Relay16-ON");
  }
  resp = "";
}
///////////////////////////////////////////////////////////////////////////
void processdata(String Text, byte rel, byte rell, byte data, bool offrelay)
{
  byte tt = 0;
  Serial2.write(0x55); Serial2.write(0x56); Serial2.write(tt); Serial2.write(tt); Serial2.write(tt); Serial2.write(rel); Serial2.write(rell); Serial2.write(data);
}
///////////////////////////////////////////////////////////////////////////
void gsmSendTextMessage(HardwareSerial * gsm_uart, String phone_no, String  sms_text)
{
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 300);
  GSM2ClickSE.write(26);
}
///////////////////////////////////////////////////////////////////////////
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
  int sensorVal = digitalRead(callPin);/// alwaya 1 HIGH
  if (sensorVal == 0)
  {
    String call = "0";

    for (int i = 3; i < dataString3.length(); i++)
    {
      call = call + dataString3[i];
    }
    GSM2ClickSE.println("ATD" + call + ";");
    Readdatarelay();
    //delay(100);
  }
  delay(300);
  Readdatarelay();
  //Serial.print(". ");
  if (ActiveCounter > 55)
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
      // digitalWrite( pinRelayCS2  , HIGH );
      // digitalWrite( pinRelaypwm1  , HIGH );
      if (ActiveCounter < 2)
      {
        processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
      }
    }
    else
    {
      // digitalWrite( pinRelayCS2  , LOW );
      // digitalWrite( pinRelaypwm1  , LOW );
      processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    }
  }
}
///////////////////////////////////////////////////////////////////////////
void SendSMSNotify(String Text)
{
  if (SMSNotify == 1)
  {
   // GSM2ClickSE.println("AT+CMGS=\"+4917673882064\"" );
    //GSM2ClickSE.println("AT+CMGS=\"+4917676552098\"" );
    GSM2ClickSE.println("AT+CMGS=\"+4915122918640\"" );
    delay(300);
    GSM2ClickSE.println(Text);
    //gsmSendTextMessage(&GSM2ClickSE, "017673882064", "OK");
    // gsmSendTextMessage(&GSM2ClickSE, "017676552098", "OK");
     gsmSendTextMessage(&GSM2ClickSE, "015122918640", "OK");
  }
  GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
}
///////////////////////////////////////////////////////////////////////////
void SendRelayNotify(String Text)
{
  if (SMSNotify == 1)
  {
    String t1 = "AT+CMGS=";
    GSM2ClickSE.println(t1 + "\"" + dataString3 + "\"");
    Readdatarelay();
    delay(100);
    GSM2ClickSE.println(Text);
    gsmSendTextMessage(&GSM2ClickSE, dataString3, "OK");
    Readdatarelay();
  }
  GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
  GSM2ClickSE.println("AT+CMGD=1,4");
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
  GSM2ClickSE.println( "AT" ); // Sending the 1st AT command
  delay(500);
}
///////////////////////////////////////////////////////////////////////////
void initaliSMS()
{
  GSM2ClickSE.println("AT+CSMS=1,1,1,1"); /// tried 1111 not good all 0 0 0 0 0  Test to see why many messages coming
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+CPMS=\"SM\"");  /// tried MT not good
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+CMGL=\"REC UNREAD\""); /// all also same
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+QINDI=1");
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+QIMODE=1");
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+CMGF=1");
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println( "AT+CNMI=2,2,0,0,0" );
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println( "AT+QCLASS0=0" );
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+CMGD=1,4");
  Readdatarelay();
  delay(500);
  GSM2ClickSE.println("AT+QMGDA=\"DEL ALL\"");
  delay(500);
}
///////////////////////////////////////////////////////////////////////////
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
  GSM2ClickSE.println("AT+QSCLK=0");
  Readdatarelay();
  processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
}
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void MultiRelaySMS(char Number)
{
  processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
  int RelaydelayWait = 2000;
  if (Number == '1')
  {
    // processdata("Relay1-ON", 0x01, 0x01, 0xAD, true);
    // delay(Relaydelay);
    // processdata("Relay1-OFF", 0x01, 0x02, 0xAE, false);
    ActiveLight = 1;
    Serial.println("111111");
    // delay(RelaydelayWait);
  }

  if (Number == '2')
  {
    processdata("Relay2-ON", 0x02, 0x01, 0xAE, true);
    delay(Relaydelay);
    processdata("Relay2-OFF", 0x02, 0x02, 0xAF, false);
    ActiveLight = 1;
    Serial.println("222222");
    delay(RelaydelayWait);
  }

  if (Number == '3')
  {
    processdata("Relay3-ON", 0x03, 0x01, 0xAF, true);
    delay(Relaydelay);
    processdata("Relay3-OFF", 0x03, 0x02, 0xB0, false);
    ActiveLight = 1;
    Serial.println("333333");
    delay(RelaydelayWait);
  }

  if (Number == '4')
  {
    processdata("Relay4-ON", 0x04, 0x01, 0xB0, true);
    delay(Relaydelay);
    processdata("Relay4-OFF", 0x04, 0x02, 0xB1, false);
    ActiveLight = 1;
    Serial.println("444444");
    delay(RelaydelayWait);
  }

  if (Number == '5')
  {
    processdata("Relay5-ON", 0x05, 0x01, 0xB1, true);
    delay(Relaydelay);
    processdata("Relay5-OFF", 0x05, 0x02, 0xB2, false);
    ActiveLight = 1;
    Serial.println("555555");
    delay(RelaydelayWait);
  }

  if (Number == '6')
  {
    processdata("Relay6-ON", 0x06, 0x01, 0xB2, true);
    delay(Relaydelay);
    processdata("Relay6-OFF", 0x06, 0x02, 0xB3, false);
    ActiveLight = 1;
    Serial.println("666666");
    delay(RelaydelayWait);
  }

  if (Number == '7')
  {
    processdata("Relay7-ON", 0x07, 0x01, 0xB3, true);
    delay(Relaydelay);
    processdata("Relay7-OFF", 0x07, 0x02, 0xB4, false);
    ActiveLight = 1;
    Serial.println("777777");
    delay(RelaydelayWait);
  }

  if (Number == '8')
  {
    processdata("Relay8-ON", 0x08, 0x01, 0xB4, true);
    delay(Relaydelay);
    processdata("Relay8-OFF", 0x08, 0x02, 0xB5, false);
    ActiveLight = 1;
    Serial.println("888888");
    delay(RelaydelayWait);
  }
  if (Number == '9')
  {
    processdata("Relay9-ON", 0x09, 0x01, 0xB5, true);
    delay(Relaydelay);
    processdata("Relay9-OFF", 0x09, 0x02, 0xB6, false);
    ActiveLight = 1;
    Serial.println("999999");
    delay(RelaydelayWait);
  }
  if (Number == 'A')
  {
    processdata("Relay10-ON", 0x0A, 0x01, 0xB6, true);
    delay(Relaydelay);
    processdata("Relay10-OFF", 0x0A, 0x02, 0xB7, false);
    ActiveLight = 1;
    Serial.println("AAAAAA");
    delay(RelaydelayWait);
  }
  if (Number == 'B')
  {
    processdata("Relay11-ON", 0x0B, 0x01, 0xB7, true);
    delay(Relaydelay);
    processdata("Relay11-OFF", 0x0B, 0x02, 0xB8, false);
    ActiveLight = 1;
    Serial.println("BBBBBB");
    delay(RelaydelayWait);
  }
  if (Number == 'C')
  {
    processdata("Relay-ON", 0x0C, 0x01, 0xB8, true);
    delay(Relaydelay);
    processdata("Relay-OFF", 0x0C, 0x02, 0xB9, false);
    ActiveLight = 1;
    Serial.println("CCCCCC");
    delay(RelaydelayWait);
  }
  if (Number == 'D')
  {
    processdata("Relay-ON", 0x0D, 0x01, 0xB9, true);
    delay(Relaydelay);
    processdata("Relay-OFF", 0x0D, 0x02, 0xBA, false);
    ActiveLight = 1;
    Serial.println("DDDDDD");
    delay(RelaydelayWait);
  }
  if (Number == 'E')
  {
    processdata("Relay-ON", 0x0E, 0x01, 0xBA, true);
    delay(Relaydelay);
    processdata("Relay-OFF", 0x0E, 0x02, 0xBB, false);
    ActiveLight = 1;
    Serial.println("EEEEEE");
    delay(RelaydelayWait);
  }
  if (Number == 'F')
  {
    processdata("Relay-ON", 0x0F, 0x01, 0xBB, true);
    delay(Relaydelay);
    processdata("Relay-OFF", 0x0F, 0x02, 0xBC, false);
    ActiveLight = 1;
    Serial.println("FFFFFF");
    delay(RelaydelayWait);
  }
  if (Number == 'G')
  {
    processdata("Relay-ON", 0x10, 0x01, 0xBC, true);
    delay(Relaydelay);
    processdata("Relay-OFF", 0x10, 0x02, 0xBD, false);
    ActiveLight = 1;
    Serial.println("GGGGGG");
    delay(RelaydelayWait);
  }
  Number = '0';

}
///////////////////////////////////////////////////////////////////////////
