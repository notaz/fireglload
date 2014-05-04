CFLAGS += -Wall -ggdb
#CFLAGS += -fvisibility=hidden
# magically makes exporting symbols with visibility("default") work..
CFLAGS += -fPIC
CFLAGS += -O2
LDLIBS += -ldl -lpciaccess -lrt
LDLIBS += -lpthread

# xorg header dep
CFLAGS += `pkg-config --cflags pixman-1`


TARGET = fglrx_loader
OBJS = main.o ddmg.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)
clean:
	$(RM) $(TARGET) $(OBJS)
