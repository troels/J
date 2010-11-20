#include "J.hpp"
#include "ParserCombinators.hpp"
#include "JEvaluator.hpp"
#include "JExecutor.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE J

#include <boost/test/unit_test.hpp>

using namespace ::J;
using namespace ::J::JParser;
using namespace ::ParserCombinators;
using namespace ::J::JTokens;

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
  JNoun::Ptr arr(new JArray<JInt>(Dimensions(3, 2, 3, 4), shared_ptr<vector<int> >(new vector<int>(24, 1))));
  JNoun::Ptr arr2(new JArray<JInt>(Dimensions(3, 3, 2, 5), shared_ptr<vector<int> >(new vector<int>(30, 2))));
  JNoun::Ptr arr3(new JArray<JInt>(Dimensions(3, 3, 2, 10), shared_ptr<vector<int> >(new vector<int>(60, 3))));
  
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
  
  JArray<JFloat> arr4(Dimensions(3,4, 2, 2), 1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,
		      11.0,12.0,13.0,14.0,15.0,16.0);

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
  
  BOOST_CHECK_EQUAL(*verb(m, JArray<JInt>(Dimensions(3, 10, 0, 10))),
		    JArray<JInt>(Dimensions(2, 0, 10)));

  JArray<JInt> table(Dimensions(1, 5), -2, -1, 0, 1, 2);
  JArray<JInt> answer(Dimensions(2,5,5), 
		      -4, -3, -2, -1,  0, 
		      -3, -2, -1,  0,  1, 
		      -2, -1,  0,  1,  2, 
		      -1,  0,  1,  2,  3, 
		      0,  1,  2,  3,  4);
  
  BOOST_CHECK_EQUAL(answer, *verb(m, table, table));
  BOOST_CHECK_EQUAL(*verb(m, answer), JArray<JInt>(Dimensions(1, 5), -10, -5, 0, 5, 10));

  BOOST_CHECK_EQUAL(*verb(m, JArray<JInt>(Dimensions(2, 1, 5), 1,2,3,4,5)),
		    JArray<JInt>(Dimensions(1, 5), 1,2,3,4,5));
}
  
BOOST_AUTO_TEST_CASE ( test_prefix_infix_adverb ) {
  JMachine::Ptr m(JMachine::new_machine());
  JVerb::Ptr minus(new MinusVerb());
  JAdverb::Ptr adverb(new PrefixInfixAdverb());
  JVerb::Ptr realverb(boost::static_pointer_cast<JVerb>((*adverb)(m, minus)));
  JNoun::Ptr noun0(new JArray<JInt>(Dimensions(2, 2, 3), 1,2,3,4,5,6));
  JNoun::Ptr noun1(new JArray<JInt>(Dimensions(1, 6), 1,2,3,4,5,6));

  BOOST_CHECK_EQUAL(*(*realverb)(m, *noun0),
		    JArray<JInt>(Dimensions(3,2,2,3),
				 -1,-2,-3,0,0,0, -1,-2,-3,-4,-5,-6));

  BOOST_CHECK_EQUAL(*(*realverb)(m, *noun1),
		    JArray<JInt>(Dimensions(2,6,6),
				 -1,0,0, 0,0,0,
				 -1,-2,0,0,0,0,
				 -1,-2,-3,0,0,0,
				 -1,-2,-3,-4,0,0,
				 -1,-2,-3,-4,-5,0,
				 -1,-2,-3,-4,-5,-6));
  
  JVerb::Ptr otherverb(boost::static_pointer_cast<JVerb>(JInsertTableAdverb()(m, minus)));
  JVerb::Ptr otherverbwithinsert(boost::static_pointer_cast<JVerb>((*adverb)(m, otherverb)));
  
  BOOST_CHECK_EQUAL(*(*otherverbwithinsert)(m, *noun0),
		    JArray<JInt>(Dimensions(2,2,3), 1,2,3,-3,-3,-3));
  
  BOOST_CHECK_EQUAL(*(*otherverbwithinsert)(m, *noun1),
		    JArray<JInt>(Dimensions(1, 6), 1,-1,2,-2,3,-3));

  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(0), 2), *noun0)),
		    JArray<JInt>(Dimensions(2, 1, 3), -3, -3, -3)); 
  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(0), 2), *noun1)),
		    JArray<JInt>(Dimensions(1, 5), -1,-1,-1,-1,-1));
  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(0), 7), *noun1)),
		    JArray<JInt>(Dimensions(1, 0)));
  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(0), -2), *noun0)),
		    JArray<JInt>(Dimensions(2, 1 , 3), -3,-3,-3));
  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(0), -4), *noun1)),
		    JArray<JInt>(Dimensions(1, 2), -2, -1));

  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(1, 2), -4, 4), *noun1)),
		    JArray<JInt>(Dimensions(2, 2, 3), -2, -1, 0, -2, -2, -2));

  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(2, 0, 4)))),
					     JArray<JInt>(Dimensions(2, 0, 4)));
  BOOST_CHECK_EQUAL((*(*otherverbwithinsert)(m, JArray<JInt>(Dimensions(3, 10, 0, 4)))),
		    JArray<JInt>(Dimensions(3, 10, 0, 4)));
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

  shared_ptr<JNoun> rank_array2(new JArray<JInt>(Dimensions(1,3), -1, 0, 0));
  shared_ptr<JVerb> sum_rank2(boost::static_pointer_cast<JVerb>(RankConjunction()(m, sum, rank_array2)));
  JArray<JInt> test_subject2(Dimensions(3, 2,3,4), 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24);
  BOOST_CHECK_EQUAL(*(*sum_rank2)(m, test_subject2), JArray<JInt>(Dimensions(2, 2,4), 15,18,21,24,51,54,57,60));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( parsercombinators )

BOOST_AUTO_TEST_CASE( test_regex_parser ) {
  RegexParser<string::iterator> parser("a+b+");
  
  string test("aaaabbbbcccc");
  string::iterator iter = test.begin();
  shared_ptr<vector<string> > v(parser.parse(&iter, test.end()));
  BOOST_CHECK_EQUAL((*v)[0], "aaaabbbb");
  BOOST_CHECK(iter == test.begin() + 8);

  test = "bbbcccc";
  iter = test.begin();
  BOOST_CHECK_THROW( parser.parse(&iter, test.end()),  MatchFailure);
}

BOOST_AUTO_TEST_CASE ( test_constant_regexp ) {
  ParseConstant<string::iterator> parser("a+b+");
  string test("a+b+a+b+");

  string::iterator iter = test.begin();
  parser.parse(&iter, test.end());
  parser.parse(&iter, test.end());
  BOOST_CHECK_THROW( parser.parse(&iter, test.end()), MatchFailure);
}

typedef ParseOr<string::iterator, shared_ptr<vector<string> > > StringParseOr;
typedef shared_ptr<StringParseOr> StringParseOrPtr;
typedef shared_ptr<Parser<string::iterator, shared_ptr<vector<string> > > > ParserPtr;

BOOST_AUTO_TEST_CASE ( test_parser_or ) {
  string test("aaaabbbbbcccdddeee");
  
  StringParseOr::OurParserPtr parserPtr(new RegexParser<string::iterator>("bbb"));
  StringParseOrPtr parser(new StringParseOr(parserPtr));
  parser = parser->add_or(StringParseOr::OurParserPtr(new RegexParser<string::iterator>("aaa")));
  parser = parser->add_or(StringParseOr::OurParserPtr(new RegexParser<string::iterator>("aaaa")));
  parser = parser->add_or(StringParseOr::OurParserPtr(new RegexParser<string::iterator>("abbb")));
  parser = parser->add_or(StringParseOr::OurParserPtr(new RegexParser<string::iterator>("bb")));
  
  string::iterator begin = test.begin();
  parser->parse(&begin, test.end());
  
  BOOST_CHECK_EQUAL(distance(test.begin(), begin), 3);
  parser->parse(&begin, test.end());
  BOOST_CHECK_EQUAL(distance(test.begin(), begin), 7);
  shared_ptr<vector<string> > v(parser->parse(&begin, test.end()));
  BOOST_CHECK_EQUAL(string("bb"), (*v)[0]);
  BOOST_CHECK_THROW(parser->parse(&begin, test.end()), MatchFailure);
}

BOOST_AUTO_TEST_CASE ( test_interspersed_parser ) {
  Parser<string::iterator, StringVecPtr>::Ptr p(new RegexParser<string::iterator>("ab\\s+"));
  Parser<string::iterator, StringVecPtr>::Ptr i(new RegexParser<string::iterator>("-\\d+-"));
  InterspersedParser<string::iterator, StringVecPtr, StringVecPtr> parser(p, i);

  string test1("ab  -123213-ab   -232-");
  string::iterator iter = test1.begin();
  shared_ptr<deque<StringVecPtr > > v(parser.parse(&iter, test1.end()));
  BOOST_CHECK(iter == test1.begin() + 17);
  BOOST_CHECK_EQUAL(v->at(0)->at(0),"ab  ");
  BOOST_CHECK_EQUAL(v->at(1)->at(0),"ab   ");

  string test2("yadayada");
  iter = test2.begin();
  shared_ptr<deque<StringVecPtr > > v2(parser.parse(&iter, test2.end()));
  BOOST_CHECK_EQUAL(v2->size(), 0);
  BOOST_CHECK(iter == test2.begin());
  
  InterspersedParser1<string::iterator, StringVecPtr, StringVecPtr> parser2(p, i);
  BOOST_CHECK_THROW(parser2.parse(&iter, test2.end()), MatchFailure);
  
  
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( jparser )

BOOST_AUTO_TEST_CASE ( integerparse ) {
  IntegerParser<string::iterator> parser;
  
  string test1("_10231");
  string::iterator iter = test1.begin();
  BOOST_CHECK_EQUAL(parser.parse(&iter, test1.end()), -10231);
  BOOST_CHECK(iter == test1.end());

  string test2("102131");
  iter = test2.begin();
  BOOST_CHECK_EQUAL(parser.parse(&iter, test2.end()), 102131);
  BOOST_CHECK(iter == test2.end());  

  string test3(" _23132");
  string::iterator cache = iter = test3.begin();
  BOOST_CHECK_THROW(parser.parse(&iter, test3.end()), MatchFailure);
  BOOST_CHECK(iter ==  cache);
}

BOOST_AUTO_TEST_CASE ( floatingpointparse ) {
  FloatingPointParser<string::iterator> parser;
  string test1("_1023");
  string::iterator iter = test1.begin();
  BOOST_CHECK_THROW( parser.parse(&iter, test1.end()), MatchFailure );
  
  string test2 = "_1023.123";
  iter = test2.begin();
  BOOST_CHECK_EQUAL( parser.parse(&iter, test2.end()), -1023.123);

  string test3 = "_1023e10";
  iter = test3.begin();
  BOOST_CHECK_EQUAL( parser.parse(&iter, test3.end()), -1023e10);

  string test4 = "_1023.1234e_10";
  iter = test4.begin();
  BOOST_CHECK_EQUAL( parser.parse(&iter, test4.end()), -1023.1234e-10);

  string test5 = "_.1";
  iter = test5.begin();
  BOOST_CHECK_EQUAL( parser.parse(&iter, test5.end()), -0.1);

  string test6 = "_.e1";
  iter = test6.begin();
  BOOST_CHECK_THROW( parser.parse(&iter, test6.end()), MatchFailure);
}

BOOST_AUTO_TEST_CASE ( numberparser ) { 
  NumberParser<string::iterator> parser;
  string test1("_11329");
  string::iterator iter = test1.begin();
  BOOST_CHECK_EQUAL(*parser.parse(&iter, test1.end()), ParsedNumber<JInt>(-11329));

  string test2("123213.131e1");
  iter = test2.begin();
  BOOST_CHECK_EQUAL(*parser.parse(&iter, test2.end()), ParsedNumber<JFloat>(123213.131e1));
}

BOOST_AUTO_TEST_CASE ( noun_parser ) {
  ParseNoun<string::iterator> parser;
  string test1("12 12 _1234");
  string::iterator iter = test1.begin();
  
  JNoun::Ptr n(parser.parse(&iter, test1.end()));
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(1, 3), 12, 12, -1234), *n);
  
  string test2("12 12 _1234e10 _1.1 _5.3e10 3");
  iter = test2.begin();
  JNoun::Ptr n2(parser.parse(&iter, test2.end()));
  BOOST_CHECK_EQUAL(JArray<JFloat>(Dimensions(1,6), 12.0, 12.0, -1234e10, -1.1, -5.3e10, 3.0), *n2);
}

BOOST_AUTO_TEST_CASE ( builtin_parser ) {
  JMachine::Ptr m = JMachine::new_machine();
  string test1("+/ i.");
  shared_ptr<vector<string> > symbols(m->list_symbols());
  Parser<string::iterator, JTokenBase::Ptr>::Ptr symbol_parser(new OperatorParser<string::iterator>
							     (symbols->begin(), symbols->end()));
  Parser<string::iterator, void>::Ptr ws_parser(new WhitespaceParser<string::iterator>());

  InterspersedParser1<string::iterator, JTokenBase::Ptr> parser(symbol_parser, ws_parser);
  
  string::iterator begin = test1.begin();
  BOOST_CHECK_EQUAL(parser.parse(&begin, test1.end())->size(), 3);
}
  

BOOST_AUTO_TEST_CASE ( sequence_parser ) {
  vector<string> builtins;
  builtins.push_back("+");
  builtins.push_back("-");
  builtins.push_back("i.");
  builtins.push_back("g.");
  builtins.push_back("c.");
  builtins.push_back("=.");
  builtins.push_back("=.");

  string test1("10.123 100.1 +i.abc_ asdads i.");
  
  JTokenizer<string::iterator>::Ptr parser
    (JTokenizer<string::iterator>::Instantiate(builtins.begin(), builtins.end()));
  string::iterator iter = test1.begin();
  
  JTokenizer<string::iterator>::return_type res( parser->parse(&iter, test1.end( ) ) );
  
  // JTokenSequence* js(static_cast<JTokenSequence*>(res.get()));
  // BOOST_CHECK_EQUAL(distance(js->begin(), js->end()), 6);
  // JTokenSequence::const_iterator res_iter = js->begin();
  
  // BOOST_CHECK_EQUAL((*res_iter)->get_j_ast_elem_type(), j_ast_elem_type_noun);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_operator);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_operator);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_name);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_name);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_operator);

  // BOOST_CHECK(++res_iter == js->end());

  // string test2("10.123 (+ - 3) 120 afadsd_");
  // iter = test2.begin();
  // res = parser->parse(&iter, test2.end());
  // BOOST_CHECK_EQUAL(res->get_j_ast_elem_type(), j_ast_elem_type_sequence);

  // js = static_cast<JTokenSequence*>(res.get());
  // BOOST_CHECK_EQUAL(distance(js->begin(), js->end()), 4);
  
  // res_iter = js->begin();
  // BOOST_CHECK_EQUAL((*res_iter)->get_j_ast_elem_type(), j_ast_elem_type_noun);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_sequence);
  // JTokenSequence::const_iterator res_iter2 = res_iter;
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_noun);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_name);
  // js = static_cast<JTokenSequence*>(res_iter2->get());
  // BOOST_CHECK_EQUAL(distance(js->begin(), js->end()), 3);
  // res_iter = js->begin();

  // BOOST_CHECK_EQUAL((*res_iter)->get_j_ast_elem_type(), j_ast_elem_type_operator);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_operator);
  // BOOST_CHECK_EQUAL((*++res_iter)->get_j_ast_elem_type(), j_ast_elem_type_noun);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE ( criteria )

using namespace ::J::JEvaluator;

BOOST_AUTO_TEST_CASE (criteria0) {
  JMachine::Ptr m(JMachine::new_machine());
  JWordCriteria<JVerb> criteria(m);
  JTokenBase::Ptr token(JTokenOperator::Instantiate("+"));
  
  BOOST_CHECK(criteria(token));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE ( transformation_rules )

using namespace ::J::JEvaluator;

BOOST_AUTO_TEST_CASE ( transformation_rule0 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleMonad0 monad0(m);

  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), 
	     JTokenWord<JNoun>::Instantiate(shared_ptr<JNoun>(new JArray<JInt>(Dimensions(2,2,2), 0, 1,2,3))));
  lst.insert(lst.end(), JTokenRParen::Instantiate());
  
  BOOST_CHECK(monad0(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  list<JTokenBase::Ptr>::iterator it(lst.begin());
  ++it;
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(2,2,2), 0, -1, -2, -3),
		    *get_word<JNoun>(*it, m));
}

BOOST_AUTO_TEST_CASE ( transformation_rule1 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleMonad1 monad1(m);

  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (shared_ptr<JNoun>(new JArray<JInt>(Dimensions(1, 3), 1,  2,  3))));
  
  BOOST_CHECK(monad1(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  BOOST_CHECK_EQUAL(*get_word<JNoun>(lst.back(), m),
		    JArray<JInt>(Dimensions(1, 3), -1, -2, -3));
}
  
BOOST_AUTO_TEST_CASE ( transformation_rule2 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleDyad2 rule(m);

  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (shared_ptr<JNoun>(new JArray<JInt>(Dimensions(2, 3, 2), 6,  2,  3, 1, 2, 3))));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (shared_ptr<JNoun>(new JArray<JInt>(Dimensions(1, 3), 1,  2,  3))));
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  BOOST_CHECK_EQUAL(JArray<JInt>(Dimensions(2, 3, 2), 5,1,1,-1, -1,0),
		    *get_word<JNoun>(lst.back(), m));
}
  
BOOST_AUTO_TEST_CASE ( transformation_rule3 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleAdverb3 rule(m);
  
  list<JTokenBase::Ptr> lst; 
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("+"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("/"));
  lst.insert(lst.end(), JTokenRParen::Instantiate());
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  JArray<JInt> testObj(Dimensions(1, 5), 1,2,3,4,5);
  list<JTokenBase::Ptr>::iterator it(lst.begin());
  ++it;
  
  BOOST_CHECK_EQUAL(*(*get_word<JVerb>(*it, m))(m, testObj),
		    JArray<JInt>(Dimensions(0), 15));
}

BOOST_AUTO_TEST_CASE ( transformation_rule4 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleConjunction4 rule(m);
  
  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("+"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("\""));
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (JNoun::Ptr(new JArray<JInt>(Dimensions(0), 1))));
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  JVerb::Ptr verb(get_word<JVerb>(lst.back(), m));
  
  BOOST_CHECK_EQUAL(*(*verb)(m, JArray<JInt>(Dimensions(2, 2, 3), 1,2,3,4,5,6),
			     JArray<JInt>(Dimensions(1, 3), 1,2,3)),
		    JArray<JInt>(Dimensions(2,2, 3), 2,4, 6, 5,7, 9));
}
		  

BOOST_AUTO_TEST_CASE ( transformation_rule5 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleFork5 rule(m);
  
  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (JNoun::Ptr(new JArray<JInt>(Dimensions(0), -11))));
  lst.insert(lst.end(), JTokenOperator::Instantiate("+"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  
  JNoun::Ptr noun(new JArray<JInt>(Dimensions(0), 1));
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  
  JVerb::Ptr verb(get_word<JVerb>(lst.back(), m));
  BOOST_CHECK_EQUAL(*(*verb)(m, *noun, *noun),
		    JArray<JInt>(Dimensions(0), -11));

  BOOST_CHECK_EQUAL(*(*verb)(m, *noun),
		    JArray<JInt>(Dimensions(0), -12));

  lst.clear();
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("+"));
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  
  verb = get_word<JVerb>(lst.back(), m);
  JNoun::Ptr noun2(new JArray<JInt>(Dimensions(1, 3), 1,2,3));
  
  BOOST_CHECK_EQUAL(*(*verb)(m, *noun2), 
		    JArray<JInt>(Dimensions(1, 3), -2, -4, -6));

  BOOST_CHECK_EQUAL(*(*verb)(m, *noun2, *noun),
		    JArray<JInt>(Dimensions(1,3), -2, -2, -2));

  lst.clear();
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenCap::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  
  verb = get_word<JVerb>(lst.back(), m);
  BOOST_CHECK_EQUAL(*(*verb)(m, JArray<JInt>(Dimensions(0), 1)),
		    JArray<JInt>(Dimensions(0), 1));

  BOOST_CHECK_EQUAL(*(*verb)(m, 
			     JArray<JInt>(Dimensions(0), 1),
			     JArray<JInt>(Dimensions(0), 1)),
		    JArray<JInt>(Dimensions(0), 0));
}

BOOST_AUTO_TEST_CASE ( transformation_rule6 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleBident6 rule(m);
  
  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("/"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("\\"));
  lst.insert(lst.end(), JTokenDummy::Instantiate());

  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  
  list<JTokenBase::Ptr>::iterator iter(lst.begin());
  JAdverb::Ptr adverb(get_word<JAdverb>(*++iter, m));
  JVerb::Ptr plus(new PlusVerb());
  JVerb::Ptr total(boost::static_pointer_cast<JVerb>((*adverb)(m, plus)));
  
  BOOST_CHECK_EQUAL(*(*total)(m, JArray<JInt>(Dimensions(2, 3, 3), 1,2,3,4,5,6, 7, 8, 9)),
		    JArray<JInt>(Dimensions(2, 3, 3), 1,2,3, 5,7,9, 12,15,18));
  
  
  lst.clear();
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("+"));
  lst.insert(lst.end(), JTokenOperator::Instantiate("-"));
  lst.insert(lst.end(), JTokenDummy::Instantiate());

  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  
  iter = lst.begin();
  JVerb::Ptr verb(get_word<JVerb>(*++iter, m));
  BOOST_CHECK_EQUAL(*(*verb)(m, 
			     JArray<JInt>(Dimensions(0), 1),
			     JArray<JInt>(Dimensions(0), 5)),
		    JArray<JInt>(Dimensions(0), -4));
  
  lst.clear();
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenOperator::Instantiate("\""));
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (JNoun::Ptr(new JArray<JInt>(Dimensions(0), 1))));
  lst.insert(lst.end(), JTokenRParen::Instantiate());

  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 3);
  
  iter = lst.begin(); 
  adverb = get_word<JAdverb>(*++iter, m);
  verb = boost::static_pointer_cast<JVerb>((*adverb)(m, plus));
  
  BOOST_CHECK_EQUAL(*(*verb)(m, 
			     JArray<JInt>(Dimensions(3, 2,2,3), 10,10,10,10,10,10,10,10,10,10,10,10),
			     JArray<JInt>(Dimensions(2, 2, 3), 10, 10, 10,10,10,10)),
		    JArray<JInt>(Dimensions(3,2,2,3), 20,20,20,20,20,20,20,20,20,20,20,20));
}

BOOST_AUTO_TEST_CASE ( transformation_rule7 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleAssignment7 rule(m);
  
  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenName::Instantiate("a"));
  lst.insert(lst.end(), JTokenAssignment::Instantiate("=:"));
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (JNoun::Ptr(new JArray<JInt>(Dimensions(0), 10))));
  lst.insert(lst.end(), JTokenDummy::Instantiate());
  
  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  
  optional<JWord::Ptr> word(m->lookup_name("a"));
  BOOST_CHECK(word);
  BOOST_CHECK_EQUAL(static_cast<JNoun&>(**word),
		    JArray<JInt>(Dimensions(0), 10));
}

BOOST_AUTO_TEST_CASE ( transformation_rule8 ) {
  JMachine::Ptr m(JMachine::new_machine());
  JRuleParens8 rule(m);
  
  list<JTokenBase::Ptr> lst;
  lst.insert(lst.end(), JTokenLParen::Instantiate());
  lst.insert(lst.end(), JTokenWord<JNoun>::Instantiate
	     (JNoun::Ptr(new JArray<JInt>(Dimensions(0), 25))));
  lst.insert(lst.end(), JTokenRParen::Instantiate());
  lst.insert(lst.end(), JTokenDummy::Instantiate());

  BOOST_CHECK(rule(&lst, lst.begin()));
  BOOST_CHECK_EQUAL(lst.size(), 2);
  
  BOOST_CHECK_EQUAL(*get_word<JNoun>(lst.front(), m),
		    JArray<JInt>(Dimensions(0), 25));
}

BOOST_AUTO_TEST_CASE ( big_eval_loop_test ) {
  JMachine::Ptr m(JMachine::new_machine());
  
  vector<JTokenBase::Ptr> program;

  program.push_back(JTokenStart::Instantiate());
  program.push_back(JTokenOperator::Instantiate("+"));
  program.push_back(JTokenOperator::Instantiate("/"));
  program.push_back(JTokenWord<JNoun>::Instantiate
		    (JNoun::Ptr(new JArray<JInt>(Dimensions(2, 2, 3), 1,2,3,4,5,6))));

  JNoun::Ptr res(boost::static_pointer_cast<JNoun>(big_eval_loop(m, program.rbegin(), program.rend())));
  BOOST_CHECK_EQUAL(*res, JArray<JInt>(Dimensions(1, 3), 5, 7, 9));

  program.clear();
  program.push_back(JTokenStart::Instantiate());
  program.push_back(JTokenOperator::Instantiate("+"));
  program.push_back(JTokenOperator::Instantiate("/"));
  program.push_back(JTokenLParen::Instantiate());
  program.push_back(JTokenOperator::Instantiate("\""));
  program.push_back(JTokenWord<JNoun>::Instantiate
		    (JNoun::Ptr(new JArray<JInt>(Dimensions(0), 1))));
  program.push_back(JTokenRParen::Instantiate());
  program.push_back(JTokenWord<JNoun>::Instantiate
		    (JNoun::Ptr(new JArray<JInt>(Dimensions(2, 2, 3), 1,2,3,4,5,6))));
  
  res = boost::static_pointer_cast<JNoun>(big_eval_loop(m, program.rbegin(), program.rend()));
  BOOST_CHECK_EQUAL(*res, JArray<JInt>(Dimensions(1, 2), 6, 15));
  
}

BOOST_AUTO_TEST_CASE( executor_test ) {
  JMachine::Ptr m(JMachine::new_machine());
  JExecutor executor(m);
  JNoun::Ptr res(boost::static_pointer_cast<JNoun>(executor("10 10 10 + 20 20 20")));

  BOOST_CHECK_EQUAL(*executor("10"), JArray<JInt>(Dimensions(0), 10));
  BOOST_CHECK_EQUAL(*executor("30 30 30"), *res);
  
  BOOST_CHECK_EQUAL(*executor("a=:10 10 10 + - 10"), *executor("0 0 0"));
}

BOOST_AUTO_TEST_CASE ( executor_shape_test ) {
  JMachine::Ptr m(JMachine::new_machine());
  JExecutor executor(m);
  BOOST_CHECK_EQUAL(*executor("10 $ 1"), *executor("1 1 1 1 1 1 1 1 1 1"));
  BOOST_CHECK_EQUAL(*executor("2 2 $ 1 2 3"), JArray<JInt>(Dimensions(2,2,2), 
							   1, 2,3,1));

  BOOST_CHECK_EQUAL(*executor("0 $ 1 2 3"), JArray<JInt>(Dimensions(1,0)));
  BOOST_CHECK_EQUAL(*executor("$ 2 2 2 $ 1 2 3"), *executor("2 2 2"));
  BOOST_CHECK_EQUAL(*executor("$ $ 2 2 2 $ 1 2 3"), *executor("1 $ 3"));
  BOOST_CHECK_EQUAL(*executor("$ $ $ 2 2 2 $ 1 2 3"), *executor("1 $ 1"));
  BOOST_CHECK_EQUAL(*executor("2 3 4 $ 1 2 3 4 5 6 7"),
		    JArray<JInt>(Dimensions(3, 2,3,4), 
				 1,2,3,4,5,6,7,
				 1,2,3,4,5,6,7,
				 1,2,3,4,5,6,7,
				 1,2,3));

  BOOST_CHECK_EQUAL(*executor("10 $ 10 0 10 $ 1 2 3"),
		    JArray<JInt>(Dimensions(3, 10, 0, 10)));
  
  BOOST_CHECK_EQUAL(*executor("1 2 3 ($\"1 0) 1 2 3"),
		    JArray<JInt>(Dimensions(4, 3, 1,2,3),
				 1,1,1,1,1,1,
				 2,2,2,2,2,2,
				 3,3,3,3,3,3));
  BOOST_CHECK_EQUAL(*executor("0 0 $ 0 0 10 $ 0"), 
		    JArray<JInt>(Dimensions(4, 0, 0, 0, 10)));
  BOOST_CHECK_THROW(*executor("2 2 $ 0 10 10 $ 10"), 
		    JIllegalDimensionsException)
}

BOOST_AUTO_TEST_CASE ( test_ravel_append ) {
  JMachine::Ptr m(JMachine::new_machine());
  JExecutor executor(m);
  
  BOOST_CHECK_EQUAL(*executor(", 2 3 $ 1 2 3 4 5 6"), 
		    *executor("1 2 3 4 5 6"));
}

BOOST_AUTO_TEST_SUITE_END()
  
  
