#include "../include/passengers.h"
#include <time.h>
#include <stdlib.h>
#include <string.h> 


typedef struct passenger {
    char *document_number;    // número do documento de identificação do passageiro
    char *first_name;         // primeiro nome do passageiro
    char *last_name;          // último nome do passageiro
    time_t dob;               // data de nascimento do passageiro
    char *nationality;        // nacionalidade do passageiro
    char gender;              // género do passageiro
    char *email;              // email do passageiro
    char *phone;              // número de telefone do passageiro
    char *address;            // morada do passageiro
    char *photo;              // fotografia do passageiro
} Passenger;

// criar
Passenger *passenger_create(const char *document_number, const char *first_name, const char *last_name, time_t dob, const char *nationality, char gender, const char *email, const char *phone, const char *address, const char *photo) {
    if (!document_number || strlen(document_number) == 0) return NULL;

    Passenger *passenger = malloc(sizeof(Passenger));
    if (!passenger) return NULL;

    passenger->document_number = strdup(document_number);
    passenger->first_name = first_name ? strdup(first_name) : NULL;
    passenger->last_name = last_name ? strdup(last_name) : NULL;
    passenger->dob = dob;
    passenger->nationality = nationality ? strdup(nationality) : NULL;
    passenger->gender = gender;
    passenger->email = email ? strdup(email) : NULL;
    passenger->phone = phone ? strdup(phone) : NULL;
    passenger->address = address ? strdup(address) : NULL;
    passenger->photo = photo ? strdup(photo) : NULL;

    return passenger;
}

// destruir
void passenger_destroy(Passenger *passenger) {
    if (!passenger) return;

    free(passenger->document_number);
    free(passenger->first_name);
    free(passenger->last_name);
    free(passenger->nationality);
    free(passenger->email);
    free(passenger->phone);
    free(passenger->address);
    free(passenger->photo);

    free(passenger);
}

// getters
const char *passenger_get_document_number(const Passenger *passenger) { return passenger ? passenger->document_number : NULL; }

const char *passenger_get_first_name(const Passenger *passenger) { return passenger ? passenger->first_name : NULL; }

const char *passenger_get_last_name(const Passenger *passenger) { return passenger ? passenger->last_name : NULL; }

time_t passenger_get_dob(const Passenger *passenger) { return passenger ? passenger->dob : 0; }

const char *passenger_get_nationality(const Passenger *passenger) { return passenger ? passenger->nationality : NULL; }

char passenger_get_gender(const Passenger *passenger) { return passenger ? passenger->gender : '\0'; }

const char *passenger_get_email(const Passenger *passenger) { return passenger ? passenger->email : NULL; }

const char *passenger_get_phone(const Passenger *passenger) { return passenger ? passenger->phone : NULL; }

const char *passenger_get_address(const Passenger *passenger) { return passenger ? passenger->address : NULL; }

const char *passenger_get_photo(const Passenger *passenger) { return passenger ? passenger->photo : NULL; }


