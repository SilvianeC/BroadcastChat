# BroadcastChat

Implemented features:
- Each Client connect to the Server via TCP/IP.
- The Server host more than 3 clients. 
- Each Client connected to the Server SHALL be able to send to the Server a character string passed by argument via command line arguments.
- The Server forward the received messages to all connected Clients.
- The Server identify the client that has sent the message.
- The Client switch his status from ONLINE to AFK if has not sent any new message for more than a minute.
- Usage of POSIX threads and device drivers.

-------------------------------------------------------------------------------------------------------------------------------------------

To use the code:

- The file server.c must be compiled to the host(computer).

- The file client.c must be compiled to the Raspberry Pi.

- In the Makefile the ARCH and the CROSS_COMPILE must be changed according to the architecture used, and the KDIR must be changed according to the location of linux-custom and the host.

- Its needed to write "make" on the terminal, and this will create the file led.ko that must be sent to the Raspberry.

- The executable for the client.c must be sent to the Raspberry.

- To execute the server.c its needed to write: ./<name_of_the_executable> <port>

- To execute the client.c its needed to write: ./<name_of_the_executable> <IP_adress> <port> . The port must be the same that the one used in the server.

- When the client is in execution, it will ask for a name, this name must be writen and then pressed enter, and its ready to use. Every writen message will be sent to the server and it will broadcast it to all the clients.

- When a client is connected to the server, the led on the Raspberry Pi will be turned on and when it disconnect the led will be turned off.





