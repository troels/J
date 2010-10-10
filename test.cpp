#include "J.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE J

#include <boost/test/unit_test.hpp>

using namespace ::J;

BOOST_AUTO_TEST_SUITE ( dimensions_tests )

BOOST_AUTO_TEST_CASE( test_dimensions1 ) {
  Dimensions d(4, 3, 2, 1, 1);
  
  BOOST_CHECK_EQUAL(d.get_rank(), 4);
  BOOST_CHECK_EQUAL(d[2], 1);
}

BOOST_AUTO_TEST_CASE (test_dimensions_equal) {
  Dimensions d;
  Dimensions e;
  BOOST_CHECK_EQUAL(d, e);

  Dimensions f(2, 1, 2);
  shared_ptr<vector<int> > v(new vector<int>(2));
  v->operator[](0) = 1;
  v->operator[](1) = 2;
  Dimensions g(v, v->begin(), v->end());
  
  BOOST_CHECK_EQUAL(g, f);
}

BOOST_AUTO_TEST_CASE ( test_dimensions_addition ) {
  Dimensions d;
  Dimensions e(4, 3, 4, 5, 6);
  Dimensions f(2, 1, 2);
  
  BOOST_CHECK_EQUAL(d + e, e);
  BOOST_CHECK_EQUAL(f + e, Dimensions(6, 1, 2, 3, 4, 5, 6));
}

BOOST_AUTO_TEST_CASE ( test_dimensions_starfix) {
  Dimensions d;
  
  BOOST_CHECK_EQUAL(d.suffix(1), d);
  BOOST_CHECK_EQUAL(d.prefix(2), d);

  Dimensions e(5, 1, 2, 3, 4, 5);
  BOOST_CHECK_EQUAL(e.suffix(3), Dimensions(3, 3, 4, 5));
  BOOST_CHECK_EQUAL(e.suffix(3), e.suffix(-2));
  BOOST_CHECK_EQUAL(e.prefix(-3), Dimensions(2, 1, 2));

  BOOST_CHECK_EQUAL(e.prefix(5), e);
}

BOOST_AUTO_TEST_CASE ( test_dimensions_starfix_match ) {
  Dimensions d(5, 1, 2, 3, 4, 5);
  
  BOOST_CHECK(d.prefix_match(Dimensions(3, 1, 2, 3)));
  BOOST_CHECK(d.prefix_match(Dimensions()));
  BOOST_CHECK(d.suffix_match(Dimensions(2, 4, 5)));
  BOOST_CHECK(d.prefix_match(d));
}

BOOST_AUTO_TEST_CASE ( test_dimensions_number_of_elems ) {
  Dimensions d;
  
  BOOST_CHECK_EQUAL(1, d.number_of_elems());
  
  Dimensions e(4, 3, 4, 5, 6);
  BOOST_CHECK_EQUAL(360, e.number_of_elems());
}

BOOST_AUTO_TEST_CASE ( test_dimensions_number_index ) {
  Dimensions e(4, 3, 4, 5, 6);
  
  BOOST_CHECK_EQUAL(e[2], 5);
  BOOST_CHECK_EQUAL(e.suffix(-1)[2], 6);
  BOOST_CHECK_EQUAL(e.prefix(-1)[1], 4);

  BOOST_CHECK_EQUAL(e.suffix(-1).number_of_elems(), 120);
  BOOST_CHECK_EQUAL(e.prefix(-2).number_of_elems(), 12);
}
  
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE ( vectorcounter_tests ) 

BOOST_AUTO_TEST_CASE ( vectorcounter_test ){
  Dimensions d(4, 2, 3, 4, 4);
  Dimensions e(4, 4, 4, 6, 8);
  
  VectorCounter vc(d);
  VectorCounter vc2(e);
  
  
  pair<int, int> my_pair;
  for (int i = 0; i < 3; ++i ) {
    my_pair = add_row(vc, vc2);
    BOOST_CHECK_EQUAL(my_pair.first, 4);
    BOOST_CHECK_EQUAL(my_pair.second, 8);
    my_pair = add_row(vc, vc2);
    BOOST_CHECK_EQUAL(my_pair.first, 4);
    BOOST_CHECK_EQUAL(my_pair.second, 8);
    my_pair = add_row(vc, vc2);
    BOOST_CHECK_EQUAL(my_pair.first, 4);
    BOOST_CHECK_EQUAL(my_pair.second, 8);

    my_pair = add_row(vc, vc2);
    BOOST_CHECK_EQUAL(my_pair.first, 4);
    BOOST_CHECK_EQUAL(my_pair.second, i != 2 ? 24 : 72 );
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE ( jarray_tests ) 

BOOST_AUTO_TEST_CASE ( jarray_index ) {
  shared_ptr<vector<JInt> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<JInt> arr(Dimensions(3, 3, 4, 5), v);
  BOOST_CHECK_EQUAL(arr[1], 
		    JArray<int>(Dimensions(2, 4, 5), 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39));
}

BOOST_AUTO_TEST_CASE ( jarray_deep_coordinate ) {
  shared_ptr<vector<int> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<int> arr(Dimensions(3, 3, 4, 5), v);
  
  BOOST_CHECK_EQUAL(arr[1], *arr.coordinate(1, 1));
  BOOST_CHECK_EQUAL(arr[2][3], *arr.coordinate(2, 2, 3));
  BOOST_CHECK_EQUAL(arr[2][3][2], *arr.coordinate(3, 2, 3, 2));
}

BOOST_AUTO_TEST_CASE ( jarray_extend ) {
  shared_ptr<vector<int> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<int> arr(Dimensions(3, 3, 4, 5), v);

  BOOST_CHECK_EQUAL(*(arr.extend(Dimensions(3,4,6, 10))),
		    JArray<int>(Dimensions(3,4, 6, 10),
				0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 
				5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 
				10, 11, 12, 13, 14, 0, 0, 0, 0, 0, 
				15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				
				20, 21, 22, 23, 24, 0, 0, 0, 0, 0, 
				25, 26, 27, 28, 29, 0, 0, 0, 0, 0, 
				30, 31, 32, 33, 34, 0, 0, 0, 0, 0, 
				35, 36, 37, 38, 39, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				
				40, 41, 42, 43, 44, 0, 0, 0, 0, 0, 
				45, 46, 47, 48, 49, 0, 0, 0, 0, 0, 
				50, 51, 52, 53, 54, 0, 0, 0, 0, 0, 
				55, 56, 57, 58, 59, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  BOOST_CHECK_EQUAL(*JArray<int>(Dimensions(1, 5), 1, 2, 3, 4, 5).extend(Dimensions(1, 10)),
		    JArray<int>(Dimensions(1, 10), 1, 2, 3, 4, 5, 0, 0, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE ( jarray_iterator ) {
  shared_ptr<vector<int> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<int> arr(Dimensions(3, 3, 4, 5), v);
  
  OperationIterator<int> iter(arr, Dimensions(3, 3, 4, 10), 1);

  for (int i = 25; i > 0; --i) ++iter;
  BOOST_CHECK_EQUAL(**iter, JArray<int>(Dimensions(1, 5), 10, 11, 12,13, 14));
  for (int i = 94; i > 0; --i) ++iter;
  BOOST_CHECK_EQUAL(**iter, JArray<int>(Dimensions(1, 5), 55, 56,57,58,59));
  
  JArray<int> arr2(Dimensions(1, 3), 1, 2, 3);
  OperationIterator<int> iter2(arr2, Dimensions(3, 3, 2, 1), 0);
  BOOST_CHECK_EQUAL(**(++iter2), JArray<int>(Dimensions(0), 1));
  ++iter2; ++iter2;
  BOOST_CHECK_EQUAL(**iter2, JArray<int>(Dimensions(0), 2));
  BOOST_CHECK_EQUAL(**(++iter2), JArray<int>(Dimensions(0), 3));
  BOOST_CHECK_EQUAL(**(++iter2), JArray<int>(Dimensions(0), 3));
  
  JArray<int> arr3(Dimensions(1, 2), 1, 2);
  OperationIterator<int> iter3(arr3, Dimensions(3, 3, 3, 3), 100);
  
  for (int i = 10; i > 0; --i) ++iter3;
  BOOST_CHECK_EQUAL(**iter3, arr3);
  for (int i = 16; i > 0; --i) ++iter3;
  BOOST_CHECK_EQUAL(**iter3, arr3);
} 


BOOST_AUTO_TEST_CASE ( jarray_scalarop_iterator ) {
  JArray<JInt> arr(Dimensions(3, 2, 3, 2), 2, 1,2, 3,3,3, 4,4,4,5,5,5);
  
  OperationScalarIterator<JInt> osi(arr, Dimensions(3, 2, 3, 2));
  for (int i = 0; i < 5; ++i) ++osi;
  BOOST_CHECK_EQUAL(*osi, 3);
}

BOOST_AUTO_TEST_SUITE_END()
