<!-- Explanation of the assignment and how to code works -->
# Packet Sniffing and Spoofing

Submitters: Noy Rosenbaum and Yael Rosen

## Description

**NOTE:** For detailed explanations about any of the code files, check out the comments in each one of the scripts.

### Part 1 - Sniffer

*Question:* Why do you need the root privilege to run a sniffer program? Where does the program fail if it is executed without the root privilege?
*Answer:* Sniffing data packets is a privileged task as it raises concerns about privacy and security. If non-privileged users are able to access this function, they may potentially steal personal information and account passwords, violating others' privacy.
Run with normal permissions, the returned socket descriptor is -1, indicating that the creation of raw socket failed.

### Part 2 - Spoofer

*Question:* Can you set the IP packet length field to an arbitrary value, regardless of how big the actual packet is?
*Answer:*   The IP packet length field must match the actual size of the packet, which is specified as a 16 bits value that represents the total length of the packet, including header and payload. If this field is set to a value that does not match the actual size of the packet, it will not be able to be forwarded.
*Question:* Using the raw socket programming, do you have to calculate the checksum for the IP header?
*Answer:*   You do not need to calculate the checksum for the IP header it is filled by the operating system.

### Part 3 - Snoofer - combines both Sniffer and Spoofer

### Part 4 - Gateway

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
cd ~/Networking_Course/Ex5/
```
2. Compile files:
```sh
make all
```

#### Run files

#### Sniffer

1. for Sniffer, run on seperate terminals:
2. Terminal 1: \
Go to this directory:
```sh
cd ~/Networking_Course/Ex2/
```
Run this command:
```sh
python3 ./server.py
```
3. Terminal 2: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/
```
Run this command after you compiled the file:
```sh
sudo ./Sniffer
```
1. Terminal 3: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex2/
```
Run this command:
```sh
python3 ./client.py
```

#### Spoofer
   
1. for Spoofer: \
Go to this directory:
```sh
cd ~/Networking_Course/Ex5/
```
2. Run this command after you compiled the file:
```sh
sudo ./Spoofer <Source IP> <Destination IP>
```
For example:
```sh
sudo ./Spoofer 1.2.3.4 8.8.8.8
```

#### Delete exe files

```sh
make clean
```

