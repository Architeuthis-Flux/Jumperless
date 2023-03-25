
#include "JumperlessDefines.h"
#include "jMatrixControl.h"


//This is basically copied from the old code and is working well so this can be taken for granted



jMatrixControl::jMatrixControl(void)
{

}


int jMatrixControl::connectDumbMode(int x,int y, int chip, int connectDisconnect) //overloaded to accept char, int or ascii ints as the chip selection
{
    //digitalWriteFast(RESET, LOW);
    char charChip = chipIntToChar(chip); //this converts the chip into a char and back to allow it to accept ascii number values and stuff

    if (charChip == ' ') //if the chip sent was invalid (chipIntToChar returns a space), return 0
        {
          Serial.println("Bad Chip!");
            return 0; 
        }
    int intChipSanitized = chipCharToInt(charChip);
    Serial.println(intChipSanitized);

    setAddress(x,y);
    selectChip(intChipSanitized);

    

    strobeItIn(connectDisconnect);

    deselectChip();
    //Serial.printf("connected X %d to Y %d on chip %c\n\n\r", x,y,chipIntToChar(chip));
    return 1;



}


int jMatrixControl::connectDumbMode(int x,int y, char chip, int connectDisconnect)
{
//digitalWriteFast(RESET, LOW);
int intChipSanitized = chipCharToInt(chip); //converts to an int and allows upper or lower case

if (intChipSanitized == -1) // returns 0 if the chip is invalid
{
  Serial.println("Bad Chip!");
    return 0;
}

    setAddress(x,y);
    selectChip(intChipSanitized);
    strobeItIn(connectDisconnect);

    deselectChip();
    //Serial.printf("connected X %d to Y %d on chip %c\n\n\r", x,y,chipIntToChar(chip));
    return 1;

}



void jMatrixControl::setAddress(int Xaddr, int Yaddr)
{

const byte MTfuckedUpTruthTable [16] = {0,1,2,3,4,5,8,9,10,11,12,13,6,7,14,15}; //apparently X12 and X13 needed to be crammed in between X5 and X6

  byte XaddrFixed = MTfuckedUpTruthTable[Xaddr];


  digitalWrite(AX3, LOW);
  digitalWrite(AX2, LOW);
  digitalWrite(AX1, LOW);
  digitalWrite(AX0, LOW);

  digitalWrite(AY2, LOW);
  digitalWrite(AY1, LOW);
  digitalWrite(AY0, LOW);

  
  delayMicroseconds(310);

  int tempAX3 = (XaddrFixed & B00001000);
  tempAX3 = tempAX3 >> 3;
    int tempAX2 = (XaddrFixed & B00000100);
  tempAX2 = tempAX2 >> 2;
    int tempAX1 = (XaddrFixed & B00000010);
  tempAX1 = tempAX1 >> 1;
    int tempAX0 = (XaddrFixed & B00000001);


  digitalWrite(AX3, tempAX3); //this only writes the line high if that bit is set in binary value of Xaddr

  digitalWrite(AX2, tempAX2); //for example Xaddr = 6 or B00000110          //note: && is logical AND, and & is bitwise AND

  digitalWrite(AX1, tempAX1); //this bitwise ANDs Xaddr and a binary value with just one bit set

  digitalWrite(AX0, tempAX0); //so we get          Xaddr   00000110
  /*
Serial.print ("X ");
    Serial.print (tempAX3);
    Serial.print (tempAX2);
    Serial.print (tempAX1);
  Serial.println (tempAX0);
*/
int tempAY2 = Yaddr & B00000100;
tempAY2 = tempAY2 >> 2;
int tempAY1 = Yaddr & B00000010;
tempAY1 = tempAY1 >> 1;
int tempAY0 = Yaddr & B00000001;


  //                                                          AND bit selector   00001000 = 0
  digitalWrite(AY2, tempAY2); //then we AND that again with HIGH (which is just interpreted as 1)

  digitalWrite(AY1, tempAY1); //to get 1(HIGH) && 0(the result from above) = 0 (which is LOW)

  digitalWrite(AY0, tempAY0); //we do that for each bit to end up with the address lines LLLLLHHL
/*
Serial.print ("\nY ");
    Serial.print (tempAY2);
    Serial.print (tempAY1);
  Serial.println (tempAY0);
*/
  delayMicroseconds(925);

}

void jMatrixControl::strobeItIn(int connectDisconnect)
{

  if (connectDisconnect == 0)
  {
    digitalWrite(DATAPIN, LOW);
  }
  else
  {
    digitalWrite(DATAPIN, HIGH);
  }

  //pinMode(STROBE, OUTPUT);
  //delayMicroseconds(2);       //Hold time in the datasheet for the MT8816 says this only needs to be 10 nanoseconds
  digitalWrite(STROBE, HIGH); //but we're not super concerned with speed so I'll give it 1000X more just to be safe
  delayMicroseconds(250);       //Strobe has to be a minimum of 20 nanoseconds, but I dont want to think about the
  //Serial.println("!!!!!!!!!!!!");
  digitalWrite(STROBE, LOW); //fact that light only travels like 4 meters in that time through copper
  //pinMode(STROBE, OUTPUT);
  delayMicroseconds(250);
  digitalWrite(DATAPIN, LOW);
  //delayMicroseconds(30);
}

int jMatrixControl::selectChip(int chipInt)
{ //asserts whichever chip select line we send it

  digitalWrite(CS_A, LOW);  //Chip Selects are Active High on the MT8816
  digitalWrite(CS_B, LOW);  //make sure they're all deselected first
  digitalWrite(CS_C, LOW);
  digitalWrite(CS_D, LOW);
  digitalWrite(CS_E, LOW);
  digitalWrite(CS_F, LOW);
  digitalWrite(CS_G, LOW);
  digitalWrite(CS_H, LOW);
  digitalWrite(CS_I, LOW);
  digitalWrite(CS_J, LOW);
  digitalWrite(CS_K, LOW);

  delayMicroseconds(200);
  //Serial.print(chipToChar(chip));

    switch(chipInt)
    {
        case 0:
        digitalWrite(CS_A, HIGH); 
            return 1;
            break;

        case 1:
        digitalWrite(CS_B, HIGH);
            return 1;
            break;

        case 2:
        digitalWrite(CS_C, HIGH);
            return 1;
            break;

        case 3:
        digitalWrite(CS_D, HIGH);
            return 1;
            break;

        case 4:
        digitalWrite(CS_E, HIGH);
            return 1;
            break;

        case 5:
        digitalWrite(CS_F, HIGH);
            return 1;
            break;

        case 6:
        digitalWrite(CS_G, HIGH);
            return 1;
            break;

        case 7:
        digitalWrite(CS_H, HIGH);
            return 1;
            break;

        case 8:
        digitalWrite(CS_I, HIGH);
            return 1;
            break;

        case 9:
        digitalWrite(CS_J, HIGH);
            return 1;
            break;

        case 10:
        digitalWrite(CS_K, HIGH);
            return 1;
            break;
            
        default:
            return 0;

    }
}

void jMatrixControl::deselectChip(void)
{ //this should be fairly obvious

  digitalWrite(CS_A, LOW);
  digitalWrite(CS_B, LOW);
  digitalWrite(CS_C, LOW);
  digitalWrite(CS_D, LOW);
  digitalWrite(CS_E, LOW);
  digitalWrite(CS_F, LOW);
  digitalWrite(CS_G, LOW);
  digitalWrite(CS_H, LOW);
  digitalWrite(CS_I, LOW);
  digitalWrite(CS_J, LOW);
  digitalWrite(CS_K, LOW);

  digitalWrite(AX3, LOW);
  digitalWrite(AX2, LOW);
  digitalWrite(AX1, LOW);
  digitalWrite(AX0, LOW);

  digitalWrite(AY2, LOW);
  digitalWrite(AY1, LOW);
  digitalWrite(AY0, LOW);

  digitalWrite(DATAPIN, LOW);
  digitalWrite(STROBE, LOW);
  digitalWriteFast(RESET, LOW);

  return;
}

void jMatrixControl::clearAllConnections(void)
{ //when you send a reset pulse, all previous connections are cleared on whichever chip is CS_ed but we'll do both for now
/*
 digitalWriteFast(CS_A, HIGH);
 digitalWriteFast(CS_B, HIGH);
 digitalWriteFast(CS_C, HIGH);
 digitalWriteFast(CS_D, HIGH);
 digitalWriteFast(CS_E, HIGH);
 digitalWriteFast(CS_F, HIGH);
 digitalWriteFast(CS_G, HIGH);
 digitalWriteFast(CS_H, HIGH);
 digitalWriteFast(CS_I, HIGH);
 digitalWriteFast(CS_J, HIGH);
 digitalWriteFast(CS_K, HIGH);
*/
 digitalWriteFast(RESET, HIGH);
  delayMicroseconds(2000); //datasheet says 40 nanoseconds minimum, this is a lot more than that
 digitalWriteFast(RESET, LOW);
  delayMicroseconds(925);

 digitalWriteFast(CS_A, LOW);
 digitalWriteFast(CS_B, LOW);
 digitalWriteFast(CS_C, LOW);
 digitalWriteFast(CS_D, LOW);
 digitalWriteFast(CS_E, LOW);
 digitalWriteFast(CS_F, LOW);
 digitalWriteFast(CS_G, LOW);
 digitalWriteFast(CS_H, LOW);
 digitalWriteFast(CS_I, LOW);
 digitalWriteFast(CS_J, LOW);
 digitalWriteFast(CS_K, LOW);
}



char jMatrixControl::chipIntToChar(int chipInt)//also accepts the raw ascii values (65=A, 97=a, 66=B, 98=b...)
{

    switch (chipInt)
    {
            case 0:                   //fall through
            case 65:
            case 97:
                return 'A';
                break;

            case 1:
            case 66:
            case 98:
                return 'B';
                break;

            case 2:
            case 67:
            case 99:
                return 'C';
                break;

            case 3:
            case 68:
            case 100:
                return 'D';
                break;

            case 4:
            case 69:
            case 101:
                return 'E';
                break;

            case 5:
            case 70:
            case 102:
                return 'F';
                break;

            case 6:
            case 71:
            case 103:
                return 'G';
                break;

            case 7:
            case 72:
            case 104:
                return 'H';
                break;
                
            case 8:
            case 73:
            case 105:
                return 'I';
                break;

            case 9:
            case 74:
            case 106:
                return 'J';
                break;

            case 10:
            case 75:
            case 107:
                return 'K';
                break;

            default:
                return ' ';

    }
}

int jMatrixControl::chipCharToInt(char chipChar)
{

    switch (chipChar)
    {
            case 'A':
            case 'a':
                return 0;
                break;

            case 'B':
            case 'b':
                return 1;
                break;

            case 'C':
            case 'c':
                return 2;
                break;

            case 'D':
            case 'd':
                return 3;
                break;

            case 'E':
            case 'e':
                return 4;
                break;

            case 'F':
            case 'f':
                return 5;
                break;

            case 'G':
            case 'g':
                return 6;
                break;

            case 'H':
            case 'h':
                return 7;
                break;

            case 'I':
            case 'i':
                return 8;
                break;

            case 'J':
            case 'j':
                return 9;
                break;

            case 'K':
            case 'k':
                return 10;
                break;

            default:
                return -1;
    }
}


int8_t bottomRowTranslation (int8_t rowToTranslate)
{
  if(rowToTranslate <= 30) 
  {
    return rowToTranslate + 31;
  } else if (rowToTranslate > 30 && rowToTranslate <= 60){
    return rowToTranslate - 31;
  } else {
    Serial.println("Invalid Row!");
    return -1;
  }

}