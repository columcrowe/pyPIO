// Minimal - no HAL at all
#define RCC_AHB1ENR   (*(volatile unsigned int*)0x40023830)
#define GPIOD_MODER   (*(volatile unsigned int*)0x40020C00)  
#define GPIOD_BSRR    (*(volatile unsigned int*)0x40020C18)

int main(void)
{
    // Enable GPIOD clock
    RCC_AHB1ENR |= (1 << 3);
    
    // Set PD12 as output  
    GPIOD_MODER |= (1 << 24);
    
    while(1)
    {
        // LED ON
        GPIOD_BSRR = (1 << 12);
        for(volatile int i = 0; i < 500000; i++);
        
        // LED OFF  
        GPIOD_BSRR = (1 << 28);  // Reset bit 12
        for(volatile int i = 0; i < 500000; i++);
    }
}