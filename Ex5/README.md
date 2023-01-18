<!-- Explanation of the assignment and how to code works -->
# Packet Sniffing and Spoofing

Submitters: Noy Rosenbaum and Yael Rosen

## Description

**NOTE:** For detailed explanations about any of the code files, check out the comments in each one of the scripts.

### Part 1 - Sniffer

Packet sniffing involves capturing live data as it travels across a network. To capture all packets on the network, it is done by using a raw socket, because with a normal socket like a stream or datagram, the application will not receive the packet headers, information like MAC address, source IP, etc.
Generally, when sniffing packets, we are only interested in specific types of packets, so we need to do some filtering on the packets, the filter must be as close to the NIC as possible (filter as early as
possible). To filter and only capture specific types of packets, a BSD packet filtering system is used, which sniffers can specify to the kernel, the packets they are interested in.
Without the pcap library, creating a sniffer that works across different operating systems and allows for easy filtering can be difficult.
In our custom Sniffer, we captured TCP packets and exported some values to a log file.

*Question:* Why do you need the root privilege to run a sniffer program? Where does the program fail if it is executed without the root privilege?
*Answer:* Sniffing data packets is a privileged task as it raises concerns about privacy and security. If non-privileged users are able to access this function, they may potentially steal personal information and account passwords, violating others' privacy.
Run with normal permissions, the returned socket descriptor is -1, indicating that the creation of raw socket failed.

### Part 2 - Spoofer

Spoofer fakes an ICMP ping echo request and sends it to destination.
It creates false source ICMP packets from the Spoofer.

*Question:* Can you set the IP packet length field to an arbitrary value, regardless of how big the actual packet is? \
*Answer:*   The IP packet length field must match the actual size of the packet, which is specified as a 16 bits value that represents the total length of the packet, including header and payload. If this field is set to a value that does not match the actual size of the packet, it will not be able to be forwarded. \
*Question:* Using the raw socket programming, do you have to calculate the checksum for the IP header? \
*Answer:*   You do not need to calculate the checksum for the IP header it is filled by the operating system.

### Part 3 - Snoofer - combines both Sniffer and Spoofer

we need to capture packets first, and then spoof a response based on the captured packets.
From the user container, you can generate an IPX. This will generate an ICMP Echo Request packet. If X is activated, the ping program will receive an echo response and print out the response. Your sniffing and spoofing program runs on a VM, which monitors the LAN through packet sniffing. Whenever it sees an ICMP echo request, your program should immediately send an echo reply using packet spoofing techniques, no matter what the destination IP address is. So, regardless of whether machine X is active or not, the ping program will always receive a reply indicating that X is active.


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

For Sniffer, run on seperate terminals:
1. Terminal 1: \
Go to this directory:
```sh
cd ~/Networking_Course/Ex2/
```
Run this command:
```sh
python3 ./server.py
```
2. Terminal 2: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/
```
Run this command after you compiled the file:
```sh
sudo ./Sniffer
```
3. Terminal 3: \
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
#### Snoofer

##### Setup Dockers

Here are the setups to each one of the terminals: 
1. Terminal 1: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/
```
Run these commands by their order:
```sh
sudo apt install docker-compose
```
Go to this directory: 
```sh
cd ./Labsetup
```
Run:
```sh
sudo docker-compose build
```
```sh
sudo docker-compose up
```
2. Terminal 2: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/Labsetup
```
Check out containers' status:
```sh
sudo docker ps
```
Go with root privilage to HostA's shell:
```sh
sudo docker exec -it <CONTAINER ID> /bin/bash
```
Run the commands:
```sh
apt-get update
```
```sh
apt-get upgrade
```
```sh
apt-get install build-essential gcc gdb perl make dkms libpcap-dev
```
3. Terminal 3: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/Labsetup
```
Check out containers' status:
```sh
sudo docker ps
```
Go with root privilage to HostB's shell:
```sh
sudo docker exec -it <CONTAINER ID> /bin/bash
```
Run the commands:
```sh
apt-get update
```
```sh
apt-get upgrade
```
```sh
apt-get install build-essential gcc gdb perl make dkms libpcap-dev
```
3. Terminal 3: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/Labsetup
```
Check out containers' status:
```sh
sudo docker ps
```
Go with root privilage to Attacker's shell:
```sh
sudo docker exec -it <CONTAINER ID> /bin/bash
```
Run the commands:
```sh
apt-get update
```
```sh
apt-get upgrade
```
```sh
apt-get install build-essential gcc gdb perl make dkms libpcap-dev
```
**Make sure to copy the Snoofer exe file to Attacker's volume file**
Go to this directory: 
```sh
cd ./volumes
```
Give Snoofer permissions:
```sh
chmod +x ./Snoofer
```

##### Run Snoofer

1. Run Snoofer:
```sh
./Snoofer <Interface>
```
for example:
```sh
./Snoofer br-f0642a86f103
```

2. Run a Ping command from Host A:
```sh
ping <IP>
```

#### Gateway

1. Terminal 1: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/
```
Run this command after you compiled the file:
```sh
./Gateway <port>
```
For example:
```sh
./Gateway 8000
```
2. Terminal 2: \
Go to this directory: 
```sh
cd ~/Networking_Course/Ex5/
```
Run this command:
```sh
echo -n this-is-a-message | nc -4u -w1 127.0.0.1 <port>
```
For example:
```sh
echo -n this-is-a-message | nc -4u -w1 127.0.0.1 8000
```

#### Delete exe files

```sh
make clean
```

