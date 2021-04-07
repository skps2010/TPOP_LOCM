CC = g++
CCFLAGS =
OBJS = $(shell ls *.cpp|sed --expression='s/.cpp/.o/g')
EXE = exe_bin
NOSTRIP = 0

ifeq ("$(DEBUG)","1")
	CCFLAGS += -g -DDEBUG
	NOSTRIP = 1
endif

%.o:%.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

all:$(OBJS)
	$(CC) $(OBJS) -o $(EXE)
	if [ "$(NOSTRIP)" = "0" ]; then \
		strip $(EXE); \
	fi
	cat $(EXE)|base64 > $(EXE).b64

clean:
	rm -rf $(OBJS) $(EXE) $(EXE).b64
