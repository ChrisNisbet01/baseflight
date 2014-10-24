#ifndef __CF_COOS_H__
#define __CF_COOS_H__

extern OS_FlagID mainLoopFlagID;

void initCoos( void );
void startCoos( void );
void CoosSysTickHandler();

#endif /* __CF_COOS_H__ */
