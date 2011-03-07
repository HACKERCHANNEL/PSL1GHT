TARGET		:= sprxlinker
SOURCE		:= .
INCLUDE		:= .

CFLAGS		:= -O2 -Wall --std=gnu99 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
				-I/opt/local/include # MacPorts
LDFLAGS		:= -lelf \
				-L/opt/local/lib # MacPorts

CC			:= gcc

CFILES		:= $(foreach dir,$(SOURCE),$(wildcard $(dir)/*.c))

CFLAGS		+= $(foreach dir,$(INCLUDE),-I$(dir))

all: $(TARGET)

clean:
	@echo clean...
	@rm -rf $(TARGET) *.exe

install: all
	@cp -f $(TARGET) $(PSL1GHT)/host/bin/

$(TARGET): $(CFILES)
	$(CC) $< $(LDFLAGS) $(CFLAGS) -o $@
