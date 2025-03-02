#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "add.h"

#define LED_PORT GPIO_PORTF_BASE
#define LED_PIN  GPIO_PIN_2  // Blue LED on PF2

void delay_ms(uint32_t ms);

int main(void) {
    // Set system clock to 50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable clock for Port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Wait for the GPIO module to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Configure PF2 as output (for Blue LED)
    GPIOPinTypeGPIOOutput(LED_PORT, LED_PIN);

    while (1) {
        // Toggle LED
        GPIOPinWrite(LED_PORT, LED_PIN, LED_PIN);
        delay_ms(add(250,250));  // Delay 500ms
        GPIOPinWrite(LED_PORT, LED_PIN, 0);
        delay_ms(500);
    }
}

void delay_ms(uint32_t ms) {
    SysCtlDelay((SysCtlClockGet() / 3 / 1000) * ms);
}
