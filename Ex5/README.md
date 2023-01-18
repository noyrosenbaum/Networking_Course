<!-- Explanation of the assignment and how to code works -->
# Packet Sniffing and Spoofing

Submitters: Noy Rosenbaum and Yael Rosen

## Description

**NOTE:** For detailed explanations about any of the code files, check out the comments in each one of the scripts.

### Part 1 - Sniffer

### Part 2 - Spoofer

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
2. Terminal 1:
Go to this directory:
```sh
cd ~/Networking_Course/Ex2/
```
Run this command:
```sh
python3 ./server.py
```
3. Terminal 2:
Go to this directory:
```sh
cd ~/Networking_Course/Ex5/
```
Run this command after you compiled the file:
```sh
sudo ./Sniffer
```
4. Terminal 3:
Go to this directory:
```sh
cd ~/Networking_Course/Ex2/
```
Run this command:
```sh
python3 ./client.py
```

#### Spoofer
   
1. for Spoofer, run:
```sh
sudo ./PartB <IP_DESTINATION>
```

#### Delete exe files

```sh
make clean
```

