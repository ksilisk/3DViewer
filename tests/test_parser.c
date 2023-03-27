#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/s21_parser.h"

START_TEST(test_load_destroy_cube) {
  Obj* cube = parse_obj("models/cube.obj");
  ck_assert_ptr_ne(cube, NULL);
  ck_assert_ptr_ne(cube->vertices, NULL);
  ck_assert_ptr_ne(cube->faces, NULL);
  ck_assert_ptr_ne(cube->normals, NULL);
  ck_assert_ptr_ne(cube->textures, NULL);
  ck_assert_int_eq(cube->vertices->count, 8);
  ck_assert_int_eq(cube->faces->count, 6);
  ck_assert_int_eq(cube->normals->count, 0);
  ck_assert_int_eq(cube->textures->count, 0);

  int error = 0;

  Triangles triangles = triangulate(cube, &error);
  ck_assert_int_eq(triangles.count, 12);
  ck_assert_ptr_ne(triangles.triangles, NULL);
  ck_assert_int_eq(error, 0);

  VertexBuffer buffer = create_vertex_buffer(cube, triangles, &error);
  ck_assert_int_eq(buffer.count, 36);
  ck_assert_ptr_ne(buffer.data, NULL);
  ck_assert_int_eq(error, 0);

  safe_free(triangles.triangles);
  safe_free(buffer.data);
  destroy_obj(cube);
}
END_TEST

START_TEST(test_load_wrong_file) {
  Obj* null_obj = parse_obj("models/does_not_exist.obj");
  ck_assert_ptr_eq(null_obj, NULL);
}
END_TEST

START_TEST(test_load_complex_obj) {
  Obj* obj = parse_obj("models/Girl.obj");
  ck_assert_ptr_ne(obj, NULL);
  ck_assert_ptr_ne(obj->vertices, NULL);
  ck_assert_ptr_ne(obj->faces, NULL);
  ck_assert_ptr_ne(obj->normals, NULL);
  ck_assert_ptr_ne(obj->textures, NULL);
  destroy_obj(obj);
}
END_TEST

Suite*

parser_suite(void) {
  Suite* s;
  TCase* tc_pos;

  s = suite_create("parser");

  /* Positive test case */
  tc_pos = tcase_create("positives");

  tcase_add_test(tc_pos, test_load_destroy_cube);
  tcase_add_test(tc_pos, test_load_wrong_file);
  tcase_add_test(tc_pos, test_load_complex_obj);
  suite_add_tcase(s, tc_pos);

  return s;
}

int test_parser() {
  int no_failed = 0;
  Suite* s;
  SRunner* sr;

  s = parser_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  no_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return no_failed;
}