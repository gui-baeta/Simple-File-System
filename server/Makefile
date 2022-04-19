CC   = gcc
LD   = gcc
CFLAGS += -MMD -MP -Wall -std=gnu99 -g -lpthread -lrt -I../
LDFLAGS=-lm

.PHONY: build info run_default clean_all clean_trash clean_dependencies clean_objects clean_vgcores clean_textfiles

build: tecnicofs

SRC = $(wildcard *.c lib/*.c)

tecnicofs: $(SRC:%.c=%.o)
	$(LD) $(LDFLAGS) $(CFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

info:
	@echo --------------------------------------------------------------------------------------------
	@echo "type make..."
	@echo "...run_default        -> run server with /tmp/socket.stream socketname and 20 numbuckets"
	@echo "...clean_all          -> clean every .txt, .d, .o, vgcore. file and executable"
	@echo "...clean_dependencies -> clean only every .d file"
	@echo "...clean_vgcores      -> clean only vgcore files"
	@echo --------------------------------------------------------------------------------------------

run_default: build
	@echo running by default...
	./tecnicofs /tmp/socket.stream output.txt 10

clean_all:
	@echo Server Makefile: Cleaning all junk...
	rm -f lib/*.o *.o *.txt *.d lib/*.d vgcore.* tecnicofs

clean_trash: clean_dependencies clean_objects clean_vgcores clean_textfiles

clean_dependencies:
	@echo Cleaning dependencies...
	rm -f lib/*.d *.d

clean_objects:
	@echo Cleaning source...
	rm -f lib/*.o *.o

clean_vgcores:
	@echo Cleaning valgrind output...
	rm -f vgcore.*

clean_textfiles:
	@echo Cleaning text files...
	rm -f *.txt
