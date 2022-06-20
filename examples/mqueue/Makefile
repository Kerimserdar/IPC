TARGET_01        = receiver
SOURCE_01        = receiver.c

TARGET_02        = sender
SOURCE_02        = sender.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG

.PHONY: all
all: $(TARGET_01) $(TARGET_02)

$(TARGET_01): $(SOURCE_01)

$(TARGET_02): $(SOURCE_02)

.PHONY: clean
clean:
	rm $(TARGET_01) $(TARGET_02) $(OBJS)
