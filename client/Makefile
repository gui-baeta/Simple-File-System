CC   = gcc
LD   = gcc
CFLAGS += -MMD -MP -Wall -std=gnu99 -g -lpthread -lrt -I../
LDFLAGS=-lm

API_TESTS=$(wildcard ../api-tests/*.c)

TESTS=$(API_TESTS:%.c=%)

.PHONY: build info run_default clean_all clean_trash clean_dependencies clean_objects clean_vgcores clean_textfiles

build: $(TESTS)

SRC = $(wildcard *.c ../api-tests/*.c)

$(TESTS): %: %.o tecnicofs-client-api.o
	$(LD) $(LDFLAGS) $(CFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

info:
	@echo --------------------------------------------------------------------------------------
	@echo "type make..."
	@echo "...run_default        -> run client with /tmp/socket.stream"
	@echo "...clean_all          -> clean every .txt, .d, .o, vgcore. file and executable"
	@echo "...clean_dependencies -> clean only every .d file"
	@echo "...clean_vgcores      -> clean only vgcore files"
	@echo --------------------------------------------------------------------------------------

run_default: build
	@echo running by default...
	cd ../api-tests;./client-api-test-create /tmp/socket.stream;cd ../client

clean_all:
	@echo Server Makefile: Cleaning all junk...
	rm -f ../api-tests/*.d *.o *.txt *.d vgcore.* $(TESTS) $(TESTS:%=%.o)

clean_trash: clean_dependencies clean_objects clean_vgcores clean_textfiles

clean_dependencies:
	@echo Cleaning dependencies...
	rm -f ../api-tests/*.d *.d

clean_objects:
	@echo Cleaning source...
	rm -f ../api-tests/*.o *.o

clean_vgcores:
	@echo Cleaning valgrind output...
	rm -f vgcore.* ../api-tests/vgcore.*

clean_textfiles:
	@echo Cleaning text files...
	rm -f *.txt
