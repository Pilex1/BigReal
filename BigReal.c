// BigReal.c
// Stores and performs mathematical operations on 
// arbitrary precision fixed point real numbers
// Copyright Alex Tan 2017

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "BigReal.h"

typedef struct _bigReal {
    int sign;
    storeType values[INTEGER_DIGITS + DECIMAL_DIGITS];
} bigReal;

static BigReal newBigRealFromArr(storeType arr[], int length);
static BigReal newBigRealFromHexArr(int arr[]);
static int mostSignificantDigitIndex(long long arr[]);
static long long mostSignificantDigit(long long arr[]);
static int isZero(BigReal br);
static void checkForZero(BigReal br);
static char *uintToHexString(unsigned int x);
static storeType *hexStringToBigInt(char *str, int maxSize);
static storeType *hexStringToBigIntReverse(char *str, int maxSize);
static int hexCharToInt(char c);
static char intToHexChar(int x);
static int *splitToHexArr(BigReal x);
static int *shiftArr(int *arr, int length, int shift);
static int powRI(int x, unsigned int y);

static char *trimTrailingZeroes(char *str);
static char *trimLeadingZeroes(char *str);

static BigReal multiplyS(BigReal x, BigReal y, int safe);
static BigReal bitShiftLeftS(BigReal x, int bits, int safe);
static BigReal bitShiftRightS(BigReal x, int bits, int safe);


// calculates x to the power of y
// by iterative multiplication
static int powRI(int x, unsigned int y) {
    int result = 1;
    while (y >= 1) {
        result *= x;
        y--;
    }
    return result;
}

// raises x to the power of y by iterative multiplication
// y must be a non-negative integer
static BigReal powI(BigReal x, BigReal y) {
    assert(isInteger(y));
    assert(isGreaterOrEqual(y, newBigRealHex("0")));
    BigReal one = newBigRealHex("1");
    BigReal result = newBigRealHex("1");
    BigReal yCopy = copy(y);
    while (isGreaterOrEqual(yCopy, one)) {
        BigReal temp = multiply(result, x);
        disposeBigReal(result);
        result = temp;

        temp = subtract(yCopy, one);
        disposeBigReal(yCopy);
        yCopy = temp;
    }
    return result;
}

static int isZero(BigReal br) {
    int cond = TRUE;
    int i = 0;
    while (i < DIGITS && cond == TRUE) {
        cond = (br->values[i] == 0);
        i++;
    }
    return cond;
}

static void checkForZero(BigReal br) {
    if (isZero(br) == TRUE) {
        br->sign = ZERO;
    }
}

static storeType *stringToBigInt(char *str, int maxSize) {
    storeType *number = malloc(maxSize * sizeof(storeType));
    int i = 0;
    while (i < maxSize) {
        number[i] = 0;
        i++;
    }

    int strLength = strlen(str);
    i = strLength - 1;
    unsigned int j = 0;
    long long curDigit = 0;
    int digitIndex = 0;
    while (i >= 0) {
        curDigit += powRI(10, j) * hexCharToInt(str[i]);
        if (curDigit >= BASE) {
            assert(curDigit - BASE < BASE);
            number[i] =
                digitIndex++;
        }
        str[i] = '\0';
        i--;
        j++;
    }
}

static storeType *hexStringToBigInt(char *str, int maxSize) {
    storeType *number = malloc(maxSize * sizeof(storeType));
    int i = 0;
    while (i < maxSize) {
        number[i] = 0;
        i++;
    }

    int strLength = strlen(str);
    assert(strLength > 0);

    //digits in the number in base 2^32
    int digits = (strLength - 1) / HEX_DIGITS_PER_DIGIT + 1;
    assert(digits <= maxSize);

    int stringIndex = strLength - 1;
    i = 0;
    while (i < maxSize && stringIndex >= 0) {
        // computes the current base 2^32 digit

        unsigned int j = 0;
        storeType digit = 0;
        while (j < HEX_DIGITS_PER_DIGIT && stringIndex >= 0) {
            int hex = hexCharToInt(str[stringIndex]);
            digit += powRI(16, j) * hex;
            stringIndex--;
            j++;
        }
        number[maxSize - 1 - i] = digit;
        i++;
    }
    return number;
}

static storeType *hexStringToBigIntReverse(char *str, int maxSize) {
    storeType *number = malloc(maxSize * sizeof(storeType));
    int i = 0;
    while (i < maxSize) {
        number[i] = 0;
        i++;
    }

    int strLength = strlen(str);
    assert(strLength > 0);

    //digits in the number in base 2^32
    int digits = (strLength - 1) / HEX_DIGITS_PER_DIGIT + 1;
    assert(digits <= maxSize);

    int stringIndex = 0;
    i = 0;
    while (i < maxSize && stringIndex < strLength) {
        // computes the current base 2^32 digit

        unsigned int j = 0;
        storeType digit = 0;
        while (j < HEX_DIGITS_PER_DIGIT && stringIndex < strLength) {
            int hex = hexCharToInt(str[stringIndex]);
            digit += powRI(16, HEX_DIGITS_PER_DIGIT - 1 - j) * hex;
            stringIndex++;
            j++;
        }
        number[i] = digit;
        i++;
    }
    return number;
}


static int hexCharToInt(char c) {
    assert((c >= '0'&&c <= '9') || (c >= 'A'&&c <= 'F'));
    int val = 0;
    if (c >= '0' && c <= '9') {
        val = c - '0';
    } else {
        val = 10 + (c - 'A');
    }
    return val;
}

void disposeBigReal(BigReal x) {
    free(x);
}

// must be positive
static BigReal newBigRealFromArr(storeType arr[], int length) {
    assert(length <= DIGITS);
    BigReal new = newBigRealHex("0");
    new->sign = POSITIVE;
    int i = DIGITS - 1;
    int j = length - 1;
    while (j >= 0) {

        assert(i >= 0);

        new->values[i] = arr[j];

        i--;
        j--;
    }

    checkForZero(new);
    return new;
}


// creates a new BigReal, given an input
// string in decimal
BigReal newBigRealDec(char *originalString) {

    BigReal new = newBigRealHex("0");

    // original string length + extra charactes for '.' '0'
    // in case those aren't added on + null terminator
    char *string = malloc(strlen(originalString) + 2 + 1);
    strcpy(string, originalString);

    char *radixPoint = strchr(string, '.');
    if (radixPoint == NULL) {
        // if there is no radix point, then artifically add ".0"
        // to the end

        char *end = strchr(string, '\0');
        *end = '.';
        *(end + 1) = '0';
        *(end + 2) = '\0';

        radixPoint = end;
    }

    // split into two strings - integer and decimal part
    *radixPoint = '\0';
    char *integer = string;
    char *decimal = radixPoint + 1;

    // check for negative sign
    int isNegative = FALSE;
    if (*integer == '-') {
        isNegative = TRUE;
        integer++;
    }

    BigReal one = newBigRealHex("1");
    BigReal ten = newBigRealHex("A");
    BigReal tenth = reciprocal(ten);

    // deal with integer part
    BigReal b = copy(one);
    int integerDigits = strlen(integer);
    int i = 0;
    while (i < integerDigits) {

        char currentDigitString[] = { integer[integerDigits - 1 - i], '\0' };
        BigReal currentDigit = newBigRealHex(currentDigitString);
        BigReal val = multiply(b, currentDigit);

        // new += val
        BigReal temp = add(new, val);
        disposeBigReal(new);
        new = temp;

        disposeBigReal(currentDigit);
        disposeBigReal(val);

        // b *= 10
        temp = multiply(b, ten);
        disposeBigReal(b);
        b = temp;

        i++;
    }
    disposeBigReal(b);

    // deal with decimal part
    b = copy(tenth);
    int decimalDigits = strlen(decimal);
    i = 0;
    while (i < decimalDigits) {

        char currentDigitString[] = { decimal[i], '\0' };
        BigReal currentDigit = newBigRealHex(currentDigitString);
        BigReal val = multiply(b, currentDigit);

        // new += val
        BigReal temp = add(new, val);
        disposeBigReal(new);
        new = temp;

        disposeBigReal(currentDigit);
        disposeBigReal(val);

        // b /= 10
        temp = multiplyS(b, tenth, FALSE);
        disposeBigReal(b);
        b = temp;

        i++;
    }

    disposeBigReal(one);
    disposeBigReal(ten);
    disposeBigReal(tenth);
    free(string);

    if (isNegative == TRUE) {
        new->sign = NEGATIVE;
    }

    return new;

    /*int digitIndex = strlen(decimal) - 1;
    BigReal i = newBigRealHex("0");
    BigReal result = newBigRealHex("0");
    BigReal one = newBigRealHex("1");
    while (digitIndex >= 0) {
    BigReal powTen = newBigRealHex("A");
    powTen = powI(powTen, i);
    char digitString[2] = { decimal[digitIndex],'\0' };
    BigReal digitBr = newBigRealHex(digitString);
    powTen = multiply(powTen, digitBr);
    result = add(result, powTen);

    BigReal temp = add(i, one);
    disposeBigReal(i);
    i = temp;

    digitIndex--;
    }
    disposeBigReal(one);
    return result;
    */
}


// has to be a hex string
BigReal newBigRealHex(char *stringOld) {
    char *string = malloc(strlen(stringOld) + 1);
    char *stringC = string;
    strcpy(string, stringOld);
    BigReal new = malloc(sizeof(bigReal));

    //checks for negative
    if (*string == '-') {
        new->sign = NEGATIVE;
        string++;
    } else {
        new->sign = POSITIVE;
    }

    storeType *integerArr;
    storeType *decimalArr;
    char *radixPoint = strchr(string, '.');
    //if it has a decimal point
    if (radixPoint != NULL) {
        *radixPoint = '\0';
        char *integer = string;
        char *decimal = radixPoint + 1;
        integerArr = hexStringToBigInt(integer, INTEGER_DIGITS);
        decimalArr = hexStringToBigIntReverse(decimal, DECIMAL_DIGITS);
    } else {
        //else is an integer
        integerArr = hexStringToBigInt(string, INTEGER_DIGITS);
        decimalArr = hexStringToBigIntReverse("0", DECIMAL_DIGITS);
    }

    //copy data into bigReal
    int i = 0;
    while (i < INTEGER_DIGITS) {
        new->values[INTEGER_INDEX + i] = integerArr[i];
        i++;
    }

    i = 0;
    while (i < DECIMAL_DIGITS) {
        new->values[DECIMAL_INDEX + i] = decimalArr[i];
        i++;
    }

    //making sure that negative zero and positive zero are the same
    checkForZero(new);

    free(integerArr);
    free(decimalArr);
    free(stringC);

    return new;
}



static int mostSignificantDigitIndex(long long arr[]) {
    int i = 0;
    while (i < DIGITS && arr[i] == 0) {
        i++;
    }
    return i;
}

static long long mostSignificantDigit(long long arr[]) {
    int i = 0;
    long long result = 0;
    while (i < DIGITS && result == 0) {
        result = arr[i];
        i++;
    }
    return result;
}

BigReal add(BigReal x, BigReal y) {
    BigReal result = newBigRealHex("0");

    long long resultArray[DIGITS] = { 0 };
    int carry = 0;
    int i = DIGITS - 1;
    // adds up term by term all the digits in x and y and stores it
    // in resultArray
    // negative terms are added instead if the sign is negative
    // this can result in some of the digits being negative
    // this inconsistency is sorted out later down
    while (i >= 0) {
        // add the two digits from x and y
        // and also the carry from the previous digit

        long long xValue = x->values[i];
        if (x->sign == NEGATIVE) {
            xValue *= -1;
        }

        long long yValue = y->values[i];
        if (y->sign == NEGATIVE) {
            yValue *= -1;
        }

        long long l = xValue + yValue + carry;
        carry = 0;
        if (l >= BASE) {
            // implement carry
            carry = 1;
            l -= BASE;
            assert(l < BASE);
        }
        if (l <= -BASE) {
            // implement carry

            carry = -1;
            l += BASE;
            assert(l > -BASE);
        }

        resultArray[i] = l;

        if (i == 0) {
            // assert that there is no carry on the left most digit
            // otherwise this indicates overflow
            //assert(carry == 0);
        }
        i--;
    }

    // sorts out inconsistency of negative digits

    // checks to see if the number is negative
    // by looking at the sign of the most significant
    // digit (i.e. the first non-zero digit from the left)

    // if it is negative, then the sign variable is set
    // to NEGATIVE, and all the signs of digits in the number
    // are flipped
    int sign = POSITIVE;
    if (mostSignificantDigit(resultArray) < 0) {
        sign = NEGATIVE;
        // flip all signs
        int i = 0;
        while (i < DIGITS) {
            resultArray[i] *= -1;
            i++;
        }
    }
    result->sign = sign;

    // renoves all negative digits by "borrowing" from
    // digits on the left
    // works from left to right

    i = DIGITS - 1;
    while (i >= 1) {
        if (resultArray[i] < 0) {
            resultArray[i - 1]--;
            resultArray[i] += BASE;
            assert(resultArray[i] >= 0 && resultArray[i] < BASE);
        }
        i--;
    }

    //copy the resultArray into result
    i = 0;
    while (i < DIGITS) {
        assert(resultArray[i] >= 0 && resultArray[i] < BASE);
        result->values[i] = (storeType)resultArray[i];
        i++;
    }

    checkForZero(result);

    return result;
}

BigReal subtract(BigReal x, BigReal y) {
    BigReal yCopy = copy(y);
    yCopy->sign *= -1;
    BigReal result = add(x, yCopy);
    disposeBigReal(yCopy);
    return result;
}

// splits an array arr[] of length n into two arrays
// a[] of length n/2
// b[] of length n/2
static void split(storeType arr[], int n, storeType a[], storeType b[]) {
    int i = 0;
    int j = 0;
    while (i < n / 2) {
        a[j] = arr[i];
        j++;
        i++;
    }

    j = 0;
    while (i < n) {
        b[j] = arr[i];
        j++;
        i++;
    }
}


// Karatsuba algorithm:
// 
// x and y are two numbers with n digits in base B = 2^32
//
// 1. split up x and y into two smaller numbers each with n/2 digits
//    x -> a, b
//    y -> c, d
// 2. compute a * c recursively
// 3. compute b * d recursively
// 4. compute (a+b) * (c+d) by evaluating the sum first
//    and then multiplying out recursively (i.e. not expanding)
// 5. compute (ad + bc) = (a+b) * (c+d) - (a * c) - (b * d)
// 6. result = B^n * ac + B^(n/2) * (ad + bc) + bd
//
// Note the base case when x and y have 1 digit, then just multiply
// them out normally
//
// https://en.wikipedia.org/wiki/Karatsuba_algorithm
// https://www.youtube.com/watch?v=OtzDFLnIREc
//
// Has an asymptotic complexity of O(n^(log_2(3)) ~ O(n^1.585)
// In comparison, the naive schoolbook approach has
// a complexity of O(n^2)
static storeType *multiplyKaratsuba(storeType x[], storeType y[],
    int n, int recursionDepth) {
    // when multiplying two n-digit numbers, the maximum number
    // of digits in the result has 2n digits
    // however, at the top recursion level, it is limited to
    // the maximum amount of digits available

    int resultDigits = 0;
    resultDigits = 2 * n;
    storeType *result = malloc(sizeof(result) * resultDigits);

    if (n == 1) {
        // base case

        long long resultL = x[0] * y[0];
        result[1] = (storeType)(resultL % BASE);
        result[0] = (storeType)(resultL / BASE);
    } else {

        // step 1

        storeType *a = malloc(sizeof(a) * n / 2);
        storeType *b = malloc(sizeof(b) * n / 2);
        storeType *c = malloc(sizeof(c) * n / 2);
        storeType *d = malloc(sizeof(d) * n / 2);

        split(x, n, a, b);
        split(y, n, c, d);

        // these all have length n/2

        BigReal brA = newBigRealFromArr(a, n / 2);
        BigReal brB = newBigRealFromArr(b, n / 2);
        BigReal brC = newBigRealFromArr(c, n / 2);
        BigReal brD = newBigRealFromArr(d, n / 2);

        // step 2
        BigReal ac = newBigRealFromArr(
            multiplyKaratsuba(a, c, n / 2, recursionDepth + 1), n);

        // step 3
        BigReal bd = newBigRealFromArr(
            multiplyKaratsuba(b, d, n / 2, recursionDepth + 1), n);

        //step 4

        BigReal aPlusB = add(brA, brB);
        BigReal cPlusD = add(brC, brD);

        // recursive step

        BigReal product = newBigRealFromArr(multiplyKaratsuba(
            aPlusB->values + DIGITS - n / 2, cPlusD->values + DIGITS - n / 2, n / 2, recursionDepth + 1), n);

        // step 5
        BigReal productMinusAc = subtract(product, ac);
        BigReal adPlusBc = subtract(productMinusAc, bd);

        // step 6

        // B^n * ac

        // shifts ac n digits to the left


        BigReal sum1 = newBigRealHex("0");
        sum1->sign = POSITIVE;

        // last element
        int acIndex = n - 1;

        // last element shifted to the left by n
        int sumIndex = DIGITS - 1 - n;

        while (acIndex >= 0 && sumIndex >= 0) {

            assert(sumIndex >= 0 && sumIndex < DIGITS);
            assert(acIndex >= 0 && acIndex < n);

            sum1->values[sumIndex] = ac->values[acIndex];

            sumIndex--;
            acIndex--;
        }

        // B^(n/2) * (ad + bc)
        BigReal sum2 = newBigRealHex("0");
        sum2->sign = POSITIVE;


        // last index
        int adPlusBcIndex = n - 1;

        // last index shifted to the left by n/2
        sumIndex = DIGITS - n / 2 - 1;

        while (adPlusBcIndex >= 0 && sumIndex >= 0) {

            assert(sumIndex >= 0 && sumIndex < DIGITS);
            assert(adPlusBcIndex >= 0 && adPlusBcIndex < n);

            sum2->values[sumIndex] = adPlusBc->values[adPlusBcIndex];

            adPlusBcIndex--;
            sumIndex--;
        }

        // bd is calculated already

        // finally, to add them all together

        BigReal resultBr = add(add(sum1, sum2), bd);
        int i = 0;
        while (i < resultDigits) {
            result[i] = resultBr->values[i];
            i++;
        }

        // dispose everything
        disposeBigReal(ac);
        disposeBigReal(bd);
        disposeBigReal(brA);
        disposeBigReal(brB);
        disposeBigReal(brC);
        disposeBigReal(brD);
        disposeBigReal(aPlusB);
        disposeBigReal(cPlusD);
        disposeBigReal(product);
        disposeBigReal(productMinusAc);
        disposeBigReal(adPlusBc);
        disposeBigReal(sum1);
        disposeBigReal(sum2);
        disposeBigReal(resultBr);

        free(a);
        free(b);
        free(c);
        free(d);
    }

    return result;
}

// karatsuba multiplication - not working - TODO
/*
int xSign = x->sign;
int ySign = y->sign;
x->sign = POSITIVE;
y->sign = POSITIVE;
storeType *resultArr = multiplyKaratsuba(x->values, y->values,
SIZE, 0);


BigReal result = newBigRealFromArr(resultArr , SIZE);
result->sign = xSign * ySign;
x->sign = xSign;
y->sign = ySign;
return result;
*/

static BigReal multiplyS(BigReal x, BigReal y, int safe) {

    // schoolbook method O(n^2)

    BigReal result = newBigRealHex("0");
    int i = DIGITS - 1;
    while (i >= 0) {

        BigReal partialSum = newBigRealHex("0");

        storeType xDigit = x->values[i];

        int j = DIGITS - 1;
        while (j >= 0) {

            // multiply y by the current x digit
            storeType yDigit = y->values[j];
            unsigned long long digitFull = (unsigned long long)xDigit * (unsigned long long)yDigit;

            storeType digit = (storeType)(digitFull % BASE);
            storeType carry = (storeType)(digitFull / BASE);

            BigReal currentSum = newBigRealHex("0");
            currentSum->sign = POSITIVE;
            currentSum->values[j] = digit;
            if (j == 0) {
                assert(carry == 0);
            } else {
                currentSum->values[j - 1] = carry;
            }

            BigReal temp = add(partialSum, currentSum);
            disposeBigReal(partialSum);
            partialSum = temp;

            disposeBigReal(currentSum);

            j--;
        }

        int bitShift = DIGITS - 1 - i - DECIMAL_DIGITS;

        BigReal temp;
        if (bitShift < 0) {
            // don't worry about underflow ==> rounds down to zero
            temp = bitShiftLeftS(partialSum, bitShift, FALSE);
        } else {
            // assert failure on overflow
            temp = bitShiftLeftS(partialSum, bitShift, safe);
        }
        disposeBigReal(partialSum);
        partialSum = temp;

        temp = add(result, partialSum);
        disposeBigReal(result);
        result = temp;

        disposeBigReal(partialSum);

        i--;
    }

    result->sign = x->sign * y->sign;

    return result;
}

BigReal multiply(BigReal x, BigReal y) {
    //return multiplyS(x, y, TRUE);
    // temporary
    return multiplyS(x, y, FALSE);
}

BigReal divide(BigReal x, BigReal y) {
    BigReal yReciprocal = reciprocal(y);
    BigReal result = multiply(x, yReciprocal);
    disposeBigReal(yReciprocal);
    result->sign = x->sign * y->sign;
    return result;
}

BigReal modulus(BigReal x, BigReal y) {
    BigReal quotient = divide(x, y);
    BigReal temp = round1(quotient, 0);
    disposeBigReal(quotient);
    quotient = temp;
    BigReal floorQuotient = floor1(quotient);
    BigReal z = multiply(floorQuotient, y);
    BigReal result = subtract(x, z);
    disposeBigReal(quotient);
    disposeBigReal(floorQuotient);
    disposeBigReal(z);
    return result;
}


// sign is ignored
static int *splitToHexArr(BigReal x) {
    int *arr = malloc(sizeof(arr) * DIGITS * HEX_DIGITS_PER_DIGIT);
    int index = DIGITS * HEX_DIGITS_PER_DIGIT - 1;
    int i = DIGITS - 1;
    while (i >= 0) {
        storeType digit = x->values[i];
        int j = 0;
        while (j < HEX_DIGITS_PER_DIGIT) {
            int hex = digit % 16;
            digit >>= 4;
            arr[index] = hex;
            index--;
            j++;
        }
        i--;
    }
    return arr;
}

// arr must be of length HEX_DIGITS_PER_DIGIT * DIGITS
// arr contents must only be between 0 and 15
// only works with positive numbers
static BigReal newBigRealFromHexArr(int arr[]) {
    int length = HEX_DIGITS_PER_DIGIT * DIGITS;
    BigReal new = newBigRealHex("0");
    new->sign = POSITIVE;
    int index = length - 1;
    int i = DIGITS - 1;
    while (i >= 0) {
        int j = 0;
        storeType digit = 0;
        while (j < HEX_DIGITS_PER_DIGIT) {
            assert(arr[index] >= 0 && arr[index] <= 15);
            digit += arr[index] * (1 << (4 * j));
            j++;
            index--;
        }

        new->values[i] = digit;
        i--;
    }
    return new;
}

static storeType searchMaximumN(BigReal divisor, BigReal dividend) {
    int p = 30;
    storeType currentSearch = 1 << 31;
    while (p >= 0) {

        BigReal brCurrent = newBigRealHex("0");
        brCurrent->sign = POSITIVE;
        brCurrent->values[RADIX_POINT - 1] = currentSearch;

        BigReal multiple = multiply(divisor, brCurrent);
        if (isGreater(multiple, dividend)) {
            currentSearch -= 1 << p;
        } else if (isEqual(multiple, dividend)) {
            p = 0;
        } else {
            currentSearch += 1 << p;
        }

        disposeBigReal(brCurrent);
        disposeBigReal(multiple);

        p--;
    }

    BigReal brFinal = newBigRealHex("0");
    brFinal->sign = POSITIVE;
    brFinal->values[RADIX_POINT - 1] = currentSearch;
    BigReal multiple = multiply(divisor, brFinal);
    if (isGreater(multiple, dividend)) {
        currentSearch--;
    }
    disposeBigReal(brFinal);
    disposeBigReal(multiple);

    return currentSearch;
}

BigReal reciprocal(BigReal originalDivisor) {

    BigReal result = newBigRealHex("0");
    result->sign = originalDivisor->sign;

    // initialise dividend to 1 since we are taking the reciprocal
    BigReal dividend = newBigRealHex("1");

    // make a copy of the divisor, as we will be performing
    // operations to it
    BigReal divisor = copy(originalDivisor);
    divisor->sign = POSITIVE;

    // stores the index of where the next digit in the result
    // is to be placed
    int digitPointer = RADIX_POINT - 1;

    // we need the divisor to be an integer
    // so we keep shifting the divisor leftwards until it is an integer
    // or until we can't shift anymore
    // (this will result in rounding errors)
    // for each divisor shift leftwards, the digitPointer shifts
    // to the left

    while (!isInteger(divisor) && divisor->values[0] == 0) {
        // shift the divisor leftwards
        BigReal temp = bitShiftLeft(divisor, 1);
        disposeBigReal(divisor);
        divisor = temp;

        // shift the digit pointer left
        digitPointer--;
    }

    // find the largest multiple of the divisor, n * divisor that will
    // go into the dividend 
    // i.e. maximum of n such that n * divisor <= dividend
    // result[digitPointer] = n
    // dividend -= n * divisor
    // add a 0 to the end of the dividend i.e. shifting to the left
    // move the digitPointer one digit to the right

    while (digitPointer < DIGITS) {

        // perform a binary search to find the maximum value of n
        storeType n = searchMaximumN(divisor, dividend);
        result->values[digitPointer] = n;

        // calculate the multiple = n * divisor
        BigReal brN = newBigRealHex("0");
        brN->values[RADIX_POINT - 1] = n;
        if (n != 0) {
            brN->sign = POSITIVE;
        }
        BigReal multiple = multiply(brN, divisor);
        disposeBigReal(brN);

        // update the result
        result->values[digitPointer] = n;

        // dividend -= n * divisor;
        BigReal temp = subtract(dividend, multiple);
        disposeBigReal(dividend);
        dividend = temp;

        disposeBigReal(multiple);

        /*
        // add a 0 to the end of the dividend i.e. shifting to the left
        temp = bitShiftLeft(dividend, 1);
        disposeBigReal(dividend);
        dividend = temp;
        */

        // shift the divisor to the right
        temp = bitShiftRight(divisor, 1);
        disposeBigReal(divisor);
        divisor = temp;

        digitPointer++;
    }

    disposeBigReal(dividend);
    disposeBigReal(divisor);

    return result;
}

// x must be a non-negative integer
BigReal factorial(BigReal x) {
    assert(isInteger(x) == TRUE);
    BigReal zero = newBigRealHex("0");
    assert(isGreaterOrEqual(x, zero));
    disposeBigReal(zero);

    BigReal result = newBigRealHex("1");

    BigReal i = newBigRealHex("2");
    BigReal one = newBigRealHex("1");
    while (isLessOrEqual(i, x)) {

        BigReal temp;

        temp = multiply(result, i);
        disposeBigReal(result);
        result = temp;

        temp = add(i, one);
        disposeBigReal(i);
        i = temp;

        // char *str = toHexString(i);
         //printf("Factorial progress: %s\n", str);
        // free(str);
    }
    disposeBigReal(i);
    disposeBigReal(one);

    return result;
}

BigReal nPr(BigReal n, BigReal r) {
    assert(isInteger(n) == TRUE);
    assert(isInteger(r) == TRUE);
    BigReal zero = newBigRealHex("0");
    assert(isGreaterOrEqual(r, zero));
    disposeBigReal(zero);
    assert(isGreaterOrEqual(n, r));

    BigReal one = newBigRealHex("1");
    BigReal result = newBigRealHex("1");
    BigReal i = copy(n);
    while (isGreaterOrEqual(i, r)) {
        BigReal temp = multiply(result, i);
        disposeBigReal(result);
        result = temp;

        temp = subtract(i, one);
        disposeBigReal(i);
        i = temp;
    }

    disposeBigReal(one);
    disposeBigReal(i);

    return result;
}

BigReal nCr(BigReal n, BigReal r) {
    BigReal npr = nPr(n, r);
    BigReal rFactorial = factorial(r);
    BigReal result = divide(npr, rFactorial);
    disposeBigReal(npr);
    disposeBigReal(rFactorial);
    return result;
}

int isEqual(BigReal x, BigReal y) {
    int i = 0;
    int equal = TRUE;

    //if the signs are different, then they are not equal
    equal = (x->sign == y->sign);

    //tests if every single digit is equal
    while (i < DIGITS && equal == TRUE) {
        equal = (x->values[i] == y->values[i]);
        i++;
    }
    return equal;
}

int isLessOrEqual(BigReal x, BigReal y) {
    int result = TRUE;

    if (x->sign > y->sign) {
        //if the sign of x is "higher" than y, 
        // then x is greater than y
        result = FALSE;
    } else if (x->sign == POSITIVE && y->sign == POSITIVE) {
        // if both signs are positive

        int flag = TRUE;
        // from right to left perform digit wise comparison
        int i = 0;
        while (i < DIGITS && flag == TRUE) {
            // error checking
            assert(x->values[i] >= 0 && y->values[i] >= 0);

            if (x->values[i] > y->values[i]) {
                // if the x's digit is higher than y's
                // then x is definitely greater than y
                result = FALSE;
                flag = FALSE;
            } else if (x->values[i] < y->values[i]) {
                // if the x's digit is lower than y's
                // then x is definitely less than y
                result = TRUE;
                flag = FALSE;
            }
            // else if both digits are equal, then move onto next digit
            i++;
        }
    } else if (x->sign == NEGATIVE && y->sign == NEGATIVE) {
        // if both signs are negative

        int flag = TRUE;
        // from right to left perform digit wise comparison
        int i = 0;
        while (i < DIGITS && flag == TRUE) {
            // error checking
            assert(x->values[i] >= 0 && y->values[i] >= 0);

            if (x->values[i] < y->values[i]) {
                // if the x's digit is lower (in magnitude) than y's
                // then x is definitely greater than y
                // since both x and y are negative
                result = FALSE;
                flag = FALSE;
            } else if (x->values[i] > y->values[i]) {
                // if the x's digit is higher (in magnitude) than y's
                // then x is definitely less than y
                // since both x and y are negative
                result = TRUE;
                flag = FALSE;
            }
            // else if both digits are equal, then move onto next digit
            i++;
        }
    } else {
        // the remaining case is that the sign of x is "lower" than y
        // in this case x is definitely less than y
        // the result is set to TRUE by default, so we don't have
        // to do anything here
    }
    return result;
}

int isLess(BigReal x, BigReal y) {
    int result = FALSE;
    if (isEqual(x, y)) {
        // if x and y are equal, then x is not less than y
        // the result is set to FALSE by default, so we don't have
        // to do anything here
    } else {
        // the condition that both x and y are equal is excluded
        // by the if statement above
        result = isLessOrEqual(x, y);
    }
    return result;
}

int isGreater(BigReal x, BigReal y) {
    return !(isLessOrEqual(x, y));
}

int isGreaterOrEqual(BigReal x, BigReal y) {
    return !(isLess(x, y));
}

BigReal floor1(BigReal x) {
    BigReal result = copy(x);
    int i = RADIX_POINT;
    while (i < DIGITS) {
        result->values[i] = 0;
        i++;
    }
    BigReal zero = newBigRealHex("0");
    BigReal one = newBigRealHex("1");
    if (isLess(x, zero) && isInteger(x) == FALSE) {
        BigReal temp = subtract(result, one);
        disposeBigReal(result);
        result = temp;
    }

    disposeBigReal(one);
    disposeBigReal(zero);
    return result;
}
BigReal ceil1(BigReal x) {
    BigReal result = floor1(x);
    if (isInteger(x) == FALSE) {
        BigReal one = newBigRealHex("1");
        BigReal temp = add(result, one);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(one);
    }
    return result;
}

// rounds x to the specified number of "decimal" places
BigReal round1(BigReal x, unsigned int decimalPlaces) {
    BigReal result = copy(x);
    assert(decimalPlaces <= DECIMAL_DIGITS);
    if (decimalPlaces < DECIMAL_DIGITS) {
        result->sign = POSITIVE;
        storeType digit = x->values[RADIX_POINT + decimalPlaces];
        if (digit >= 1U << 31) {
            // round up-
            BigReal y = newBigRealHex("0");
            y->values[RADIX_POINT + decimalPlaces - 1] = 1;
            y->sign = POSITIVE;
            BigReal temp = add(result, y);
            disposeBigReal(result);
            result = temp;
            disposeBigReal(y);
        }

        int i = RADIX_POINT + decimalPlaces;
        while (i < DIGITS) {
            result->values[i] = 0;
            i++;
        }
        result->sign = x->sign;
    }
    return result;
}

// shifts all the values of the array by specified amount
// positive shift to the left, negative to right
// numbers which overflow in the shift are ignored
// 0 for new numbers being shifted in
static int *shiftArr(int *arr, int length, int shift) {
    int *result = malloc(sizeof(int) * length);
    //set everything to 0
    int i = 0;
    while (i < length) {
        result[i] = 0;
        i++;
    }

    int originalIndex = 0;
    while (originalIndex < length) {
        int newIndex = originalIndex - shift;
        if (newIndex >= 0 && newIndex < length) {
            result[newIndex] = arr[originalIndex];
        }
        originalIndex++;
    }

    return result;
}

BigReal bitShiftLeftS(BigReal x, int bits, int safe) {
    BigReal result;
    if (bits < 0) {
        result = bitShiftRightS(x, -bits, safe);
    } else {
        result = newBigRealHex("0");
        int i = DIGITS - 1;
        while (i >= 0) {
            int j = i - bits;
            if (j < 0) {
                if (safe == TRUE) {
                    // make sure we are not losing any data when
                    // shifting
                    //assert(x->values[i] == 0);
                }
            } else {
                result->values[j] = x->values[i];
            }
            i--;
        }
    }
    result->sign = x->sign;
    return result;
}

BigReal bitShiftLeft(BigReal x, int bits) {
    return bitShiftLeftS(x, bits, TRUE);
}

static BigReal bitShiftRightS(BigReal x, int bits, int safe) {
    BigReal result;
    if (bits < 0) {
        result = bitShiftLeftS(x, -bits, safe);
    } else {
        result = newBigRealHex("0");
        int i = 0;
        while (i < DIGITS) {
            int j = i + bits;
            if (j >= DIGITS) {
                if (safe == TRUE) {
                    // mae sure we are not losing any data when
                    // shifting
                    //assert(x->values[i] == 0);

                }
            } else {
                result->values[j] = x->values[i];
            }
            i++;
        }
    }
    result->sign = x->sign;
    return result;
}

BigReal bitShiftRight(BigReal x, int bits) {
    return bitShiftRightS(x, bits, TRUE);
}

// Maclaurin series
// sin(x) = x - (x^3)/3! + (x^5)/5! - (x^7)/7! + ...
BigReal sin1(BigReal x) {
    BigReal result = newBigRealHex("0");

    int sign = POSITIVE;
    BigReal n = newBigRealHex("1");
    BigReal two = newBigRealHex("2");
    BigReal zero = newBigRealHex("0");
    int flag = TRUE;
    while (flag == TRUE) {

        BigReal xPowN = powI(x, n);
        BigReal nFactorial = factorial(n);
        BigReal term = divide(xPowN, nFactorial);
        term->sign = sign;

        checkForZero(term);
        if (isEqual(term, zero)) {
            // stop the loop if the terms have rounded down to zero
            flag = FALSE;
        }
        if (xPowN->values[0] > 0) {
            // stop the loop if the numerator is growing very large
            flag = FALSE;
        }
        if (nFactorial->values[0] > 0) {
            // stop the loop if the denominator is growing very large
            flag = FALSE;
        }

        //printf("%s\t%s\t%s\n", toHexString(xPowN), toHexString(nFactorial), toHexString(term));

        disposeBigReal(xPowN);
        disposeBigReal(nFactorial);

        // add the term to the result
        BigReal temp = add(result, term);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(term);

        // increment n by 2
        temp = add(n, two);
        disposeBigReal(n);
        n = temp;

        // flip the sign
        sign *= -1;
    }
    disposeBigReal(two);
    disposeBigReal(n);

    return result;
}

// Maclaurin series
// cos(x) = 1 - (x^2)/2! + (x^4)/4! - (x^6)/6! + ...
BigReal cos1(BigReal x) {
    BigReal result = newBigRealHex("0");

    int sign = POSITIVE;
    BigReal n = newBigRealHex("0");
    BigReal two = newBigRealHex("2");
    BigReal zero = newBigRealHex("0");
    int flag = TRUE;
    while (flag == TRUE) {

        BigReal xPowN = powI(x, n);
        BigReal nFactorial = factorial(n);
        BigReal term = divide(xPowN, nFactorial);
        term->sign = sign;

        checkForZero(term);
        if (isEqual(term, zero)) {
            // stop the loop if the terms have rounded down to zero
            flag = FALSE;
        }
        if (xPowN->values[0] > 0) {
            // stop the loop if the numerator is growing very large
            flag = FALSE;
        }
        if (nFactorial->values[0] > 0) {
            // stop the loop if the denominator is growing very large
            flag = FALSE;
        }

        //printf("%s\t%s\t%s\n", toHexString(xPowN), toHexString(nFactorial), toHexString(term));

        disposeBigReal(xPowN);
        disposeBigReal(nFactorial);

        // add the term to the result
        BigReal temp = add(result, term);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(term);

        // increment n by 2
        temp = add(n, two);
        disposeBigReal(n);
        n = temp;

        // flip the sign
        sign *= -1;
    }
    disposeBigReal(two);
    disposeBigReal(n);

    return result;
}

// tan(x) = sin(x) / cos(x)
BigReal tan1(BigReal x) {
    BigReal sinX = sin1(x);
    BigReal cosX = cos1(x);
    BigReal result = divide(sinX, cosX);
    disposeBigReal(sinX);
    disposeBigReal(cosX);
    return result;
}

BigReal csc(BigReal x) {
    BigReal sinX = sin1(x);
    BigReal result = reciprocal(sinX);
    disposeBigReal(sinX);
    return result;
}
BigReal sec(BigReal x) {
    BigReal cosX = cos1(x);
    BigReal result = reciprocal(cosX);
    disposeBigReal(cosX);
    return result;
}
BigReal cot(BigReal x) {
    BigReal tanX = tan1(x);
    BigReal result = reciprocal(tanX);
    disposeBigReal(tanX);
    return result;
}

// sinh(x) = x + (x^3)/3! + (x^5)/5! + ...
BigReal sinh1(BigReal x) {
    BigReal result = newBigRealHex("0");

    BigReal n = newBigRealHex("1");
    BigReal two = newBigRealHex("2");
    BigReal zero = newBigRealHex("0");
    int flag = TRUE;
    while (flag == TRUE) {

        BigReal xPowN = powI(x, n);
        BigReal nFactorial = factorial(n);
        BigReal term = divide(xPowN, nFactorial);

        checkForZero(term);
        if (isEqual(term, zero)) {
            // stop the loop if the terms have rounded down to zero
            flag = FALSE;
        }
        if (xPowN->values[0] > 0) {
            // stop the loop if the numerator is growing very large
            flag = FALSE;
        }
        if (nFactorial->values[0] > 0) {
            // stop the loop if the denominator is growing very large
            flag = FALSE;
        }

        //printf("%s\t%s\t%s\n", toHexString(xPowN), toHexString(nFactorial), toHexString(term));

        disposeBigReal(xPowN);
        disposeBigReal(nFactorial);

        // add the term to the result
        BigReal temp = add(result, term);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(term);

        // increment n by 2
        temp = add(n, two);
        disposeBigReal(n);
        n = temp;

    }
    disposeBigReal(two);
    disposeBigReal(n);

    return result;
}

// cosh(x) = 1 + (x^2)/2! + (x^4)/4! + ...
BigReal cosh1(BigReal x) {
    BigReal result = newBigRealHex("0");

    BigReal n = newBigRealHex("0");
    BigReal two = newBigRealHex("2");
    BigReal zero = newBigRealHex("0");
    int flag = TRUE;
    while (flag == TRUE) {

        BigReal xPowN = powI(x, n);
        BigReal nFactorial = factorial(n);
        BigReal term = divide(xPowN, nFactorial);

        checkForZero(term);
        if (isEqual(term, zero)) {
            // stop the loop if the terms have rounded down to zero
            flag = FALSE;
        }
        if (xPowN->values[0] > 0) {
            // stop the loop if the numerator is growing very large
            flag = FALSE;
        }
        if (nFactorial->values[0] > 0) {
            // stop the loop if the denominator is growing very large
            flag = FALSE;
        }

        //printf("%s\t%s\t%s\n", toHexString(xPowN), toHexString(nFactorial), toHexString(term));

        disposeBigReal(xPowN);
        disposeBigReal(nFactorial);

        // add the term to the result
        BigReal temp = add(result, term);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(term);

        // increment n by 2
        temp = add(n, two);
        disposeBigReal(n);
        n = temp;

    }
    disposeBigReal(two);
    disposeBigReal(n);

    return result;
}

// tanh(x) = sinh(x) / cosh(x)
BigReal tanh1(BigReal x) {
    BigReal sinhX = sinh1(x);
    BigReal coshX = cosh1(x);
    BigReal result = divide(sinhX, coshX);
    disposeBigReal(sinhX);
    disposeBigReal(coshX);
    return result;
}

// e^x = 1 + x + (x^2)/2! + (x^3)/3! + (x^4)/4! + ...
BigReal exp1(BigReal x) {
    BigReal result = newBigRealHex("0");

    BigReal n = newBigRealHex("0");
    BigReal one = newBigRealHex("1");
    BigReal zero = newBigRealHex("0");
    int flag = TRUE;
    while (flag == TRUE) {

        BigReal xPowN = powI(x, n);
        BigReal nFactorial = factorial(n);
        BigReal term = divide(xPowN, nFactorial);

        checkForZero(term);
        if (isEqual(term, zero)) {
            // stop the loop if the terms have rounded down to zero
            flag = FALSE;
        }
        if (xPowN->values[0] > 0) {
            // stop the loop if the numerator is growing very large
            flag = FALSE;
        }
        if (nFactorial->values[0] > 0) {
            // stop the loop if the denominator is growing very large
            flag = FALSE;
        }


        disposeBigReal(xPowN);
        disposeBigReal(nFactorial);

        // add the term to the result
        BigReal temp = add(result, term);
        disposeBigReal(result);
        result = temp;
        disposeBigReal(term);

        // increment n by 1
        temp = add(n, one);
        disposeBigReal(n);
        n = temp;

    }
    disposeBigReal(one);
    disposeBigReal(n);

    return result;
}


static char intToHexChar(int x) {
    assert(x >= 0 && x <= 15);
    char c = '\0';
    if (x >= 0 && x <= 9) {
        c = '0' + x;
    } else {
        c = 'A' + (x - 10);
    }
    return c;
}

// returns a char pointer of length 9
static char *uintToHexString(unsigned int x) {
    int bits = 8 * sizeof(x);
    int hexSize = bits / 4;
    char *string = malloc(hexSize + 1);
    int i = hexSize - 1;
    while (i >= 0) {
        string[i] = intToHexChar(x % 16);
        x >>= 4;
        i--;
    }
    string[hexSize] = '\0';
    return string;
}

char *toDecString(BigReal xOld) {
    // BASE = 2^32 ~ 4 * 10^9 < 10^10
    // so for each 2^32 digit, we reserve ten chars

    // for integer part plus null terminator
    int integerLength = 1 + 10 * INTEGER_DIGITS + 1;

    // for decimal part plus null terminator
    int decimalLength = 10 * DECIMAL_DIGITS + 1;

    // length of the full string =
    // length of integer part (minus null terminator)
    // + radix point
    // + length of decimal part (minus null terminator)
    // + null terminator
    int fullLength = (integerLength - 1) + 1 + (decimalLength - 1) + 1;

    BigReal ten = newBigRealHex("A");

    // integer part
    char *integerString = malloc(sizeof(char) * integerLength);
    int i = 0;
    while (i < integerLength - 1) {
        integerString[i] = '0';
        i++;
    }
    integerString[integerLength - 1] = '\0';

    BigReal x = copy(xOld);
    x->sign = POSITIVE;


    // x % 10^n
    i = integerLength - 2;

    while (i >= 0) {

        // x % 10
        BigReal mod = modulus(x, ten);
        storeType digit = mod->values[RADIX_POINT - 1];
        assert(digit >= 0 && digit <= 9);
        integerString[i] = '0' + digit;


        // x /= 10
        BigReal temp = divide(x, ten);
        disposeBigReal(x);
        x = round1(temp, 0);
        disposeBigReal(temp);
        i--;
    }

    disposeBigReal(ten);

    // decimal part
    char *decimalString = malloc(sizeof(char) * decimalLength);
    decimalString[0] = 'H';
    decimalString[1] = '\0';

    char *result = malloc(sizeof(char) * fullLength);
    result[0] = '\0';
    if (xOld->sign == NEGATIVE) {
        strcat(result, "-");
    }
    strcat(result, integerString);
    strcat(result, ".");
    strcat(result, decimalString);

    free(integerString);
    free(decimalString);

    return result;


}

static char *trimLeadingZeroes(char *str) {
    char *result = malloc(strlen(str) + 1);
    while (str[0] == '0') {
        str++;
    }
    strcpy(result, str);
    return result;
}

static char *trimTrailingZeroes(char *str) {
    int end = strlen(str);
    char *result = malloc(end + 1);
    strcpy(result, str);
    end--;
    while (result[end] == '0') {
        result[end] = '\0';
        end--;
    }
    return result;
}

char *toHexString(BigReal x) {

    // calculating memory needed for integer part of the string
    // integer part + null terminator (negative sign ignored here)
    int integerSize = INTEGER_DIGITS * HEX_DIGITS_PER_DIGIT + 1;

    // calculating memory needed for decimal part of the string
    // decimalp part + null terminator
    int decimalSize = DECIMAL_DIGITS * HEX_DIGITS_PER_DIGIT + 1;

    // calculating memory needed for the full string
    // negative sign + integerSize - null terminator +
    // radix point + decimalSize
    int size = integerSize - 1 + 1 + decimalSize;

    char *integerString = malloc(integerSize);
    integerString[0] = '\0';
    int i = 0;
    // concatenate all the integer digits of the number
    while (i < INTEGER_DIGITS) {
        char *integer = uintToHexString(x->values[INTEGER_INDEX + i]);
        strcat(integerString, integer);
        free(integer);
        i++;
    }

    char *decimalString = malloc(decimalSize);
    decimalString[0] = '\0';
    i = 0;
    // concatenate all the decimal digits of the number
    while (i < DECIMAL_DIGITS) {
        char *decimal = uintToHexString(x->values[DECIMAL_INDEX + i]);
        strcat(decimalString, decimal);
        free(decimal);
        i++;
    }


    char *trimmedIntegerString = trimLeadingZeroes(integerString);
    if (trimmedIntegerString[0] == '\0') {
        trimmedIntegerString[0] = '0';
        trimmedIntegerString[1] = '\0';
    }
    char *trimmedDecimalString = trimTrailingZeroes(decimalString);

    char *fullString = malloc(size);
    fullString[0] = '\0';
    if (x->sign == NEGATIVE) {
        strcat(fullString, "-");
    }
    strcat(fullString, trimmedIntegerString);
    if (trimmedDecimalString[0] != '\0') {
        strcat(fullString, ".");
        strcat(fullString, trimmedDecimalString);
    }

    free(trimmedIntegerString);
    free(trimmedDecimalString);
    free(integerString);
    free(decimalString);

    return fullString;
}

int isInteger(BigReal x) {
    int result = TRUE;
    int i = RADIX_POINT;
    while (i < DIGITS && result == TRUE) {
        result = (x->values[i] == 0);
        i++;
    }
    return result;
}

BigReal copy(BigReal x) {
    BigReal result = malloc(sizeof(bigReal));
    result->sign = x->sign;
    int i = 0;
    while (i < DIGITS) {
        result->values[i] = x->values[i];
        i++;
    }
    return result;
}