<!-- Explanation of the assignment and how to code works -->
# Pinger using ICMP protocol and Watchdog

## Table of Contents
- [Pinger using ICMP protocol and Watchdog](#pinger-using-icmp-protocol-and-watchdog)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
    - [Part 1 - prototyped Pinger](#part-1---prototyped-pinger)
  - [Code run tutorial - Ubuntu](#code-run-tutorial---ubuntu)
    - [Setup project locally](#setup-project-locally)
      - [Clone an existing repository (**if it does not exist locally already**):](#clone-an-existing-repository-if-it-does-not-exist-locally-already)
      - [Compilation using Makefile](#compilation-using-makefile)
      - [Run files](#run-files)
      - [Delete exe files](#delete-exe-files)

## Description

### Part 1 - prototyped Pinger

"Prototyped Pinger" (ping.c) is  a utility used to test the reachability of a host on an Internet Protocol (IP) network and to measure the round-trip time for messages sent from the originating host to a destination host.


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
1. Compile files:
```sh
make all
```

#### Run files

1. for prototyped Pinger (ping.c) run:
```sh
sudo ./partA <IP_DESTINATION>
```
1. for Pinger with Watchdog (better_ping.c) run:
```sh
sudo ./partB <IP_DESTINATION>
```

#### Delete exe files

```sh
make clean
```


