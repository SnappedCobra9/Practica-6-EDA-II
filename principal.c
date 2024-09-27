#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "List.h"  // Asegúrate de que esta biblioteca esté implementada correctamente

#ifndef DBG_HELP
#define DBG_HELP 0
#endif  

#if DBG_HELP > 0
#define DBG_PRINT(...) do{ fprintf(stderr, "DBG: " __VA_ARGS__); } while(0)
#else
#define DBG_PRINT(...) ;
#endif  

typedef enum {
    BLACK,
    GRAY,
    WHITE
} eGraphColors;

typedef struct {
    int id;                
    char iata_code[4];    
    char country[65];     
    char city[65];        
    char name[65];        
    int utc_time;         
} Airport;

typedef struct {
    Airport data;         
    List* neighbors;      
    eGraphColors color;   
    int distance;         
    int predecessor;      
} Vertex;

void Vertex_SetColor(Vertex* v, eGraphColors color) {
    assert(v != NULL);
    v->color = color;
}

eGraphColors Vertex_GetColor(Vertex* v) {
    assert(v != NULL);
    return v->color;
}

void Vertex_SetDistance(Vertex* v, int distance) {
    assert(v != NULL);
    v->distance = distance;
}

int Vertex_GetDistance(Vertex* v) {
    assert(v != NULL);
    return v->distance;
}

void Vertex_SetPredecessor(Vertex* v, int predecessor_idx) {
    assert(v != NULL);
    v->predecessor = predecessor_idx;
}

int Vertex_GetPredecessor(Vertex* v) {
    assert(v != NULL);
    return v->predecessor;
}

void Vertex_Start(Vertex* v) {
    assert(v && v->neighbors);
    List_Cursor_front(v->neighbors);
}

void Vertex_Next(Vertex* v) {
    assert(v && v->neighbors);
    List_Cursor_next(v->neighbors);
}

bool Vertex_End(const Vertex* v) {
    assert(v && v->neighbors);
    return List_Cursor_end(v->neighbors);
}

Data Vertex_GetNeighbor(const Vertex* v) {
    assert(v && v->neighbors);
    return List_Cursor_get(v->neighbors);
}

typedef enum { 
    eGraphType_UNDIRECTED, 
    eGraphType_DIRECTED    
} eGraphType; 

typedef struct {
    Vertex* vertices; 
    int size;      
    int len;  
    eGraphType type; 
} Graph;

static int find(Vertex vertices[], int size, int key) {
    for (int i = 0; i < size; ++i) {
        if (vertices[i].data.id == key) return i;
    }
    return -1;
}

static bool find_neighbor(Vertex* v, int index) {
    return v->neighbors ? List_Find(v->neighbors, index) : false;
}

static void insert(Vertex* vertex, int index, float weight) {
    if (!vertex->neighbors) {
        vertex->neighbors = List_New();
    }
    if (vertex->neighbors && !find_neighbor(vertex, index)) {
        List_Push_back(vertex->neighbors, index, weight);
        DBG_PRINT("insert(): Inserting the neighbor with idx: %d\n", index);
    } else {
        DBG_PRINT("insert: duplicated index\n");
    }
}

Graph* Graph_New(int size, eGraphType type) {
    assert(size > 0);
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (g) {
        g->size = size;
        g->len = 0;
        g->type = type;
        g->vertices = (Vertex*)calloc(size, sizeof(Vertex));
        if (!g->vertices) {
            free(g);
            g = NULL;
        }
    }
    return g;
}

void Graph_Delete(Graph** g) {
    assert(*g);
    Graph* graph = *g;
    for (int i = 0; i < graph->size; ++i) {
        Vertex* vertex = &graph->vertices[i];
        if (vertex->neighbors) {
            List_Delete(&(vertex->neighbors));
        }
    }
    free(graph->vertices);
    free(graph);
    *g = NULL;
}

void Graph_Print(Graph* g, int depth) {
    for (int i = 0; i < g->len; ++i) {
        Vertex* vertex = &g->vertices[i];
        printf("[%d] Airport ID: %d => ", i, vertex->data.id);
        printf("<map.key:%d, map.data_idx:%d>\n", vertex->data.id, i);
        printf("  IATA Code: %s\n", vertex->data.iata_code);
        printf("  Country: %s\n", vertex->data.country);
        printf("  City: %s\n", vertex->data.city);
        printf("  Name: %s\n", vertex->data.name);
        printf("  UTC Time: %d\n", vertex->data.utc_time);

        if (vertex->neighbors) {
            printf("  Neighbors: ");
            for (List_Cursor_front(vertex->neighbors); !List_Cursor_end(vertex->neighbors); List_Cursor_next(vertex->neighbors)) {
                Data neighbor = List_Cursor_get(vertex->neighbors);
                printf("-> %d (Weight: %.2f) ", g->vertices[neighbor.index].data.id, neighbor.weight);
            }
        } else {
            printf("  No neighbors.");
        }
        printf("\n");
    }
    printf("\n");
}

void Graph_AddVertex(Graph* g, Airport airport) {
    assert(g->len < g->size);
    Vertex* vertex = &g->vertices[g->len];
    vertex->data = airport;  
    vertex->neighbors = NULL; 
    vertex->color = BLACK;    
    vertex->distance = 0;     
    vertex->predecessor = -1; 
    ++g->len;
}

bool Graph_AddWeightedEdge(Graph* g, int start, int finish, float weight) {
    assert(g->len > 0);
    int start_idx = find(g->vertices, g->size, start);
    int finish_idx = find(g->vertices, g->size, finish);
    DBG_PRINT("AddWeightedEdge(): from: %d (with index: %d), to: %d (with index: %d)\n", start, start_idx, finish, finish_idx);
    if (start_idx == -1 || finish_idx == -1) return false;
    insert(&g->vertices[start_idx], finish_idx, weight);
    if (g->type == eGraphType_UNDIRECTED) insert(&g->vertices[finish_idx], start_idx, weight);
    return true;
}

#define INFINITE 1000.0

float Graph_GetWeight(Graph* g, int start, int finish) {
    assert(g->len > 0);
    int start_idx = find(g->vertices, g->size, start);
    int finish_idx = find(g->vertices, g->size, finish);
    DBG_PRINT("GetWeight(): from: %d (with index: %d), to: %d (with index: %d)\n", start, start_idx, finish, finish_idx);
    if (start_idx == -1 || finish_idx == -1) return INFINITE;
    Vertex* vertex = &g->vertices[start_idx];
    for (Vertex_Start(vertex); !Vertex_End(vertex); Vertex_Next(vertex)) {
        Data neighbor = Vertex_GetNeighbor(vertex);
        if (neighbor.index == finish_idx) {
            return neighbor.weight;
        }
    }
    return INFINITE;
}

int Graph_GetIndexByValue(const Graph* g, int vertex_val) {
    assert(g != NULL);
    for (int index = 0; index < g->len; ++index) {
        Vertex* vertex = &g->vertices[index];
        if (vertex->data.id == vertex_val) {
            return index; 
        }
    }
    return -1; 
}

bool Graph_IsNeighborOf(Graph* g, int dest, int src) {
    assert(g != NULL); 
    int src_idx = Graph_GetIndexByValue(g, src);
    if (src_idx == -1) return false; 

    Vertex* src_vertex = &g->vertices[src_idx];
    if (src_vertex->neighbors) {
        for (List_Cursor_front(src_vertex->neighbors); !List_Cursor_end(src_vertex->neighbors); List_Cursor_next(src_vertex->neighbors)) {
            Data neighbor = List_Cursor_get(src_vertex->neighbors);
            if (neighbor.index == Graph_GetIndexByValue(g, dest)) {
                return true; 
            }
        }
    }
    return false; 
}

int main() {
    Graph* grafo = Graph_New(5, eGraphType_DIRECTED);

    Airport aeropuerto1 = {1, "LAX", "USA", "Los Angeles", "Los Angeles International Airport", -8};
    Airport aeropuerto2 = {2, "JFK", "USA", "New York", "John F. Kennedy International Airport", -5};
    Airport aeropuerto3 = {3, "ORD", "USA", "Chicago", "O'Hare International Airport", -6};
    Airport aeropuerto4 = {4, "DFW", "USA", "Dallas", "Dallas/Fort Worth International Airport", -6};
    Airport aeropuerto5 = {5, "ATL", "USA", "Atlanta", "Hartsfield-Jackson Atlanta International Airport", -5};

    Graph_AddVertex(grafo, aeropuerto1);
    Graph_AddVertex(grafo, aeropuerto2);
    Graph_AddVertex(grafo, aeropuerto3);
    Graph_AddVertex(grafo, aeropuerto4);
    Graph_AddVertex(grafo, aeropuerto5);

    Graph_AddWeightedEdge(grafo, 1, 2, 1.5);
    Graph_AddWeightedEdge(grafo, 1, 3, 2.0);
    Graph_AddWeightedEdge(grafo, 2, 4, 1.0);
    Graph_AddWeightedEdge(grafo, 3, 5, 3.0);
    Graph_AddWeightedEdge(grafo, 4, 1, 2.5);

    int selected_airport_id;
    printf("Seleccione el ID del aeropuerto (1-5): ");
    scanf("%d", &selected_airport_id);

    int airport_index = Graph_GetIndexByValue(grafo, selected_airport_id);
    if (airport_index != -1) {
        Vertex* selected_vertex = &grafo->vertices[airport_index];
        printf("Detalles del aeropuerto seleccionado:\n");
        printf("ID: %d\n", selected_vertex->data.id);
        printf("IATA: %s\n", selected_vertex->data.iata_code);
        printf("País: %s\n", selected_vertex->data.country);
        printf("Ciudad: %s\n", selected_vertex->data.city);
        printf("Nombre: %s\n", selected_vertex->data.name);
        printf("Hora UTC: %d\n", selected_vertex->data.utc_time);

        if (selected_vertex->neighbors) {
            printf("Conexiones con otros aeropuertos:\n");
            for (List_Cursor_front(selected_vertex->neighbors); !List_Cursor_end(selected_vertex->neighbors); List_Cursor_next(selected_vertex->neighbors)) {
                Data neighbor = List_Cursor_get(selected_vertex->neighbors);
                printf("-> Aeropuerto ID: %d (Peso: %.2f)\n", grafo->vertices[neighbor.index].data.id, neighbor.weight);
            }
        } else {
            printf("No hay conexiones con otros aeropuertos.\n");
        }
    } else {
        printf("ID de aeropuerto no válido.\n");
    }

    Graph_Print(grafo, 0);
    Graph_Delete(&grafo); 

    return 0;
}

