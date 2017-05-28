#include <stdio.h>
#include <stdlib.h>

#include "testBigReal.h"

int main(int argc, char *argv[]) {

    //  testAll();

  /*  BigReal x = factorial(newBigRealDec("6"));
    printf("%s\n", toString(x));


    BigReal i = newBigRealDec("1");
    while (isLess(i, newBigRealDec("20"))) {
        printf("%s\n", toString(factorial(i)));
        i = add(i, newBigRealDec("1"));

    }*/

    // printf("%s\n", toString(newBigRealDec("3.1415926535897932384626")));
   //  newBigRealDec("3.1415926535897932384626");

     //  BigReal x = add(newBigRealHex("1.00000002D8BC02A996262B7A8F7023B5"), newBigRealHex("-0.00000002DF5B73E2AAE8722A341D78C"));
     //  printf("%s\n", x);

    // BigReal x = add(newBigRealHex("-0.F9DC977B6350D39A70E8EEC2661AF83B"), newBigRealHex("-0.069B47CA8812A2EA69EDCDD89208A37B"));
    // printf("%s\n", toString(x));

     //printf("%s\n", toString(sin1(divide(newBigRealHex("3  .243F6A8885A308D313198A2E03707"), newBigRealHex("2")))));
   //  printf("%s\n", toString(sin1(newBigRealHex("3.243F6A8885A308D313198A2E03707"))));
   //  printf("%s\n", toString(sin1(newBigRealHex("1"))));

     //printf("%s\n", toString(sin1(divide(newBigRealDec("3.1415926535897932384626"), newBigRealDec("2")))));

    //sin1(newBigRealDec("3.1415926535897932384626"));

    printf("%s\n", toHexString(round1(sin1(newBigRealDec("3.1415926535897932384626")), DECIMAL_DIGITS-1)));
    printf("%s\n", toHexString(round1(sin1(newBigRealDec("1.5707963267948966192313")), DECIMAL_DIGITS - 1)));
    BigReal s1 = round1(sin1(newBigRealDec("1")), DECIMAL_DIGITS - 1);
    printf("%s\n", toHexString(s1));
    printf("%s\n", toHexString(round1(sin1(newBigRealDec("0.7853981633974483096156")), DECIMAL_DIGITS - 1)));

    printf("\n");

    printf("%s\n", toHexString(round1(cos1(newBigRealDec("3.1415926535897932384626")), DECIMAL_DIGITS - 1)));
    printf("%s\n", toHexString(round1(cos1(newBigRealDec("1.5707963267948966192313")), DECIMAL_DIGITS - 1)));
    BigReal c1 = round1(cos1(newBigRealDec("1")), DECIMAL_DIGITS - 1);
    printf("%s\n", toHexString(c1));
    printf("%s\n", toHexString(round1(cos1(newBigRealDec("0.7853981633974483096156")), DECIMAL_DIGITS - 1)));

    printf("\n");
    BigReal one = round1(add(multiply(s1, s1), multiply(c1, c1)), DECIMAL_DIGITS-1);
    printf("%s\n", toHexString(one));
    // printf("%s\n", toString(cos1(divide(newBigRealHex("3.243F6A8885A308D313198A2E03707"), newBigRealHex("2")))));
    // printf("%s\n", toString(cos1(newBigRealHex("3.243F6A8885A308D313198A2E03707"))));
   //  printf("%s\n", toString(cos1(newBigRealHex("1"))));

     /*  BigReal x = newBigRealHex("1.2");
       BigReal y = reciprocal(x);
       BigReal z = multiply(x, y);


       printf("%s\n", toString(x));
       printf("%s\n", toString(y));
       printf("%s\n", toString(z));
       disposeBigReal(x);
       disposeBigReal(y);
       disposeBigReal(z);*/
       /* x = newBigRealDec("6");
        y = reciprocal(x);
        z = multiply(x, y);
        printf("%s\n", toString(y));
        printf("%s\n", toString(z));
        disposeBigReal(x);
        disposeBigReal(y);
        disposeBigReal(z);

        x = newBigRealDec("5");
        y = reciprocal(x);
        z = multiply(x, y);
        BigReal w = round(z, DIGITS - RADIX_POINT - 1);
        printf("%s\n", toString(y));
        printf("%s\n", toString(z));
        printf("%s\n", toString(w));
        disposeBigReal(x);
        disposeBigReal(y);
        disposeBigReal(z);

        x = newBigRealHex("F");
        y = reciprocal(x);
        printf("%s\n", toString(x));
        printf("%s\n", toString(y));


        disposeBigReal(x);
        disposeBigReal(y);*/

        /* BigReal x = newBigRealDec("12");
         BigReal y = newBigRealDec("6");
         BigReal z = divide(x, y);
         printf("%s\n",toString(z));*/

         /*BigReal n = newBigRealHex("100");
         BigReal nFactorial = factorial(n);
         printf("%s\n", toString(nFactorial));
         disposeBigReal(n);
         disposeBigReal(nFactorial);*/

         // BigReal x = newBigRealHex("10");
         // BigReal y = newBigRealHex("20");
         // BigReal product = multiply(x, y);
         // char *string = toString(product);
         // printf("%s\n", string);

         //// free(string);
         // disposeBigReal(x);
         // disposeBigReal(y);
         // disposeBigReal(product);


      // BigReal x = newBigRealDec("12");
       //printf("%s\n", toString(multiply(x, x)));

       /*
       printf("%s\n", toString(add(newBigReal("1111111111.1111111111"), newBigReal("-2222222222.2222222222"))));


       BigReal pi = newBigReal("3.243F6A8885A308D313198A2E03707");
       BigReal e = newBigReal ("2.B7E151628AED2A6ABF7158809CF4F");
       printf("Pi:   %s\n", toString(pi));
       printf("E:    %s\n", toString(e));
       printf("Pi+E: %s\n", toString(add(pi, e)));

       BigReal a = newBigReal("0.123456789ABCDEFEDCBA987654321");
       BigReal b = newBigReal("0.FEDCBA98765432123456789ABCDEF");
       printf("%s\n", toString(add(a, b)));


       BigReal x = newBigReal("123456789ABCDEF.123456789ABCDEF");
       printf("%s\n", toString(x));

       BigReal y = newBigReal("1F.001234");
       printf("%s\n", toString(y));

       printf("%s\n", toString(newBigReal("FFFFFFFFFFFFFFFF")));
       */

    return EXIT_SUCCESS;
}