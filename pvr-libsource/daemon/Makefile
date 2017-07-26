CROSS_COMPLILE		:= arm-none-linux-gnueabi-
CC   							:= $(CROSS_COMPLILE)gcc
LD								:= $(CROSS_COMPLILE)gcc
STRIP							:= $(CROSS_COMPLILE)strip
RANLIB 						:= $(CROSS_COMPLILE)ranlib
AR 								:= $(CROSS_COMPLILE)ar
INDENT						:= $(shell which indent) 
HOSTCC						:= gcc

CFLAGS						:= -c -O2 -Wall $(INCLUDE_PATH) -D_GNU_SOURCE -g

LDFLAG += -lpthread

.PHONY: all install clean
all: daemon

daemon: Daemon.o
	$(LD) $(LDFLAG) $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f daemon Daemon.o