#!/bin/bash

# Script : format_all.sh
# Description : Formate tous les fichiers .c et .h du dossier courant (et sous-dossiers)
# Dépendance : clang-format

# Vérifie si clang-format est installé
if ! command -v clang-format &> /dev/null; then
    echo "❌ Erreur : clang-format n'est pas installé."
    echo "Installe-le avec : sudo apt install clang-format"
    exit 1
fi

# Formate tous les fichiers .c et .h trouvés
echo "🔧 Formatage des fichiers .c et .h..."

find . -type f \( -name "*.c" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
    echo " → $file"
    clang-format -i "$file"
done

echo "✅ Formatage terminé."