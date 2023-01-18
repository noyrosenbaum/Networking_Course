all: Sniffer Spoofer Snoofer Gateway

Sniffer: Sniffer.c
	gcc Sniffer.c -o Sniffer -lpcap

Spoofer: Spoofer.c
	gcc Spoofer.c -o Spoofer -lpcap

Snoofer: Snoofer.c
	gcc Snoofer.c -o Snoofer -lpcap

Gateway: Gateway.c
	gcc Gateway.c -o Gateway -lpcap

clean:
	rm -f *.o  Sniffer Spoofer Snoofer Gateway