
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  delay(1600);
  Serial.begin(115200);
  delay(600);
}

int count = 2;

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1230);
  digitalWrite(LED_BUILTIN, LOW);
  delay(10);

  if (digitalRead(10) == 0) {
    delay(3000);
  }

  Serial.write("f 105-"); //5V
  Serial.print(count);
  Serial.write(",116-70,117-71,");  //these 2 connections reconnect the UART lines so it's ready to send the next connection

  // Serial.write("::bridgelist["); //5V
  // Serial.print(count);
  // Serial.write("-105,116-70,117-71,]");  //these 2 connections reconnect the UART lines so it's ready to send the next connection


  count++;
  if (count > 29) {
    count = 3;
  }
}
