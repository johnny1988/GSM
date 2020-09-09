// ----------------------------
// INCLUDE DIRECTIVES/LIBRARIES
//

//
// ----------------------------

// ----------------------------
// DEFINE DIRECTIVES/MACROS
//
#include <avr/io.h>
#include <HardwareSerial.h>

#define PWRKEY_PIN 49
#define STATUS_PIN A0
#include <SoftwareSerial.h>
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
byte received1[64];
SoftwareSerial myserial(15, 14);

#define MCU_UART Serial2
#define GSM_UART Serial
#define ADC_RES 1024
#define STATUS_TIMEOUT 3000
//
// ----------------------------

// ----------------------------
// GLOBAL VARIABLES
//
int g_stat = 0;                // GMS modem status
String g_resp = "";            // GSM modem response
char message1[64];
//
// ----------------------------

// ----------------------------
// FUNCTION PROTOTYPES
//
void pinSetup();
void gsmFlushBuffer(HardwareSerial* gsm_uart);
void gsmFillBuffer(HardwareSerial* gsm_uart);
bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout);
bool gsmSendCommand(HardwareSerial* gsm_uart, String at_command);
bool gsmGetStatus(int status_pin);
bool gsmPowerDown(HardwareSerial* gsm_uart);
bool gsmPowerUp(HardwareSerial* gsm_uart);
void gsmEchoOn(HardwareSerial* gsm_uart);
//void gsmEchoOff(HardwareSerial* gsm_uart);
void gsmGetFirmwareVersion(HardwareSerial* gsm_uart);
void gsmShowICCID(HardwareSerial* gsm_uart);
void gsmShowIMEI(HardwareSerial* gsm_uart);
void gsmSetTextingMode(HardwareSerial* gsm_uart);
void gsmSetTextingMemoryToSIM(HardwareSerial* gsm_uart);
void gsmGetTextingServiceCenter(HardwareSerial* gsm_uart);
void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
//
// ----------------------------

void setup()
{

  //  UCSR1C = 0b00000110
  //  UCSR1C = (1 << UCSZ11) | (1 << UCSZ10)

  // initialize serial:
  myserial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // Reserve 256 bytes/chars for the output buffer
  g_resp.reserve(256);
  // myserial.begin(115200);
  //  while (!myserial) {
  //    ; // Loop until UART0 is open!
  //  }
  // Open the MCU serial COM port
  MCU_UART.begin(115200);//SERIAL_8E1
  while (!MCU_UART) {
    ; // Loop until UART0 is open!
  }
  MCU_UART.println("Starting MCU UART...");

  // Open the GSM modem serial COM port
  GSM_UART.begin(115200);//, SERIAL_8E1);  //
  while (!GSM_UART) {
    ; // Loop until UART1 is open!
  }
  MCU_UART.println("Starting GSM2 UART...");

  // Setup the IO pins
  pinSetup();
  //if (myserial.isListening()) {
  //   myserial.println("Port One is listening!");
  //}
  // GSM modem power procedure and list basic info
  // gsmPowerDown(&GSM_UART);
  delay(1000);
  gsmPowerUp(&MCU_UART);
  //  gsmEchoOn(&MCU_UART);
  //  gsmGetFirmwareVersion(&MCU_UART);
  //  gsmShowICCID(&MCU_UART);
  //  gsmShowIMEI(&MCU_UART);
  gsmSetTextingMode(&MCU_UART);


  gsmSetTextingMemoryToSIM(&GSM_UART);
  gsmGetTextingServiceCenter(&GSM_UART);


  gsmSendTextMessage(&MCU_UART, "017676552098", "Johnny !");
  gsmcall(&MCU_UART);

  gsmSendTextMessage(&MCU_UART, "017676552098", "jany2 World!");
  //  gsmSendTextMessage(&MCU_UART, "017673882064", "jany3 World!");

}

void loop()
{
  //  //gsmSendCommandWithTimeout(&MCU_UART, "AT+CMGR=3", 500);
  //  //gsmSendCommandWithTimeout(&MCU_UART, "AT+IPR?", 300);
  //  //MCU_UART.write(55);
  //  int count = 0;
  //  while (GSM_UART.available() > 0)
  //  {
  //    char c = GSM_UART.read();
  //    if ((GSM_UART.read() != '\r') || (GSM_UART.read() != '\n'))
  //    {
  //      byte tr = GSM_UART.read();
  //      received[count++] = tr;
  //      g_resp.concat(c);
  //    }
  //  }
  //  MCU_UART.println(g_resp);
  //  MCU_UART.println(received[1]);
  //  MCU_UART.println(received[2]);
  //  MCU_UART.println(received[3]);
  //  MCU_UART.println(received[4]);
  //
  //  GSM_UART.println(g_resp);
  //  GSM_UART.println(received[1]);
  //  GSM_UART.println(received[2]);
  //  GSM_UART.println(received[3]);
  //  GSM_UART.println(received[4]);
  //
  //  g_resp = "";
  //
  //  //  gsmFillBuffer(&MCU_UART);
  //  //  gsmFlush(&MCU_UART);
  //  delay(5000);
}

void pinSetup()
{
  pinMode(PWRKEY_PIN, OUTPUT);
  pinMode(STATUS_PIN, INPUT);
  // Set pins initial state
  digitalWrite(PWRKEY_PIN, LOW);
}

void gsmFlush(HardwareSerial* gsm_uart)
{
  while (gsm_uart->available())
  {
    gsm_uart->read(); // 'Flush-out' everything that was read by the UART before issuing the next AT command!
  }
}

void gsmFillBuffer(HardwareSerial* gsm_uart)
{
  gsm_uart->readBytes(message1, 64);
  gsm_uart->println(message1);
  gsm_uart->readBytesUntil("OK", message1, 64);
  gsm_uart->println(message1);

  gsm_uart->readBytes(received1, 64);
  gsm_uart->println(received1[2]);
  gsm_uart->readBytesUntil('K', received1, 64);
  gsm_uart->println(received1[2]);

  //  if (gsm_uart->readStringUntil("OK"))
  //  {
  //    gsm_uart->println("OK");
  //  }
  //  if (gsm_uart->readStringUntil("+CMGF:1"))
  //  {
  //    gsm_uart->println("+CMGF:1");
  //  }
  //  if (gsm_uart->readStringUntil("+CMGF:0"))
  //  {
  //    gsm_uart->println("+CMGF:0");
  //  }
  //  while (gsm_uart->available())
  //  {
  //    char c = gsm_uart->read();
  //    gsm_uart->println((Serial.readString()));
  //    g_resp.concat(c);
  //  }
  //  g_resp = "";
  delay(250);

}

bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout)
{
  gsm_uart->println(at_command);
  MCU_UART.write(13);
  delay(timeout);

  return true;
}

bool gsmSendCommand(HardwareSerial* gsm_uart, String at_command)
{
  return gsmSendCommandWithTimeout(gsm_uart, at_command, 0);
}

bool gsmGetStatus(int status_pin)
{
  bool gsm_status = false;
  unsigned int t1 = millis();
  unsigned int t2 = 0;
  unsigned int delta_t = 0;

  // Wait until STATUS is at logic HIGH or timeout has been reached
  while (true)
  {
    g_stat = analogRead(status_pin);
    t2 = millis();
    delta_t = t2 - t1;

    if ( g_stat == ADC_RES - 1 )
    {
      MCU_UART.println("GSM Status: OK.");
      gsm_status = true;
      break; // 1st possible condition to exit from the while loop
    }

    if ( delta_t >= STATUS_TIMEOUT )
    {
      MCU_UART.println("GSM Status: KO!");
      break; // 2nd possible condition to exit from the while loop
    }
  }

  return gsm_status;
}

bool gsmPowerDown(HardwareSerial* gsm_uart)
{
  bool gsm_power = false;

  gsmSendCommandWithTimeout(gsm_uart, "AT+QPOWD=1", 5000);

  gsmFillBuffer(gsm_uart);

  return gsm_power;
}

bool gsmPowerUp(HardwareSerial* gsm_uart)
{
  bool gsm_power = false;

  gsmSendCommandWithTimeout(gsm_uart, "AT", 4000);
  gsmFillBuffer(gsm_uart);

  //  gsmSendCommandWithTimeout(gsm_uart, "AT&C0", 300);
  //  gsmFillBuffer(gsm_uart);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT&D0", 300);
  //  gsmFillBuffer(gsm_uart);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+ICF=3,3", 300);/// test1
  //  gsmFillBuffer(gsm_uart);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+IFC=1,1", 300); /// test2
  //  gsmFillBuffer(gsm_uart);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+ILRR=1", 300); /// test3
  //
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+IPR=115200", 300); /// test4
  //
    gsmSendCommandWithTimeout(gsm_uart, "AT+IPR?", 300); /// test5
  gsmFillBuffer(gsm_uart);
  //gsmSendCommandWithTimeout(gsm_uart, "AT+QCLASS0=1", 300);

  //  gsmSendCommandWithTimeout(gsm_uart, "ATI", 1000);
  //  gsmFillBuffer(gsm_uart);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+QGSN ", 1000);
  //  gsmFillBuffer(gsm_uart);
  //
  //  gsmFillBuffer(gsm_uart);
  //gsmFillBuffer(gsm_uart);
  //gsmSendCommandWithTimeout(gsm_uart, "AT+CMUX=0,0,5,127,10,3,30,10,2", 3000);

  // gsmSendCommandWithTimeout(gsm_uart, "AT+CSCS=”GSM”", 3000);
  // gsmSendCommandWithTimeout(gsm_uart, "AT", 1000); // At this point, we should get an answer 'OK'
  //gsmFillBuffer(gsm_uart);

  //gsmFillBuffer(gsm_uart);
  //gsmSendCommandWithTimeout(gsm_uart, "AT+IPR?", 1000);
  //gsmFillBuffer(gsm_uart);

  return gsm_power;
}

void gsmEchoOn(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "ATE1", 250);

  gsmFillBuffer(gsm_uart);
}

void gsmEchoOff(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "ATE1", 250);

  gsmFillBuffer(gsm_uart);
}

void gsmGetFirmwareVersion(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "ATI", 1000);

  gsmFillBuffer(gsm_uart);
}

void gsmShowICCID(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "AT+QCCID", 1000);

  gsmFillBuffer(gsm_uart);
}

void gsmShowIMEI(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "AT+GSN", 1000);

  gsmFillBuffer(gsm_uart);
}
void gsmcall(HardwareSerial* gsm_uart)
{

  //  gsmSendCommandWithTimeout(gsm_uart, "ATS0=1", 300);  /// Set Number of Rings before Automatically Answering Call
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+CRSL=100", 250);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+CLVL=100", 250);
  //  gsmSendCommandWithTimeout(gsm_uart, "ATL3", 300);
  //  gsmSendCommandWithTimeout(gsm_uart, "ATM2", 250);
  //gsmSendCommandWithTimeout(gsm_uart, "AT+CMUT=0", 250);

  // gsmSendCommandWithTimeout(gsm_uart, "AT+CLIP=1", 250);
  // gsmSendCommandWithTimeout(gsm_uart, "AT+COLP=1", 250);
  // gsmSendCommandWithTimeout(gsm_uart, "AT+QMIC=1,10", 250);
  gsmSendCommandWithTimeout(gsm_uart, "AT+QAUDCH=2", 250);

  //gsmSendCommandWithTimeout(gsm_uart, "AT+QTONEP=1", 250);
  //gsmSendCommandWithTimeout(gsm_uart, "AT+QWDTMF=7,0,”0A5,50,50,1,55,50,23,100,50”", 250);

  gsmSendCommandWithTimeout(gsm_uart, "ATD017676552098;", 500);
  gsmSendCommandWithTimeout(gsm_uart, "AT&W0", 1000);

  //gsmSendCommandWithTimeout(gsm_uart, "AT+VTS=1", 250);
}
void gsmSetTextingMode(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "AT+CFUN=1", 300);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGF=1", 250);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGF?", 250);
  gsmFillBuffer(gsm_uart);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CNMI=2,2,0,0,0", 250);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGR=1", 500);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGL=\"REC UNREAD\"", 300);
  gsmSendCommandWithTimeout(gsm_uart, " AT+CMGL=\"ALL\"", 300);
  gsmFillBuffer(gsm_uart);
}

void gsmSetTextingMemoryToSIM(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "AT+CPMS=\"SM\"", 100);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CPMS=?", 500);

  gsmFillBuffer(gsm_uart);
}

void gsmGetTextingServiceCenter(HardwareSerial* gsm_uart)
{
  gsmSendCommandWithTimeout(gsm_uart, "AT+CSCA?", 500);
  gsmFillBuffer(gsm_uart);
}

void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String  sms_text)
{
  // The AT command to send SMS
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGS=\"" + phone_no + "\"", 500);
  // The actual SMS text
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 500);
  // The Ctrl+Z terminator
  MCU_UART.write(26);
  delay(5000);
  MCU_UART.write(26); // Just for debugging to see the Ctrl+Z byte being sent
  //MCU_UART.write(13);
  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGS=?", 500);
  gsmSendCommandWithTimeout(gsm_uart, "AT&W0", 500);

  gsmFillBuffer(gsm_uart);
}
