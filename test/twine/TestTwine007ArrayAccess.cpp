/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine007ArrayAccess_Read();
void TestTwine007ArrayAccess_ReadWrite();
void TestTwine007ArrayAccess_BoundsChecking();

void TestTwine007ArrayAccess()
{
	TestTwine007ArrayAccess_Read();
	TestTwine007ArrayAccess_ReadWrite();
	TestTwine007ArrayAccess_BoundsChecking();

}

void TestTwine007ArrayAccess_Read()
{
	BEGIN_TEST_METHOD( "TestTwine007ArrayAccess_Read" )

	twine t1 = "12345678901234567890";

	ASSERT_EQUALS( '1', t1[0], "'1' != t1[0]" );
	ASSERT_EQUALS( '3', t1[2], "'3' != t1[2]" );
	ASSERT_EQUALS( '5', t1[4], "'5' != t1[4]" );
	ASSERT_EQUALS( '7', t1[6], "'7' != t1[6]" );
	ASSERT_EQUALS( '9', t1[8], "'9' != t1[8]" );
	ASSERT_EQUALS( '0', t1[9], "'0' != t1[9]" );

	END_TEST_METHOD
}

void TestTwine007ArrayAccess_ReadWrite()
{
	BEGIN_TEST_METHOD( "TestTwine007ArrayAccess_ReadWrite" )

	twine t1 = "12345678901234567890";
	for(size_t i = 10; i < t1.size(); i++){
		t1[i] = 'b';
	}

	ASSERT_EQUALS( 0, memcmp( t1(), "1234567890bbbbbbbbbb", t1.size() ), "t1 != '1234567890bbbbbbbbbb'" );

	END_TEST_METHOD
}

void TestTwine007ArrayAccess_BoundsChecking()
{
	BEGIN_TEST_METHOD( "TestTwine007ArrayAccess_BoundsChecking" )

	twine t1 = "12345678901234567890";

	try {
		t1[ 20 ]; // should throw an exception
		ASSERT_FALSE(true, "t1[20] should have thrown out of bounds exception.");
	} catch (AnException& ){
		ASSERT_TRUE(true, "We expect this.");
	}

	try {
		t1[ 40 ]; // should throw an exception
		ASSERT_FALSE(true, "t1[40] should have thrown out of bounds exception.");
	} catch (AnException& ){
		ASSERT_TRUE(true, "We expect this.");
	}

	END_TEST_METHOD
}

