#include "ast.h"

void add_element(struct element* scope, struct element* element){
    if (scope->num_elements == 0){
        scope->elements = calloc(1, sizeof(struct element*));
    } else if (is_power_of_two(scope->num_elements)){
        scope->elements = calloc(scope->num_elements * 2, sizeof(struct element*));
    }
    scope->elements[scope->num_elements++] = element;
    element->parent = scope;
}

void add_field(struct element* scope, struct field* field){
    if (scope->num_fields == 0){
        scope->fields = calloc(1, sizeof(struct field*));
    } else if (is_power_of_two(scope->num_fields)){
        scope->fields = calloc(scope->num_fields * 2, sizeof(struct field*));
    }
    scope->fields[scope->num_fields++] = field;
}

struct element* create_element(struct element* parent, char* key){
    struct element* element = malloc(sizeof(struct element)); // will leak
    element->key = key;
    element->fields = NULL;
    element->elements = NULL;
    element->num_fields = 0;
    element->num_elements = 0;
    add_element(parent, element);
    return element;
}

void create_field(struct element* parent, char* key, enum token_type t, union field_value* value){
    struct field* field = malloc(sizeof(struct field));
    field->key = key;
    field->value = value;
    field->type = t;
    add_field(parent, field);
}

struct field* element_field(struct element* e, const char* key){
    for (int i = 0; i < e->num_fields; i++){
        if (strcmp(e->fields[i]->key, key) == 0){
            return e->fields[i];
        }
    }
    return NULL;
}

struct element* element_element(struct element* e, const char* key){
    for (int i = 0; i < e->num_elements; i++){
        if (strcmp(e->elements[i]->key, key) == 0){
            return e->elements[i];
        }
    }
    return NULL;
}
