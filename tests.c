/*tests.c*/

/**
  * @brief unit tests for nuPython's memory unit
  *
  * @note Corey Zhang
  *
  * @note Initial tests by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtest/gtest.h>

#include "ram.h"

TEST(memory_module, initialization)
{
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);
  ASSERT_TRUE(memory->cells != NULL);
  ASSERT_TRUE(memory->map != NULL);

  ASSERT_EQ(ram_size(memory), 0);
  ASSERT_EQ(ram_capacity(memory), 4);
  
  for (int i=0; i<ram_capacity(memory); i++) {
    ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
  }

  ram_destroy(memory);
}

TEST(memory_module, write_one_int) 
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE i;
  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].types.i, 123);
  ASSERT_STREQ(memory->map[0].varname, "x");
  ASSERT_EQ(memory->map[0].cell, 0);

  ram_destroy(memory);
}

TEST(memory_module, write_one_int_read_back) 
{
  struct RAM* memory = ram_init();

  struct RAM_VALUE i;
  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");

  ASSERT_TRUE(value != NULL);  
  ASSERT_EQ(value->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value->types.i, 123);

  ram_free_value(value);
  ram_destroy(memory);
}

TEST(memory_module, write_two_variables_reverse_alphabetical)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val_z;
    val_z.value_type = RAM_TYPE_INT;
    val_z.types.i = 100;
    ram_write_cell_by_name(memory, val_z, "z");
    
    struct RAM_VALUE val_a;
    val_a.value_type = RAM_TYPE_INT;
    val_a.types.i = 200;
    ram_write_cell_by_name(memory, val_a, "a");
    
    ASSERT_EQ(ram_size(memory), 2);
    
    ASSERT_STREQ(memory->map[0].varname, "a");
    ASSERT_STREQ(memory->map[1].varname, "z");
    
    ASSERT_EQ(memory->map[0].cell, 1);
    ASSERT_EQ(memory->map[1].cell, 0);
    
    ram_destroy(memory);
}

TEST(memory_module, write_three_variables_mixed_order)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "y");
    
    val.types.i = 200;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 300;
    ram_write_cell_by_name(memory, val, "m");
    
    ASSERT_EQ(ram_size(memory), 3);
    
    ASSERT_STREQ(memory->map[0].varname, "a");
    ASSERT_STREQ(memory->map[1].varname, "m");
    ASSERT_STREQ(memory->map[2].varname, "y");
    
    ASSERT_EQ(memory->map[0].cell, 1);
    ASSERT_EQ(memory->map[1].cell, 2);
    ASSERT_EQ(memory->map[2].cell, 0);
    
    ram_destroy(memory);
}

TEST(memory_module, overwrite_variable)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    ASSERT_EQ(ram_size(memory), 1);
    
    val.types.i = 999;
    ram_write_cell_by_name(memory, val, "x");
    
    ASSERT_EQ(ram_size(memory), 1);
    
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(value != NULL);
    ASSERT_EQ(value->types.i, 999);
    
    ram_free_value(value);
    ram_destroy(memory);
}

TEST(memory_module, read_cell_by_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 111;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 222;
    ram_write_cell_by_name(memory, val, "b");
    
    struct RAM_VALUE* value0 = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(value0 != NULL);
    ASSERT_EQ(value0->types.i, 111);
    
    struct RAM_VALUE* value1 = ram_read_cell_by_addr(memory, 1);
    ASSERT_TRUE(value1 != NULL);
    ASSERT_EQ(value1->types.i, 222);
    
    ram_free_value(value0);
    ram_free_value(value1);
    ram_destroy(memory);
}

TEST(memory_module, get_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    
    ram_write_cell_by_name(memory, val, "x");
    ram_write_cell_by_name(memory, val, "y");
    ram_write_cell_by_name(memory, val, "z");
    
    int addr_x = ram_get_addr(memory, "x");
    int addr_y = ram_get_addr(memory, "y");
    int addr_z = ram_get_addr(memory, "z");
    
    ASSERT_EQ(addr_x, 0);
    ASSERT_EQ(addr_y, 1);
    ASSERT_EQ(addr_z, 2);
    
    ram_destroy(memory);
}

TEST(memory_module, get_address_not_found)
{
    struct RAM* memory = ram_init();
    
    int addr = ram_get_addr(memory, "nonexistent");
    ASSERT_EQ(addr, -1);
    
    ram_destroy(memory);
}

TEST(memory_module, read_nonexistent_by_name)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "nonexistent");
    ASSERT_TRUE(value == NULL);
    
    ram_destroy(memory);
}

TEST(memory_module, read_invalid_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE* value = ram_read_cell_by_addr(memory, 10);
    ASSERT_TRUE(value == NULL);
    
    value = ram_read_cell_by_addr(memory, -1);
    ASSERT_TRUE(value == NULL);
    
    ram_destroy(memory);
}

TEST(memory_module, write_by_invalid_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    
    bool success = ram_write_cell_by_addr(memory, val, 10);
    ASSERT_FALSE(success);
    
    success = ram_write_cell_by_addr(memory, val, -1);
    ASSERT_FALSE(success);
    
    ram_destroy(memory);
}

TEST(memory_module, write_by_address)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_TRUE(success);
    
    struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(value != NULL);
    ASSERT_EQ(value->types.i, 999);
    
    ram_free_value(value);
    ram_destroy(memory);
}

TEST(memory_module, memory_growth)
{
    struct RAM* memory = ram_init();
    
    ASSERT_EQ(ram_capacity(memory), 4);
    
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
    
    val.types.i = 5;
    ram_write_cell_by_name(memory, val, "e");
    
    ASSERT_EQ(ram_size(memory), 5);
    ASSERT_EQ(ram_capacity(memory), 8);
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "e");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 5);
    ram_free_value(v);
    
    ram_destroy(memory);
}

TEST(memory_module, multiple_growths)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    char name[2];
    name[1] = '\0';
    
    for (int i = 0; i < 10; i++) {
        name[0] = 'A' + i;
        val.types.i = i;
        ram_write_cell_by_name(memory, val, name);
    }
    
    ASSERT_EQ(ram_size(memory), 10);
    ASSERT_EQ(ram_capacity(memory), 16);
    
    for (int i = 0; i < 10; i++) {
        name[0] = 'A' + i;
        struct RAM_VALUE* v = ram_read_cell_by_name(memory, name);
        ASSERT_TRUE(v != NULL);
        if (v != NULL) {
            ASSERT_EQ(v->types.i, i);
            ram_free_value(v);
        }
    }
    
    ram_destroy(memory);
}

TEST(memory_module, real_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_REAL;
    val.types.d = 3.14159;
    
    ram_write_cell_by_name(memory, val, "pi");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "pi");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_REAL);
    ASSERT_DOUBLE_EQ(v->types.d, 3.14159);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, string_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"hello world";
    
    ram_write_cell_by_name(memory, val, "message");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "message");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_STR);
    ASSERT_STREQ(v->types.s, "hello world");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, overwrite_string)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"first";
    ram_write_cell_by_name(memory, val, "str");
    
    val.types.s = (char*)"second";
    ram_write_cell_by_name(memory, val, "str");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "str");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "second");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, boolean_values)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_BOOLEAN;
    
    val.types.i = 1;
    ram_write_cell_by_name(memory, val, "flag");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "flag");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_BOOLEAN);
    ASSERT_EQ(v->types.i, 1);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, mixed_data_types)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    ram_write_cell_by_name(memory, val, "num");
    
    val.value_type = RAM_TYPE_REAL;
    val.types.d = 2.718;
    ram_write_cell_by_name(memory, val, "e");
    
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"test";
    ram_write_cell_by_name(memory, val, "text");
    
    val.value_type = RAM_TYPE_BOOLEAN;
    val.types.i = 0;
    ram_write_cell_by_name(memory, val, "flag");
    
    ASSERT_EQ(ram_size(memory), 4);
    
    struct RAM_VALUE* v_e = ram_read_cell_by_name(memory, "e");
    ASSERT_TRUE(v_e != NULL);
    if (v_e) {
        ASSERT_DOUBLE_EQ(v_e->types.d, 2.718);
        ram_free_value(v_e);
    }
    
    struct RAM_VALUE* v_flag = ram_read_cell_by_name(memory, "flag");
    ASSERT_TRUE(v_flag != NULL);
    if (v_flag) {
        ASSERT_EQ(v_flag->types.i, 0);
        ram_free_value(v_flag);
    }
    
    struct RAM_VALUE* v_num = ram_read_cell_by_name(memory, "num");
    ASSERT_TRUE(v_num != NULL);
    if (v_num) {
        ASSERT_EQ(v_num->types.i, 42);
        ram_free_value(v_num);
    }
    
    struct RAM_VALUE* v_text = ram_read_cell_by_name(memory, "text");
    ASSERT_TRUE(v_text != NULL);
    if (v_text) {
        ASSERT_STREQ(v_text->types.s, "test");
        ram_free_value(v_text);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, stress_test_many_variables)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    char name[10];
    for (int i = 0; i < 100; i++) {
        sprintf(name, "var%d", i);
        val.types.i = i * 10;
        ram_write_cell_by_name(memory, val, name);
    }
    
    ASSERT_EQ(ram_size(memory), 100);
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "var50");
    ASSERT_TRUE(v != NULL);
    if (v) {
        ASSERT_EQ(v->types.i, 500);
        ram_free_value(v);
    }
    
    v = ram_read_cell_by_name(memory, "var99");
    ASSERT_TRUE(v != NULL);
    if (v) {
        ASSERT_EQ(v->types.i, 990);
        ram_free_value(v);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, alphabetical_ordering_many_vars)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 1;
    
    ram_write_cell_by_name(memory, val, "elephant");
    ram_write_cell_by_name(memory, val, "apple");
    ram_write_cell_by_name(memory, val, "donkey");
    ram_write_cell_by_name(memory, val, "banana");
    ram_write_cell_by_name(memory, val, "cherry");
    
    ASSERT_STREQ(memory->map[0].varname, "apple");
    ASSERT_STREQ(memory->map[1].varname, "banana");
    ASSERT_STREQ(memory->map[2].varname, "cherry");
    ASSERT_STREQ(memory->map[3].varname, "donkey");
    ASSERT_STREQ(memory->map[4].varname, "elephant");
    
    ram_destroy(memory);
}

TEST(memory_module, ptr_type)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_PTR;
    val.types.i = 0x1234;
    
    ram_write_cell_by_name(memory, val, "ptr");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "ptr");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_PTR);
    ASSERT_EQ(v->types.i, 0x1234);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, none_type_initial)
{
    struct RAM* memory = ram_init();
    
    for (int i = 0; i < ram_capacity(memory); i++) {
        ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, empty_string)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"";
    
    ram_write_cell_by_name(memory, val, "empty");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "empty");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, string_special_chars)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"tab\there\nnewline";
    
    ram_write_cell_by_name(memory, val, "special");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "special");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "tab\there\nnewline");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, overwrite_string_longer)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"hi";
    ram_write_cell_by_name(memory, val, "msg");
    
    val.types.s = (char*)"this is a much longer string than before";
    ram_write_cell_by_name(memory, val, "msg");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "msg");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "this is a much longer string than before");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, overwrite_string_shorter)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"this is a very long string";
    ram_write_cell_by_name(memory, val, "msg");
    
    val.types.s = (char*)"short";
    ram_write_cell_by_name(memory, val, "msg");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "msg");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "short");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, string_case_sensitive)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    
    val.types.s = (char*)"Hello";
    ram_write_cell_by_name(memory, val, "greeting");
    
    val.types.s = (char*)"HELLO";
    ram_write_cell_by_name(memory, val, "GREETING");
    
    ASSERT_EQ(ram_size(memory), 2);
    
    struct RAM_VALUE* v1 = ram_read_cell_by_name(memory, "greeting");
    ASSERT_TRUE(v1 != NULL);
    ASSERT_STREQ(v1->types.s, "Hello");
    
    struct RAM_VALUE* v2 = ram_read_cell_by_name(memory, "GREETING");
    ASSERT_TRUE(v2 != NULL);
    ASSERT_STREQ(v2->types.s, "HELLO");
    
    ram_free_value(v1);
    ram_free_value(v2);
    ram_destroy(memory);
}

TEST(memory_module, overwrite_int_with_string)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    ram_write_cell_by_name(memory, val, "x");
    
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"now a string";
    ram_write_cell_by_name(memory, val, "x");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_STR);
    ASSERT_STREQ(v->types.s, "now a string");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, overwrite_string_with_int)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"original string";
    ram_write_cell_by_name(memory, val, "x");
    
    val.value_type = RAM_TYPE_INT;
    val.types.i = 999;
    ram_write_cell_by_name(memory, val, "x");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->value_type, RAM_TYPE_INT);
    ASSERT_EQ(v->types.i, 999);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, address_constant_after_growth)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "first");
    
    int addr_before = ram_get_addr(memory, "first");
    ASSERT_EQ(addr_before, 0);
    
    for (int i = 0; i < 4; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        ram_write_cell_by_name(memory, val, name);
    }
    
    int addr_after = ram_get_addr(memory, "first");
    ASSERT_EQ(addr_after, 0);
    ASSERT_EQ(addr_before, addr_after);
    
    ram_destroy(memory);
}

TEST(memory_module, read_by_addr_after_growth)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 111;
    ram_write_cell_by_name(memory, val, "a");
    
    for (int i = 0; i < 5; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        val.types.i = i;
        ram_write_cell_by_name(memory, val, name);
    }
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 111);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, write_by_addr_after_write_by_name)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    int addr = ram_get_addr(memory, "x");
    ASSERT_EQ(addr, 0);
    
    val.types.i = 200;
    bool success = ram_write_cell_by_addr(memory, val, addr);
    ASSERT_TRUE(success);
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 200);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, multiple_reads_by_addr)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    ram_write_cell_by_name(memory, val, "x");
    
    for (int i = 0; i < 5; i++) {
        struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
        ASSERT_TRUE(v != NULL);
        ASSERT_EQ(v->types.i, 42);
        ram_free_value(v);
    }
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 42);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, address_zero)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 999;
    ram_write_cell_by_name(memory, val, "first");
    
    int addr = ram_get_addr(memory, "first");
    ASSERT_EQ(addr, 0);
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 999);
    
    val.types.i = 111;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_TRUE(success);
    
    ram_free_value(v);
    v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 111);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, address_mapping_consistent)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 10;
    ram_write_cell_by_name(memory, val, "z");
    
    val.types.i = 20;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 30;
    ram_write_cell_by_name(memory, val, "m");
    
    int addr_z = ram_get_addr(memory, "z");
    int addr_a = ram_get_addr(memory, "a");
    int addr_m = ram_get_addr(memory, "m");
    
    ASSERT_EQ(addr_z, 0);
    ASSERT_EQ(addr_a, 1);
    ASSERT_EQ(addr_m, 2);
    
    struct RAM_VALUE* v_z = ram_read_cell_by_addr(memory, addr_z);
    struct RAM_VALUE* v_a = ram_read_cell_by_addr(memory, addr_a);
    struct RAM_VALUE* v_m = ram_read_cell_by_addr(memory, addr_m);
    
    ASSERT_TRUE(v_z != NULL && v_a != NULL && v_m != NULL);
    ASSERT_EQ(v_z->types.i, 10);
    ASSERT_EQ(v_a->types.i, 20);
    ASSERT_EQ(v_m->types.i, 30);
    
    ram_free_value(v_z);
    ram_free_value(v_a);
    ram_free_value(v_m);
    ram_destroy(memory);
}

TEST(memory_module, string_independence)
{
    struct RAM* memory = ram_init();
    
    char original[] = "original";
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = original;
    
    ram_write_cell_by_name(memory, val, "test");
    
    strcpy(original, "modified");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "test");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "original");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, multiple_vars_same_string)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"same";
    
    ram_write_cell_by_name(memory, val, "a");
    ram_write_cell_by_name(memory, val, "b");
    ram_write_cell_by_name(memory, val, "c");
    
    struct RAM_VALUE* v_a = ram_read_cell_by_name(memory, "a");
    struct RAM_VALUE* v_b = ram_read_cell_by_name(memory, "b");
    struct RAM_VALUE* v_c = ram_read_cell_by_name(memory, "c");
    
    ASSERT_TRUE(v_a != NULL && v_b != NULL && v_c != NULL);
    ASSERT_STREQ(v_a->types.s, "same");
    ASSERT_STREQ(v_b->types.s, "same");
    ASSERT_STREQ(v_c->types.s, "same");
    
    ASSERT_TRUE(v_a->types.s != v_b->types.s);
    ASSERT_TRUE(v_b->types.s != v_c->types.s);
    
    ram_free_value(v_a);
    ram_free_value(v_b);
    ram_free_value(v_c);
    ram_destroy(memory);
}

TEST(memory_module, string_only_spaces)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"    ";
    
    ram_write_cell_by_name(memory, val, "spaces");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "spaces");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "    ");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, string_single_char)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"x";
    
    ram_write_cell_by_name(memory, val, "single");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "single");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "x");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, string_numeric)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_STR;
    val.types.s = (char*)"12345";
    
    ram_write_cell_by_name(memory, val, "numbers");
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "numbers");
    ASSERT_TRUE(v != NULL);
    ASSERT_STREQ(v->types.s, "12345");
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, get_addr_immediately_after_write)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    
    ram_write_cell_by_name(memory, val, "x");
    int addr = ram_get_addr(memory, "x");
    ASSERT_EQ(addr, 0);
    
    ram_write_cell_by_name(memory, val, "y");
    addr = ram_get_addr(memory, "y");
    ASSERT_EQ(addr, 1);
    
    ram_destroy(memory);
}

TEST(memory_module, read_by_addr_immediately_after_write_by_name)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    
    ram_write_cell_by_name(memory, val, "x");
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 42);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, write_read_sequence_by_addr)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 10;
    ram_write_cell_by_name(memory, val, "a");
    
    val.types.i = 20;
    ram_write_cell_by_name(memory, val, "b");
    
    val.types.i = 30;
    ram_write_cell_by_name(memory, val, "c");
    
    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 1);
    ASSERT_TRUE(success);
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 1);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 999);
    ram_free_value(v);
    
    v = ram_read_cell_by_name(memory, "b");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 999);
    ram_free_value(v);
    
    ram_destroy(memory);
}

TEST(memory_module, addr_of_last_variable)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 1;
    
    ram_write_cell_by_name(memory, val, "a");
    ram_write_cell_by_name(memory, val, "b");
    ram_write_cell_by_name(memory, val, "c");
    
    int addr_c = ram_get_addr(memory, "c");
    ASSERT_EQ(addr_c, 2);
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, addr_c);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 1);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, write_by_addr_after_overwrite)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    val.types.i = 200;
    ram_write_cell_by_name(memory, val, "x");
    
    int addr = ram_get_addr(memory, "x");
    val.types.i = 300;
    bool success = ram_write_cell_by_addr(memory, val, addr);
    ASSERT_TRUE(success);
    
    struct RAM_VALUE* v = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 300);
    
    ram_free_value(v);
    ram_destroy(memory);
}

TEST(memory_module, addr_boundary_at_capacity)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    for (int i = 0; i < 4; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        val.types.i = i;
        ram_write_cell_by_name(memory, val, name);
    }
    
    ASSERT_EQ(ram_size(memory), 4);
    ASSERT_EQ(ram_capacity(memory), 4);
    
    for (int i = 0; i < 4; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        int addr = ram_get_addr(memory, name);
        ASSERT_EQ(addr, i);
        
        struct RAM_VALUE* v = ram_read_cell_by_addr(memory, addr);
        ASSERT_TRUE(v != NULL);
        ASSERT_EQ(v->types.i, i);
        ram_free_value(v);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, write_addr_at_size_boundary)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    
    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 1);
    ASSERT_FALSE(success);
    
    ram_destroy(memory);
}

TEST(memory_module, read_addr_at_size_boundary)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "x");
    ram_write_cell_by_name(memory, val, "y");
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 2);
    ASSERT_TRUE(v == NULL);
    
    ram_destroy(memory);
}

TEST(memory_module, addr_at_exact_size_minus_one)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    
    val.types.i = 10;
    ram_write_cell_by_name(memory, val, "a");
    val.types.i = 20;
    ram_write_cell_by_name(memory, val, "b");
    val.types.i = 30;
    ram_write_cell_by_name(memory, val, "c");
    
    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 2);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 30);
    ram_free_value(v);

    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 2);
    ASSERT_TRUE(success);
    
    ram_destroy(memory);
}

TEST(memory_module, sequential_write_by_addr)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;

    val.types.i = 1;
    ram_write_cell_by_name(memory, val, "a");
    ram_write_cell_by_name(memory, val, "b");
    ram_write_cell_by_name(memory, val, "c");
    
    for (int i = 0; i < 3; i++) {
        val.types.i = i * 100;
        bool success = ram_write_cell_by_addr(memory, val, i);
        ASSERT_TRUE(success);
    }

    for (int i = 0; i < 3; i++) {
        struct RAM_VALUE* v = ram_read_cell_by_addr(memory, i);
        ASSERT_TRUE(v != NULL);
        ASSERT_EQ(v->types.i, i * 100);
        ram_free_value(v);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, get_addr_all_variables)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 1;
    
    char names[5][10] = {"var0", "var1", "var2", "var3", "var4"};
    
    for (int i = 0; i < 5; i++) {
        ram_write_cell_by_name(memory, val, names[i]);
    }
    
    for (int i = 0; i < 5; i++) {
        int addr = ram_get_addr(memory, names[i]);
        ASSERT_GE(addr, 0);
        ASSERT_LT(addr, 5);
    }
    
    ram_destroy(memory);
}

TEST(memory_module, addr_operations_empty_memory)
{
    struct RAM* memory = ram_init();

    int addr = ram_get_addr(memory, "nonexistent");
    ASSERT_EQ(addr, -1);

    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v == NULL);

    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 100;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_FALSE(success);
    
    ram_destroy(memory);
}

TEST(memory_module, alternating_operations)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;

    val.types.i = 100;
    ram_write_cell_by_name(memory, val, "a");
 
    val.types.i = 200;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_TRUE(success);

    val.types.i = 300;
    ram_write_cell_by_name(memory, val, "b");

    val.types.i = 400;
    success = ram_write_cell_by_addr(memory, val, 1);
    ASSERT_TRUE(success);

    struct RAM_VALUE* v_a = ram_read_cell_by_name(memory, "a");
    struct RAM_VALUE* v_b = ram_read_cell_by_name(memory, "b");
    
    ASSERT_TRUE(v_a != NULL && v_b != NULL);
    ASSERT_EQ(v_a->types.i, 200);
    ASSERT_EQ(v_b->types.i, 400);
    
    ram_free_value(v_a);
    ram_free_value(v_b);
    ram_destroy(memory);
}

TEST(memory_module, addr_with_single_variable)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
    val.types.i = 42;
    
    ram_write_cell_by_name(memory, val, "only");

    int addr = ram_get_addr(memory, "only");
    ASSERT_EQ(addr, 0);

    struct RAM_VALUE* v = ram_read_cell_by_addr(memory, 0);
    ASSERT_TRUE(v != NULL);
    ASSERT_EQ(v->types.i, 42);
    ram_free_value(v);
  
    v = ram_read_cell_by_addr(memory, 1);
    ASSERT_TRUE(v == NULL);

    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 0);
    ASSERT_TRUE(success);

    success = ram_write_cell_by_addr(memory, val, 1);
    ASSERT_FALSE(success);
    
    ram_destroy(memory);
}

TEST(memory_module, boundary_after_each_growth)
{
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE val;
    val.value_type = RAM_TYPE_INT;
   
    for (int i = 0; i < 4; i++) {
        char name[10];
        sprintf(name, "v%d", i);
        val.types.i = i;
        ram_write_cell_by_name(memory, val, name);
    }

    val.types.i = 999;
    bool success = ram_write_cell_by_addr(memory, val, 4);
    ASSERT_FALSE(success);
    
    char name[10];
    sprintf(name, "v%d", 4);
    val.types.i = 4;
    ram_write_cell_by_name(memory, val, name);

    val.types.i = 999;
    success = ram_write_cell_by_addr(memory, val, 5);
    ASSERT_FALSE(success);
    
    success = ram_write_cell_by_addr(memory, val, 4);
    ASSERT_TRUE(success);
    
    ram_destroy(memory);
}