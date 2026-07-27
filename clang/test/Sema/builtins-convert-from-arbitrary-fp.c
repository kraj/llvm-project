// RUN: %clang_cc1 -triple x86_64-unknown-unknown -fsyntax-only -verify %s

typedef unsigned char v4u8 __attribute__((ext_vector_type(4)));
typedef float v4f32 __attribute__((ext_vector_type(4)));
typedef float v2f32 __attribute__((ext_vector_type(2)));

const char *runtime_format;

void test_format(unsigned char b) {
  (void)__builtin_convert_from_arbitrary_fp(b, runtime_format, float); // expected-error {{expression is not a string literal}}
  (void)__builtin_convert_from_arbitrary_fp(b, "", float);             // expected-error {{'' is not a supported arbitrary floating-point format}}
  (void)__builtin_convert_from_arbitrary_fp(b, "float8e5m2", float);   // expected-error {{'float8e5m2' is not a supported arbitrary floating-point format}}
  (void)__builtin_convert_from_arbitrary_fp(b, u8"Float8E5M2", float); // expected-error {{expression is not a string literal}}
}

void test_arity(unsigned char b) {
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2"); // expected-error {{expected ','}}
  (void)__builtin_convert_from_arbitrary_fp(b, float);        // expected-error {{expected expression}}
}

void test_width(unsigned short s, unsigned char b, unsigned _BitInt(4) b4) {
  (void)__builtin_convert_from_arbitrary_fp(s, "Float8E5M2", float);   // expected-error {{argument type 'unsigned short' must be an integer type 8 bits wide to match format 'Float8E5M2'}}
  (void)__builtin_convert_from_arbitrary_fp(b, "Float6E3M2FN", float); // expected-error {{argument type 'unsigned char' must be an integer type 6 bits wide to match format 'Float6E3M2FN'}}
  (void)__builtin_convert_from_arbitrary_fp(b4, "Float8E5M2", float);  // expected-error {{argument type 'unsigned _BitInt(4)' must be an integer type 8 bits wide to match format 'Float8E5M2'}}
}

void test_operand_types(unsigned char b, float f, void *p) {
  (void)__builtin_convert_from_arbitrary_fp(f, "Float8E5M2", float); // expected-error {{first argument to __builtin_convert_from_arbitrary_fp must be an integer type or a vector of integer types}}
  (void)__builtin_convert_from_arbitrary_fp(p, "Float8E5M2", float); // expected-error {{first argument to __builtin_convert_from_arbitrary_fp must be an integer type or a vector of integer types}}
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2", int);   // expected-error {{third argument to __builtin_convert_from_arbitrary_fp must be a floating-point type or a vector of floating-point types}}
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2", void);  // expected-error {{third argument to __builtin_convert_from_arbitrary_fp must be a floating-point type or a vector of floating-point types}}
}

void test_vectors(unsigned char b, v4u8 vb) {
  (void)__builtin_convert_from_arbitrary_fp(vb, "Float8E5M2", float); // expected-error {{third argument to __builtin_convert_from_arbitrary_fp must be of vector type}}
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2", v4f32);  // expected-error {{first argument to __builtin_convert_from_arbitrary_fp must be of vector type}}
  (void)__builtin_convert_from_arbitrary_fp(vb, "Float8E5M2", v2f32); // expected-error {{floating-point and integer arguments to __builtin_convert_from_arbitrary_fp must have the same number of elements}}
}

// Formats that are valid but that no target lowers yet are accepted here; the
// backend reports them.
void test_accepted(unsigned char b, unsigned _BitInt(6) b6, unsigned _BitInt(4) b4,
                   v4u8 vb) {
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E5M2FNUZ", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E4M3", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E4M3FN", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E4M3FNUZ", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E4M3B11FNUZ", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E3M4", float);
  (void)__builtin_convert_from_arbitrary_fp(b, "Float8E8M0FNU", float);
  (void)__builtin_convert_from_arbitrary_fp(b6, "Float6E3M2FN", float);
  (void)__builtin_convert_from_arbitrary_fp(b6, "Float6E2M3FN", float);
  (void)__builtin_convert_from_arbitrary_fp(b4, "Float4E2M1FN", float);
  (void)__builtin_convert_from_arbitrary_fp((signed char)b, "Float8E5M2", float);
  (void)__builtin_convert_from_arbitrary_fp(b, ("Float8E5M2"), double);
  (void)__builtin_convert_from_arbitrary_fp(vb, "Float8E5M2", v4f32);
}

_Static_assert(__has_builtin(__builtin_convert_from_arbitrary_fp), "");
