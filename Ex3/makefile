# all: Receiver Sender

# Receiver: Receiver.c
# 	gcc -o Receiver Receiver.c

# Sender: Sender.c
# 	gcc -o Sender Sender.c

# .PHONY: clean

# clean:
# 	rm -f *.o Receiver Sender

# /////////////////////////

# all: ServerReady ClientReady

# ServerReady: ServerReady.c
# 	gcc -o ServerReady ServerReady.c

# ClientReady: ClientReady.c
# 	gcc -o ClientReady ClientReady.c

# .PHONY: clean

# clean:
# 	rm -f *.o ServerReady ClientReady

# /////////////////////////

all: ReceiverDraft SenderDraft

ReceiverDraft: ReceiverDraft.c
	gcc -o ReceiverDraft ReceiverDraft.c

SenderDraft: SenderDraft.c
	gcc -o SenderDraft SenderDraft.c

.PHONY: clean

clean:
	rm -f *.o ReceiverDraft SenderDraft