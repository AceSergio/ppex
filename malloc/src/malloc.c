#include <pthread.h> // Pour les mutex
#include <stddef.h>
#include <string.h> // Pour memset
#include <sys/mman.h> // Pour mmap, munmap
#include <unistd.h> // Pour sysconf, mmap, munmap
// Définition de l'alignement (pour long double, généralement 16 octets)
#define ALIGNMENT 16
// Structure pour un bloc de mémoire
typedef struct block
{
    size_t size; // Taille du bloc utilisateur (alignée)
    int free; // 1 si libre, 0 si occupé
    struct block *next; // Pointeur vers le prochain bloc dans la liste
    void *data; // Pointeur vers le début du bloc utilisateur (optionnel, pour
                // faciliter)
} block_t;
// Variable globale : tête de la liste des blocs
static block_t *head = NULL;
// Mutex pour la sécurité des threads (initialisé plus tard)
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Fonction auxiliaire pour aligner une taille
static size_t align_size(size_t size)
{
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}
// Fonction auxiliaire pour trouver un bloc libre suffisant (premier ajustement)
static block_t *find_free_block(size_t size)
{
    block_t *current = head;
    while (current)
    {
        if (current->free && current->size >= size)
        {
            return current;
        }

        __attribute__((visibility("default"))) void *malloc(size_t size)
        {
            if (size == 0)
                return NULL; // Selon la man page, malloc(0) peut retourner NULL
                             // ou un pointeur valide
            pthread_mutex_lock(
                &mutex); // Protège l'accès (ajouté pour les threads)
            size_t aligned_size = align_size(size);
            block_t *block = find_free_block(aligned_size);
            if (!block)
            {
                block = allocate_new_page(aligned_size);
                if (!block)
                {
                    pthread_mutex_unlock(&mutex);
                    return NULL;
                }
            }
            else
            {
                block->free = 0; // Marquer comme occupé
                // Optionnel : diviser le bloc si trop grand (pour optimiser,
                // mais commencez simple)
            }
            pthread_mutex_unlock(&mutex);
            return block->data;
        }

        __attribute__((visibility("default"))) void free(void *ptr)
        {
            if (!ptr)
                return; // free(NULL) ne fait rien
            pthread_mutex_lock(&mutex);
            block_t *current = head;
            while (current)
            {
                if (current->data == ptr)
                {
                    current->free = 1; // Marquer comme libre
                    // Fusionner avec les blocs adjacents si possible (optionnel
                    // pour l'instant) Si toute la page est libre, munmap
                    // (avancé)
                    break;
                }
                current = current->next;
            }
            pthread_mutex_unlock(&mutex);
        }

        __attribute__((visibility("default"))) void *realloc(void *ptr,
                                                             size_t size)
        {
            if (!ptr)
                return malloc(size); // realloc(NULL, size) == malloc(size)
            if (size == 0)
            {
                free(ptr);
                return NULL; // realloc(ptr, 0) libère et retourne NULL
            }
            pthread_mutex_lock(&mutex);
            // Trouver le bloc actuel
            block_t *current = head;
            while (current && current->data != ptr)
            {
                current = current->next;
            }
            if (!current)
            {
                pthread_mutex_unlock(&mutex);
                return NULL; // Pointeur invalide
            }
            if (size <= current->size)
            {
                // Réduire la taille (optionnel : diviser le bloc)
                current->size = align_size(size);
                pthread_mutex_unlock(&mutex);
                return ptr;
            }
            else
            {
                // Agrandir : allouer un nouveau bloc, copier, libérer l'ancien
                void *new_ptr = malloc(size);
                if (new_ptr)
                {
                    memcpy(new_ptr, ptr, current->size); // Copier les données
                    free(ptr);
                }
                pthread_mutex_unlock(&mutex);
                return new_ptr;
            }
        }

        __attribute__((visibility("default"))) void *calloc(size_t nmemb,
                                                            size_t size)
        {
            if (nmemb == 0 || size == 0)
                return NULL;
            // Vérifier l'overflow : nmemb * size ne doit pas dépasser SIZE_MAX
            if (size > SIZE_MAX / nmemb)
                return NULL;
            size_t total_size = nmemb * size;
            void *ptr = malloc(total_size);
            if (ptr)
            {
                memset(ptr, 0, total_size); // Initialiser à zéro
            }
            return ptr;
        }
