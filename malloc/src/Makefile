# Makefile pour le projet libmalloc.so

# Variables
CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC -pthread
LDFLAGS = -shared
TARGET = libmalloc.so
SRC = malloc.c
OBJ = $(SRC:.c=.o)

# Cible principale : compiler la bibliothèque partagée
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle pour compiler les objets (avec les flags)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle check : tester la bibliothèque avec des programmes simples et les binaires fournis
# (Adaptez les chemins si nécessaire ; suppose que les binaires sont dans le répertoire courant)
check: $(TARGET)
	@echo "Test simple avec ls :"
	LD_PRELOAD=./$(TARGET) ls
	@echo "Test avec cat Makefile :"
	LD_PRELOAD=./$(TARGET) cat Makefile
	@echo "Lancement des tests avancés (si les binaires existent) :"
	-./speed  # Test de vitesse (ignore les erreurs si le binaire n'existe pas)
	-./corruptionproof  # Test anti-corruption
	-./memoryfootprint  # Test empreinte mémoire

# Règle clean : supprimer les fichiers générés
clean:
	rm -f $(OBJ) $(TARGET)

# Règle debug : compiler avec des symboles de debug (-g) et lancer GDB sur un programme de test
# (Créez un programme test.c simple si nécessaire, ou adaptez)
debug: CFLAGS += -g
debug: $(TARGET)
	@echo "Compilation en mode debug terminée."
	@echo "Pour déboguer : gdb --args env LD_PRELOAD=./$(TARGET) ./votre_programme_test"
	# Exemple : gdb --args env LD_PRELOAD=./$(TARGET) ls

# Règle pour recompiler tout
all: $(TARGET)

# Dépendances (ajoutez d'autres fichiers .h si vous en avez)
$(OBJ): $(SRC)
