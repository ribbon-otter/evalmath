//This file is licensed under MIT-0
//Copyright 2026 Ribbon-otter
//It is the unit tests for evalmath, a simple math string interpeter
#include <gtest/gtest.h>

#include "evalmath.h"
#include <cmath>
#include <string>

using namespace evalmath;

TEST(evalmath, eval)
{
    EXPECT_EQ(1.0, *eval("1"));
    EXPECT_FALSE(eval("1 +"));
    EXPECT_EQ(2.0, *eval("1+1"));
    EXPECT_EQ(220.0, *eval(" 120 + 100 ")) ;
    EXPECT_EQ(4.0, *eval("1+1+ 2")) ;
    EXPECT_EQ(5.0, *eval("1+2 * 2")) ;
    EXPECT_EQ(9.0, *eval("2*3+3")) ;
    EXPECT_EQ(12.0, *eval("6+2*3")); 
    EXPECT_EQ(2.0, *eval("9-7"));  
    EXPECT_EQ(7.0, *eval("6-2+3"));  
    EXPECT_EQ(1.0, *eval("6-2-3")); 
    EXPECT_EQ(3.0, *eval("6/2")); 
    EXPECT_EQ(6.0, *eval("6 / 2+3")); 
    EXPECT_FALSE(eval("6+2*")); 
    EXPECT_FALSE(eval("6 2")); 
    EXPECT_FALSE(eval("6**2")); 
    EXPECT_FALSE(eval("+")); 
    EXPECT_FALSE(eval("1(5)")) ; 
}

TEST(evalmath, eval_paren)
{
    EXPECT_EQ(12.0, *eval("2*(3+3)")) ;
    EXPECT_EQ(12.0, *eval("(3+3)*2")) ;
    EXPECT_EQ(3.0, *eval("(3+3) / 2")) ;
    EXPECT_EQ(3.0, *eval("((3+ (3 ) ) / 2)")) ;
    EXPECT_FALSE(eval("(3+3 / 2")) ;
    EXPECT_FALSE(eval("3(")) ;
    EXPECT_FALSE(eval("(3")) ;
    EXPECT_FALSE(eval(")3")) ;
    EXPECT_FALSE(eval("3)")) ;
    EXPECT_FALSE(eval("1+()2")) ;
    EXPECT_FALSE(eval("()")) ;
    EXPECT_FALSE(eval("3(+)3 / 2")) ;
    EXPECT_FALSE(eval("3+(3 /) 2")) ;
    EXPECT_FALSE(eval("3+3 (/ 2)")) ;
    EXPECT_FALSE(eval("((3+ (3  ) / 2)")) ;
}

TEST(evalmath, eval_decimal)
{
    EXPECT_EQ(5.5, *eval("5.5")) ;
    EXPECT_EQ(0.5, *eval("0.5")) ;
    EXPECT_EQ(5, *eval("5.")) ;
    EXPECT_FALSE(eval("0.5.3")) ;
    EXPECT_FALSE(eval("0..3")) ;
    EXPECT_EQ(0.5, eval(".5")) ; 
}

TEST(evalmath, eval_negative)
{
    EXPECT_EQ(-5, *eval("-5")) ;
    EXPECT_EQ(-4, *eval("1 + -5")) ;
    EXPECT_EQ(-4, *eval("-5 + 1")) ;
    EXPECT_EQ(8, eval("6--2")); 
    EXPECT_FALSE(eval("1(-5)")) ; 
}

TEST(evalmath, eval_uniary)
{
    EXPECT_EQ(5, *eval("+5")) ;
    EXPECT_EQ(7, *eval("+(2+5)")) ;
    EXPECT_EQ(7, *eval("(+(2+5))")) ;
    EXPECT_EQ(10, *eval("2*+5")) ;
    EXPECT_EQ(7, *eval("2+++++++5")) ;
    EXPECT_EQ(-3, *eval("3+-3*2")) ;
    EXPECT_EQ(-18, *eval("-(3*-(-3*2))")) ;
}

TEST(evalmath, eval_longer_numbers)
{
    //just more tests to confirm that multidigit math works
    EXPECT_EQ(1111'1111, *eval("11110000+1111")) ;
    EXPECT_EQ(3333'3333, *eval("-10+33330000+1121+1111+1111")) ;
}

TEST(evalmath, eval_nonreals) //deal with the nasty strange numbers
{
    //numbers too big and too small to fit inside doubles
    EXPECT_FALSE(eval("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999")) ;
    EXPECT_FALSE(eval("0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"));
    //division by zero is an error (not infinity)
    EXPECT_FALSE(eval("1/0"));
    EXPECT_FALSE(eval("-5/(3-3)"));
    EXPECT_FALSE(eval("5/(3-3)"));
    
    ///mathing numbers to get infinity is error
    std::string large_num = "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    EXPECT_TRUE(eval(large_num)); //fits inside a double
    EXPECT_TRUE(eval("-"+large_num)); //fits inside a double
    EXPECT_FALSE(eval(large_num + "*" + large_num)); //but our result doesn't.
    EXPECT_FALSE(eval("-" + large_num + "*" + large_num)); //and negative numbers
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// vim: ts=4 sw=4 expandtab
