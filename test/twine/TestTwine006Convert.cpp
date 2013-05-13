/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine006Convert_ToInt();
void TestTwine006Convert_ToFloat();
void TestTwine006Convert_ToDouble();

void TestTwine006Convert()
{
	TestTwine006Convert_ToInt();
	TestTwine006Convert_ToFloat();
	TestTwine006Convert_ToDouble();

}

void TestTwine006Convert_ToInt()
{
	BEGIN_TEST_METHOD( "TestTwine006Convert_ToInt" )

	twine t1 = "1";
	twine t2 = "12345";
	twine t3 = "456789";

	ASSERT_EQUALS( 1, t1.get_int(), "1 != t1.get_int()" );
	ASSERT_EQUALS( 12345, t2.get_int(), "12345 != t2.get_int()" );
	ASSERT_EQUALS( 456789, t3.get_int(), "456789 != t3.get_int()" );

	END_TEST_METHOD
}

void TestTwine006Convert_ToFloat()
{
	BEGIN_TEST_METHOD( "TestTwine006Convert_ToFloat" )

	twine t1 = "1.1";
	twine t2 = "12.345";
	twine t3 = "456.789";

	ASSERT_EQUALS( (float)1.1, t1.get_float(), "1.1 != t1.get_float()" );
	ASSERT_EQUALS( (float)12.345, t2.get_float(), "12.345 != t2.get_float()" );
	ASSERT_EQUALS( (float)456.789, t3.get_float(), "456.789 != t3.get_float()" );

	END_TEST_METHOD
}

void TestTwine006Convert_ToDouble()
{
	BEGIN_TEST_METHOD( "TestTwine006Convert_ToDouble" )

	twine t1 = "1.1";
	twine t2 = "12.345";
	twine t3 = "456.789";

	ASSERT_EQUALS( 1.1, t1.get_double(), "1.1 != t1.get_double()" );
	ASSERT_EQUALS( 12.345, t2.get_double(), "12.345 != t2.get_double()" );
	ASSERT_EQUALS( 456.789, t3.get_double(), "456.789 != t3.get_double()" );

	END_TEST_METHOD
}

