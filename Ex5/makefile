all: Sniffer Spoofer Snoofer

Sniffer: Sniffer.c
	gcc Sniffer.c -o Sniffer -lpcap

Spoofer: Spoofer.c
	gcc Spoofer.c -o Spoofer -lpcap

Snoofer: Snoofer.c
	gcc Snoofer.c -o Snoofer -lpcap

clean:
	rm -f *.o  Sniffer Spoofer Snoofer