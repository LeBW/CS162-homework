CC=gcc
CFLAGS=-ggdb3 -c -Wall -std=gnu99
LDFLAGS=-pthread
SOURCES=httpserver.c libhttp.c wq.c
OBJECTS=$(SOURCES:.c=.o)
TEST_SOURCES=wq.c test_wq.c
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)
EXECUTABLE=httpserver
TEST_EXEC=test_wq

all: $(SOURCES) $(EXECUTABLE)

test: $(TEST_SOURCES) $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) $(TEST_OBJECTS) -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) test_wq wq.o test_wq.o
