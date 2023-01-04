<!-- Explanation of the assignment and how to code works -->
# Pinger using ICMP protocol and Watchdog

Submitters: Noy Rosenbaum and Yael Rosen

## Table of Contents
- [Pinger using ICMP protocol and Watchdog](#pinger-using-icmp-protocol-and-watchdog)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
    - [Part 1 - prototyped Pinger](#part-1---prototyped-pinger)
    - [Part 2 - Pinger with Watchdog](#part-2---pinger-with-watchdog)
  - [Code run tutorial - Ubuntu](#code-run-tutorial---ubuntu)
    - [Setup project locally](#setup-project-locally)
      - [Clone an existing repository (**if it does not exist locally already**):](#clone-an-existing-repository-if-it-does-not-exist-locally-already)
      - [Compilation using Makefile](#compilation-using-makefile)
      - [Run files](#run-files)
      - [Delete exe files](#delete-exe-files)

## Description

### Part 1 - prototyped Pinger

"Prototyped Pinger" (ping.c) is  a utility used to test the reachability of a host on an Internet Protocol (IP) network and to measure the round-trip time for messages sent from the host \
to a destination host.
The Ping command works by sending Internet Control Message Protocol (ICMP) Echo Request messages to the target host and waiting for an ICMP Echo Reply. The Echo Request message contains a sequence number and a timestamp, and the Echo Reply message includes the same sequence number and timestamp, so the originator of the request can determine how long it took for the reply \
to be received. The Ping command can be implemented using raw sockets, which allow a program to access the underlying transport layer protocols directly.
When using raw sockets to implement the Ping command, the program will create an ICMP Echo Request message, send it using a raw socket, and wait for an ICMP Echo Reply message to be returned.

### Part 2 - Pinger with Watchdog

"Pinger with Watchdog" (better_ping.c and watchdog.c) is an improved version of "Prototyped Pinger" by using Watchdog timer. \
This addition is used to detect when the ping request has not received a reply within a certain amount of time (10 seconds) and to take appropriate action, such as terminating the ping request. \
The program first creates an ICMP Echo Request message and send it using a raw socket as described above. It would then start the watchdog timer with a predefined timeout period everytime it receives a "signal" packet from Pinger via sockets (defined with TCP connection and it happends whenever pinger sends a packet to IP destination). If the program receives an ICMP Echo Reply message before the watchdog timer expires, it resets the timer back to 0 seconds. If the watchdog timer expires before an Echo Reply is received, the program terminates the ping attempt and the entire process.
We implenent Part 2 with fork + exec which creates a new process by duplicating the calling process.

*Execution order:*
1. Checks if given arguments are valid. (Destination IP)
2. Creates a raw socket.
3. Gets in "watchdog" process (child part in fork())./
   3.1. Creates a Stream socket (server side)./
4. Make a TCP connection between 2 sides./
   3.2. Creates a "sender" socket (accept()).
5. Set up ICMP header.
6. Enters an infinite loop where we set ICMP packet's values, send this packet via raw socket, send a "signal" to watchdog. \
   3.3. Runs timer in a while loop, increments by 1 second. \
        if timer reaches 10 seconds, gets out of the loop and kill 2 processes. \
        else, another "signal" has received, - Reset timer back to 0 seconds.
7. Still in an the infinite loop, gets the ping response and finish measure time of RTT, prints ICMP packet details.

## Code run tutorial - Ubuntu

### Setup project locally

#### Clone an existing repository (**if it does not exist locally already**):

NOTE: *Get in the directory where you want to execute the program before.* \
To clone a repository locally do:
```
git clone <repo URL>
```
For example:
```
git clone https://github.com/noyrosenbaum/Networking_Course.git
```

#### Compilation using Makefile

1. `cd` to the existing repo directory.
```sh
cd ~/Networking_Course/Ex4/fork + exec
```
2. Compile files:
```sh
make all
```

#### Run files

1. for prototyped Pinger (ping.c) run:
```sh
sudo ./partA <IP_DESTINATION>
```
2. for Pinger with Watchdog (better_ping.c) run:
```sh
sudo ./partB <IP_DESTINATION>
```

#### Delete exe files

```sh
make clean
```


