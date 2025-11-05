/*ram.c*/

/**
  * @brief Random access memory (RAM) for nuPython
  *
  * To execute Python code, the nuPython interpreter needs to store the
  * values of Python variables. The RAM functions serve this purpose.
  *
  * @note Corey Zhang
  *
  * @note template by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // true, false
#include <string.h>
#include <assert.h>

#include "ram.h"

/**
 * @brief binary_search: searches the map for a variable name
 * 
 * Performs binary search on the alphabetically-sorted map array.
 * Returns the index in the map if found, -1 otherwise.
 * 
 * @param memory Pointer to RAM struct
 * @param varname Variable name to search for
 * @return Index in map if found, -1 if not found
 */
static int binary_search(struct RAM* memory, char* varname)
{
  int left = 0;
  int right = memory->size - 1;
  
  while (left <= right) {
    int mid = (left + right) / 2;
    int cmp = strcmp(memory->map[mid].varname, varname);
    
    if (cmp == 0) {
      return mid;
    }
    else if (cmp < 0) {
      left = mid + 1;
    }
    else {
      right = mid - 1; 
    }
  }
  
  return -1;
}

/**
 * @brief grow_if_needed: doubles the capacity if memory is full
 * 
 * Checks if size has reached capacity, and if so, doubles the
 * capacity of both the cells and map arrays.
 * 
 * @param memory Pointer to RAM struct
 */
static void grow_if_needed(struct RAM* memory)
{
  if (memory->size >= memory->capacity) {
    int new_capacity = memory->capacity * 2;
    
    memory->cells = (struct RAM_VALUE*) realloc(memory->cells, 
                                                 new_capacity * sizeof(struct RAM_VALUE));

    for (int i = memory->capacity; i < new_capacity; i++) {
      memory->cells[i].value_type = RAM_TYPE_NONE;
    }

    memory->map = (struct RAM_MAP*) realloc(memory->map, 
                                             new_capacity * sizeof(struct RAM_MAP));
    
    memory->capacity = new_capacity;
  }
}

/**
 * @brief insert_into_map: inserts a new variable into the map
 * 
 * Inserts the variable in alphabetical order by shifting existing
 * elements to the right as needed.
 * 
 * @param memory Pointer to RAM struct
 * @param varname Variable name to insert (will be duplicated)
 * @param cell Cell number where the variable's value is stored
 */
static void insert_into_map(struct RAM* memory, char* varname, int cell)
{
  // Find insertion position (where this varname should go alphabetically)
  int insert_pos = 0;
  while (insert_pos < memory->size && 
         strcmp(memory->map[insert_pos].varname, varname) < 0) {
    insert_pos++;
  }
  
  // Shift elements to the right to make room
  for (int i = memory->size; i > insert_pos; i--) {
    memory->map[i] = memory->map[i-1];
  }
  
  // Insert the new entry
  memory->map[insert_pos].varname = strdup(varname);
  memory->map[insert_pos].cell = cell;
}

/**
 * @brief copy_value: creates a deep copy of a RAM_VALUE
 * 
 * Allocates memory for a new RAM_VALUE and copies the contents.
 * For strings, creates a duplicate of the string.
 * 
 * @param original Pointer to the value to copy
 * @return Pointer to newly allocated copy
 */
static struct RAM_VALUE* copy_value(struct RAM_VALUE* original)
{
  struct RAM_VALUE* copy = (struct RAM_VALUE*) malloc(sizeof(struct RAM_VALUE));
  
  copy->value_type = original->value_type;
  
  if (original->value_type == RAM_TYPE_STR) {
    copy->types.s = strdup(original->types.s);
  }
  else {
    copy->types = original->types;
  }
  
  return copy;
}


//
// Public functions:
//

/**
  * @brief ram_init: initialize memory unit
  *
  * Returns a pointer to a dynamically-allocated memory
  * for storing nuPython variables and their values. All
  * memory cells are initialized to the value None. You
  * take ownership of the returned memory and must call
  * ram_destroy() when you are done.
  *
  * @return pointer to struct denoting memory unit
  */
struct RAM* ram_init(void)
{
  struct RAM* memory = (struct RAM*) malloc(sizeof(struct RAM));

  memory->capacity = 4;
  memory->size = 0;

  memory->cells = (struct RAM_VALUE*) malloc(memory->capacity * sizeof(struct RAM_VALUE));

  for (int i = 0; i < memory->capacity; i++) {
    memory->cells[i].value_type = RAM_TYPE_NONE;
  }

  memory->map = (struct RAM_MAP*) malloc(memory->capacity * sizeof(struct RAM_MAP));

  return memory;
}


/**
  * @brief ram_destroy: frees memory associated with memory unit
  * 
  * Frees the dynamically-allocated memory associated with
  * the given memory. After the call returns, you cannot
  * use the memory.
  *
  * @return void
  */
void ram_destroy(struct RAM* memory)
{
  if (memory == NULL) {
    return;
  }

  for (int i = 0; i < memory->capacity; i++) {
    if (memory->cells[i].value_type == RAM_TYPE_STR && 
        memory->cells[i].types.s != NULL) {
      free(memory->cells[i].types.s);
    }
  }

  for (int i = 0; i < memory->size; i++) {
    if (memory->map[i].varname != NULL) {
      free(memory->map[i].varname);
    }
  }

  free(memory->cells);
  free(memory->map);
  free(memory);


}


/**
  * @brief ram_size: # of vars in memory
  *
  * Returns the # of variables currently stored in memory.
  *
  * @return # of vars in memory
  */
int ram_size(struct RAM* memory)
{
  return memory->size;
}


/**
  * @brief ram_capacity: # of cells available in memory
  *
  * Returns the # of cells currently available in memory.
  *
  * @return # of cells available in memory
  */
int ram_capacity(struct RAM* memory)
{
  return memory->capacity;
}


/**
  * @brief ram_get_addr: address of memory cell occupied by variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns the address of this variable --- an integer
  * in the range 0..N-1 where N is the number of vars currently 
  * stored in memory. Returns -1 if no such variable exists 
  * in memory. 
  *
  * NOTE: a variable has to be written to memory before you can
  * get its address. Once a variable is written to memory, its
  * address never changes. 
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return address of variable or -1 if doesn't exist
  */
int ram_get_addr(struct RAM* memory, char* varname)
{
  int map_index = binary_search(memory, varname);

  if (map_index == -1) {
    return -1;
  }

  return memory->map[map_index].cell;
}


/**
  * @brief ram_read_cell_by_addr: returns value in memory cell at this address
  *
  * Given a memory address (an integer in the range 0..N-1), 
  * returns a COPY of the value contained in that memory cell.
  * Returns NULL if the address is not valid.
  * 
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param address memory cell address
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_addr(struct RAM* memory, int address)
{
  if (address < 0 || address >= memory->size) {
    return NULL;
  }

  return copy_value(&memory->cells[address]);
}


/**
  * @brief ram_read_cell_by_name: returns value in memory cell for this variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns a COPY of the value contained in memory.
  * Returns NULL if no such name exists in memory.
  *
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_name(struct RAM* memory, char* varname)
{
  int map_index = binary_search(memory, varname);

  if (map_index == -1) {
    return NULL;
  }

  int cell = memory->map[map_index].cell;

  return copy_value(&memory->cells[cell]);
}


/**
  * @brief ram_free_value: free value returned by read_cell() functions
  *
  * Frees the memory value returned by ram_read_cell_by_name and
  * ram_read_cell_by_addr.
  *
  * @param value Pointer to struct containing value
  * @return void
  */
void ram_free_value(struct RAM_VALUE* value)
{
  if (value == NULL) {
    return;
  }

  if (value->value_type == RAM_TYPE_STR && value->types.s != NULL) {
    free(value->types.s);
  }

  free(value);
}


/**
  * @brief ram_write_cell_by_addr: writes a value to memory cell at this address
  *
  * Writes the given value to the memory cell at the given 
  * address. If a value already exists at this address, that
  * value is overwritten by this new value. Returns true if 
  * the value was successfully written, false if not (which 
  * implies the memory address is invalid).
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  * 
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param address memory cell address
  * @return true if successful, false if not (invalid address)
  */
bool ram_write_cell_by_addr(struct RAM* memory, struct RAM_VALUE value, int address)
{
  if (address < 0 || address >= memory->size) {
    return false;
  }

  if (memory->cells[address].value_type == RAM_TYPE_STR && 
      memory->cells[address].types.s != NULL) {
    free(memory->cells[address].types.s);
  }

  memory->cells[address].value_type = value.value_type;

  if (value.value_type == RAM_TYPE_STR) {
    memory->cells[address].types.s = strdup(value.types.s);
  } else {
    memory->cells[address].types = value.types;
  }

  return true;
}


/**
  * ram_write_cell_by_name
  *
  * Writes the given value to a memory cell named by the given
  * variable. If a memory cell already exists with this name,
  * the existing value is overwritten by this new value. Returns
  * true since this operation always succeeds.
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param varname variable name
  * @return true (always successful)
  */
bool ram_write_cell_by_name(struct RAM* memory, struct RAM_VALUE value, char* varname)
{
  int map_index = binary_search(memory, varname);

  if (map_index != -1) {
    int cell = memory->map[map_index].cell;

    if (memory->cells[cell].value_type == RAM_TYPE_STR && 
        memory->cells[cell].types.s != NULL) {
      free(memory->cells[cell].types.s);
    }
    
    memory->cells[cell].value_type = value.value_type;
    
    if (value.value_type == RAM_TYPE_STR) {
      memory->cells[cell].types.s = strdup(value.types.s);
    } else {
      memory->cells[cell].types = value.types;
    }

  } else {
    grow_if_needed(memory);

    int cell = memory->size;

    memory->cells[cell].value_type = value.value_type;

    if (value.value_type == RAM_TYPE_STR) {
      memory->cells[cell].types.s = strdup(value.types.s);
    } else {
      memory->cells[cell].types = value.types;
    }

    insert_into_map(memory, varname, cell);

    memory->size++;
  }

  return true;
}


/**
  * @brief ram_print: prints the contents of memory
  *
  * Prints the contents of RAM to the console, for debugging.
  * RAM is printed in alphabetical order by variable name.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print(struct RAM* memory)
{
  printf("**MEMORY PRINT**\n");

  printf("Size: %d\n", memory->size);
  printf("Capacity: %d\n", memory->capacity);
  printf("Contents:\n");

  for (int i = 0; i < memory->size; i++) {
    char* varname = memory->map[i].varname;
    int cell = memory->map[i].cell;
    struct RAM_VALUE* value = &memory->cells[cell];
    
    printf("%d: %s, ", i, varname);
    switch (value->value_type) {
      case RAM_TYPE_INT:
        printf("int, %d\n", value->types.i);
        break;
      case RAM_TYPE_REAL:
        printf("real, %lf\n", value->types.d);
        break;
      case RAM_TYPE_STR:
        printf("str, '%s'\n", value->types.s);
        break;
      case RAM_TYPE_PTR:
        printf("ptr, %d\n", value->types.i);
        break;
      case RAM_TYPE_BOOLEAN:
        printf("boolean, %s\n", value->types.i ? "True" : "False");
        break;
      case RAM_TYPE_NONE:
        printf("None\n");
        break;
    }
  }

  printf("**END PRINT**\n");
}


/**
  * @brief ram_print_map: prints the contents of memory map
  *
  * Prints contents of RAM map to the console, for debugging.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print_map(struct RAM* memory)
{
  printf("**MEMORY MAP PRINT**\n");

  for (int i = 0; i < memory->size; i++)
  {
    printf("%d: '%s' -> cell %d\n", i, memory->map[i].varname, memory->map[i].cell);
  }

  printf("**END PRINT**\n");
}
