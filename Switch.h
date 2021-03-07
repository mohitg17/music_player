#ifndef SWITCH_H
#define SWITCH_H

void SwitchInit(void(*playPause)(), void(*rewind)(), void(*setInst)());

void GPIOPortB_Handler(void);

#endif
