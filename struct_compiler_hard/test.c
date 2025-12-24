#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

struct field_info *make_field_infos(size_t n) {
  return calloc(n, sizeof(struct field_info));
}

struct field_info *make_field_info(enum type t) {
  struct field_info *info = make_field_infos(1);
  info->type = t;
  return info;
}

#define SIMPLE(t) ((struct field_info){.type = t})

#define STRUCT(...)                                                  \
  ((struct field_info){                                              \
      .type = FIELD_TYPE_STRUCT,                                     \
      .struct_info = {                                               \
          .num_fields = sizeof((struct field_info[]){__VA_ARGS__}) / \
                        sizeof(struct field_info),                   \
          .fields = (struct field_info[]){__VA_ARGS__}}})

#define UNION(...)                                                   \
  ((struct field_info){                                              \
      .type = FIELD_TYPE_UNION,                                      \
      .union_info = {                                                \
          .num_fields = sizeof((struct field_info[]){__VA_ARGS__}) / \
                        sizeof(struct field_info),                   \
          .fields = (struct field_info[]){__VA_ARGS__}}})

static inline struct field_info make_array(struct field_info elem, size_t len) {
  struct field_info *elem_ptr = make_field_infos(1);
  *elem_ptr = elem;
  struct field_info arr;
  arr.type = FIELD_TYPE_ARRAY;
  arr.array_info.element_type = elem_ptr;
  arr.array_info.length = len;
  return arr;
}

#define ARRAY(elem, len) make_array(elem, len)

TEST test1(void) {
  typedef union {
    double d;
    void *p;
  } u_t;

  struct test1 {
    char c;
    int i;
    u_t u;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_INT),
             UNION(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_POINTER)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test1), size);
  ASSERT_EQ(offsetof(struct test1, c), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test1, i), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test1, u), info.struct_info.fields[2].offset);

  for (size_t i = 0; i < info.struct_info.fields[2].union_info.num_fields;
       ++i) {
    ASSERT_EQ(0, info.struct_info.fields[2].union_info.fields[i].offset);
  }

  PASS();
}

TEST test2(void) {
  struct test2 {
    char a;
    double b;
    int c;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_DOUBLE),
             SIMPLE(FIELD_TYPE_INT));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test2), size);
  ASSERT_EQ(offsetof(struct test2, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test2, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test2, c), info.struct_info.fields[2].offset);

  PASS();
}

TEST test3(void) {
  typedef struct {
    char y;
    float z;
  } nested_t;

  struct test3 {
    int x;
    nested_t nested;
    double w;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_INT),
             STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_FLOAT)),
             SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test3), size);
  ASSERT_EQ(offsetof(struct test3, x), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test3, nested), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(nested_t, z),
            info.struct_info.fields[1].struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test3, w), info.struct_info.fields[2].offset);

  PASS();
}

TEST test4(void) {
  struct test4 {
    char a;
    char b;
    char c;
    char d;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR),
             SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test4), size);
  ASSERT_EQ(offsetof(struct test4, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test4, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test4, c), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test4, d), info.struct_info.fields[3].offset);

  PASS();
}

TEST test5(void) {
  struct test5 {
    double x;
    double y;
    double z;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_DOUBLE),
             SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test5), size);
  ASSERT_EQ(offsetof(struct test5, x), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test5, y), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test5, z), info.struct_info.fields[2].offset);

  PASS();
}

TEST test6(void) {
  typedef union {
    char c;
    int i;
    double d;
  } u_t;

  struct test6 {
    u_t u;
  };

  struct field_info info =
      STRUCT(UNION(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_INT),
                   SIMPLE(FIELD_TYPE_DOUBLE)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test6), size);
  ASSERT_EQ(offsetof(struct test6, u), info.struct_info.fields[0].offset);

  for (size_t i = 0; i < info.struct_info.fields[0].union_info.num_fields;
       ++i) {
    ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[i].offset);
  }

  PASS();
}

TEST test7(void) {
  typedef struct {
    float c;
    double d;
  } inner_t;

  typedef struct {
    int b;
    inner_t inner;
  } outer_t;

  struct test7 {
    char a;
    outer_t outer;
  };

  struct field_info info = STRUCT(
      SIMPLE(FIELD_TYPE_CHAR),
      STRUCT(SIMPLE(FIELD_TYPE_INT),
             STRUCT(SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_DOUBLE))));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test7), size);
  ASSERT_EQ(offsetof(struct test7, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test7, outer), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(outer_t, inner),
            info.struct_info.fields[1].struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1]
                   .struct_info.fields[1]
                   .struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(inner_t, d), info.struct_info.fields[1]
                                      .struct_info.fields[1]
                                      .struct_info.fields[1]
                                      .offset);

  PASS();
}

TEST test8(void) {
  typedef union {
    int i;
    float f;
  } u1_t;

  typedef union {
    double d;
    void *p;
  } u2_t;

  struct test8 {
    u1_t u1;
    u2_t u2;
  };

  struct field_info info =
      STRUCT(UNION(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_FLOAT)),
             UNION(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_POINTER)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test8), size);
  ASSERT_EQ(offsetof(struct test8, u1), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test8, u2), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].union_info.fields[1].offset);

  PASS();
}

TEST test9(void) {
  struct test9 {
    char a;
    char b;
    int c;
  };

  struct field_info info = STRUCT(
      SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_INT));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test9), size);
  ASSERT_EQ(offsetof(struct test9, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test9, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test9, c), info.struct_info.fields[2].offset);

  PASS();
}

TEST test10(void) {
  struct test10 {
    void *p1;
    void *p2;
    void *p3;
    int i;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_POINTER), SIMPLE(FIELD_TYPE_POINTER),
             SIMPLE(FIELD_TYPE_POINTER), SIMPLE(FIELD_TYPE_INT));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test10), size);
  ASSERT_EQ(offsetof(struct test10, p1), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test10, p2), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test10, p3), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test10, i), info.struct_info.fields[3].offset);

  PASS();
}

TEST test11(void) {
  typedef struct {
    char a;
    char b;
    char c;
    char d;
  } s_t;

  typedef union {
    double d;
    s_t s;
  } u_t;

  struct test11 {
    char x;
    u_t u;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR),
             UNION(SIMPLE(FIELD_TYPE_DOUBLE),
                   STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR),
                          SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR))));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test11), size);
  ASSERT_EQ(offsetof(struct test11, u), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].union_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1]
                   .union_info.fields[1]
                   .struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(s_t, d), info.struct_info.fields[1]
                                  .union_info.fields[1]
                                  .struct_info.fields[3]
                                  .offset);

  PASS();
}

TEST test12(void) {
  struct test12 {
    char a;
    double b;
    char c;
    double d;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_DOUBLE),
             SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test12), size);
  ASSERT_EQ(offsetof(struct test12, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test12, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test12, c), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test12, d), info.struct_info.fields[3].offset);

  PASS();
}

TEST test13(void) {
  struct test13 {
    float a;
    float b;
    float c;
    float d;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_FLOAT),
             SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_FLOAT));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test13), size);
  ASSERT_EQ(offsetof(struct test13, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test13, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test13, c), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test13, d), info.struct_info.fields[3].offset);

  PASS();
}

TEST test14(void) {
  struct test14 {
    int a;
    char b;
    int c;
    char d;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_CHAR),
             SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test14), size);
  ASSERT_EQ(offsetof(struct test14, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test14, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test14, c), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test14, d), info.struct_info.fields[3].offset);

  PASS();
}

TEST test15(void) {
  typedef union {
    float f;
    int i;
  } u_t;

  typedef struct {
    int b;
    u_t u;
  } s_t;

  struct test15 {
    char a;
    s_t s;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR),
             STRUCT(SIMPLE(FIELD_TYPE_INT),
                    UNION(SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_INT))));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test15), size);
  ASSERT_EQ(offsetof(struct test15, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test15, s), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(s_t, u),
            info.struct_info.fields[1].struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1]
                   .struct_info.fields[1]
                   .union_info.fields[0]
                   .offset);
  ASSERT_EQ(0, info.struct_info.fields[1]
                   .struct_info.fields[1]
                   .union_info.fields[1]
                   .offset);

  PASS();
}

TEST test16(void) {
  typedef struct {
    double d1;
    double d2;
  } s_t;

  typedef union {
    char c;
    s_t s;
    int i;
  } u_t;

  struct test16 {
    u_t u;
  };

  struct field_info info =
      STRUCT(UNION(SIMPLE(FIELD_TYPE_CHAR),
                   STRUCT(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_DOUBLE)),
                   SIMPLE(FIELD_TYPE_INT)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test16), size);
  ASSERT_EQ(offsetof(struct test16, u), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .union_info.fields[1]
                   .struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(s_t, d2), info.struct_info.fields[0]
                                   .union_info.fields[1]
                                   .struct_info.fields[1]
                                   .offset);

  PASS();
}

TEST test17(void) {
  typedef struct {
    char a;
    int b;
  } inner_t;

  typedef struct {
    inner_t inner;
    float c;
  } middle_t;

  struct test17 {
    middle_t middle;
    double d;
  };

  struct field_info info =
      STRUCT(STRUCT(STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_INT)),
                    SIMPLE(FIELD_TYPE_FLOAT)),
             SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test17), size);
  ASSERT_EQ(offsetof(struct test17, middle), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .struct_info.fields[0]
                   .struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(inner_t, b), info.struct_info.fields[0]
                                      .struct_info.fields[0]
                                      .struct_info.fields[1]
                                      .offset);
  ASSERT_EQ(offsetof(middle_t, c),
            info.struct_info.fields[0].struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test17, d), info.struct_info.fields[1].offset);

  PASS();
}

TEST test18(void) {
  struct test18 {
    char c;
    double d;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test18), size);
  ASSERT_EQ(offsetof(struct test18, c), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test18, d), info.struct_info.fields[1].offset);

  PASS();
}

TEST test19(void) {
  typedef struct {
    int a;
    int b;
  } s1_t;

  typedef struct {
    float c;
    float d;
  } s2_t;

  typedef struct {
    char e;
    char f;
    char g;
    char h;
  } s3_t;

  typedef union {
    s1_t s1;
    s2_t s2;
    s3_t s3;
  } u_t;

  struct test19 {
    u_t u;
  };

  struct field_info info =
      STRUCT(UNION(STRUCT(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_INT)),
                   STRUCT(SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_FLOAT)),
                   STRUCT(SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR),
                          SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR))));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test19), size);
  ASSERT_EQ(offsetof(struct test19, u), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[2].offset);
  ASSERT_EQ(offsetof(s1_t, b), info.struct_info.fields[0]
                                   .union_info.fields[0]
                                   .struct_info.fields[1]
                                   .offset);
  ASSERT_EQ(offsetof(s3_t, h), info.struct_info.fields[0]
                                   .union_info.fields[2]
                                   .struct_info.fields[3]
                                   .offset);

  PASS();
}

TEST test20(void) {
  typedef struct {
    float x;
    float y;
  } coords_t;

  typedef union {
    double value;
    void *ptr;
  } payload_t;

  struct test20 {
    int id;
    void *data;
    coords_t coords;
    payload_t payload;
    char flag;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_POINTER),
             STRUCT(SIMPLE(FIELD_TYPE_FLOAT), SIMPLE(FIELD_TYPE_FLOAT)),
             UNION(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_POINTER)),
             SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test20), size);
  ASSERT_EQ(offsetof(struct test20, id), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[2].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(coords_t, y),
            info.struct_info.fields[2].struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test20, payload),
            info.struct_info.fields[3].offset);
  ASSERT_EQ(0, info.struct_info.fields[3].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[3].union_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test20, flag), info.struct_info.fields[4].offset);

  PASS();
}

TEST test21(void) {
  struct test21 {
    char c;
  };

  struct field_info info = STRUCT(SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test21), size);
  ASSERT_EQ(offsetof(struct test21, c), info.struct_info.fields[0].offset);

  PASS();
}

TEST test22(void) {
  struct test22 {
    double d;
    char c;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test22), size);
  ASSERT_EQ(0, size % 8);

  PASS();
}

TEST test23(void) {
  struct test23 {
    struct {
      struct {
        struct {
          char a;
        } level3;
      } level2;
    } level1;
  };

  struct field_info info =
      STRUCT(STRUCT(STRUCT(STRUCT(SIMPLE(FIELD_TYPE_CHAR)))));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test23), size);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .struct_info.fields[0]
                   .struct_info.fields[0]
                   .struct_info.fields[0]
                   .offset);

  PASS();
}

TEST test24(void) {
  typedef union {
    double d;
  } u_t;

  struct test24 {
    u_t u;
  };

  struct field_info info = STRUCT(UNION(SIMPLE(FIELD_TYPE_DOUBLE)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test24), size);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[0].offset);

  PASS();
}

TEST test25(void) {
  struct test25 {
    char arr[10];
  };

  struct field_info info = STRUCT(ARRAY(SIMPLE(FIELD_TYPE_CHAR), 10));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test25), size);
  ASSERT_EQ(offsetof(struct test25, arr), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].array_info.element_type->offset);

  PASS();
}

TEST test26(void) {
  struct test26 {
    int arr[5];
  };

  struct field_info info = STRUCT(ARRAY(SIMPLE(FIELD_TYPE_INT), 5));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test26), size);
  ASSERT_EQ(0, info.struct_info.fields[0].array_info.element_type->offset);

  PASS();
}

TEST test27(void) {
  struct test27 {
    char c;
    double arr[3];
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_CHAR), ARRAY(SIMPLE(FIELD_TYPE_DOUBLE), 3));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test27), size);
  ASSERT_EQ(offsetof(struct test27, c), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test27, arr), info.struct_info.fields[1].offset);

  PASS();
}

TEST test28(void) {
  struct test28 {
    int arr[4];
    char c;
  };

  struct field_info info =
      STRUCT(ARRAY(SIMPLE(FIELD_TYPE_INT), 4), SIMPLE(FIELD_TYPE_CHAR));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test28), size);
  ASSERT_EQ(offsetof(struct test28, arr), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test28, c), info.struct_info.fields[1].offset);

  PASS();
}

TEST test29(void) {
  typedef struct {
    float arr[2];
    int x;
  } nested_t;

  struct test29 {
    nested_t nested;
  };

  struct field_info info = STRUCT(
      STRUCT(ARRAY(SIMPLE(FIELD_TYPE_FLOAT), 2), SIMPLE(FIELD_TYPE_INT)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test29), size);
  ASSERT_EQ(0, info.struct_info.fields[0].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(nested_t, x),
            info.struct_info.fields[0].struct_info.fields[1].offset);

  PASS();
}

TEST test30(void) {
  typedef union {
    char arr[16];
    double d;
  } u_t;

  struct test30 {
    u_t u;
  };

  struct field_info info = STRUCT(
      UNION(ARRAY(SIMPLE(FIELD_TYPE_CHAR), 16), SIMPLE(FIELD_TYPE_DOUBLE)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test30), size);
  ASSERT_EQ(offsetof(struct test30, u), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);

  PASS();
}

TEST test31(void) {
  struct test31 {
    char a;
    char b;
    char c;
    char d;
    char e;
    char f;
    char g;
    double h;
  };

  struct field_info info = STRUCT(
      SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR),
      SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_CHAR),
      SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test31), size);
  ASSERT_EQ(offsetof(struct test31, h), info.struct_info.fields[7].offset);

  PASS();
}

TEST test32(void) {
  struct test32 {
    double a;
    char b;
    double c;
    char d;
    double e;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_CHAR),
             SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_CHAR),
             SIMPLE(FIELD_TYPE_DOUBLE));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test32), size);
  ASSERT_EQ(offsetof(struct test32, a), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test32, b), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test32, c), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test32, d), info.struct_info.fields[3].offset);
  ASSERT_EQ(offsetof(struct test32, e), info.struct_info.fields[4].offset);

  PASS();
}

TEST test33(void) {
  typedef struct {
  } empty_t;

  struct test33 {
    empty_t empty;
    int x;
  };

  struct field_info info = STRUCT(STRUCT(), SIMPLE(FIELD_TYPE_INT));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test33), size);
  ASSERT_EQ(offsetof(struct test33, empty), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test33, x), info.struct_info.fields[1].offset);

  PASS();
}

TEST test34(void) {
  struct test34 {
    void *a;
    void *b;
    void *c;
    void *d;
    void *e;
  };

  struct field_info info =
      STRUCT(SIMPLE(FIELD_TYPE_POINTER), SIMPLE(FIELD_TYPE_POINTER),
             SIMPLE(FIELD_TYPE_POINTER), SIMPLE(FIELD_TYPE_POINTER),
             SIMPLE(FIELD_TYPE_POINTER));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test34), size);

  PASS();
}

TEST test35(void) {
  typedef union {
    int i;
    float f;
  } inner_t;

  typedef union {
    inner_t inner;
    double d;
  } outer_t;

  struct test35 {
    outer_t outer;
  };

  struct field_info info =
      STRUCT(UNION(UNION(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_FLOAT)),
                   SIMPLE(FIELD_TYPE_DOUBLE)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test35), size);
  ASSERT_EQ(offsetof(struct test35, outer), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .union_info.fields[0]
                   .union_info.fields[0]
                   .offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);

  PASS();
}

TEST test36(void) {
  struct test36 {
    char c;
    int i;
    float f;
    double d;
    void *p;
  };

  struct field_info info = STRUCT(
      SIMPLE(FIELD_TYPE_CHAR), SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_FLOAT),
      SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_POINTER));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test36), size);
  ASSERT_EQ(offsetof(struct test36, c), info.struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test36, i), info.struct_info.fields[1].offset);
  ASSERT_EQ(offsetof(struct test36, f), info.struct_info.fields[2].offset);
  ASSERT_EQ(offsetof(struct test36, d), info.struct_info.fields[3].offset);
  ASSERT_EQ(offsetof(struct test36, p), info.struct_info.fields[4].offset);

  PASS();
}

TEST test37(void) {
  typedef struct {
    int x;
    char y;
  } elem_t;

  struct test37 {
    elem_t arr[3];
  };

  struct field_info info =
      STRUCT(ARRAY(STRUCT(SIMPLE(FIELD_TYPE_INT), SIMPLE(FIELD_TYPE_CHAR)), 3));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test37), size);
  ASSERT_EQ(offsetof(struct test37, arr), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .array_info.element_type->struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(elem_t, y),
            info.struct_info.fields[0]
                .array_info.element_type->struct_info.fields[1]
                .offset);

  PASS();
}

TEST test38(void) {
  typedef struct {
    double a;
    double b;
    double c;
  } large_t;

  typedef union {
    char small;
    large_t large;
    int medium;
  } u_t;

  struct test38 {
    u_t u;
  };

  struct field_info info =
      STRUCT(UNION(SIMPLE(FIELD_TYPE_CHAR),
                   STRUCT(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_DOUBLE),
                          SIMPLE(FIELD_TYPE_DOUBLE)),
                   SIMPLE(FIELD_TYPE_INT)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test38), size);
  ASSERT_EQ(offsetof(struct test38, u), info.struct_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[0].union_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .union_info.fields[1]
                   .struct_info.fields[0]
                   .offset);
  ASSERT_EQ(offsetof(large_t, c), info.struct_info.fields[0]
                                      .union_info.fields[1]
                                      .struct_info.fields[2]
                                      .offset);

  PASS();
}

TEST test39(void) {
  typedef struct {
    int a;
  } s1_t;
  typedef union {
    double d;
    float f;
  } u_t;
  typedef struct {
    char b;
  } s2_t;

  struct test39 {
    s1_t s1;
    u_t u;
    s2_t s2;
  };

  struct field_info info =
      STRUCT(STRUCT(SIMPLE(FIELD_TYPE_INT)),
             UNION(SIMPLE(FIELD_TYPE_DOUBLE), SIMPLE(FIELD_TYPE_FLOAT)),
             STRUCT(SIMPLE(FIELD_TYPE_CHAR)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test39), size);
  ASSERT_EQ(0, info.struct_info.fields[0].struct_info.fields[0].offset);
  ASSERT_EQ(offsetof(struct test39, u), info.struct_info.fields[1].offset);
  ASSERT_EQ(0, info.struct_info.fields[1].union_info.fields[0].offset);
  ASSERT_EQ(0, info.struct_info.fields[2].struct_info.fields[0].offset);

  PASS();
}

TEST test40(void) {
  typedef struct {
    int arr[10];
  } s_t;
  typedef union {
    s_t s;
    double d;
  } u_t;

  struct test40 {
    u_t u;
  };

  struct field_info info = STRUCT(UNION(
      STRUCT(ARRAY(SIMPLE(FIELD_TYPE_INT), 10)), SIMPLE(FIELD_TYPE_DOUBLE)));

  size_t size = compile(&info);
  ASSERT_EQ(sizeof(struct test40), size);
  ASSERT_EQ(0, info.struct_info.fields[0]
                   .union_info.fields[0]
                   .struct_info.fields[0]
                   .offset);

  PASS();
}

SUITE(struct_layout_tests) {
  RUN_TEST(test1);
  RUN_TEST(test2);
  RUN_TEST(test3);
  RUN_TEST(test4);
  RUN_TEST(test5);
  RUN_TEST(test6);
  RUN_TEST(test7);
  RUN_TEST(test8);
  RUN_TEST(test9);
  RUN_TEST(test10);
  RUN_TEST(test11);
  RUN_TEST(test12);
  RUN_TEST(test13);
  RUN_TEST(test14);
  RUN_TEST(test15);
  RUN_TEST(test16);
  RUN_TEST(test17);
  RUN_TEST(test18);
  RUN_TEST(test19);
  RUN_TEST(test20);
  RUN_TEST(test21);
  RUN_TEST(test22);
  RUN_TEST(test23);
  RUN_TEST(test24);
  RUN_TEST(test25);
  RUN_TEST(test26);
  RUN_TEST(test27);
  RUN_TEST(test28);
  RUN_TEST(test29);
  RUN_TEST(test30);
  RUN_TEST(test31);
  RUN_TEST(test32);
  RUN_TEST(test33);
  RUN_TEST(test34);
  RUN_TEST(test35);
  RUN_TEST(test36);
  RUN_TEST(test37);
  RUN_TEST(test38);
  RUN_TEST(test39);
  RUN_TEST(test40);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(struct_layout_tests);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
