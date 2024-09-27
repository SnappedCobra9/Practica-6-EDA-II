

#ifndef  DLL_INC
#define  DLL_INC

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct
{
   int   index;
   float weight;
} Data;

typedef struct Node
{
//   int data;
   Data data;

   struct Node* next;
   struct Node* prev;
} Node;

typedef struct
{
   Node* first;
   Node* last;
   Node* cursor;
} List;

List* List_New();
void List_Delete( List** p_list );

void List_Push_back( List* list, int index, float weight );
void List_Pop_back( List* list );

void List_Push_front( List* list, int index, float weight );
void List_Pop_front( List* list );

bool List_Is_empty( List* list );

/**
 * @brief Busca la primer ocurrencia con la llave key y si la encuentra coloca ahí al cursor.
 *
 * @param list Referencia a una lista.
 * @param key La llave de búsqueda.
 *
 * @return true si encontró el elemento; false en caso contrario.
 * @post Si hubo una ocurrencia el cursor es movido a esa posición; en caso contrario el cursor no se mueve.
 */
bool List_Find( List* list, int key );

bool List_Remove( List* list, int key );

void List_Cursor_front( List* list );
void List_Cursor_back( List* list );
bool List_Cursor_next( List* list );
bool List_Cursor_prev( List* list );
bool List_Cursor_end( List* list );

/**
 * @brief Devuelve una copia del elemento apuntado por el cursor.
 *
 * @param list Referencia a una lista.
 *
 * @pre El cursor debe apuntar a una posición válida.
 */
Data List_Cursor_get( List* list );

/**
 * @brief Elimina el elemento apuntado por el cursor.
 *
 * @param list Referencia a una lista.
 *
 * @pre El cursor debe apuntar a una posición válida.
 * @post El cursor queda apuntando al elemento a la derecha del elemento eliminado; si
 * este hubiese sido el último, entonces el cursor apunta al primer elemento de la lista.
 */
void List_Cursor_erase( List* list );

/**
 * @brief Aplica la función fn() a cada elemento de la lista. La función fn() es una función unaria.
 *
 * @param list Una lista.
 * @param fn Función unaria que será aplicada a cada elemento de la lista.
 */
void List_For_each( List* list, void (*fn)( int, float ) );

#endif   /* ----- #ifndef DLL_INC  ----- */