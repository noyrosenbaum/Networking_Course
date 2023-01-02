all: watchdog better_ping
# ping: ping.c
# 	gcc ping.c -o parta
watchdog: watchdog.c
	gcc watchdog.c -o watchdog
better_ping: better_ping.c
	gcc better_ping.c -o partb

clean:
	rm -f *.o  watchdog partb
