all: Receiver Sender

Receiver: Receiver.c
	gcc -o Receiver Receiver.c

Sender: Sender.c
	gcc -o Sender Sender.c

.PHONY: clean

clean:
	rm -f *.o Receiver Sender