ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

all: writer
	+$(MAKE) -C server 

writer: writer.c
	$(CC) $(CFLAGS) $(INCLUDES) writer.c -o writer $(LDFLAGS)

# writer.o: writer.c
# 	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c -o writer.o writer.c

clean:	
	rm -rf *o writer
	rm  -rf ./server/aesdsocket.o ./server/aesdsocket 