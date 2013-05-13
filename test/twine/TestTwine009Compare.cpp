/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine009Compare_ToTwine();
void TestTwine009Compare_ToCharStar();
void TestTwine009Compare_ToTwineCount();
void TestTwine009Compare_ToCharStarCount();
//void TestTwine009Compare_ToInt();
//void TestTwine009Compare_ToFloat();

void TestTwine009Compare()
{
	TestTwine009Compare_ToTwine();
	TestTwine009Compare_ToCharStar();
	TestTwine009Compare_ToTwineCount();
	TestTwine009Compare_ToCharStarCount();
	//TestTwine009Compare_ToInt();
	//TestTwine009Compare_ToFloat();

}

void TestTwine009Compare_ToTwine()
{
	BEGIN_TEST_METHOD( "TestTwine009Compare_ToTwine" )
	
	twine empty1;
	twine empty2;
	twine short1 = "Something Short";
	twine short2 = "Something Short";
	twine long1 = "Something A Bit Longer that Will cause us to allocate memory";
	twine long2 = "Something A Bit Longer that Will cause us to allocate memory";

	// Ensure that everything has different data pointers:
	ASSERT_NOTEQUALS( empty1(), empty2(), "empty1 and empty2 point to the same memory");
	ASSERT_NOTEQUALS( short1(), short2(), "short1 and short2 point to the same memory");
	ASSERT_NOTEQUALS( long1(), long2(), "long1 and long2 point to the same memory");

	ASSERT_EQUALS( 0, empty1.compare( empty2 ), "empty1.compare( empty2 ) != 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short1 ), "empty1.compare( short1 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short2 ), "empty1.compare( short2 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long1 ), "empty1.compare( long1) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long2 ), "empty1.compare( long2) == 0");

	ASSERT_EQUALS( 0, short1.compare( short2 ), "short1.compare( short2 ) != 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty1 ), "short1.compare( empty1 ) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty2), "short1.compare( empty2) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( long1 ), "short1.compare( long1) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( long2 ), "short1.compare( long2) == 0");
	
	ASSERT_EQUALS( 0, long1.compare( long2 ), "long1.compare( long2 ) != 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty1 ), "long1.compare( empty1 ) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty2), "long1.compare( empty2) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( short1 ), "long1.compare( short1) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( short2 ), "long1.compare( short2) == 0");

	END_TEST_METHOD
}

void TestTwine009Compare_ToCharStar()
{
	BEGIN_TEST_METHOD( "TestTwine009Compare_ToCharStar" )

	twine empty1;
	const char* empty2 = NULL;
	twine short1 = "Something Short";
	const char* short2 = "Something Short";
	twine long1 = "Something A Bit Longer that Will cause us to allocate memory";
	const char* long2 = "Something A Bit Longer that Will cause us to allocate memory";

	// Ensure that everything has different data pointers:
	ASSERT_NOTEQUALS( empty1(), empty2, "empty1 and empty2 point to the same memory");
	ASSERT_NOTEQUALS( short1(), short2, "short1 and short2 point to the same memory");
	ASSERT_NOTEQUALS( long1(), long2, "long1 and long2 point to the same memory");

	ASSERT_EQUALS( 0, empty1.compare( empty2 ), "empty1.compare( empty2 ) != 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short1 ), "empty1.compare( short1 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short2 ), "empty1.compare( short2 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long1 ), "empty1.compare( long1) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long2 ), "empty1.compare( long2) == 0");

	ASSERT_EQUALS( 0, short1.compare( short2 ), "short1.compare( short2 ) != 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty1 ), "short1.compare( empty1 ) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty2), "short1.compare( empty2) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( long1 ), "short1.compare( long1) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( long2 ), "short1.compare( long2) == 0");
	
	ASSERT_EQUALS( 0, long1.compare( long2 ), "long1.compare( long2 ) != 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty1 ), "long1.compare( empty1 ) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty2), "long1.compare( empty2) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( short1 ), "long1.compare( short1) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( short2 ), "long1.compare( short2) == 0");

	END_TEST_METHOD
}

void TestTwine009Compare_ToTwineCount()
{
	BEGIN_TEST_METHOD( "TestTwine009Compare_ToTwineCount" )
	
	twine empty1;
	twine empty2;
	twine short1 = "Something Short";
	twine short2 = "Something Short";
	twine long1 = "Something A Bit Longer that Will cause us to allocate memory";
	twine long2 = "Something A Bit Longer that Will cause us to allocate memory";

	// Ensure that everything has different data pointers:
	ASSERT_NOTEQUALS( empty1(), empty2(), "empty1 and empty2 point to the same memory");
	ASSERT_NOTEQUALS( short1(), short2(), "short1 and short2 point to the same memory");
	ASSERT_NOTEQUALS( long1(), long2(), "long1 and long2 point to the same memory");

	ASSERT_EQUALS( 0, empty1.compare( empty2, 10 ), "empty1.compare( empty2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short1, 10 ), "empty1.compare( short1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short2, 10 ), "empty1.compare( short2, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long1, 10 ), "empty1.compare( long1, 10) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long2, 10 ), "empty1.compare( long2, 10) == 0");

	ASSERT_EQUALS( 0, short1.compare( short2, 10 ), "short1.compare( short2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty1, 10 ), "short1.compare( empty1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty2, 10), "short1.compare( empty2, 10) == 0");
	ASSERT_EQUALS( 0, short1.compare( long1, 10 ), "short1.compare( long1, 10) != 0");
	ASSERT_EQUALS( 0, short1.compare( long2, 10 ), "short1.compare( long2, 10) != 0");
	
	ASSERT_EQUALS( 0, long1.compare( long2, 10 ), "long1.compare( long2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty1, 10 ), "long1.compare( empty1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty2, 10), "long1.compare( empty2, 10) == 0");
	ASSERT_EQUALS( 0, long1.compare( short1, 10 ), "long1.compare( short1, 10) != 0");
	ASSERT_EQUALS( 0, long1.compare( short2, 10 ), "long1.compare( short2, 10) != 0");

	END_TEST_METHOD
}

void TestTwine009Compare_ToCharStarCount()
{
	BEGIN_TEST_METHOD( "TestTwine009Compare_ToCharStarCount" )

	twine empty1;
	const char* empty2 = NULL;
	twine short1 = "Something Short";
	const char* short2 = "Something Short";
	twine long1 = "Something A Bit Longer that Will cause us to allocate memory";
	const char* long2 = "Something A Bit Longer that Will cause us to allocate memory";

	// Ensure that everything has different data pointers:
	ASSERT_NOTEQUALS( empty1(), empty2, "empty1 and empty2 point to the same memory");
	ASSERT_NOTEQUALS( short1(), short2, "short1 and short2 point to the same memory");
	ASSERT_NOTEQUALS( long1(), long2, "long1 and long2 point to the same memory");

	ASSERT_EQUALS( 0, empty1.compare( empty2, 10 ), "empty1.compare( empty2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short1, 10 ), "empty1.compare( short1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( short2, 10 ), "empty1.compare( short2, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long1, 10 ), "empty1.compare( long1, 10) == 0");
	ASSERT_NOTEQUALS( 0, empty1.compare( long2, 10 ), "empty1.compare( long2, 10) == 0");

	ASSERT_EQUALS( 0, short1.compare( short2, 10 ), "short1.compare( short2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty1, 10 ), "short1.compare( empty1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, short1.compare( empty2, 10), "short1.compare( empty2, 10) == 0");
	ASSERT_EQUALS( 0, short1.compare( long1, 10 ), "short1.compare( long1, 10) != 0");
	ASSERT_EQUALS( 0, short1.compare( long2, 10 ), "short1.compare( long2, 10) != 0");
	
	ASSERT_EQUALS( 0, long1.compare( long2, 10 ), "long1.compare( long2, 10 ) != 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty1, 10 ), "long1.compare( empty1, 10 ) == 0");
	ASSERT_NOTEQUALS( 0, long1.compare( empty2, 10), "long1.compare( empty2, 10) == 0");
	ASSERT_EQUALS( 0, long1.compare( short1, 10 ), "long1.compare( short1, 10) != 0");
	ASSERT_EQUALS( 0, long1.compare( short2, 10 ), "long1.compare( short2, 10) != 0");

	END_TEST_METHOD
}

