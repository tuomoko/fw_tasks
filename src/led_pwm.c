/*
 * Copyright (c) 2025 Tuomo Kohtamäki
 * 
 * This is the main function file, which also includes all device configurations.
 * 
 * Based on examples in TiVaWare.
 */

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
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// User libraries
#include "led_pwm.h"
#include "serial_handler.h"

// LED configuration
#define LED_R_PWM_OUT PWM_OUT_5
#define LED_G_PWM_OUT PWM_OUT_7
#define LED_B_PWM_OUT PWM_OUT_6

// UART configuration
#define BAUD_RATE 9600    // 9600 bps

// UART handler
SerialPortHandler handler;

/**
 * @brief UART1 interrupt handler.
 * 
 * This function is called when a character is received on UART1.
 */
void UARTIntHandler(void) // cppcheck-suppress unusedFunction - this is defined in the ISR vector table
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
        serial_receive_char(&handler, UARTCharGetNonBlocking(UART1_BASE));
    }
}

/**
 * @brief Handles the PWM signal for the LED.
 * 
 * This function sets the PWM signal for the LED based on the received values.
 * 
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 */
void led_pwm_handler(__uint8_t r, __uint8_t g, __uint8_t b) {
    // Set the LED colors by PWM. As our period is 100, we can use the received values directly.
    PWMPulseWidthSet(PWM1_BASE, LED_R_PWM_OUT, r);
    PWMPulseWidthSet(PWM1_BASE, LED_G_PWM_OUT, g);
    PWMPulseWidthSet(PWM1_BASE, LED_B_PWM_OUT, b);
}

/**
 * @brief Sends a character to the UART.
 * 
 * This function sends a character to the UART.
 * 
 * @param c The character to send.
 */
void uart_send_handler(unsigned char c)
{
    UARTCharPut(UART1_BASE, c);
}

/**
 * @brief Main function.
 * 
 * Set up all and start eternal loop.
 * 
 * @todo move initializations to separate function(s).
 */
int main(void) {
    // Set system clock to 50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Set PWM clock divider to 1
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    // Enable clock for Port F & wait for it to be ready
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Enable the UART1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));

    // Enable the Port B for UART1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Configure UART1 pins
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure UART1
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable the PWM1 peripheral that can drive the LED pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM1));

    // Configure the GPIO pad for PWM function on pins PF1, PF3 and PF2.
    GPIOPinConfigure(GPIO_PF1_M1PWM5); //R
    GPIOPinConfigure(GPIO_PF3_M1PWM7); //G 
    GPIOPinConfigure(GPIO_PF2_M1PWM6); //B
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Configure the PWM1 to generate a PWM signal
    // PWM_GEN_2 for output 5
    // PWM_GEN_3 for outputs 6,7
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 100); // Set period to 100, means 500 Hz PWM signal
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 100); // Set period to 100, means 500 Hz PWM signal
    PWMPulseWidthSet(PWM1_BASE, LED_R_PWM_OUT, 0); // 0% duty cycle
    PWMPulseWidthSet(PWM1_BASE, LED_G_PWM_OUT, 0); // 0% duty cycle
    PWMPulseWidthSet(PWM1_BASE, LED_B_PWM_OUT, 0); // 0% duty cycle
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_7_BIT | PWM_OUT_6_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Init serial port handler
    init_serial_port_handler(&handler, led_pwm_handler, uart_send_handler);

    // Enable UART1 interrupt to start receiving data
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

    // Infinite loop
    while (1) {    }
}
