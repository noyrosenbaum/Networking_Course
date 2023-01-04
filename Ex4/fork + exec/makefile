all: ping watchdog better_ping
ping: ping.c
	gcc ping.c -o PartA
watchdog: watchdog.c
	gcc watchdog.c -o watchdog
better_ping: better_ping.c
	gcc better_ping.c -o PartB

clean:
	rm -f *.o  PartA watchdog PartB
