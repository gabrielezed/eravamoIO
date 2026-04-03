CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

TARGET = eravamoIO

SRCS = main.c lib/eravamoIO_db.c lib/sqlite3.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
