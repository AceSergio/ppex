CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC -pthread
LDFLAGS = -shared

SRC_DIR = src

SRC = $(SRC_DIR)/malloc.c
OBJ = $(SRC:.c=.o) 

TARGET = libmalloc.so
TEST_SRC = $(SRC_DIR)/test.c
TEST_EXEC = test_basic


$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^



$(TEST_EXEC): $(TEST_SRC)
	$(CC) $(CFLAGS) $< -o $@


check: $(TARGET) $(TEST_EXEC)
	@echo "--- Lancement des tests ---"
	@echo "Test simple avec $(TEST_EXEC) :"
	# Utilise LD_PRELOAD pour forcer l'utilisation de votre libmalloc.so
	LD_PRELOAD=./$(TARGET) ./$(TEST_EXEC)
	@echo "Test simple avec ls :"
	LD_PRELOAD=./$(TARGET) ls
	@echo "Test avec cat Makefile :"
	LD_PRELOAD=./$(TARGET) cat Makefile
	@echo "Lancement des tests avancés (si les binaires existent) :"
	# CORRECTION : Le chemin est maintenant ./tests/speed
	-./tests/speed
	# CORRECTION : Le chemin est maintenant ./tests/corruptionproof
	-./tests/corruptionproof
	# CORRECTION : Le chemin est maintenant ./tests/memoryfootprint
	-./tests/memoryfootprint


clean:
	rm -f $(OBJ) $(TARGET) $(TEST_EXEC)


debug: CFLAGS += -g
debug: $(TARGET) $(TEST_EXEC)
	@echo "Compilation en mode debug terminée."
	@echo "Pour déboguer : gdb --args env LD_PRELOAD=./$(TARGET) ./$(TEST_EXEC)"
	# Exemple : gdb --args env LD_PRELOAD=./$(TARGET) ls


all: $(TARGET) $(TEST_EXEC)


$(OBJ): $(SRC)
