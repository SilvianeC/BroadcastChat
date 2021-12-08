# BroadcastChat

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)


## General-info
This program implements a Broadcasting Chat Service using TCP/IP and it has the following features:

- Each Client connect to the Server via TCP/IP.
- The Server host more than 3 clients. 
- Each Client connected to the Server is able to send to the Server a character string passed by argument via command line arguments.
- The Server forward the received messages to all connected Clients.
- The Server identify the client that has sent the message.
- The Client switch his status from ONLINE to AFK if has not sent any new message for more than a minute.

## Technologies
This project uses:
- TCP/IP
- POSIX Threads
- Device drivers

To run this project, it is necessary:
- a Raspberry PI 4B
- Buildroot
- GCC (GNU Compiler, to compile the C files)

## Setup

**ATTENTION: the program is developed in a way that the client must be Raspberry PI 4!!If it isnt some errors might occur**

**1. Open the MAKEFILE and make this changes:**

KDIR := <write_the_path_to_linux_custom_on_your_computer>

ARCH ?= <write_the_architecture_of_the_Raspberry>  _eg: arm,arm64,aarch64_

CROSS_COMPILE?= <write_the_path_to_gcc_for_the_architecture_of_the_Raspberry>

**2. Compile the code by writing on the terminal the following commands:**

  `gcc server.c -o server.elf -pthread` _To compile the server to the host(your computer)_ 

  `<path_to_gcc_for_the_architecture_of_the_Raspberry> client.c -o client.elf -pthread` _To compile the client_ 

  `make` _This will generate the file led.ko, used for the device driver_
  
  **_Note:_** This step must be done with the terminal opened in the same directory as the downloaded code
  
**3.Open a new terminal and connect to the Raspberry PI, using this command:**

 `ssh root@<write_the_IP_adress_of_the_Raspberry>`

**4.Send the led.ko file and the client.c to the Raspberry PI, using this commands:**

  `scp led.ko root@<write_the_IP_adress_of_the_Raspberry>:/etc`
  
  `scp client.elf root@<write_the_IP_adress_of_the_Raspberry>:/etc`
  
   **_Note:_** This step must be done with the terminal opened in the same directory as the downloaded code

**5. Execute the program**

- In the host, write on a terminal:
`./server.elf <write_the_port>`
**_Note:_** This step must be done with the terminal opened in the same directory as the downloaded code

- In the Raspberry PI, write on the terminal of step 3:
```
cd /
cd etc
ls
./client.elf <write_the_IP_adress_of_the_client> <write_the_port> 
```
**_Notes:_** 
- The IP adress of the client must belong to the same netmask as the Raspberry PI
- The port must be the same that the one used in the server.

**6. During execution**

- When the client is in execution, this message will be shown: `Write your name`, you must write your name and press enter.After this, you can write your messages and press enter and every writen message will be sent to the server.
- The server will broadcast it to all the clients identifying the one who sended the message.
- The client can disconnect by writing "CLOSE".
- When a client is connected to the server, the led on the Raspberry Pi will be turned on and when it disconnect the led will be turned off.





