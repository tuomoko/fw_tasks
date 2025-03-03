// Standard libraries
#include <stdint.h>
#include <stdbool.h>

// TivaWare driver libraries
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// User libraries
#include "add.h"
#include "led_pwm.h"
#include "serial_handler.h"

// LED PIN configuration
#define LED_R_PORT GPIO_PORTF_BASE
#define LED_R_PIN  GPIO_PIN_2  // Red LED on PF1
#define LED_G_PORT GPIO_PORTF_BASE
#define LED_G_PIN  GPIO_PIN_2  // Green LED on PF3
#define LED_B_PORT GPIO_PORTF_BASE
#define LED_B_PIN  GPIO_PIN_2  // Blue LED on PF2

// UART configuration
#define UART_PORT UART1_BASE
#define BAUD_RATE 9600    // 9600 bps

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void) // cppcheck-suppress unusedFunction - this is defined in the ISR vector table
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART1_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART1_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART1_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART1_BASE,
                                   UARTCharGetNonBlocking(UART1_BASE));

        //
        // Blink the LED to show a character transfer is occuring.
        //
        GPIOPinWrite(LED_R_PORT, LED_R_PIN, LED_R_PIN);

        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        //
        SysCtlDelay(SysCtlClockGet() / (1000 * 3));

        //
        // Turn off the LED
        //
        GPIOPinWrite(LED_R_PORT, LED_R_PIN, 0);

    }
}

void delay_ms(uint32_t ms);

int main(void) {
    // Set system clock to 50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable clock for Port F & wait for it to be ready
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Enable the UART1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));

    // Enable the Port B for UART1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Configure PF2 as output (for Blue LED)
    GPIOPinTypeGPIOOutput(LED_B_PORT, LED_B_PIN);

    // Configure UART1 pins
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure UART1
    UARTConfigSetExpClk(UART_PORT, SysCtlClockGet(), BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable UART1 interrupt
    IntEnable(INT_UART1);
    UARTIntEnable(UART_PORT, UART_INT_RX | UART_INT_RT);

    // Write to UART0
    //UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);

    SerialPortHandler handler;
    init_serial_port_handler(&handler);

    //const char *input = "HELLO\\nWORLD\n";
    //for (int i = 0; input[i] != '\0'; ++i) {
    //    serial_receive_char(&handler, input[i]);
    //}
    

    while (1) {
        // Toggle LED
        GPIOPinWrite(LED_B_PORT, LED_B_PIN, LED_B_PIN);
        delay_ms(add(250,250));  // Delay 500ms
        GPIOPinWrite(LED_B_PORT, LED_B_PIN, 0);
        delay_ms(500);
    }
}

void delay_ms(uint32_t ms) {
    SysCtlDelay((SysCtlClockGet() / 3 / 1000) * ms);
}
