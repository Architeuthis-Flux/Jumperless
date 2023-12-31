
void setup() {
  // put your setup code here, to run once:
pinMode(LED_BUILTIN, OUTPUT);

pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);

//int EEPROM.get(2);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(LED_BUILTIN, HIGH);

for (int i = 2; i <= 8; i++)
{
  digitalWrite(i, HIGH);
  delay(600);
  digitalWrite(i, LOW);
  



}

digitalWrite(LED_BUILTIN, LOW);
//delay(300);
}
