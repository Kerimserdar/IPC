TARGET_01        = producer
SOURCE_01        = producer.c

TARGET_02        = consumer
SOURCE_02        = consumer.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG

.PHONY: all
all: $(TARGET_01) $(TARGET_02)

$(TARGET_01): $(SOURCE_01)

$(TARGET_02): $(SOURCE_02)

.PHONY: clean
clean:
	rm $(TARGET_01) $(TARGET_02) $(OBJS)
