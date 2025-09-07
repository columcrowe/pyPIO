#ifndef __STM32_F4_DISC_HALBLINKY
#define __STM32_F4_DISC_HALBLINKY

#ifdef __cplusplus
 extern "C" {
#endif

void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_F4_DISC_HALBLINKY */