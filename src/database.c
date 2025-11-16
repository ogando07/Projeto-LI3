#include "../include/database.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HASHTABLE_SIZE 1009 
#define FLIGHTS_HASHTABLE_SIZE 10007

// Hash table node for chaining
typedef struct hash_node {
    void* data;                    // Pointer to the actual entity (Airport*, Flight*, etc.)
    char* key;                     // String key (ID, code, document number, etc.)
    struct hash_node* next;        // Next node in the chain
} HashNode;

// Hash table structure
typedef struct hash_table {
    HashNode** buckets;            // Array of pointers to first node in each bucket
    size_t size;                   // Number of buckets
    size_t count;                  // Number of elements stored
} HashTable;

// Database structure using hash tables
typedef struct database {
    HashTable* airports;           // Hash table for airports (key: airport code)
    HashTable* aircrafts;          // Hash table for aircrafts (key: aircraft id)
    HashTable* flights;            // Hash table for flights (key: flight id)
    HashTable* passengers;         // Hash table for passengers (key: document number)
    HashTable* reservations;       // Hash table for reservations (key: reservation id)
} Database;

// Hash function (djb2 algorithm)
static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381UL;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned char)c; // hash * 33 + c
    }
    return hash;
}

// Create a new hash table with given number of buckets
static HashTable* hashtable_create(size_t initial_size) {
    if (initial_size == 0) initial_size = DEFAULT_HASHTABLE_SIZE;

    HashTable* ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    ht->size = initial_size;
    ht->count = 0;
    ht->buckets = calloc(initial_size, sizeof(HashNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    return ht;
}

// Insert into hash table (returns 0 on success, -1 on error/duplicate)
static int hashtable_insert(HashTable* ht, const char* key, void* data) {
    if (!ht || !key || !data) return -1;
    
    unsigned long raw = hash_string(key);
    unsigned int index = (unsigned int)(raw % ht->size);
    HashNode* current = ht->buckets[index];
    
    // Check for duplicates
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return -1; // Duplicate key
        }
        current = current->next;
    }
    
    // Create new node
    HashNode* new_node = malloc(sizeof(HashNode));
    if (!new_node) return -1;
    
    new_node->key = strdup(key);
    new_node->data = data;
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->count++;
    
    return 0;
}

// Search in hash table (returns data or NULL if not found)
static void* hashtable_search(HashTable* ht, const char* key) {
    if (!ht || !key) return NULL;
    
    unsigned long raw = hash_string(key);
    unsigned int index = (unsigned int)(raw % ht->size);
    HashNode* current = ht->buckets[index];
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->data;
        }
        current = current->next;
    }
    
    return NULL;
}

// Get all items from hash table (for iteration)
static void** hashtable_get_all(HashTable* ht, size_t* count) {
    if (!ht || !count) return NULL;
    
    *count = ht->count;
    if (ht->count == 0) return NULL;
    
    void** items = malloc(ht->count * sizeof(void*));
    if (!items) return NULL;
    
    size_t idx = 0;
    for (size_t i = 0; i < ht->size; i++) {
        HashNode* current = ht->buckets[i];
        while (current) {
            items[idx++] = current->data;
            current = current->next;
        }
    }
    
    return items;
}

// Destroy hash table and all its nodes (but not the data)
static void hashtable_destroy(HashTable* ht) {
    if (!ht) return;
    
    for (size_t i = 0; i < ht->size; i++) {
        HashNode* current = ht->buckets[i];
        while (current) {
            HashNode* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    
    free(ht->buckets);
    free(ht);
}

Database* database_create(void) {
    Database* db = malloc(sizeof(Database));
    if (!db) return NULL;
    
    // Initialize hash tables
    db->airports = hashtable_create(DEFAULT_HASHTABLE_SIZE);
    db->aircrafts = hashtable_create(DEFAULT_HASHTABLE_SIZE);
    db->flights = hashtable_create(FLIGHTS_HASHTABLE_SIZE);
    db->passengers = hashtable_create(DEFAULT_HASHTABLE_SIZE);
    db->reservations = hashtable_create(DEFAULT_HASHTABLE_SIZE);
    
    // Check if all hash tables were created successfully
    if (!db->airports || !db->aircrafts || !db->flights || 
        !db->passengers || !db->reservations) {
        // Cleanup on failure
        if (db->airports) hashtable_destroy(db->airports);
        if (db->aircrafts) hashtable_destroy(db->aircrafts);
        if (db->flights) hashtable_destroy(db->flights);
        if (db->passengers) hashtable_destroy(db->passengers);
        if (db->reservations) hashtable_destroy(db->reservations);
        free(db);
        return NULL;
    }
    
    return db;
}

void database_destroy(Database* db) {
    if (!db) return;
    
    // Destroy all airports
    if (db->airports) {
        for (size_t i = 0; i < db->airports->size; i++) {
            HashNode* current = db->airports->buckets[i];
            while (current) {
                airport_destroy((Airport*)current->data);
                current = current->next;
            }
        }
        hashtable_destroy(db->airports);
    }
    
    // Destroy all aircrafts
    if (db->aircrafts) {
        for (size_t i = 0; i < db->aircrafts->size; i++) {
            HashNode* current = db->aircrafts->buckets[i];
            while (current) {
                aircraft_destroy((Aircraft*)current->data);
                current = current->next;
            }
        }
        hashtable_destroy(db->aircrafts);
    }
    
    // Destroy all flights
    if (db->flights) {
        for (size_t i = 0; i < db->flights->size; i++) {
            HashNode* current = db->flights->buckets[i];
            while (current) {
                flight_destroy((Flight*)current->data);
                current = current->next;
            }
        }
        hashtable_destroy(db->flights);
    }
    
    // Destroy all passengers
    if (db->passengers) {
        for (size_t i = 0; i < db->passengers->size; i++) {
            HashNode* current = db->passengers->buckets[i];
            while (current) {
                passenger_destroy((Passenger*)current->data);
                current = current->next;
            }
        }
        hashtable_destroy(db->passengers);
    }
    
    // Destroy all reservations
    if (db->reservations) {
        for (size_t i = 0; i < db->reservations->size; i++) {
            HashNode* current = db->reservations->buckets[i];
            while (current) {
                reservation_destroy((Reservation*)current->data);
                current = current->next;
            }
        }
        hashtable_destroy(db->reservations);
    }
    
    free(db);
}

// Add airport
int database_add_airport(Database* db, Airport* airport) {
    if (!db || !airport) return -1;
    
    const char* code = airport_get_code(airport);
    return hashtable_insert(db->airports, code, airport);
}

// Add aircraft
int database_add_aircraft(Database* db, Aircraft* aircraft) {
    if (!db || !aircraft) return -1;
    
    const char* id = aircraft_get_id(aircraft);
    return hashtable_insert(db->aircrafts, id, aircraft);
}

// Add flight
int database_add_flight(Database* db, Flight* flight) {
    if (!db || !flight) return -1;
    
    const char* id = flight_get_id(flight);
    return hashtable_insert(db->flights, id, flight);
}

// Add passenger
int database_add_passenger(Database* db, Passenger* passenger) {
    if (!db || !passenger) return -1;
    
    const char* doc = passenger_get_document_number(passenger);
    return hashtable_insert(db->passengers, doc, passenger);
}

// Add reservation
int database_add_reservation(Database* db, Reservation* reservation) {
    if (!db || !reservation) return -1;
    
    const char* id = reservation_get_id(reservation);
    return hashtable_insert(db->reservations, id, reservation);
}

// Lookup airport
Airport* database_get_airport(Database* db, const char* code) {
    if (!db || !code) return NULL;
    return (Airport*)hashtable_search(db->airports, code);
}

// Lookup aircraft
Aircraft* database_get_aircraft(Database* db, const char* id) {
    if (!db || !id) return NULL;
    return (Aircraft*)hashtable_search(db->aircrafts, id);
}

// Lookup flight
Flight* database_get_flight(Database* db, const char* id) {
    if (!db || !id) return NULL;
    return (Flight*)hashtable_search(db->flights, id);
}

// Lookup passenger
Passenger* database_get_passenger(Database* db, const char* doc_number) {
    if (!db || !doc_number) return NULL;
    return (Passenger*)hashtable_search(db->passengers, doc_number);
}

// Lookup reservation
Reservation* database_get_reservation(Database* db, const char* id) {
    if (!db || !id) return NULL;
    return (Reservation*)hashtable_search(db->reservations, id);
}

// Get all airports
Airport** database_get_all_airports(Database* db, size_t* count) {
    if (!db || !count) return NULL;
    return (Airport**)hashtable_get_all(db->airports, count);
}

// Get all aircrafts
Aircraft** database_get_all_aircrafts(Database* db, size_t* count) {
    if (!db || !count) return NULL;
    return (Aircraft**)hashtable_get_all(db->aircrafts, count);
}

// Get all flights
Flight** database_get_all_flights(Database* db, size_t* count) {
    if (!db || !count) return NULL;
    return (Flight**)hashtable_get_all(db->flights, count);
}

// Get all passengers
Passenger** database_get_all_passengers(Database* db, size_t* count) {
    if (!db || !count) return NULL;
    return (Passenger**)hashtable_get_all(db->passengers, count);
}

// Get all reservations
Reservation** database_get_all_reservations(Database* db, size_t* count) {
    if (!db || !count) return NULL;
    return (Reservation**)hashtable_get_all(db->reservations, count);
}
