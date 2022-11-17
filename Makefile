CC:=gcc
CFLAGS:= -Wall -Werror -pthread -g
TARGETS:=pmm

all: $(TARGETS)

test:
	@tests/bin/test-pmm.csh 1

run:
	@tests/bin/test-pmm.csh 0

clean:
	@rm -f $(TARGETS)
