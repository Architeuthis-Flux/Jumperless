#ifndef CH446Q_H
#define CH446Q_H


void initCH446Q(void);

void sendAllPaths(void); // should we sort them by chip? for now, no

void sendPath(int path, int setOrClear = 1);

void createXYarray(void);

#endif