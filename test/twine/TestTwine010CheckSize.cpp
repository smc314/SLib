/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine010CheckSize_WithinBounds();
void TestTwine010CheckSize_OutOfBounds();

void TestTwine010CheckSize()
{
	TestTwine010CheckSize_WithinBounds();
	TestTwine010CheckSize_OutOfBounds();

}

void TestTwine010CheckSize_WithinBounds()
{
	BEGIN_TEST_METHOD( "TestTwine010CheckSize_WithinBounds" )

	const char* c1 = "Something less than 32";
	const char* c2 = "Something that will be bigger than 32";
	twine t1;
	twine t2;

	sprintf(t1.data(), "%s", c1 );
	t1.check_size();
	ASSERT_EQUALS( t1.size(), strlen( c1 ), "t1.size() != strlen( c1 )" );
	ASSERT_EQUALS( 0, memcmp( t1(), c1, t1.size() ), "t1 != c1" );
	ASSERT_EQUALS( '\0', t1[ t1.size() ], "t1 should be null terminated" );

	t2.reserve( strlen(c2) );
	sprintf(t2.data(), "%s", c2 );
	t2.check_size();
	ASSERT_EQUALS( t2.size(), strlen( c2 ), "t2.size() != strlen( c2 )" );
	ASSERT_EQUALS( 0, memcmp( t2(), c2, t2.size() ), "t2 != c2" );
	ASSERT_EQUALS( '\0', t2[ t2.size() ], "t2 should be null terminated" );

	END_TEST_METHOD
}

void TestTwine010CheckSize_OutOfBounds()
{
	BEGIN_TEST_METHOD( "TestTwine010CheckSize_OutOfBounds" )

	const char* c1 = "Something less than 32";
	const char* c2 = "Something that will be bigger than 32";
	twine t1;
	twine t2;

	sprintf(t1.data(), "%s", c2 ); // this may cause a segfault
	ASSERT_EXCEPTION(t1.check_size(), "t1.check_size() should cause an exception");
	ASSERT_NOTEQUALS( t1.size(), strlen( c1 ), "t1.size() == strlen( c1 )" );
	ASSERT_EQUALS( 0, memcmp( t1(), c1, t1.size() ), "t1 != c1" );
	ASSERT_EQUALS( '\0', t1[ t1.size() ], "t1 should be null terminated" );

	// don't use t2 here, because it may be overwritten by the above memory corruption


	END_TEST_METHOD
}

