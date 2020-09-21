/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.
*/

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

int pinRelaypwm1 = 77;
int pinRelayCS2 = 6;

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  test1.begin(115200);
  SerialUSB.begin(9600);
  
  /////////
  pinMode( pinRelayCS2  , OUTPUT );
  pinMode( pinRelaypwm1  , OUTPUT );
  /////////

  initialGSM();
  initaliSMS();
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
  resp[resp.length() - 1] = '\0';

  //  50 >   52 is actual
  //  Serial.println(relay1); // The M95 modem should reply OK
  //  Serial.println(relay2); // The M95 modem should reply OK
  //  Serial.println(relay3); // The M95 modem should reply OK
  //  Serial.println(relay4); // The M95 modem should reply OK
  //  Serial.println(relay5); // The M95 modem should reply OK
  resp = "";
}

void loop()
{
  delay(300);
  Readdatarelay();
  Serial.println(". ");
  byte t = 0;
  delay(2000);
  SerialUSB.write(0x55); SerialUSB.write(0x56);
  SerialUSB.write(t); SerialUSB.write(t); SerialUSB.write(t); SerialUSB.write(0x01); SerialUSB.write(0x03); SerialUSB.write(0xAF);
}

void initialGSM()
{
  test1.println( "AT" ); // Sending the 1st AT command
  delay(500);
  test1.println("AT+IPR=115200");
  delay(500);
  digitalWrite( pinRelayCS2  , HIGH );
  delay(1000);
  digitalWrite( pinRelaypwm1  , HIGH );
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
  test1.println( "AT+CNMI=2,2,0,0,0" );  // TEST1  0,2-S  1.2 S  3.2S->Test3 2.0 N.G

  digitalWrite( pinRelayCS2  , LOW );
  delay(1000);
  digitalWrite( pinRelaypwm1  , LOW );
}
