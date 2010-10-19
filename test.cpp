#include "J.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE J

#include <boost/test/unit_test.hpp>

using namespace ::J;
using namespace ::J::JParser;

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
		    JArray<JInt>(Dimensions(2, 4, 5), 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39));
}

BOOST_AUTO_TEST_CASE ( jarray_deep_coordinate ) {
  shared_ptr<vector<int> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<JInt> arr(Dimensions(3, 3, 4, 5), v);
  
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
    
  JArray<JInt> arr(Dimensions(3, 3, 4, 5), v);

  BOOST_CHECK_EQUAL(*(arr.extend(Dimensions(3,4,6, 10))),
		    JArray<JInt>(Dimensions(3,4, 6, 10),
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

  BOOST_CHECK_EQUAL(*JArray<JInt>(Dimensions(1, 5), 1, 2, 3, 4, 5).extend(Dimensions(1, 10)),
		    JArray<JInt>(Dimensions(1, 10), 1, 2, 3, 4, 5, 0, 0, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE ( jarray_iterator ) {
  shared_ptr<vector<int> > v(new vector<int>(60, 0));
  int i = 0;
  for (vector<int>::iterator iter = v->begin(); iter != v->end(); ++iter, ++i) {
    *iter = i;
  }
    
  JArray<JInt> arr(Dimensions(3, 3, 4, 5), v);
  
  OperationIterator<int> iter(arr, Dimensions(3, 3, 4, 10), 1);

  for (int i = 25; i > 0; --i) ++iter;
  BOOST_CHECK_EQUAL(**iter, JArray<JInt>(Dimensions(1, 5), 10, 11, 12,13, 14));
  for (int i = 94; i > 0; --i) ++iter;
  BOOST_CHECK_EQUAL(**iter, JArray<JInt>(Dimensions(1, 5), 55, 56,57,58,59));
  
  JArray<JInt> arr2(Dimensions(1, 3), 1, 2, 3);
  OperationIterator<int> iter2(arr2, Dimensions(3, 3, 2, 1), 0);
  BOOST_CHECK_EQUAL(**(++iter2), JArray<JInt>(Dimensions(0), 1));
  ++iter2; ++iter2;
  BOOST_CHECK_EQUAL(**iter2, JArray<JInt>(Dimensions(0), 2));
  BOOST_CHECK_EQUAL(**(++iter2), JArray<JInt>(Dimensions(0), 3));
  BOOST_CHECK_EQUAL(**(++iter2), JArray<JInt>(Dimensions(0), 3));
  
  JArray<JInt> arr3(Dimensions(1, 2), 1, 2);
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

BOOST_AUTO_TEST_SUITE ( verbs )

BOOST_AUTO_TEST_CASE (test_find_max_dims) {
  shared_ptr<JNoun> arr(new JArray<JInt>(Dimensions(3, 2, 3, 4), shared_ptr<vector<int> >(new vector<int>(24, 1))));
  shared_ptr<JNoun> arr2(new JArray<JInt>(Dimensions(3, 3, 2, 5), shared_ptr<vector<int> >(new vector<int>(30, 2))));
  shared_ptr<JNoun> arr3(new JArray<JInt>(Dimensions(3, 3, 2, 10), shared_ptr<vector<int> >(new vector<int>(60, 3))));
  
  JResult res(Dimensions(2, 3, 2));
  res.add_noun(arr); res.add_noun(arr2); res.add_noun(arr3);
  res.add_noun(arr); res.add_noun(arr2); res.add_noun(arr3);

  BOOST_CHECK_EQUAL(res.assemble_result()->get_dims(), Dimensions(5, 3, 2, 3, 3, 10));
  BOOST_CHECK_EQUAL(static_cast<JArray<JInt>& >(*res.assemble_result())[0][0], 
		    *(static_cast<JArray<JInt>* >(&*arr)->extend(Dimensions(3, 3, 3, 10))));
}

BOOST_AUTO_TEST_CASE ( test_find_frame ) {
  JArray<JInt> arr(Dimensions(2,2,2), 1, 1,1,1);
  
  BOOST_CHECK_EQUAL(find_frame(0, 0, arr.get_dims(), arr.get_dims()), Dimensions(2,2,2));
}

BOOST_AUTO_TEST_CASE ( test_plus_op ) {
  shared_ptr<JMachine> m(JMachine::new_machine());
  JArray<JInt> arr(Dimensions(2,2,2), 1,2,3 ,4);
  JArray<JInt> arr2(Dimensions(3,2,2,2), 1, 2,3,4,5,6,7,8);
  
  PlusVerb verb;
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(3,2,2,2), 2,3,5,6,8,9, 11, 12),
		    *verb(m, arr, arr2));

  JArray<JFloat> arr3(Dimensions(1, 2), 1.5, 2.5);

  BOOST_CHECK_EQUAL(JArray<JFloat>(Dimensions(2,2,2), 2.5, 3.5, 5.5, 6.5),
		    *verb(m, arr, arr3));

  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(2,2,2), 2, 4, 6, 8),
		    *verb(m, arr, arr));
  
  BOOST_CHECK_EQUAL(*verb(m, arr), arr);
  BOOST_CHECK_EQUAL(*verb(m, arr3), arr3);
}

BOOST_AUTO_TEST_CASE ( test_minus_op ) {
  shared_ptr<JMachine> m(JMachine::new_machine());
  JArray<JInt> arr(Dimensions(2,2,2), 1,2 ,3,4);
  JArray<JInt> arr2(Dimensions(2,2,2), 4,3,2,1);
  JArray<JInt> arr3(Dimensions(0), 1);
  
  JArray<JFloat> arr4(Dimensions(3,4, 2, 2), 1,2,3,4,5,6,7,8,9,10,
		      11,12,13,14,15,16);
  MinusVerb verb;
  BOOST_CHECK_EQUAL(*verb(m, arr2, arr),
		    JArray<JInt>(Dimensions(2,2,2), 3, 1, -1, -3));
  BOOST_CHECK_EQUAL(*verb(m, arr3, arr),
		    JArray<JInt>(Dimensions(2,2,2), 0, -1, -2, -3));

  BOOST_CHECK_EQUAL(*verb(m, arr, arr3),
		    JArray<JInt>(Dimensions(2,2,2), 0, 1, 2, 3));

  BOOST_CHECK_EQUAL(*verb(m, arr4, arr4), 
		    JArray<JFloat>(Dimensions(3, 4,2,2), 
				   0.0, 0.0, 0.0, 0.0,
				   0.0, 0.0, 0.0, 0.0, 
				   0.0, 0.0, 0.0, 0.0,
				   0.0, 0.0, 0.0, 0.0));

  BOOST_CHECK_EQUAL(*verb(m, arr),
		    JArray<JInt>(Dimensions(2,2,2), -1, -2, -3,-4));
}

BOOST_AUTO_TEST_CASE ( test_i_dot_verb ) {
  JArray<JInt> arr(Dimensions(2,2,3), 1,2,3,4,-5,6);
  shared_ptr<JMachine> m(JMachine::new_machine());
  IDotVerb v;
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(4, 2, 4, 5, 6),
				 0,   1,   2,   0,   0,   0, 
				 3,   4,   5,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 0,   0,   0,   0,   0,   0, 
				 

				 24,  25,  26,  27,  28,  29, 
				 18,  19,  20,  21,  22,  23, 
				 12,  13,  14,  15,  16,  17, 
				 6,   7,   8,   9,  10,  11, 
				 0,   1,   2,   3,   4,   5, 
				 
				 54,  55,  56,  57,  58,  59, 
				 48,  49,  50,  51,  52,  53, 
				 42,  43,  44,  45,  46,  47, 
				 36,  37,  38,  39,  40,  41, 
				 30,  31,  32,  33,  34,  35, 
				 
				 84,  85,  86,  87,  88,  89, 
				 78,  79,  80,  81,  82,  83, 
				 72,  73,  74,  75,  76,  77, 
				 66,  67,  68,  69,  70,  71, 
				 60,  61,  62,  63,  64,  65, 
				 
				 114, 115, 116, 117, 118, 119, 
				 108, 109, 110, 111, 112, 113, 
				 102, 103, 104, 105, 106, 107, 
				 96,  97,  98,  99, 100, 101, 
				 90,  91,  92,  93,  94,  95),
		    *v(m, arr));

  JArray<JInt> arr2(Dimensions(1, 3), -2, 3, -4);
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(3, 2,3,4),
				 15,14,13,12,
				 19,18,17,16,
				 23,22,21,20,
				 
				 3,2,1,0,
				 7,6,5,4,
				 11,10,9,8),
		    *v(m, arr2));
}

BOOST_AUTO_TEST_CASE ( test_i_dot_monad_verb ) {
  JArray<JInt> arr(Dimensions(2, 2, 4), 0,1,2,3,4,5,6,7);
  JArray<JInt> arr2(Dimensions(2, 4, 4), 0, 1,2,3, 3,3,3,3,0,1,2,3,4,5,6,7);
  shared_ptr<JMachine> m(JMachine::new_machine());

  IDotVerb v;
  
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(1, 4), 0, 2, 0, 1),
		    *v(m, arr, arr2));

  JArray<JInt> arr3(Dimensions(1, 4), 0,1,2,3);
  JArray<JInt> arr4(Dimensions(0), 1);
  BOOST_CHECK_EQUAL(*v(m, arr3, arr4), 
		    JArray<JInt>(Dimensions(0), 1));
  JArray<JInt> arr5(Dimensions(2, 4, 2), 0, 1,2,3, 3,3,3,3,0,1,2,3,4,5,6,7);
  BOOST_CHECK_EQUAL(*v(m, arr5, arr4), JArray<JInt>(Dimensions(0), 4));
}

BOOST_AUTO_TEST_CASE ( test_adverb ) { 
  shared_ptr<JMachine> m(JMachine::new_machine());
  shared_ptr<PlusVerb> plus(new PlusVerb);;
  JInsertTableAdverb adverb;
  JVerb verb(static_cast<JVerb&>(*adverb(m, plus)));
  BOOST_CHECK_EQUAL(*verb(m, JArray<JInt>(Dimensions(1,10), 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
		    JArray<JInt>(Dimensions(0), 55));
  
  JArray<JInt> table(Dimensions(1, 5), -2, -1, 0, 1, 2);
  JArray<JInt> answer(Dimensions(2,5,5), 
		      -4, -3, -2, -1,  0, 
		      -3, -2, -1,  0,  1, 
		      -2, -1,  0,  1,  2, 
		      -1,  0,  1,  2,  3, 
		      0,  1,  2,  3,  4);
  
  BOOST_CHECK_EQUAL(answer, *verb(m, table, table));
  BOOST_CHECK_EQUAL(*verb(m, answer), JArray<JInt>(Dimensions(1, 5), -10, -5, 0, 5, 10));
}
  
BOOST_AUTO_TEST_CASE ( test_rank_conjunction ) {
  shared_ptr<JMachine> m(JMachine::new_machine());
  shared_ptr<PlusVerb> plus(new PlusVerb);
  shared_ptr<JVerb> sum(boost::static_pointer_cast<JVerb>(JInsertTableAdverb()(m, plus)));
  shared_ptr<JNoun> rank_array(new JArray<JInt>(Dimensions(1,3), 1, 0, 0));
  shared_ptr<JVerb> sum_rank(boost::static_pointer_cast<JVerb>(RankConjunction()(m, sum, rank_array)));
  
  JArray<JInt> test_subject(Dimensions(2, 2, 5), 1,2,3,4,5,6,7,8,9,10);

  BOOST_CHECK_EQUAL(*(*sum_rank)(m, test_subject), JArray<JInt>(Dimensions(1,2), 15, 40));
  BOOST_CHECK_EQUAL(*(*sum)(m, test_subject), JArray<JInt>(Dimensions(1,5), 7,9,11,13,15));
}

BOOST_AUTO_TEST_SUITE_END()

