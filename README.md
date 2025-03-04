# Serial command interface 
This is a serial command interface for the PWM module in EK-TM4C123GXL.

## Dependencies
- **gcc-arm-none-eabi** compiler for ARM
- **gcc** compiler for unit tests
- **clang-tidy** for static code analysis
- **cppcheck** also for static code analysis

## How to build
Build, static analysis and unit tests are included in Github Actions. They may be invoked in the command line as well:
- `make` to build the binaries for the target
- `make static-analysis` to run static analysis using cppcheck and clang-tidy
- `make test` to run unit tests using Unity

## Further improvements
- Separate platform specific code to another file from the main function file (led_pwm.c) to allow better readability and reusability of the code.
- Utilize a separate task (or process in the superloop) for handling the received command, instead of doing it in the ISR to allow faster speeds. DMA transfer for received characters for even faster speed.
- Test in the actual device and potentially fix some bugs.
- Consider use of C++ instead of C for better support for object oriented programming.
