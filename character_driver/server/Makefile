ifeq ($(CC),)
	  CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	 CFLAGS = -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt 
endif

all: aesdsocket
	(mv aesdsocket ..)

aesdsocket: aesdsocket.c
	$(CC) $(CFLAGS) $(INCLUDES) aesdsocket.c -o aesdsocket $(LDFLAGS)
# aesdsocket.o: aesdsocket.c
# 	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c -o aesdsocket.o aesdsocket.c

clean:	
	rm -rf *o aesdsocket