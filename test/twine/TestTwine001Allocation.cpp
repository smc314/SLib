/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine001Allocation_Empty();
void TestTwine001Allocation_Parms();

void TestTwine001Allocation()
{
	TestTwine001Allocation_Empty();
	TestTwine001Allocation_Parms();

}

void TestTwine001Allocation_Empty()
{
	BEGIN_TEST_METHOD( "TestTwine001Allocation_Empty" )

	twine t1;
	twine t2;
	twine* t3 = new twine();

	ASSERT_EQUALS(0, t1.size(), "t1.size() != 0");
	ASSERT_EQUALS(0, t1.length(), "t1.length() != 0");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t1.max_size(), "t1.max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t1.capacity(), "t1.capacity() != TWINE_SMALL_STRING");
	ASSERT_TRUE(t1.empty(), "t1.empty()");
	
	ASSERT_EQUALS(0, t2.size(), "t2.size() != 0");
	ASSERT_EQUALS(0, t2.length(), "t2.length() != 0");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t2.max_size(), "t2.max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t2.capacity(), "t2.capacity() != TWINE_SMALL_STRING");
	ASSERT_TRUE(t2.empty(), "t2.empty()");

	ASSERT_EQUALS(0, t3->size(), "t3->size() != 0");
	ASSERT_EQUALS(0, t3->length(), "t3->length() != 0");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t3->max_size(), "t3->max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t3->capacity(), "t3->capacity() != TWINE_SMALL_STRING");
	ASSERT_TRUE(t3->empty(), "t3->empty()");

	delete t3;

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine001Allocation_Parms()
{
	BEGIN_TEST_METHOD( "TestTwine001Allocation_Parms" )

	int res;
	twine t1( "SomethingShort" );
	twine t2( "SomethingABitLongerBut<32" );
	twine t3( "SomethingThatwillExceedTheThirtyTwoSizeEasily" );
	twine t4( "/" );

	ASSERT_NOTEQUALS(0, t1.size(), "t1.size() == 0");
	ASSERT_NOTEQUALS(0, t1.length(), "t1.length() == 0");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t1.max_size(), "t1.max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t1.capacity(), "t1.capacity() != TWINE_SMALL_STRING");
	ASSERT_FALSE(t1.empty(), "t1.empty()");
	res = memcmp("SomethingShort", t1(), t1.size());
	ASSERT_EQUALS(0, res, "t1: memcmp failed");
	
	ASSERT_NOTEQUALS(0, t2.size(), "t2.size() == 0");
	ASSERT_NOTEQUALS(0, t2.length(), "t2.length() == 0");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t2.max_size(), "t2.max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t2.capacity(), "t2.capacity() != TWINE_SMALL_STRING");
	ASSERT_FALSE(t2.empty(), "t2.empty()");
	res = memcmp("SomethingABitLongerBut<32", t2(), t2.size());
	ASSERT_EQUALS(0, res, "t2: memcmp failed");
	
	ASSERT_NOTEQUALS(0, t3.size(), "t3.size() == 0");
	ASSERT_NOTEQUALS(0, t3.length(), "t3.length() == 0");
	ASSERT_NOTEQUALS(TWINE_SMALL_STRING - 1, t3.max_size(), "t3.max_size() != TWINE_SMALL_STRING");
	ASSERT_NOTEQUALS(TWINE_SMALL_STRING - 1, t3.capacity(), "t3.capacity() != TWINE_SMALL_STRING");
	ASSERT_FALSE(t3.empty(), "t3.empty()");
	res = memcmp("SomethingThatwillExceedTheThirtyTwoSizeEasily", t3(), t3.size());
	ASSERT_EQUALS(0, res, "t3: memcmp failed");
	
	ASSERT_NOTEQUALS(0, t4.size(), "t4.size() == 0");
	ASSERT_NOTEQUALS(0, t4.length(), "t4.length() == 0");
	ASSERT_EQUALS(1, t4.size(), "t4.size() != 1");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t4.max_size(), "t4.max_size() != TWINE_SMALL_STRING");
	ASSERT_EQUALS(TWINE_SMALL_STRING - 1, t4.capacity(), "t4.capacity() != TWINE_SMALL_STRING");
	ASSERT_FALSE(t4.empty(), "t4.empty()");
	

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

