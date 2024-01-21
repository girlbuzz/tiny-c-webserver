
OBJS=webserver.o
OUT?=webserver

.PHONY: all clean
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OUT)

$(OUT): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(OUT)
