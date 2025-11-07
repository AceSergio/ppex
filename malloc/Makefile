CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE

CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC
LDFLAGS = -shared -Wl,--no-undefined

SRC = src/malloc.c
OBJ = $(SRC:.c=.o) 

TARGET = libmalloc.so
TEST_SRC = tests/test.c tests/test_fragmentation.c
TEST_EXEC = test_basic test_fragmentation


$(SRC_DIR)/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


library: $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $^



test_%: tests/test_%.c 
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@


check: library $(TEST_EXEC)
	@echo "--- Lancement des tests ---"
	@echo "Test simple avec (malloc/free) :"
	LD_PRELOAD=./$(TARGET) ./test_basic
	@echo "---------------------------"
	@echo "Test de fragmentation :"
	LD_PRELOAD=./$(TARGET) ./test_fragmentation
	@echo "---------------------------"
	@echo "Test simple avec ls :"
	LD_PRELOAD=./$(TARGET) ls > /dev/null && echo "ls fonctionne"
	@echo "---------------------------"
	@echo "Test avec cat Makefile :"
	LD_PRELOAD=./$(TARGET) cat Makefile > /dev/null && echo "cat fonctionne"
	@echo "---------------------------"

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_EXEC)


all: library $(TEST_EXEC)


$(OBJ): $(SRC)
