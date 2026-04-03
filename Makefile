CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lm -ldl

TARGET = eravamoIO

PROJECT_SRCS = main.c lib/eravamoIO_db.c
PROJECT_OBJS = $(PROJECT_SRCS:.c=.o)

SQLITE_SRC = lib/sqlite3.c
SQLITE_OBJ = $(SQLITE_SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(PROJECT_OBJS) $(SQLITE_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(PROJECT_OBJS) $(SQLITE_OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROJECT_OBJS) $(TARGET)

clean_all: clean
	rm -f $(SQLITE_OBJ)
