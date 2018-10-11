#ifndef TOUCHSCREEN_H_
#define TOUCHSCREEN_H_


extern uint16_t touchscreen_readXADC(void);
extern uint16_t touchscreen_readYADC(void);
extern void setupADC(void);
extern uint8_t detect(void);

#endif /* TOUCHSCREEN_H_ */
