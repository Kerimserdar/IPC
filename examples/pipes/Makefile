TARGET_01        = fifoReader
SOURCE_01        = fifoReader.c

TARGET_02        = fifoWriter
SOURCE_02        = fifoWriter.c

TARGET_03        = pipeUN
SOURCE_03        = pipeUN.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG

.PHONY: all
all: $(TARGET_01) $(TARGET_02) $(TARGET_03)

$(TARGET_01): $(SOURCE_01)

$(TARGET_02): $(SOURCE_02)

$(TARGET_03): $(SOURCE_03)

.PHONY: clean
clean:
	rm $(TARGET_01) $(TARGET_02)  $(TARGET_03) $(OBJS)
