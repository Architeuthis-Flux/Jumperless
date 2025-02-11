#ifndef APPS_H
#define APPS_H

#define NUM_APPS 30




struct app {
  char name[20];
    const int index;
    int works = 0;
    void (*action)(void) = nullptr;
};



extern struct app apps[30];


void runApp (int index = -1, char* name = nullptr);






void scanBoard(void);
void calibrateDacs(void);

int oledTest(int sdaRow = -1, int sclRow = -1, int sdaPin = 22, int sclPin = 23);
int i2cScan(int sdaRow = -1 , int sclRow = -1, int sdaPin = 22, int sclPin = 23);








#endif