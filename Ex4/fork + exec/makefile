all: ping watchdog better_ping
ping: ping.c
	gcc ping.c -o partA
watchdog: watchdog.c
	gcc watchdog.c -o watchdog
better_ping: better_ping.c
	gcc better_ping.c -o partB

clean:
	rm -f *.o  partA watchdog partB
