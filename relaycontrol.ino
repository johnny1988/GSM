String resp = "";
void setup() {
  // put your setup code here, to run once:
  Serial3.begin(9600);
  Serial.begin(9600);
}
/// 55 56 00 00 00 01 03 AF

void loop() {
  // put your main code here, to run repeatedly:
  Serial3.write(0x55);
  Serial3.write(0x56);
  Serial3.write(0x00);
  Serial3.write(0x00);
  Serial3.write(0x00);
  Serial3.write(0x01);
  Serial3.write(0x03);
  Serial3.write(0xAF);

  delay(1000);

  //  Serial3.write(85);
  //  Serial3.write(86);
  //  Serial3.write(00);
  //  Serial3.write(00);
  //  Serial3.write(00);
  //  Serial3.write(01);
  //  Serial3.write(03);
  //  Serial3.write(175);
  byte result[8];
  int inc = 0;
  while (Serial3.available())
  {
    byte c = Serial3.read();
    resp += c;
    result[inc] = c;
    inc++;
    //Serial.println(c, HEX);
  }
  //resp[resp.length() - 1] = '\0';
  //Serial.println(resp); // The M95 modem should reply OK
  for (int i = 0; i < sizeof(result); i++)
  {
    Serial.print(result[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  resp = ""; 

  //  byte message[] = {0x55, 0x56, 0x00, 0x00, 0x00, 0x01, 0x03, 0xAF };
  //  Serial3.write(message, sizeof(message));
  //  Readdata();
  //  delay(6000);
  //  byte message1[] = {85, 56, 00, 00, 00, 01, 03, 176 };
  //
  //  Serial3.write(message1, sizeof(message));
  //  Readdata();
  //  Serial.println(message[0]);
  delay(6000);



}
