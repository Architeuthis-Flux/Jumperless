


#define MAX_CONNECTIONS 50

#define EOT '\x04'
#define ACK '\x06'
#define STX '\x02'
#define RDY '\x01'


int connectionsToMake[MAX_CONNECTIONS][2];
int numberOfConnections = 0;
int node1 = 2;
int node2 = 4;
void setup() {

  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    connectionsToMake[i][0] = -1;
    connectionsToMake[i][1] = -1;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1500);
  Serial.begin(115200);
  delay(1500);



  for (int i = 0; i < 10; i++) {
    connectionsToMake[i][0] = node1;
    connectionsToMake[i][1] = node2;
    numberOfConnections++;
    node1 += 6;
    node2 += 6;
    if (node1 > 60) {
      node1 = 1;
    }
    if (node2 > 60) {
      node2 = 4;
    }
  }
}

int count = 1;

// the transmission order is:
// Arduino (A) - Jumperless (J)
//
// each step waits for the next acknowledgement

// A -> J : STX (\x02)
// A <- J : RDY (\x01)
// A -> J : sends one bridge (trailing comma)  6-9,
// A <- J : ACK (\x06)
// A -> J : sends another bridge (trailing comma)  4-20,
// A <- J : ACK (\x06)
// repeat until all bridges are sent
// A -> J : EOT (\x04)


void loop() {

  int acknowledged = 0;  //-1 is timed out
  char readAck = ' ';
  unsigned long timeout = 0;
  delay(500);
  numberOfConnections = 0;

  for (int i = 0; i < 10; i++) {  //this just makes the connections "march" across the board
    connectionsToMake[i][0] = ((node1 + count) % 60) + 1;
    connectionsToMake[i][1] = ((node2 + count) % 60) + 1;
    numberOfConnections++;
    node1 += 6;
    node2 += 6;
    if (node1 > 60) {
      node1 = 1;
    }
    if (node2 > 60) {
      node2 = 4;
    }
  }

  count++;

  timeout = millis();


  Serial.print("f ");
    timeout = millis();

    while (Serial.available() == 0 && (millis()-timeout< 300))

  while (Serial.available() > 0) {
    char throwaway = Serial.read();
    if (throwaway == "1")//this is just to stop the compiler from optimizing this out
    {
      continue;
    }
  }

  for (int i = 0; i < numberOfConnections; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    //delay(30); //doesn't even need this now

    Serial.print(connectionsToMake[i][0]);
    Serial.print("-");
    Serial.print(connectionsToMake[i][1]);
    Serial.print(", ");  //send one bridge

    digitalWrite(LED_BUILTIN, LOW);

    timeout = millis();

    while (Serial.available() == 0  && (millis()-timeout < 300))
      ;
    while (Serial.available() > 0) {
      char throwaway = Serial.read();
          if (throwaway == "1")//this is just to stop the compiler from optimizing this out
    {
      continue;
    }
    }


  }
  Serial.println();
  //  Serial.write(EOT);  //send end of transmission
  if (count >= 60) {  //this is just to make the paths move
    count = 3;
  }
}
