/*main.c*/

/**
  * @brief Main program for running google tests. Do not modify this file.
  *
  * @note Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtest/gtest.h>


/**
  * @brief main()
  *
  * main program: do not change this, google test will find all your
  * tests and run them. So leave the code below as is:
  *
  * @return 0 => all tests passed, non-zero => 1 or more tests failed
  */
int main() 
{
  //
  // to run just certain tests, uncomment the following and
  // then comment out the line ::testing::InitGoogleTest()
  //
  // Example: run all tests with "back" in their test name:
  //
  // ::testing::GTEST_FLAG(filter) = "*back*";
  //
  // by default configure to run all tests:
  //
  ::testing::InitGoogleTest();

  //
  // run all the tests, returns 0 if all pass and 1 if any fail:
  //
  int result = RUN_ALL_TESTS();

  return result;  
}
