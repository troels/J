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

