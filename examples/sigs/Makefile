TARGET_01        = shutdown
SOURCE_01        = shutdown.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG

.PHONY: all
all: $(TARGET_01)

$(TARGET_01): $(SOURCE_01)


.PHONY: clean
clean:
	rm $(TARGET_01) $(OBJS)
