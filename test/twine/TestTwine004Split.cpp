/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine004Split_NoResults();
void TestTwine004Split_Path();
void TestTwine004Split_Word();

void TestTwine004Split()
{
	TestTwine004Split_NoResults();
	TestTwine004Split_Path();
	TestTwine004Split_Word();

}

void TestTwine004Split_NoResults()
{
	BEGIN_TEST_METHOD( "TestTwine004Split_NoResults" )

	twine t1 = "SomethingThatHasNoSplitOpportunities";
	vector<twine> splits = t1.split( "_" );
	if(m_log_steps){
		printf("++ Splitting (%s) on '_'\n", t1() );
		printf("++ split results (%d)\n", (int)splits.size() );
		for(size_t i = 0; i < splits.size(); i++){
			printf("++ split[%d] = (%s)\n", (int)i, splits[i]() );
		}
	}
	ASSERT_EQUALS( 1, splits.size(), "splits.size() != 1");

	END_TEST_METHOD
}

void TestTwine004Split_Path()
{

	BEGIN_TEST_METHOD( "TestTwine004Split_Path" )

	twine t1 = "one/two/three/four/five/six/seven";
	vector<twine> splits = t1.split("/");

	if(m_log_steps){
		printf("++ Splitting (%s) on '/'\n", t1() );
		printf("++ split results (%d)\n", (int)splits.size() );
		for(size_t i = 0; i < splits.size(); i++){
			printf("++ split[%d] = (%s)\n", (int)i, splits[i]() );
		}
	}
	ASSERT_EQUALS( 7, splits.size(), "splits.size() != 7");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine004Split_Word()
{

	BEGIN_TEST_METHOD( "TestTwine004Split_Word" )

	twine t1 = "ALongStringLongForgottenLongBeforeLongKnewAboutLongIt";
	vector<twine> splits = t1.split("Long");

	if(m_log_steps){
		printf("++ Splitting (%s) on 'Long'\n", t1() );
		printf("++ split results (%d)\n", (int)splits.size() );
		for(size_t i = 0; i < splits.size(); i++){
			printf("++ split[%d] = (%s)\n", (int)i, splits[i]() );
		}
	}
	ASSERT_EQUALS( 6, splits.size(), "splits.size() != 7");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

