TARGET := hideme

srcs := $(wildcard *.c)
objs := $(srcs:%=%.o)

all: $(TARGET)

%.c.o: %.c
	$(CC) $^ -c -o $@

$(TARGET): $(objs)
	$(CC) $^ -o $@

clean:
	rm -f $(objs)
	rm -f $(TARGET)
