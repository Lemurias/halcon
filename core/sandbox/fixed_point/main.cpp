#include <iostream>

#include "../../src/fixed_point/ac_fixed.h"

int main(){

    /* DECLARATION OF A VARIABLE */
    ac_fixed<8,3,1> fixp_var_a;
    
    std::cout << "size of: " << sizeof(fixp_var_a) << std::endl;
    std::cout << "size of: " << sizeof(double) << std::endl;

    /* ASSIGNATION */
    fixp_var_a = -1.2;
    
    /* CONVERSION TO DOUBLE */
    double double_var_a = fixp_var_a.to_double();
    
    /* CONVERSION TO INT */
    double int_var_a = fixp_var_a.to_int();

    /* CONVERSION TO AC_INT */
    double ac_int_var_a = fixp_var_a.to_ac_int();

    /* HOW TO GET THE NUMBER OF BITS */
    int number_of_bits = fixp_var_a.GetW();
    int number_int_of_bits = fixp_var_a.GetI();
    int number_frac_of_bits = fixp_var_a.GetF();

    /* GET EQUIVALENT INT VALUE FROM BITS */
    int equivalent_bits_int = fixp_var_a.GetDecValue();

    std::cout << "fixp_var_a: " << fixp_var_a << std::endl;
    std::cout << "double_var_a: " << double_var_a << std::endl;
    std::cout << "int_var_a: " << int_var_a << std::endl;
    std::cout << "ac_int_var_a: " << ac_int_var_a << std::endl;
    std::cout << "equivalent_bits_int: " << equivalent_bits_int << std::endl;
    std::cout << "number_of_bits: " << number_of_bits << std::endl;
    std::cout << "number_int_of_bits: " << number_int_of_bits << std::endl;
    std::cout << "number_frac_of_bits: " << number_frac_of_bits << std::endl;
    
    /* OPERATIONS WITH DIFFERENT RESOLUTIONS */
    ac_fixed<8,3,1> fixp_var_b;
    fixp_var_b = pow(2, -fixp_var_a.GetF()); // min value

    ac_fixed<9,3,1> fixp_var_c;
    fixp_var_c = fixp_var_b.to_double() / 2.0;  // operations needs to be 
                                                // performed in double and then
                                                // converted to ac_fixed type

    std::cout << "fixp_var_b: " << fixp_var_b << std::endl;
    std::cout << "fixp_var_c: " << fixp_var_c << std::endl;

    /* OPERATIONS WITH DIFFERENT QUANTIZATION TYPE */
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Different quantization types:" << std::endl;
    /* POSITIVE VALUES */
    std::cout << "-----------------" << std::endl;
    std::cout << "Positive values:" << std::endl;

    ac_fixed<8,3,1,AC_TRN> fixp_var_d; // (default) truncate towards -inf.
    fixp_var_d = 1.2345678;
    std::cout << "AC_TRN: Truncate towards -inf. fixp_var_d: " << fixp_var_d << std::endl;

    ac_fixed<8,3,1,AC_TRN_ZERO> fixp_var_e; // truncate towards 0.
    fixp_var_e = 1.2345678;
    std::cout << "AC_TRN_ZERO: Truncate towards 0. fixp_var_e: " << fixp_var_e << std::endl;

    ac_fixed<8,3,1,AC_RND> fixp_var_f; // round towards +inf.
    fixp_var_f = 1.2345678;
    std::cout << "AC_RND: Round towards +inf. fixp_var_f: " << fixp_var_f << std::endl;

    ac_fixed<8,3,1,AC_RND_ZERO> fixp_var_g; // round towards 0.
    fixp_var_g = 1.2345678;
    std::cout << "AC_RND_ZERO: Round towards 0. fixp_var_g: " << fixp_var_g << std::endl;

    ac_fixed<8,3,1,AC_RND_INF> fixp_var_h; // round towards +/-inf.
    fixp_var_h = 1.2345678;
    std::cout << "AC_RND_INF: Round towards +/-inf. fixp_var_h: " << fixp_var_h << std::endl;

    ac_fixed<8,3,1,AC_RND_MIN_INF> fixp_var_i; // round towards -inf.
    fixp_var_i = 1.2345678;
    std::cout << "AC_RND_MIN_INF: Round towards -inf. fixp_var_i: " << fixp_var_i << std::endl;

    ac_fixed<8,3,1,AC_RND_CONV> fixp_var_j; // round towards even quantum multiples. quantum: LSB value (2^(I-W))
    fixp_var_j = 1.2345678;
    std::cout << "AC_RND_CONV: Round towards even quantum multiples. fixp_var_j: " << fixp_var_j << std::endl;

    ac_fixed<8,3,1,AC_RND_CONV_ODD> fixp_var_k; // round towards odd quantum multiples. quantum: LSB value (2^(I-W))
    fixp_var_k = 1.2345678;
    std::cout << "AC_RND_CONV_ODD: Round towards odd quantum multiples. fixp_var_k: " << fixp_var_k << std::endl;


    /* NEGATIVE VALUES */
    std::cout << "-----------------" << std::endl;
    std::cout << "Negative values:" << std::endl;

    fixp_var_d = -1.2367238;
    std::cout << "AC_TRN: Truncate towards -inf. fixp_var_d: " << fixp_var_d << std::endl;

    fixp_var_e = -1.2367238;
    std::cout << "AC_TRN_ZERO: Truncate towards 0. fixp_var_e: " << fixp_var_e << std::endl;

    fixp_var_f = -1.2367238;
    std::cout << "AC_RND: Round towards +inf. fixp_var_f: " << fixp_var_f << std::endl;

    fixp_var_g = -1.2367238;
    std::cout << "AC_RND_ZERO: Round towards 0. fixp_var_g: " << fixp_var_g << std::endl;

    fixp_var_h = -1.2367238;
    std::cout << "AC_RND_INF: Round towards +/-inf. fixp_var_h: " << fixp_var_h << std::endl;

    fixp_var_i = -1.2367238;
    std::cout << "AC_RND_MIN_INF: Round towards -inf. fixp_var_i: " << fixp_var_i << std::endl;

    fixp_var_j = -1.2367238;
    std::cout << "AC_RND_CONV: Round towards even quantum multiples. fixp_var_j: " << fixp_var_j << std::endl;

    fixp_var_k = -1.2367238;
    std::cout << "AC_RND_CONV_ODD: Round towards odd quantum multiples. fixp_var_k: " << fixp_var_k << std::endl;


    /* OPERATIONS WITH DIFFERENT OVERFLOW TYPE */
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Different overflow types:" << std::endl;
    /* POSITIVE VALUES */
    std::cout << "-----------------" << std::endl;
    std::cout << "Positive values:" << std::endl;

    ac_fixed<8,3,1,AC_TRN,AC_WRAP> fixp_var_l;
    fixp_var_l = 4;
    std::cout << "AC_WRAP. (default) Drop bits to the left of MSB. fixp_var_l: " << fixp_var_l << std::endl;

    ac_fixed<8,3,1,AC_TRN,AC_SAT> fixp_var_m;
    fixp_var_m = 4;
    std::cout << "AC_SAT. Saturate to closest of MIN or MAX. fixp_var_m: " << fixp_var_m << std::endl;

    ac_fixed<8,3,1,AC_TRN,AC_SAT_ZERO> fixp_var_n;
    fixp_var_n = 4;
    std::cout << "AC_SAT_ZERO:Set to 0 on overflow. fixp_var_n: " << fixp_var_n << std::endl;

    ac_fixed<8,3,1,AC_TRN,AC_SAT_SYM> fixp_var_o;
    fixp_var_o = 4;
    std::cout << "AC_SAT_SYM: For unsigned: treat as AC_SAT. For signed: on overflow or number is MIN set to closest of. fixp_var_o: " << fixp_var_o << std::endl;


    /* NEGATIVE VALUES */
    std::cout << "-----------------" << std::endl;
    std::cout << "Negative values:" << std::endl;

    fixp_var_l = -4.1;
    std::cout << "AC_WRAP. (default) Drop bits to the left of MSB. fixp_var_l: " << fixp_var_l << std::endl;

    fixp_var_m = -4.1;
    std::cout << "AC_SAT. Saturate to closest of MIN or MAX. fixp_var_m: " << fixp_var_m << std::endl;

    fixp_var_n = -4.1;
    std::cout << "AC_SAT_ZERO:Set to 0 on overflow. fixp_var_n: " << fixp_var_n << std::endl;

    fixp_var_o = -4.1;
    std::cout << "AC_SAT_SYM: For unsigned: treat as AC_SAT. For signed: on overflow or number is MIN set to closest of. fixp_var_o: " << fixp_var_o << std::endl;


}