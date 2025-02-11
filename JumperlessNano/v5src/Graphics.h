#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Arduino.h>

class bread {
    public:
    bread();
    void print(const char c);
    void print(const char c, int position);
    void print(const char c, uint32_t color);
    void print(const char c, uint32_t color, int position, int topBottom);
    void print(const char c, uint32_t color, int position);
    void print(const char c, uint32_t color, uint32_t backgroundColor);
    void print(const char c, uint32_t color, uint32_t backgroundColor, int position, int topBottom);
    void print(const char c, uint32_t color, uint32_t backgroundColor, int position, int topBottom, int nudge);
    void print(const char c, uint32_t color, uint32_t backgroundColor, int position, int topBottom, int nudge, int lowercase);
    void print(const char c, uint32_t color, uint32_t backgroundColor, int position);

    void print(const char* s);
    void print(const char* s, int position);
    void print(const char* s, uint32_t color);
    void print(const char* s, uint32_t color, int position);
    void print(const char* s, uint32_t color, int position, int topBottom);
    void print(const char* s, uint32_t color, uint32_t backgroundColor);
    void print(const char* s, uint32_t color, uint32_t backgroundColor, int position, int topBottom);
    void print(const char* s, uint32_t color, uint32_t backgroundColor, int position, int topBottom, int nudge);
    void print(const char* s, uint32_t color, uint32_t backgroundColor, int position, int topBottom, int nudge, int lowercaseNumber);
    void print(const char* s, uint32_t color, uint32_t backgroundColor, int position);


    void print(int i);
    void print(int i, int position);
    void print(int i, uint32_t color);
    void print(int i, uint32_t color, int position);
    void print(int i, uint32_t color, int position, int topBottom);
    void print(int i, uint32_t color, int position, int topBottom, int nudge);
    void print(int i, uint32_t color, int position, int topBottom, int nudge, int lowercase);
    void print(int i, uint32_t color, uint32_t backgroundColor);

void barGraph(int position, int value, int maxValue, int leftRight,
                     uint32_t color, uint32_t bg);

void printMenuReminder(int menuDepth, uint32_t color);
void printRawRow(uint8_t data, int row, uint32_t color, uint32_t bg, int scale = 1);

   
    void clear(int topBottom = -1);

    // void printChar(char c, uint32_t color, uint32_t backgroundColor, int position, int topBottom);





};


    struct specialRowAnimation{
        int index;
        int net;
        unsigned long currentFrame;
        int direction;
        int numberOfFrames = 8;
        uint32_t frames[15] = {0xffffff};
        unsigned long lastFrameTime;
        unsigned long frameInterval = 100;

        
    };
    
    extern int defNudge;
extern specialRowAnimation rowAnimations[26];

extern bool animationsEnabled;
extern char defconString[16];
extern const uint8_t font[][3];
extern volatile int doomOn;

extern int menuBrightnessSetting;
extern bread b;

void playDoom(void);
void initRowAnimations (void);
void showAllRowAnimations(void);
void showRowAnimation(int row, int net);
void printGraphicsRow(uint8_t data, int row, uint32_t color = 0xFFFFFF, uint32_t bg = 0xFFFFFF);

void scrollFont(void);

void printChar(const char c, uint32_t color = 0xFFFFFF, uint32_t bg = 0xFFFFFF, int position = 0, int topBottom = -1, int nudge = 0, int lowercase = 0);

void printString(const char* s, uint32_t color = 0xFFFFFF, uint32_t bg = 0xFFFFFF, int position = 0, int topBottom = -1, int nudge = 0, int lowercase = 0);

 void drawWires(int net = -1);
 void printWireStatus(void);

void defcon(int start, int spread, int color = 0, int nudge = 1);

void printTextFromMenu(void);
int attractMode(void);












#endif
