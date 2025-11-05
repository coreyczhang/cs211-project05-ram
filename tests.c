/*tests.c*/

/**
  * @brief unit tests for nuPython's memory unit
  *
  * @note YOUR NAME
  *
  * @note Initial tests by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtest/gtest.h>

#include "ram.h"


//
// private helper functions:
//


//
// some provided unit tests to get started:
//
TEST(memory_module, initialization)
{
  //
  // create a new memory and make sure it's initialized properly:
  //
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);        // use ASSERT_TRUE with pointers
  ASSERT_TRUE(memory->cells != NULL);
  ASSERT_TRUE(memory->map != NULL);

  ASSERT_EQ(ram_size(memory), 0);
  ASSERT_EQ(ram_capacity(memory), 4);
  
  //
  // memory cells should be initialized to NONE:
  //
  for (int i=0; i<ram_capacity(memory); i++) {
    ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
  }

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, write_one_int) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // now check the memory, was x = 123 stored properly?
  //
  // since this is the first variable written, it will go
  // into memory cell 0:
  //
  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].types.i, 123);
  //
  // it will also go into map location 0:
  //
  ASSERT_STREQ(memory->map[0].varname, "x");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[0].cell, 0);

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, write_one_int_read_back) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // can we read the value back successfully?
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");

  ASSERT_TRUE(value != NULL);  
  ASSERT_EQ(value->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value->types.i, 123);

  //
  // tests passed, free memory
  //
  ram_free_value(value);
  ram_destroy(memory);
}


//
// Test: Write two variables in reverse alphabetical order
//
TEST(memory_module, write_two_variables_reverse_alphabetical)
{
    struct RAM* memory = ram_init();
    
    // Write "z" first
    struct RAM_VALUE val_z;
    val_z.value_type = RAM_TYPE_INT;
    val_z.types.i = 100;
    ram_write_cell_by_name(memory, val_z, "z");
    
    // Write "a" second (should be inserted before "z" in map)
    struct RAM_VALUE val_a;
    val_a.value_type = RAM_TYPE_INT;
    val_a.types.i = 200;
    ram_write_cell_by_name(memory, val_a, "a");
    
    ASSERT_EQ(ram_size(memory), 2);
    
    // Check the map is in alphabetical order
    ASSERT_STREQ(memory->map[0].varname, "a");
    ASSERT_STREQ(memory->map[1].varname, "z");
    
    // Check cells - "z" is still in cell 0, "a" is in cell 1
    ASSERT_EQ(memory->map[0].cell, 1);  // "a" -> cell 1
    ASSERT_EQ(memory->map[1].cell, 0);  // "z" -> cell 0
    
    ram_destroy(memory);
}

//
// Test: Write three variables and check ordering
//
TEST(memory_module, write_three_variables_mixed_order)
{
    struct RAM* memory = ram_init();
    
    // Write in order: y, a, m
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "y");
    
    val.types.i = 200;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 300;
    ram_write_cell_by_name(memory, val, "m");
    
    ASSERT_EQ(ram_size(memory), 3);
    
    // Map should be: a, m, y
    ASSERT_STREQ(memory->map[0].varname, "a");
    ASSERT_STREQ(memory->map[1].varname, "m");
    ASSERT_STREQ(memory->map[2].varname, "y");
    
    // Cells: y=cell0, a=cell1, m=cell2
    ASSERT_EQ(memory->map[0].cell, 1);  // "a" -> cell 1
    ASSERT_EQ(memory->map[1].cell, 2);  // "m" -> cell 2
    ASSERT_EQ(memory->map[2].cell, 0);  // "y" -> cell 0
    
    ram_destroy(memory);
}

//
// Test: Overwrite an existing variable
//
TEST(memory_module, overwrite_variable)
{
    struct RAM* memory = ram_init();
    
    // Write x = 100
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    ASSERT_EQ(ram_size(memory), 1);
    
    // Overwrite x = 999
    val.types.i = 999;
    ram_write_cell_by_name(memory, val, "x");
    
    // Size should still be 1
    ASSERT_EQ(ram_size(memory), 1);
    
    // Read back and verify
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
    ASSERT_EQ(value->types.i, 999);
    
    ram_free_value(value);
    ram_destroy(memory);
}

//
// Test: Read cell by address
//
TEST(memory_module, read_cell_by_address)
{
    struct RAM* memory = ram_init();
    
    // Write a = 111, b = 222
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 111;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 222;
    ram_write_cell_by_name(memory, val, "b");
    
    // "a" is in cell 0, "b" is in cell 1
    struct RAM_VALUE* value0 = ram_read_cell_by_addr(memory, 0);
    ASSERT_EQ(value0->types.i, 111);
    
    struct RAM_VALUE* value1 = ram_read_cell_by_addr(memory, 1);
    ASSERT_EQ(value1->types.i, 222);
    
    ram_free_value(value0);
    ram_free_value(value1);
    ram_destroy(memory);
}

//
// Test: Get address of variable
//
TEST(memory_module, get_address)
{
    struct RAM* memory = ram_init();
    
    // Write x, y, z
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    
    ram_write_cell_by_name(memory, val, "x");
    ram_write_cell_by_name(memory, val, "y");
    ram_write_cell_by_name(memory, val, "z");
    
    // Get addresses
    int addr_x = ram_get_addr(memory, "x");
    int addr_y = ram_get_addr(memory, "y");
    int addr_z = ram_get_addr(memory, "z");
    
    ASSERT_EQ(addr_x, 0);  // x was written first
    ASSERT_EQ(addr_y, 1);  // y was written second
    ASSERT_EQ(addr_z, 2);  // z was written third
    
    ram_destroy(memory);
}

//
// Test: Get address of non-existent variable
//
TEST(memory_module, get_address_not_found)
{
    struct RAM* memory = ram_init();
    
    int addr = ram_get_addr(memory, "nonexistent");
    ASSERT_EQ(addr, -1);
    
    ram_destroy(memory);
}

//
// Test: Read non-existent variable by name
//
TEST(memory_module, read_nonexistent_by_name)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "nonexistent");
    ASSERT_TRUE(value == NULL);
    
    ram_destroy(memory);
}

//
// Test: Read invalid address
//
TEST(memory_module, read_invalid_address)
{
    struct RAM* memory = ram_init();
    
    // Try to read beyond size
    struct RAM_VALUE* value = ram_read_cell_by_addr(memory, 10);
    ASSERT_TRUE(value == NULL);
    
    // Try negative address
    value = ram_read_cell_by_addr(memory, -1);
    ASSERT_TRUE(value == NULL);
    
    ram_destroy(memory);
}

//
// Test: Write by invalid address
//
TEST(memory_module, write_by_invalid_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    
    // Try to write to invalid address
    bool success = ram_write_cell_by_addr(memory, val, 10);
    ASSERT_FALSE(success);
    
    success = ram_write_cell_by_addr(memory, val, -1);
    ASSERT_FALSE(success);
    
    ram_destroy(memory);
}

//
// Test: Write by address (valid)
//
TEST(memory_module, write_by_address)
{
    struct RAM* memory = ram_init();
    
    // First write a variable to establish cell 0
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    // Now overwrite cell 0 by address
    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_TRUE(success);
    
    // Verify the change
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
    ASSERT_EQ(value->types.i, 999);
    
    ram_free_value(value);
    ram_destroy(memory);
}

//
// Test: Memory growth (exceeding initial capacity)
//
TEST(memory_module, memory_growth)
{
    struct RAM* memory = ram_init();
    
    ASSERT_EQ(ram_capacity(memory), 4);
    
    // Write 5 variables (exceeds initial capacity of 4)
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 1;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 2;
    ram_write_cell_by_name(memory, val, "b");
    
    val.types.i = 3;
    ram_write_cell_by_name(memory, val, "c");
    
    val.types.i = 4;
    ram_write_cell_by_name(memory, val, "d");
    
    ASSERT_EQ(ram_size(memory), 4);
    ASSERT_EQ(ram_capacity(memory), 4);
    
    // This should trigger growth
    val.types.i = 5;
    ram_write_cell_by_name(memory, val, "e");
    
    ASSERT_EQ(ram_size(memory), 5);
    ASSERT_EQ(ram_capacity(memory), 8);  // Should double
    
    // Verify all values are still accessible
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "e");
    ASSERT_EQ(v->types.i, 5);
    ram_free_value(v);
    
    ram_destroy(memory);
}

//
// Test: Multiple growths
//
TEST(memory_module, multiple_growths)
{
    struct RAM* memory = ram_init();
    
    // Initial capacity: 4
    // After 5th: 8
    // After 9th: 16
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    // Write 10 variables
    char name[2];
    name[1] = '\0';
    
    for (int i = 0; i < 10; i++) {
        name[0] = 'A' + i;
        val.types.i = i;
        ram_write_cell_by_name(memory, val, name);
    }
    
    ASSERT_EQ(ram_size(memory), 10);
    ASSERT_EQ(ram_capacity(memory), 16);
    
    // Verify all values
    for (int i = 0; i < 10; i++) {
        name[0] = 'A' + i;
        struct RAM_VALUE* v = ram_read_cell_by_name(memory, name);
        ASSERT_EQ(v->types.i, i);
        ram_free_value(v);
    }
    
    ram_destroy(memory);
}

//
// Test: Real (double) values
//
TEST(memory_module, real_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_REAL;
    val.types.d = 3.14159;
    
    ram_write_cell_by_name(memory, val, "pi");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "pi");
    ASSERT_EQ(v->value_type, RAM_TYPE_REAL);
    ASSERT_DOUBLE_EQ(v->types.d, 3.14159);
    
    ram_free_value(v);
    ram_destroy(memory);
}

//
// Test: String values
//
TEST(memory_module, string_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"hello world";
    
    ram_write_cell_by_name(memory, val, "message");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "message");
    ASSERT_EQ(v->value_type, RAM_TYPE_STR);
    ASSERT_STREQ(v->types.s, "hello world");
    
    ram_free_value(v);
    ram_destroy(memory);
}

//
// Test: Overwrite string with string
//
TEST(memory_module, overwrite_string)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"first";
    ram_write_cell_by_name(memory, val, "str");
    
    // Overwrite with different string
    val.types.s = (char*)"second";
    ram_write_cell_by_name(memory, val, "str");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "str");
    ASSERT_STREQ(v->types.s, "second");
    
    ram_free_value(v);
    ram_destroy(memory);
}

//
// Test: Boolean values
//
TEST(memory_module, boolean_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_BOOLEAN;
    
    val.types.i = 1;  // True
    ram_write_cell_by_name(memory, val, "flag");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "flag");
    ASSERT_EQ(v->value_type, RAM_TYPE_BOOLEAN);
    ASSERT_EQ(v->types.i, 1);
    
    ram_free_value(v);
    ram_destroy(memory);
}

//
// Test: Mixed data types
//
TEST(memory_module, mixed_data_types)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    
    // Integer
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    ram_write_cell_by_name(memory, val, "num");
    
    // Real
    val.value_type = RAM_TYPE_REAL;
    val.types.d = 2.718;
    ram_write_cell_by_name(memory, val, "e");
    
    // String
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"test";
    ram_write_cell_by_name(memory, val, "text");
    
    // Boolean
    val.value_type = RAM_TYPE_BOOLEAN;
    val.types.i = 0;
    ram_write_cell_by_name(memory, val, "flag");
    
    ASSERT_EQ(ram_size(memory), 4);
    
    // Verify each type
    struct RAM_VALUE* v_e = ram_read_cell_by_name(memory, "e");
    ASSERT_DOUBLE_EQ(v_e->types.d, 2.718);
    
    struct RAM_VALUE* v_flag = ram_read_cell_by_name(memory, "flag");
    ASSERT_EQ(v_flag->types.i, 0);
    
    struct RAM_VALUE* v_num = ram_read_cell_by_name(memory, "num");
    ASSERT_EQ(v_num->types.i, 42);
    
    struct RAM_VALUE* v_text = ram_read_cell_by_name(memory, "text");
    ASSERT_STREQ(v_text->types.s, "test");
    
    ram_free_value(v_e);
    ram_free_value(v_flag);
    ram_free_value(v_num);
    ram_free_value(v_text);
    ram_destroy(memory);
}

//
// Test: Stress test with many variables
//
TEST(memory_module, stress_test_many_variables)
{
    struct RAM* memory = ram_init();
    
    // Write 100 variables
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    char name[10];
    for (int i = 0; i < 100; i++) {
        sprintf(name, "var%d", i);
        val.types.i = i * 10;
        ram_write_cell_by_name(memory, val, name);
    }
    
    ASSERT_EQ(ram_size(memory), 100);
    
    // Verify random samples
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "var50");
    ASSERT_EQ(v->types.i, 500);
    ram_free_value(v);
    
    v = ram_read_cell_by_name(memory, "var99");
    ASSERT_EQ(v->types.i, 990);
    ram_free_value(v);
    
    ram_destroy(memory);
}

//
// Test: Alphabetical ordering with many variables
//
TEST(memory_module, alphabetical_ordering_many_vars)
{
    struct RAM* memory = ram_init();
    
    // Write variables in random order
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 1;
    
    ram_write_cell_by_name(memory, val, "elephant");
    ram_write_cell_by_name(memory, val, "apple");
    ram_write_cell_by_name(memory, val, "donkey");
    ram_write_cell_by_name(memory, val, "banana");
    ram_write_cell_by_name(memory, val, "cherry");
    
    // Check map is in alphabetical order
    ASSERT_STREQ(memory->map[0].varname, "apple");
    ASSERT_STREQ(memory->map[1].varname, "banana");
    ASSERT_STREQ(memory->map[2].varname, "cherry");
    ASSERT_STREQ(memory->map[3].varname, "donkey");
    ASSERT_STREQ(memory->map[4].varname, "elephant");
    
    ram_destroy(memory);
}

//
// Test: PTR type
//
TEST(memory_module, ptr_type)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_PTR;
    val.types.i = 0x1234;
    
    ram_write_cell_by_name(memory, val, "ptr");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "ptr");
    ASSERT_EQ(v->value_type, RAM_TYPE_PTR);
    ASSERT_EQ(v->types.i, 0x1234);
    
    ram_free_value(v);
    ram_destroy(memory);
}

//
// Test: NONE type remains after initialization
//
TEST(memory_module, none_type_initial)
{
    struct RAM* memory = ram_init();
    
    // All cells should be NONE initially
    for (int i = 0; i < ram_capacity(memory); i++) {
        ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
    }
    
    ram_destroy(memory);
}

