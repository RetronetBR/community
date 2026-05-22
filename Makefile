CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
SRC_DIR := src
SOURCES := $(SRC_DIR)/config.c $(SRC_DIR)/message.c $(SRC_DIR)/routing.c $(SRC_DIR)/spool.c $(SRC_DIR)/net.c
BINARIES := rnd rnsync rnpeer rnmsg rndctl

all: $(BINARIES)

%: $(SRC_DIR)/%.c $(SOURCES)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $^ -o $@

clean:
	rm -f $(BINARIES)

.PHONY: all clean
