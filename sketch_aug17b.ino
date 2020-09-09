#define ADC_RES 1024
#define PWRKEY 49
#define STATUS A0
#include <HardwareSerial.h>

int stat = 0;
String resp = "";
#define BUFFER_SIZE 80
volatile uint32_t rx_data_ready = 0;
volatile char rx_buffer[BUFFER_SIZE];
volatile char tx_buffer[BUFFER_SIZE];

bool term_f = false;
bool frag_f = false;
bool head_f = false;
bool head_t  = true;
bool data_f = false;
bool summ_f = false;
bool summ_t = false;

bool exception_f = false;
bool data_t  = true;
bool response_f = false;
bool err_f = false;
char *data_ptr;
char *error;
#define AT_TRANSFER_SIZE                                1024
int rx_idx = 1;
int err_c = 0;


#define SERIAL_RX_BUFFER_SIZE 1024;
int gt = 0;
#define MCU_UART Serial2
#define GSM_UART Serial

void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String sms_text);
bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout);

void Readdata()
{
  delay(500);
  while (Serial2.available())
  {
    char c = Serial2.read();
    resp += c;
  }
  resp[resp.length() - 1] = '\0';
  Serial.println(resp); // The M95 modem should reply OK
  resp = "";
}

bool gsmSendCommandWithTimeout(HardwareSerial* gsm_uart, String at_command, unsigned int timeout)
{
  gsm_uart->println(at_command);
  MCU_UART.write(13);
  delay(timeout);
  return true;
}

void gsmSendTextMessage(HardwareSerial* gsm_uart, String phone_no, String  sms_text)
{
  //gsmSendCommandWithTimeout(gsm_uart, "AT+CMGS=\"" + phone_no + "\"", 500);
  gsmSendCommandWithTimeout(gsm_uart, sms_text, 500);
  MCU_UART.write(26);
  //  delay(1000);
  //  MCU_UART.write(26); // Just for debugging to see the Ctrl+Z byte being sent
  //  gsmSendCommandWithTimeout(gsm_uart, "AT+CMGS=?", 500);
  //  gsmSendCommandWithTimeout(gsm_uart, "AT&W0", 500);
}

void setup()
{

  MCU_UART.begin(115200);//SERIAL_8E1
  while (!MCU_UART) {
    ; // Loop until UART0 is open!
  }
  GSM_UART.begin(115200);//, SERIAL_8E1);  //
  while (!GSM_UART) {
    ; // Loop until UART1 is open!
  }

  Serial.begin(115200);
  Serial.println("Starting MCU UART...");

  pinMode(PWRKEY, OUTPUT);
  pinMode(STATUS, INPUT);
  // Make sure the PWRKEY on the GSM2 Click board is at logic LOW
  digitalWrite(PWRKEY, LOW);

  delay(250); // This isn't really neccessary...
  // Put the PWRKEY of the GSM@ Click board at logic HIGH
  // and wait until STATUS is at logic HIGH

  digitalWrite(PWRKEY, HIGH);

  //    while (stat < (ADC_RES-1))
  //    {
  //        stat = analogRead(STATUS); // For an ADC with a 10 bit resolution 5V = 2^10-1
  //    }
  // The modem is properly powered up, and we can now send AT commands

  Serial.println("GSM modem is powered-up!");
  digitalWrite(PWRKEY, LOW);
  Serial.println("Starting GSM2 UART...");
  Serial2.begin(115200);
  Serial2.println( "AT+IPR=115200" );
  Serial2.println( "AT" ); // Sending the 1st AT command
  // A mandatory 2-3 sec delay before expecting an answer according to QUECTEL's M95 documentation
  delay(1000);
  Serial2.println( "AT" );// Sending the 2nd AT command
  delay(500);


  Serial2.println( "AT+CNMI=1,2,0,0,0" );
  Readdata();
  Serial2.println( "AT+CSCS=“GSM”" );
  Readdata();
  Serial2.println("AT+QINISTAT=3");
  Readdata();
  Serial2.println("AT+QNITZ=1");
  Readdata();
  Serial2.println("AT+CSMS?");
  Readdata();
  Serial2.println("AT+CMGF=1");
  Readdata();
  Serial2.println("AT+CPMS=\"SM\"");
  Readdata();
  Serial2.println("AT+CMGL=\"ALL\"");
  Readdata();
  Serial2.println("AT+CSAS =0");
  Readdata();
  Serial2.println("AT+CSDH=1");
  Readdata();
  Serial2.println("AT+CMGR=3");
  Readdata();
  Serial2.println("AT+QCLASS0=1");
  Readdata();

  ////////////////////////////////////////////////////////////////////////////
  Serial2.println("ATS0=0");
  Readdata();
  Serial2.println("ATL2");
  Readdata();
  Serial2.println("ATM0");
  Readdata();
  Serial2.println("AT+QMOSTAT=1");
  Readdata();
  Serial2.println("AT+CRSL=20");
  Readdata();
  Serial2.println("AT+CLVL=50");
  Readdata();
  Serial2.println("AT+CMUT=0");
  Readdata();
  Serial2.println("AT+CALM=0");
  Readdata();
  Serial2.println("AT+QSIDET=0");
  Readdata();
  Serial2.println("AT+QMIC=0,15");
  Readdata();
  Serial2.println("AT+QAUDCH=2");
  Readdata();
  Serial2.println("AT+QTONEP=3");
  Readdata();
  Serial2.println("AT+QSPCH=0,0");
  Readdata();

  Serial2.println("AT+QLTONE=?");
  Readdata();
  Serial2.println("AT+QTONEP=?");
  Readdata();
  Serial2.println("AT+QTDMOD=?");
  Readdata();
  Serial2.println("AT+QAUDCH?");
  Readdata();
  Serial2.println("AT+QTONEDET?");
  Readdata();
  Serial2.println("AT+QWDTMF=?");
  Readdata();
  Serial2.println("AT+QAUDCH=2");
  Readdata();
  Serial2.println("AT+CNMI=2,2,0,0,0");
  Readdata();
  //  Serial2.println("AT+CMSS=2");/// to send saved data
  //  Readdata();
  delay(1000);
  Serial2.println("ATD017676552098;");
  delay(5000);
  Readdata();
  Serial2.println( "AT+W0" );
  Readdata();
  //////////////////////////////////////////////////////////////////////////////////

  Serial2.println("AT+CMGS=\"+4917676552098\"" );
  // Readdata();
  delay(1000);
  Serial2.println("Testing Johnny");
  Readdata();
  delay(1000);
  gsmSendTextMessage(&MCU_UART, "017676552098", "this is a new project for hotel system !");
  Readdata();

  /////////////////////////////////////////////////////////////////////////////////
}
void loop()
{
  gt++;
  String resp1 = "";
  if (gt > 50) gt = 0;
  //Serial2.println( "AT+CNMI=2,2,0,0,0" );
  //  delay(3000);

  //Serial2.println("AT+CMGL=\"ALL\"");
  Serial2.println( "AT+CMGR=" + String(gt));
  delay(3000);
  while (Serial2.available() > 0)
  {
    char c = Serial2.read();
    resp += c;
  }
  resp[resp.length() - 1] = '\0';
  Serial.println(resp); // The M95 modem should reply OK
  resp1 = resp;
  if (resp.indexOf("RING"))
  {
    // delay(500);
    //Serial2.println("ATA");
    //  delay(2000);
  }
  resp = "";
}


}
