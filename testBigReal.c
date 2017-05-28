#include <assert.h>
#include <stdio.h>
#include "testBigReal.h"

static void testEqualAndFree(BigReal a, BigReal b);
static void testEqualAndFreeFirst(BigReal a, BigReal b);
static void testFactorialAndFree(BigReal a, BigReal b);
static void testDivideAndFree(BigReal a, BigReal b, BigReal expected);
static void testReciprocalAndFree(BigReal a);

static void testReciprocalAndFree(BigReal a) {
    BigReal rec = reciprocal(a);
    BigReal result = multiply(rec, a);
    BigReal roundedResult = round1(result, 1);
    BigReal one = newBigRealHex("1");
    assert(isEqual(roundedResult, one));
    disposeBigReal(one);
    disposeBigReal(result);
    disposeBigReal(roundedResult);
    disposeBigReal(rec);
    disposeBigReal(a);
}
static void testDivideAndFree(BigReal a, BigReal b, BigReal expected) {
    BigReal result = divide(a, b);
    BigReal roundedResult = round1(result, DECIMAL_DIGITS - 1);
    disposeBigReal(result);
    assert(isEqual(roundedResult, expected) == TRUE);
    disposeBigReal(roundedResult);
    disposeBigReal(a);
    disposeBigReal(b);
    disposeBigReal(expected);
}

static void testFactorialAndFree(BigReal a, BigReal b) {
    BigReal c = factorial(a);
    disposeBigReal(a);
    assert(isEqual(c, b));
    disposeBigReal(b);
}
static void testEqualAndFreeFirst(BigReal a, BigReal b) {
    assert(isEqual(a, b));
    disposeBigReal(a);
}
static void testEqualAndFree(BigReal a, BigReal b) {
    assert(isEqual(a, b));
    disposeBigReal(a);
    disposeBigReal(b);
}

void testAll(void) {
    printf("Testing all functions...\n");

    testEquality();
    testInequality();
    testIsInteger();
    testAdd();
    testSubtract();
    testMultiply();
    testFactorial();
    testDivide();

    printf("All tests passed. You are awesome!\n");
}

void testModulus(void) {
    printf("Testing modulus... ");

    BigReal x = newBigRealHex("11");
    BigReal y = newBigRealHex("3");
    testEqualAndFree(modulus(x, y), newBigRealHex("2"));
    disposeBigReal(y);
    y = newBigRealHex("5");
    testEqualAndFree(modulus(x, y), newBigRealHex("2"));

    printf("Passed!\n");
}


void testFactorial(void) {
    printf("Testing factorial... ");

    testFactorialAndFree(newBigRealHex("0"), newBigRealHex("1"));
    testFactorialAndFree(newBigRealHex("1"), newBigRealHex("1"));
    testFactorialAndFree(newBigRealHex("2"), newBigRealHex("2"));
    testFactorialAndFree(newBigRealHex("3"), newBigRealHex("6"));
    testFactorialAndFree(newBigRealHex("4"), newBigRealHex("18"));
    testFactorialAndFree(newBigRealHex("5"), newBigRealHex("78"));
    testFactorialAndFree(newBigRealHex("14"), newBigRealHex("21C3677C82B40000"));

    printf("Passed!\n");
}

void testAdd(void) {
    printf("Testing addition... ");
    BigReal a = newBigRealHex("12");
    BigReal b = newBigRealHex("34");
    BigReal c = newBigRealHex("-13");
    BigReal d = newBigRealHex("-4A");
    BigReal e = newBigRealHex("0");

    testEqualAndFree(add(a, b), newBigRealHex("46"));
    testEqualAndFree(add(b, a), newBigRealHex("46"));
    testEqualAndFree(add(a, c), newBigRealHex("-1"));
    testEqualAndFree(add(c, a), newBigRealHex("-1"));
    testEqualAndFree(add(a, d), newBigRealHex("-38"));
    testEqualAndFree(add(d, a), newBigRealHex("-38"));
    testEqualAndFree(add(c, d), newBigRealHex("-5D"));
    testEqualAndFree(add(d, c), newBigRealHex("-5D"));

    testEqualAndFreeFirst(add(a, e), a);
    testEqualAndFreeFirst(add(e, a), a);
    testEqualAndFreeFirst(add(b, e), b);
    testEqualAndFreeFirst(add(e, b), b);
    testEqualAndFreeFirst(add(c, e), c);
    testEqualAndFreeFirst(add(e, c), c);
    testEqualAndFreeFirst(add(d, e), d);
    testEqualAndFreeFirst(add(e, d), d);
    testEqualAndFreeFirst(add(e, e), e);


    BigReal f = newBigRealHex("2222222222.2222222222");
    BigReal g = newBigRealHex("-3333333333.3333333333");
    testEqualAndFree(add(f, g), newBigRealHex("-1111111111.1111111111"));
    testEqualAndFree(add(g, f), newBigRealHex("-1111111111.1111111111"));

    BigReal h = newBigRealHex("1234543212345.1234543212345");
    BigReal i = newBigRealHex("FFFFFFFFFFFFF.1111111111111");
    BigReal j = newBigRealHex("-FFFFFFFFFFFFF.1111111111111");

    testEqualAndFree(add(h, i), newBigRealHex("11234543212344.2345654323456"));
    testEqualAndFree(add(i, h), newBigRealHex("11234543212344.2345654323456"));

    testEqualAndFree(add(h, j), newBigRealHex("-EDCBABCDEDCB9.FEDCBCDEFEDCC"));
    testEqualAndFree(add(j, h), newBigRealHex("-EDCBABCDEDCB9.FEDCBCDEFEDCC"));

    testEqualAndFree(add(i, j), newBigRealHex("0"));
    testEqualAndFree(add(j, i), newBigRealHex("0"));

    disposeBigReal(a);
    disposeBigReal(b);
    disposeBigReal(c);
    disposeBigReal(d);
    disposeBigReal(e);
    disposeBigReal(f);
    disposeBigReal(g);
    disposeBigReal(h);
    disposeBigReal(i);
    disposeBigReal(j);

    printf("Passed!\n");
}

void testSubtract(void) {
    printf("Testing subtraction... ");
    BigReal a = newBigRealHex("7F");
    BigReal b = newBigRealHex("12");
    BigReal c = newBigRealHex("-12");
    BigReal d = newBigRealHex("-F23");
    BigReal e = newBigRealHex("0");

    testEqualAndFree(subtract(a, b), newBigRealHex("6D"));
    testEqualAndFree(subtract(b, a), newBigRealHex("-6D"));
    testEqualAndFree(subtract(a, a), newBigRealHex("0"));
    testEqualAndFree(subtract(b, b), newBigRealHex("0"));
    testEqualAndFree(subtract(c, c), newBigRealHex("0"));
    testEqualAndFree(subtract(d, d), newBigRealHex("0"));
    testEqualAndFree(subtract(e, e), newBigRealHex("0"));
    testEqualAndFree(subtract(b, c), newBigRealHex("24"));
    testEqualAndFree(subtract(a, d), newBigRealHex("FA2"));
    testEqualAndFree(subtract(d, a), newBigRealHex("-FA2"));
    testEqualAndFree(subtract(c, d), newBigRealHex("F11"));
    testEqualAndFree(subtract(d, c), newBigRealHex("-F11"));
    testEqualAndFreeFirst(subtract(a, e), a);
    testEqualAndFreeFirst(subtract(b, e), b);
    testEqualAndFreeFirst(subtract(c, e), c);
    testEqualAndFreeFirst(subtract(d, e), d);
    testEqualAndFreeFirst(subtract(e, e), e);
    testEqualAndFree(subtract(e, a), newBigRealHex("-7F"));
    testEqualAndFree(subtract(e, b), newBigRealHex("-12"));
    testEqualAndFree(subtract(e, c), newBigRealHex("12"));
    testEqualAndFree(subtract(e, d), newBigRealHex("F23"));

    BigReal f = newBigRealHex("11111111.11111111");
    BigReal g = newBigRealHex("2345678.8765432");
    BigReal h = newBigRealHex("-2345678.8765432");
    testEqualAndFree(subtract(f, g), newBigRealHex("EDCBA98.89ABCDF1"));
    testEqualAndFree(subtract(g, f), newBigRealHex("-EDCBA98.89ABCDF1"));
    testEqualAndFree(subtract(f, h), newBigRealHex("13456789.98765431"));
    testEqualAndFree(subtract(h, f), newBigRealHex("-13456789.98765431"));

    disposeBigReal(a);
    disposeBigReal(b);
    disposeBigReal(c);
    disposeBigReal(d);
    disposeBigReal(e);
    disposeBigReal(f);
    disposeBigReal(g);
    disposeBigReal(h);

    printf("Passed!\n");
}

void testMultiply(void) {
    printf("Testing multiplication... ");

    BigReal a = newBigRealHex("1");
    BigReal b = newBigRealHex("0");
    BigReal c = newBigRealHex("-1");
    BigReal d = newBigRealHex("43.4"); // 67.25
    BigReal e = newBigRealHex("12F.8"); // 303.5
    BigReal f = newBigRealHex("-F1.8"); // -241.5

    // zero
    testEqualAndFreeFirst(multiply(b, a), b);
    testEqualAndFreeFirst(multiply(b, b), b);
    testEqualAndFreeFirst(multiply(b, c), b);
    testEqualAndFreeFirst(multiply(b, d), b);
    testEqualAndFreeFirst(multiply(b, e), b);
    testEqualAndFreeFirst(multiply(b, f), b);

    testEqualAndFreeFirst(multiply(a, b), b);
    testEqualAndFreeFirst(multiply(b, b), b);
    testEqualAndFreeFirst(multiply(c, b), b);
    testEqualAndFreeFirst(multiply(d, b), b);
    testEqualAndFreeFirst(multiply(e, b), b);
    testEqualAndFreeFirst(multiply(f, b), b);


    // one and negative one
    testEqualAndFreeFirst(multiply(a, a), a);
    testEqualAndFreeFirst(multiply(a, b), b);
    testEqualAndFreeFirst(multiply(a, c), c);
    testEqualAndFreeFirst(multiply(a, d), d);
    testEqualAndFreeFirst(multiply(a, e), e);
    testEqualAndFreeFirst(multiply(a, f), f);

    testEqualAndFreeFirst(multiply(b, a), b);
    testEqualAndFreeFirst(multiply(c, a), c);
    testEqualAndFreeFirst(multiply(d, a), d);
    testEqualAndFreeFirst(multiply(e, a), e);
    testEqualAndFreeFirst(multiply(f, a), f);

    testEqualAndFree(multiply(c, d), newBigRealHex("-43.4"));
    testEqualAndFree(multiply(c, e), newBigRealHex("-12F.8"));
    testEqualAndFree(multiply(c, f), newBigRealHex("F1.8"));

    // commutativity
    testEqualAndFree(multiply(d, e), multiply(e, d));
    testEqualAndFree(multiply(d, f), multiply(f, d));
    testEqualAndFree(multiply(e, f), multiply(f, e));

    // general tests


    BigReal g = newBigRealHex("4");
    BigReal h = newBigRealHex("2.8");
    testEqualAndFree(multiply(g, h), newBigRealHex("A"));

    // 67.25 * 303.5 = 20410.375
    testEqualAndFree(multiply(d, e), newBigRealHex("4FBA.6"));

    // 67.25 * -241.5 = -16240.875
    testEqualAndFree(multiply(d, f), newBigRealHex("-3F70.E"));

    // -241.5 * -241.5 = 58322.25
    testEqualAndFree(multiply(f, f), newBigRealHex("E3D2.4"));


    BigReal j = newBigRealHex("FFFFFFFF");
    testEqualAndFree(multiply(j, j), newBigRealHex("FFFFFFFE00000001"));

    disposeBigReal(a);
    disposeBigReal(b);
    disposeBigReal(c);
    disposeBigReal(d);
    disposeBigReal(e);
    disposeBigReal(f);
    disposeBigReal(g);
    disposeBigReal(h);
    disposeBigReal(j);

    a = newBigRealHex("123456789");
    b = newBigRealHex("FFFFFFFF");
    testEqualAndFree(multiply(a, b), multiply(b, a));

    disposeBigReal(a);
    disposeBigReal(b);

    printf("Passed!\n");
}

void testDivide(void) {
    printf("Testing division... ");

    // various ways of getting 1/2
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("2"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("2"), newBigRealHex("4"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("4"), newBigRealHex("8"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("8"), newBigRealHex("10"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("10"), newBigRealHex("20"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("20"), newBigRealHex("40"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("0.8"), newBigRealHex("1"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("0.4"), newBigRealHex("0.8"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("0.2"), newBigRealHex("0.4"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("0.1"), newBigRealHex("0.2"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("0.08"), newBigRealHex("0.1"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-1"), newBigRealHex("-2"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-2"), newBigRealHex("-4"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-4"), newBigRealHex("-8"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-8"), newBigRealHex("-10"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-10"), newBigRealHex("-20"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-20"), newBigRealHex("-40"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-0.8"), newBigRealHex("-1"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-0.4"), newBigRealHex("-0.8"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-0.2"), newBigRealHex("-0.4"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-0.1"), newBigRealHex("-0.2"), newBigRealHex("0.8"));
    testDivideAndFree(newBigRealHex("-0.08"), newBigRealHex("-0.1"), newBigRealHex("0.8"));

    testDivideAndFree(newBigRealHex("-1"), newBigRealHex("2"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("-2"), newBigRealHex("4"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("-0.8"), newBigRealHex("1"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("-0.4"), newBigRealHex("0.8"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("-2"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("2"), newBigRealHex("-4"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("0.8"), newBigRealHex("-1"), newBigRealHex("-0.8"));
    testDivideAndFree(newBigRealHex("0.4"), newBigRealHex("-0.8"), newBigRealHex("-0.8"));

    // zero
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("1"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("2"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("4"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("FFFFF"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("FFFFFFFFFFF"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("0.1"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("0.00001"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("0.00000000000001"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-1"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-2"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-4"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-FFFFF"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-FFFFFFFFFFFFFFF"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-0.1"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-0.00001"), newBigRealHex("0"));
    testDivideAndFree(newBigRealHex("0"), newBigRealHex("-0.00000000000001"), newBigRealHex("0"));

    // dividing by one
    testDivideAndFree(newBigRealHex("2"), newBigRealHex("1"), newBigRealHex("2"));
    testDivideAndFree(newBigRealHex("-2"), newBigRealHex("1"), newBigRealHex("-2"));
    testDivideAndFree(newBigRealHex("2FFFFF.12345"), newBigRealHex("1"), newBigRealHex("2FFFFF.12345"));
    testDivideAndFree(newBigRealHex("0.0000129"), newBigRealHex("1"), newBigRealHex("0.0000129"));
    testDivideAndFree(newBigRealHex("2"), newBigRealHex("-1"), newBigRealHex("-2"));
    testDivideAndFree(newBigRealHex("2FFFFF.12345"), newBigRealHex("-1"), newBigRealHex("-2FFFFF.12345"));
    testDivideAndFree(newBigRealHex("-0.0000129"), newBigRealHex("1"), newBigRealHex("-0.0000129"));
    testDivideAndFree(newBigRealHex("-2"), newBigRealHex("1"), newBigRealHex("-2"));


    // testing inverse property of multiplication and division
    testReciprocalAndFree(newBigRealHex("1"));
    testReciprocalAndFree(newBigRealHex("2"));
    testReciprocalAndFree(newBigRealHex("3"));
    testReciprocalAndFree(newBigRealHex("4"));
    testReciprocalAndFree(newBigRealHex("F"));
    testReciprocalAndFree(newBigRealHex("FF"));
    testReciprocalAndFree(newBigRealHex("FFF"));
    testReciprocalAndFree(newBigRealHex("FFFF"));
    testReciprocalAndFree(newBigRealHex("FFFFF"));
    testReciprocalAndFree(newBigRealHex("FFFFFF"));
    testReciprocalAndFree(newBigRealHex("FFFFFFFFFFFFFFFF"));
    testReciprocalAndFree(newBigRealHex("1.23456789"));
    testReciprocalAndFree(newBigRealHex("0.00000000112223333"));
    testReciprocalAndFree(newBigRealHex("FFFFF.FEDFEDF1234"));
    testReciprocalAndFree(newBigRealHex("0.123456789"));
    testReciprocalAndFree(newBigRealHex("0.00000000FEDC"));
    testReciprocalAndFree(newBigRealHex("-1"));
    testReciprocalAndFree(newBigRealHex("-2"));
    testReciprocalAndFree(newBigRealHex("-3"));
    testReciprocalAndFree(newBigRealHex("-4"));
    testReciprocalAndFree(newBigRealHex("-FFFFFF"));
    testReciprocalAndFree(newBigRealHex("-FFFFFFFFFFFFFFFF"));
    testReciprocalAndFree(newBigRealHex("-1.23456789"));
    testReciprocalAndFree(newBigRealHex("-0.00000000112223333"));
    testReciprocalAndFree(newBigRealHex("-FFFFF.FEDFEDF1234"));
    testReciprocalAndFree(newBigRealHex("-0.123456789"));
    testReciprocalAndFree(newBigRealHex("-0.00000000FEDC"));

    // miscellaneous
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("10"), newBigRealHex("0.1"));
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("100"), newBigRealHex("0.01"));
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("100000000"), newBigRealHex("0.00000001"));
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("1000000000"), newBigRealHex("0.000000001"));
    testDivideAndFree(newBigRealHex("1"), newBigRealHex("-2000000000"), newBigRealHex("-0.0000000008"));

    printf("Passed!\n");
}

void testEquality(void) {
    printf("Testing equality... ");

    BigReal bf1 = newBigRealHex("123");
    BigReal bf2 = newBigRealHex("123.0");
    BigReal bf3 = newBigRealHex("123.00000000000");
    BigReal bf1c = newBigRealHex("123");
    BigReal bf2c = newBigRealHex("123.0");
    BigReal bf3c = newBigRealHex("123.00000000000");
    assert(isEqual(bf1, bf2));
    assert(isEqual(bf2, bf1));
    assert(isEqual(bf2, bf3));
    assert(isEqual(bf3, bf2));
    assert(isEqual(bf1, bf3));
    assert(isEqual(bf3, bf1));

    assert(isEqual(bf1, bf1));
    assert(isEqual(bf2, bf2));
    assert(isEqual(bf3, bf3));

    assert(isEqual(bf1, bf1c));
    assert(isEqual(bf1, bf2c));
    assert(isEqual(bf1, bf3c));
    assert(isEqual(bf2, bf1c));
    assert(isEqual(bf2, bf2c));
    assert(isEqual(bf2, bf3c));
    assert(isEqual(bf3, bf1c));
    assert(isEqual(bf3, bf2c));
    assert(isEqual(bf3, bf3c));
    disposeBigReal(bf1);
    disposeBigReal(bf2);
    disposeBigReal(bf3);
    disposeBigReal(bf1c);
    disposeBigReal(bf2c);
    disposeBigReal(bf3c);

    BigReal pi = newBigRealHex("3.14");
    BigReal e = newBigRealHex("2.72");
    assert(isEqual(pi, e) == FALSE);
    assert(isEqual(e, pi) == FALSE);
    assert(isEqual(pi, pi));
    assert(isEqual(e, e));
    disposeBigReal(pi);
    disposeBigReal(e);

    BigReal two = newBigRealHex("2");
    BigReal twoMinusEpsilon = newBigRealHex("1.999999999999999");
    assert(isEqual(two, twoMinusEpsilon) == FALSE);
    BigReal negativeTwo = newBigRealHex("-2");
    assert(isEqual(two, negativeTwo) == FALSE);
    disposeBigReal(two);
    disposeBigReal(twoMinusEpsilon);

    BigReal zero = newBigRealHex("0");
    BigReal negativeZero = newBigRealHex("-0");
    assert(isEqual(zero, negativeZero));
    disposeBigReal(zero);
    disposeBigReal(negativeZero);

    printf("Passed!\n");
}

void testInequality(void) {
    printf("Testing inequality... ");
    BigReal big = newBigRealHex("FFFFFFFFF");
    BigReal bigger = newBigRealHex("FFFFFFFFFFFFF");
    assert(isLess(big, bigger) == TRUE);
    assert(isLess(bigger, big) == FALSE);
    assert(isGreater(bigger, big) == TRUE);
    assert(isGreater(big, bigger) == FALSE);
    disposeBigReal(big);
    disposeBigReal(bigger);

    BigReal pi = newBigRealHex("3.14");
    BigReal e = newBigRealHex("2.72");
    assert(isLess(e, pi) == TRUE);
    assert(isLessOrEqual(e, pi) == TRUE);
    assert(isGreater(pi, e) == TRUE);
    assert(isGreaterOrEqual(pi, e) == TRUE);
    disposeBigReal(pi);
    disposeBigReal(e);

    BigReal negativeOne = newBigRealHex("-1");
    BigReal zero = newBigRealHex("0");
    BigReal positiveOne = newBigRealHex("1");
    assert(isLess(negativeOne, zero) == TRUE);
    assert(isLess(zero, positiveOne) == TRUE);
    assert(isLess(negativeOne, positiveOne) == TRUE);
    assert(isGreater(zero, negativeOne) == TRUE);
    assert(isGreater(positiveOne, zero) == TRUE);
    assert(isGreater(positiveOne, negativeOne) == TRUE);

    assert(isLess(positiveOne, positiveOne) == FALSE);
    assert(isLess(zero, zero) == FALSE);
    assert(isLess(negativeOne, negativeOne) == FALSE);

    assert(isLessOrEqual(positiveOne, positiveOne) == TRUE);
    assert(isLessOrEqual(zero, zero) == TRUE);
    assert(isLessOrEqual(negativeOne, negativeOne) == TRUE);

    assert(isGreater(positiveOne, positiveOne) == FALSE);
    assert(isGreater(zero, zero) == FALSE);
    assert(isGreater(negativeOne, negativeOne) == FALSE);

    assert(isGreaterOrEqual(positiveOne, positiveOne) == TRUE);
    assert(isGreaterOrEqual(zero, zero) == TRUE);
    assert(isGreaterOrEqual(negativeOne, negativeOne) == TRUE);

    assert(isEqual(positiveOne, positiveOne) == TRUE);
    assert(isEqual(zero, zero) == TRUE);
    assert(isEqual(negativeOne, negativeOne) == TRUE);


    BigReal two = newBigRealHex("2");
    BigReal twoMinusEpsilon = newBigRealHex("1.FFFFFFFFFFF");
    assert(isGreater(two, twoMinusEpsilon) == TRUE);
    assert(isGreater(twoMinusEpsilon, two) == FALSE);
    assert(isLess(two, twoMinusEpsilon) == FALSE);
    assert(isLess(twoMinusEpsilon, two) == TRUE);
    assert(isGreaterOrEqual(two, twoMinusEpsilon) == TRUE);
    assert(isGreaterOrEqual(twoMinusEpsilon, two) == FALSE);
    assert(isLessOrEqual(two, twoMinusEpsilon) == FALSE);
    assert(isLessOrEqual(twoMinusEpsilon, two) == TRUE);

    BigReal negativeTwo = newBigRealHex("-2");
    assert(isLess(negativeTwo, negativeOne) == TRUE);
    assert(isGreater(negativeOne, negativeTwo) == TRUE);

    disposeBigReal(negativeTwo);
    disposeBigReal(two);
    disposeBigReal(twoMinusEpsilon);
    disposeBigReal(negativeOne);
    disposeBigReal(zero);
    disposeBigReal(positiveOne);

    printf("Passed!\n");

}

void testIsInteger(void) {
    printf("Testing isInteger... ");

    BigReal x = newBigRealHex("3");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("3.0000000");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("-4");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("-4.000000");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("0.0000000");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("0");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("FFFFFFFFFFFF");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("FFFFFFFFFFFF.0000000");
    assert(isInteger(x) == TRUE);
    disposeBigReal(x);

    x = newBigRealHex("2.999999999");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    x = newBigRealHex("2.398420349");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    x = newBigRealHex("-3.3849048320430");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    x = newBigRealHex("-3.999999999");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    x = newBigRealHex("0.000000001");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    x = newBigRealHex("FFFFFFFFF.999999999");
    assert(isInteger(x) == FALSE);
    disposeBigReal(x);

    printf("Passed!\n");
}
