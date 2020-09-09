
#define SSerial2 Serial2
String resp = "";

int relay1off;
int relay1on;
unsigned char _rx_buffer[2048];

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
  Serial.begin(115200);
  Serial.println("Starting MCU UART...");

  Serial.println("Starting GSM2 UART...");
  SSerial2.begin(115200);

  SSerial2.println( "AT+IPR=115200" );
  SSerial2.println( "AT" ); // Sending the 1st AT command
  delay(1000);
  SSerial2.println( "AT" );// Sending the 2nd AT command
  delay(500);

  SSerial2.println("ATE1");
  Readdata();
  SSerial2.println("AT+CFUN=1");
  Readdata();
  SSerial2.println("AT+CMEE=1");
  Readdata();
  SSerial2.println("AT+CPIN?");
  Readdata();
  SSerial2.println("AT+QAUDCH=2");
  Readdata();
  SSerial2.println("AT*EREG=2");
  Readdata();
  SSerial2.println("AT+CLIP=1;+CRC=1");
  Readdata();

  SSerial2.println("ATL2");
  Readdata();
  SSerial2.println("ATM2");
  Readdata();
  SSerial2.println("AT+CLVL=50");
  Readdata();

  SSerial2.println("AT+QMIC=1,15");
  Readdata();
  SSerial2.println("ATD015122918640;");
  Readdata();

  SSerial2.println(" AT&W0");
  Readdata();
}

void loop()
{
  byte byteRead;
  int rec = 0;
  String testc;
  delay(5000);
  while (SSerial2.available() > 0)
  {
    String c;
    char c2;
    c = SSerial2.readStringUntil(';');
    c2 = SSerial2.read();
    //SSerial2.readBytes(c,2048);
    resp += c;
    rec++;
    _rx_buffer[rec] = c2;
    Serial.println(c);
    testc = c;
  }
  resp[resp.length() - 1] = '\0';
  Serial.println(resp);
  resp = "";
}
