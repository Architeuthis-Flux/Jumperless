#ifndef MENUS_H
#define MENUS_H
extern int defconDisplay;
enum actionCategories {
  SHOWACTION,
  RAILSACTION,
  SLOTSACTION,
  OUTPUTACTION,
  ARDUINOACTION,
  PROBEACTION,
  DISPLAYACTION,
  APPSACTION,
  ROUTINGACTION,
  NOCATEGORY
};


enum showOptions {
  VOLTAGE,
  CURRENT,
  GPIO5V,
  GPIO3V3,
  SHOWUART,
  SHOWI2C,
  NOSHOW
};
enum railOptions { TOP, BOTTOM, BOTH, NORAIL };
enum slotOptions { SAVETO, LOADFROM, CLEAR, NOSLOT };
enum outputOptions {
  VOLTAGE8V,
  VOLTAGE5V,
  DIGITAL5V,
  DIGITAL3V3,
  OUTPUTUART,
  OUTPUTI2C,
  NOOUTPUT
};
enum arduinoOptions { RESET, ARDUINOUART, NOARDUINO };
enum probeOptions { PROBECONNECT, PROBECLEAR, PROBECALIBRATE, NOPROBE };

// struct action {
//   actionCategories Category;
//   showOptions Show;
//   railOptions Rail;
//   slotOptions Slot;
//   outputOptions Output;
//   arduinoOptions Arduino;
//   probeOptions Probe;
//   float floatValue;
//   int intValues[10];
// };

extern int inClickMenu;
extern int selectingRotaryNode;

void readMenuFile(void);
void parseMenuFile(void);



void initMenu(void);    
int clickMenu(int menuType = -1 , int menuOption = -1, int extraOptions = 0);
int getMenuSelection(void);
int selectSubmenuOption(int menuPosition, int menuLevel);
int selectNodeAction(int whichSelection = 0);

void printActionStruct(void);
void clearAction(void);
int doMenuAction(int menuPosition = -1, int selection = -1);
void populateAction(void);

enum actionCategories getActionCategory(int menuPosition);

float getActionFloat(int menuPosition, int rail = -1);

char* findSubMenuString(int menuPosition, int selection);


char printMainMenu(int extraOptions = 0);
char LEDbrightnessMenu();


int findSubMenu(int level, int index);

void showLoss(void);

int yesNoMenu(void);









#endif