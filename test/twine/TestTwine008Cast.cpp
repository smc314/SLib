/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine008Cast_ToCharStar();
void TestTwine008Cast_ToCStr();
void TestTwine008Cast_ToXmlChar();

void TestTwine008Cast()
{
	TestTwine008Cast_ToCharStar();
	TestTwine008Cast_ToCStr();
	TestTwine008Cast_ToXmlChar();

}

void TestTwine008Cast_ToCharStar()
{
	BEGIN_TEST_METHOD( "TestTwine008Cast_ToCharStar" )

	twine t1 = "12345678901234567890";
	twine t2 = "Something that will be longer than our 32 byte inner buffer size";

	// Simple assignment should work for small strings
	const char* c1 = t1();
	const char* c2 = t1.c_str();
	ASSERT_EQUALS( c1, c2, "t1() != t1.c_str()");

	// Simple assignment should work for large strings
	c1 = t2();
	c2 = t2.c_str();
	ASSERT_EQUALS( c1, c2, "t2() != t2.c_str()");

	// When small strings become large strings, everything should still work:
	t1 += "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 20 + 26 = 46, which is > 32.
	c1 = t1();
	c2 = t1.c_str();
	ASSERT_EQUALS( c1, c2, "t1() != t1.c_str()");

	END_TEST_METHOD
}

void TestTwine008Cast_ToCStr()
{
	BEGIN_TEST_METHOD( "TestTwine008Cast_ToCStr" )

	twine t1 = "12345678901234567890";
	twine t2 = "Something that will be longer than our 32 byte inner buffer size";

	// Simple assignment should work for small strings
	const char* c1 = t1();
	const char* c2 = t1.c_str();
	ASSERT_EQUALS( c1, c2, "t1() != t1.c_str()");

	// Simple assignment should work for large strings
	c1 = t2();
	c2 = t2.c_str();
	ASSERT_EQUALS( c1, c2, "t2() != t2.c_str()");

	// When small strings become large strings, everything should still work:
	t1 += "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 20 + 26 = 46, which is > 32.
	c1 = t1();
	c2 = t1.c_str();
	ASSERT_EQUALS( c1, c2, "t1() != t1.c_str()");

	END_TEST_METHOD
}

void TestTwine008Cast_ToXmlChar()
{
	BEGIN_TEST_METHOD( "TestTwine008Cast_ToXmlChar" )

	twine t1 = "12345678901234567890";
	twine t2 = "Something that will be longer than our 32 byte inner buffer size";

	// Simple assignment should work for small strings
	const char* c1 = t1();
	const xmlChar* c2 = t1;
	ASSERT_EQUALS( (xmlChar*)c1, c2, "t1() != (const xmlChar*)t1");

	// Simple assignment should work for large strings
	c1 = t2();
	c2 = t2;
	ASSERT_EQUALS( (xmlChar*)c1, c2, "t2() != (const xmlChar*)t2");

	// When small strings become large strings, everything should still work:
	t1 += "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 20 + 26 = 46, which is > 32.
	c1 = t1();
	c2 = t1;
	ASSERT_EQUALS( (xmlChar*)c1, c2, "t1() != (const xmlChar*)t1");


	END_TEST_METHOD
}

