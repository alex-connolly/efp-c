#include "ast.h"

void resize_for_add(void* array, size_t size, size_t element_size){
    if (size == 0){
        array = calloc(1, element_size);
    } else if (is_power_of_two(size)){
        array = calloc(size * 2, element_size);
    }
}

void add_element(struct element* scope, struct element* element){
    resize_for_add(scope->elements, scope->num_elements, sizeof(struct element*));
    scope->elements[scope->num_elements++] = element;
    element->parent = scope;
}

void add_field(struct element* scope, struct field* field){
    resize_for_add(scope->fields, scope->num_fields, sizeof(struct field*));
    scope->fields[scope->num_fields++] = field;
}

void add_field_value(struct field* field, struct field_value* value){
    resize_for_add(field->values, field->num_values, sizeof(struct field_value*));
    field->values[field->num_values++] = value;
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

void create_field(struct element* parent, char* key){
    struct field* field = malloc(sizeof(struct field));
    field->key = key;
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
