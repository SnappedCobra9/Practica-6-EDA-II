
#include "List.h"

static Node* new_node( int index, float weight )
{
   Node* n = (Node*) malloc( sizeof( Node ) );
   if( n != NULL )
   {
      n->data.index = index;
      n->data.weight = weight;

      n->next = NULL;
      n->prev = NULL;
   }

   return n;
}


List* List_New()
{
   List* lst = (List*) malloc( sizeof(List) );
   if( lst )
   {
      lst->first = lst->last = lst->cursor = NULL;
   }

   return lst;
}

void List_Delete( List** p_list )
{
   assert( *p_list );

   while( (*p_list)->first )
   {
      List_Pop_back( *p_list );
   }

   free( *p_list );
   *p_list = NULL;
}

void List_Push_back( List* list, int data, float weight )
{
   Node* n = new_node( data, weight );
   assert( n );

   if( list->first != NULL )
   {
      list->last->next = n;
      n->prev = list->last;
      list->last = n;
   }
   else
   {
      list->first = list->last = list->cursor = n;
   }
}

void List_Pop_back( List* list )
{
   assert( list->first );
   // ERR: no podemos borrar nada de una lista vacía

   if( list->last != list->first )
   {
      Node* x = list->last->prev;
      free( list->last );
      x->next = NULL;
      list->last = x;
   }
   else
   {
      free( list->last );
      list->first = list->last = list->cursor = NULL;
   }

}



bool List_Is_empty( List* list )
{
   return !list->first;
}

/**
 * @brief Busca la primer ocurrencia con la llave key y si la encuentra coloca ahí al cursor.
 *
 * @param list Referencia a una lista.
 * @param key La llave de búsqueda.
 *
 * @return true si encontró el elemento; false en caso contrario.
 * @post Si hubo una ocurrencia el cursor es movido a esa posición; en caso contrario el cursor no se mueve.
 */
bool List_Find( List* list, int key )
{
   Node* start = list->first;
   while( start )
   {
      if( start->data.index == key )
      {
         list->cursor = start;
         return true;
      }

      start = start->next;
   }
   return false;
}

bool List_Remove( List* list, int key )
{
   // terminar
   return false;
}

void List_Cursor_front( List* list )
{
   list->cursor = list->first;
}

void List_Cursor_back( List* list )
{
   list->cursor = list->last;
}

bool List_Cursor_next( List* list )
{
   list->cursor = list->cursor->next;
   return list->cursor;
}

bool List_Cursor_prev( List* list )
{
   // terminar
   return false;
}

bool List_Cursor_end( List* list )
{
   return list->cursor == NULL;
}

Data  List_Cursor_get( List* list )
{
   assert( list->cursor );

   return list->cursor->data;
}

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
void List_For_each( List* list, void (*fn)( int, float ) )
{
   Node* it = list->first;
   // |it| es la abreviación de "iterator", o  en español, "iterador"

   while( it != NULL )
   {
      fn( it->data.index, it->data.weight );

      it = it->next;
   }
}