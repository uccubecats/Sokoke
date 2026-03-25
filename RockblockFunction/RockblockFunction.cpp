// code courtesy of https://github.com/HogeyDev with slight modifications

#include "RockblockFunction.h"
#include "../Sensors.h"

IridiumSBD IridiumModem(Serial1);



Table *new_table() {
    Table *t = (Table *)malloc(sizeof(Table));
    if (!t) {
        return NULL;
    }
    
    t->size = 0;
    t->capacity = 64;
    t->entries = (TableEntry *)calloc(t->capacity, sizeof(TableEntry));
    if (!t->entries) {
        free(t);
        return NULL;
    }
    
    return t;
}

void free_table(Table *t) {
    if (t) {
        free(t->entries);
        free(t);
    }
}

Table *checkTable(Table *t) {
    if (!t) {
        return new_table();
    }
    if (table_memsize(t) + sizeof(TableEntry) + 2 > 340) {

        send_table(t);
        free_table(t);
        t = new_table();
    }
    return t;
}

void add_entry(Table *t, TableEntry e) {
    if (t->size >= t->capacity) {
        unsigned short newCapacity = t->capacity * 2;
        TableEntry *newEntries = (TableEntry *)realloc(t->entries, newCapacity * sizeof(TableEntry));
        if (!newEntries) {
            return;
        }
        t->entries = newEntries;
        t->capacity = newCapacity;
    }
    t->entries[t->size++] = e;
}

// TODO: Deprecate
Table *add_sensor_data(Table *t, uint64_t time, SensorDataType type, float data) {
    t = checkTable(t);
    add_entry(t, (TableEntry){ .time = time, .type = type, .data = data, });
    return t;
}

void seal_table(Table *t) {
    TableEntry *newEntries = (TableEntry *)realloc(t->entries, t->size * sizeof(TableEntry));
    if (t->size > 0 && !newEntries) {
        return;
    }
    t->entries = newEntries;
    t->capacity = t->size;
}

SerializedTable serialize_table(Table *t) { 
    void *buffer = malloc(2 * sizeof(unsigned short) + t->size * sizeof(TableEntry));
    if (!buffer) {
        return NULL;
    }

    ((unsigned short *)buffer)[0] = t->size;
    ((unsigned short *)buffer)[1] = t->capacity;
    memcpy((char *)buffer + 2 * sizeof(unsigned short), t->entries, t->size * sizeof(TableEntry));

    return buffer;
}

// TEST: should only be used during testing
Table *deserialize_table(SerializedTable t) {
    Table *table = (Table *)malloc(sizeof(Table));
    
    table->size = ((unsigned short *)t)[0];
    table->capacity = ((unsigned short *)t)[1];
    table->entries = (TableEntry *)calloc(table->capacity, sizeof(TableEntry));

    memcpy(table->entries, (char *)t + 2 * sizeof(unsigned short), table->capacity * sizeof(TableEntry));

    return table;
}

size_t table_memsize(Table *t) {
    return 2 * sizeof(unsigned short) + t->size * sizeof(TableEntry);
}


void send_table(Table *t) {
    if (!t) {
        return;
    }

    seal_table(t);
    SerializedTable st = serialize_table(t);
    if (!st) {
        return;
    }

    size_t size = table_memsize(t);

    Serial.printf("Sending table with %u entries, size %u bytes\n", t->size, size);

    // int status = IridiumModem.sendSBDBinary((uint8_t *)st, size);

    // if (status != 0) {
    //     Serial.print("Failed to send SBD message, error code: ");
    //     Serial.println(status);
    // }

    free(st);
}
