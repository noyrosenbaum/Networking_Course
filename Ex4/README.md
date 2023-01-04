<!-- Explanation of the assignment and how to code works -->
# Pinger using ICMP protocol and Watchdog

## Description

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


