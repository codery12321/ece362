# ECE 362 Microprocessor Systems and Interfacing
## List of Lab Assignments
### lab0: Getting Started
>In ECE362, students will learn fundamentals of microcontrollers, including their operation, and usage. In addition to the lecture portion of the course, students are expected to complete a series of lab experiments using a microcontroller platform and IDE. Computers and measurement equipment are provided in the course laboratory facilities to assist students in completing labs, however, the course has also been designed to allow students to perform experiment exercises on their own computers at home or elsewhere. Instructions provided in this lab document serves as a guide to setting up the microcontroller development environment used in ECE362 to provide a consistent user experience between home and the laboratory.   

### lab1: First Blinks
>Traditionally, the "first steps" taken to verify that a microcontroller works and that you know how to use it, is to make a light blink. In this introductory exercise, you will use an integrated development environment (IDE) to interact with a microcontroller. You will type in, compile, and debug a few programs that we have prepared for you. One of the programs will cause the built-in LEDs of the STM32F091 development board to blink. You will also write a few simple assembly language subroutines.  

>Now that you know that the hardware in your lab kit works, you are comfortable with creating projects, and you've had some experience typing in programs, you will be expected to complete this lab autonomously. If you need help, ask a teaching assistant. In lab, you will receive help for and be checked off for subroutines that you complete. You will also be given a module that will evaluate the subroutines. It will generate a completion code on the serial port as with Lab 0.

### lab2: Assembly Language Programming
>Now that you know how to use an Integrated Development Environment and use assembly instructions, it is time to actually do some programming. In the prelab assignment, you will test your knowledge of how various instructions actually work. For the in-lab experiment, you will write short assembly language programs that perform simple tasks.

>Now that you know that the hardware in your lab kit works, you are comfortable with creating projects, and you've had some experience typing in programs, you will be expected to complete this lab autonomously. If you need help, ask a teaching assistant. This lab will also not need to be evaluated when you finish it. You will receive a completion code on the serial port as you did with Lab 0.

### lab3: General Purpose I/O
>In this experiment, you will learn how to connect simple input devices (push buttons and keypad) and simple output devices (LEDs) to an STM32 development board. You will learn how to configure the General Purpose Input/Output (GPIO) subsystems. You will read values from and write values to these pins.

### lab4: Interrupts
>Microprocessors consistently follow a straight sequence of instructions, and it is likely that you have only worked with this kind of programming until now. In this experiment, you will configure a microprocessor to handle exceptional events. These events will be generated in an expected manner, but the principles are the same for failures, faults, and other unexpected events. These events will invoke Exception Handlers that you write. These Exception Handlers will efficiently respond to events only when needed and will not run continually.

### lab5: Timers
>Each timer subsystem allows the microcontroller to produce periodic interrupts while the CPU is busy with other operations. In this experiment, we will use timer interrupts to periodically scan a matrix keypad and reliably report button presses.  

### lab6: Analog Input/Output
>Interfacing a microcontroller with real-world devices often means working with analog voltages that are not constrained by digital logic’s high/low dichotomy. In this lab, you will gain experience using the digital-to-analog converter and the analog-to-digital converter built in to your microcontroller. You will also practice writing code to manipulate the converted values as well as display them on an output device.

>Direct Memory Access (DMA) allows you to automatically transfer a region of memory to a peripheral, a different location in memory, or any memory mapped region of the device. In this lab, you will also gain experience using DMA.  

### lab7: Pulse Width Modulation
![intro](lab07/images/intro.png)
>A microcontroller’s interaction with the real world (and real humans) often requires the generation of voltages that can vary over a continuous spectrum. Although your microcontroller has an on-board digital-to-analog converter (DAC), it is too weak to drive any load that requires high power. Even with amplification, a DAC is inefficient for the purpose of driving heavy loads. Digital electronic systems are well-suited to switching circuits on and off. In circumstances where a load can be switched on and off in such a way that the average output voltage is treated (or perceived) as the signal, a solution is Pulse-Width Modulation (PWM). In this experiment, you will learn about configuring and using PWM to control a high-power load (a light) and generate a variable duty-cycle waveform.

### lab8: Serial Peripheral Interface (and DMA)     
> The Serial Peripheral Interface (SPI) is a widely-used method for communicating with digital devices with an economy of wires and connections. It is possible to control such devices by "bit-banging" the protocol using GPIO, but your microcontroller has high-level support for SPI devices that simplifies the use of such interfaces. This support also allows for the use of Direct Memory Access (DMA) to automatically transfer a region of memory to the device. In this lab, you will gain experience using SPI and DMA with display devices.

### lab9: Inter-Integrated Circuit (I2C) Interface
> <img src="https://github.com/codery12321/ece362/blob/744f41cc56ad9f871f6f235a81f383a68a9740be/labs/lab09/images/intro.png" height="100"> Serial interface formats are convenient because they require a small number of physical connections. They are often limited in the number of devices that can be connected at once, or they require cumbersome control logic to enable and disable devices. Inter-Integrated Circuit (I2C) is a synchronous serial interface that uses an addressing scheme to select between multiple communication targets on a shared bus. Although it is generally slower than SPI, the flexibility of easily connecting multiple devices is useful when communication speed is not critical. In this lab, you will gain experience using and testing I2C devices. You will examine I2C protocol using a USB logic analyzer to identify correct and incorrect interface signals.
### lab10: Asynchronous Serial Interface
> <img src="https://github.com/codery12321/ece362/blob/de37f3771e1833f133821f11c1c268c356113c3f/labs/lab10/images/intro.png" height="150"> Asynchronous serial communication using a Universal Asynchronous Receiver/Transmitter (UART) has been a staple of inter-computer communication since its introduction in the 1960s. Although asynchronous serial has been largely supplanted by more specialized communication protocols that are faster or more specially structured, it remains the only interface on your microcontroller that can be readily used for ad hoc interactive bidirectional communication. In this lab, you will use a UART to implement communication with an interactive program. You will examine the asynchronous serial protocol using an oscilloscope to identify the communication payload.
