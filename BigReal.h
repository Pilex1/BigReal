// Alex Tan
// 2017
// represents a fixed point number
// can perform calculations


// number of digits to store the whole number
// must be a power of two
// half of the digits are used to store the integer part
// other half for the decimal part
#define DIGITS 8

#define RADIX_POINT (DIGITS/2)

#define INTEGER_INDEX 0
#define DECIMAL_INDEX (RADIX_POINT)

// number of digits from the right that the radix point is located
// i.e. a radix point of 0 would mean an integer

#define INTEGER_DIGITS (DIGITS-RADIX_POINT)
#define DECIMAL_DIGITS (RADIX_POINT)

#define TRUE 1
#define FALSE 0

#define BASE (1LL<<(8*sizeof(storeType)))
#define HEX_DIGITS_PER_DIGIT (8 * sizeof(storeType) / 4)

#define POSITIVE 1
#define ZERO 0
#define NEGATIVE -1

#define HEXADECIMAL 16
#define DECIMAL 10


typedef unsigned int storeType;
typedef unsigned long long longStoreType;
typedef struct _bigReal *BigReal;

BigReal newBigRealDec(char *decimal);
BigReal newBigRealHex(char *number);
void disposeBigReal(BigReal x);
BigReal copy(BigReal x);

BigReal add(BigReal x, BigReal y);
BigReal subtract(BigReal x, BigReal y);
BigReal multiply(BigReal x, BigReal y);
BigReal divide(BigReal x, BigReal y);
BigReal modulus(BigReal x, BigReal y);

BigReal reciprocal(BigReal x);
BigReal factorial(BigReal x);
BigReal nPr(BigReal n, BigReal r);
BigReal nCr(BigReal n, BigReal r);

BigReal power(BigReal x, BigReal y);

BigReal bitShiftLeft(BigReal x, int bits);
BigReal bitShiftRight(BigReal x, int bits);

// floor, ceil, and round seem to be reserved functions,
// so I've given those functions here a suffix
BigReal floor1(BigReal x);
BigReal ceil1(BigReal x);
BigReal round1(BigReal x, unsigned int digits);

// same with these
BigReal sin1(BigReal x);
BigReal cos1(BigReal x);
BigReal tan1(BigReal x);
BigReal csc1(BigReal x);
BigReal sec1(BigReal x);
BigReal cot1(BigReal x);

BigReal sinh1(BigReal x);
BigReal cosh1(BigReal x);
BigReal tanh1(BigReal x);
BigReal csch1(BigReal x);
BigReal sech1(BigReal x);
BigReal coth1(BigReal x);

BigReal exp1(BigReal x);

int isEqual(BigReal x, BigReal y);
int isLessOrEqual(BigReal x, BigReal y);
int isLess(BigReal x, BigReal y);
int isGreater(BigReal x, BigReal y);
int isGreaterOrEqual(BigReal x, BigReal y);

int isInteger(BigReal x);

char *toHexString(BigReal x);
char *toDecString(BigReal x); // not currentl working