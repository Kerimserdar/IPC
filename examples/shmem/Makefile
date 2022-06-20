TARGET_01        = memreader
SOURCE_01        = memreader.c

TARGET_02        = memwriter
SOURCE_02        = memwriter.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG
LDLIBS  = -lpthread -lrt

.PHONY: all
all: $(TARGET_01) $(TARGET_02)

$(TARGET_01): $(SOURCE_01)

$(TARGET_02): $(SOURCE_02)

.PHONY: clean
clean:
	rm $(TARGET_01) $(TARGET_02) $(OBJS)
