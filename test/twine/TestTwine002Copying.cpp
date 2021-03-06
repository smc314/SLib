/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine002Copying_FromTwine();
void TestTwine002Copying_FromCharStar();
void TestTwine002Copying_FromXmlCharStar();
void TestTwine002Copying_FromChar();
void TestTwine002Copying_FromXmlNodePtr();

void TestTwine002Copying()
{

	TestTwine002Copying_FromTwine();
	TestTwine002Copying_FromCharStar();
	TestTwine002Copying_FromXmlCharStar();
	TestTwine002Copying_FromChar();
	TestTwine002Copying_FromXmlNodePtr();

}

void TestTwine002Copying_FromTwine()
{
	BEGIN_TEST_METHOD( "TestTwine002Copying_FromTwine" )

	twine t1("SomethingShort");
	twine t2("Something That will exceed the 32 byte small internal size buffer.");
	twine t3;

	twine c1( t1 );
	twine c2( t2 );
	twine c3( t3 );

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), t1.size(), "c1.size() != t1.size()" );
	ASSERT_EQUALS( c2.size(), t2.size(), "c2.size() != t2.size()" );
	ASSERT_EQUALS( c3.size(), t3.size(), "c3.size() != t3.size()" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), t1.data(), "c1.data() == t1.data()" );
	ASSERT_NOTEQUALS( c2.data(), t2.data(), "c2.data() == t2.data()" );
	ASSERT_NOTEQUALS( c3.data(), t3.data(), "c3.data() == t3.data()" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), t1(), t1.size() ), "c1 != t1" );
	ASSERT_EQUALS( 0, memcmp( c2(), t2(), t2.size() ), "c2 != t2" );
	ASSERT_EQUALS( 0, memcmp( c3(), t3(), t3.size() ), "c3 != t3" );

	// Ensure null termination:
	ASSERT_EQUALS( '\0', c1.data()[ c1.size() ], "c1 not null terminated");
	ASSERT_EQUALS( '\0', c2.data()[ c2.size() ], "c2 not null terminated");
	ASSERT_EQUALS( '\0', c3.data()[ c3.size() ], "c3 not null terminated");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine002Copying_FromCharStar()
{
	BEGIN_TEST_METHOD( "TestTwine002Copying_FromCharStar" )

	const char* t1 = "SomethingShort";
	const char* t2 = "Something That will exceed the 32 byte small internal size buffer.";
	const char* t3 = "";

	twine c1( t1 );
	twine c2( t2 );
	twine c3( t3 );

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), strlen(t1), "c1.size() != strlen(t1)" );
	ASSERT_EQUALS( c2.size(), strlen(t2), "c2.size() != strlen(t2)" );
	ASSERT_EQUALS( c3.size(), strlen(t3), "c3.size() != strlen(t3)" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), t1, "c1.data() == t1" );
	ASSERT_NOTEQUALS( c2.data(), t2, "c2.data() == t2" );
	ASSERT_NOTEQUALS( c3.data(), t3, "c3.data() == t3" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), t1, c1.size() ), "c1 != t1" );
	ASSERT_EQUALS( 0, memcmp( c2(), t2, c2.size() ), "c2 != t2" );
	ASSERT_EQUALS( 0, memcmp( c3(), t3, c3.size() ), "c3 != t3" );

	// Ensure null termination:
	ASSERT_EQUALS( '\0', c1.data()[ c1.size() ], "c1 not null terminated");
	ASSERT_EQUALS( '\0', c2.data()[ c2.size() ], "c2 not null terminated");
	ASSERT_EQUALS( '\0', c3.data()[ c3.size() ], "c3 not null terminated");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine002Copying_FromXmlCharStar()
{
	BEGIN_TEST_METHOD( "TestTwine002Copying_FromXmlCharStar" )

	const xmlChar* t1 = (const xmlChar*)"SomethingShort";
	const xmlChar* t2 = (const xmlChar*)"Something That will exceed the 32 byte small internal size buffer.";
	const xmlChar* t3 = (const xmlChar*)"";

	twine c1( t1 );
	twine c2( t2 );
	twine c3( t3 );

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), strlen((const char*)t1), "c1.size() != strlen(t1)" );
	ASSERT_EQUALS( c2.size(), strlen((const char*)t2), "c2.size() != strlen(t2)" );
	ASSERT_EQUALS( c3.size(), strlen((const char*)t3), "c3.size() != strlen(t3)" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), (char*)t1, "c1.data() == t1" );
	ASSERT_NOTEQUALS( c2.data(), (char*)t2, "c2.data() == t2" );
	ASSERT_NOTEQUALS( c3.data(), (char*)t3, "c3.data() == t3" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), t1, c1.size() ), "c1 != t1" );
	ASSERT_EQUALS( 0, memcmp( c2(), t2, c2.size() ), "c2 != t2" );
	ASSERT_EQUALS( 0, memcmp( c3(), t3, c3.size() ), "c3 != t3" );

	// Ensure null termination:
	ASSERT_EQUALS( '\0', c1.data()[ c1.size() ], "c1 not null terminated");
	ASSERT_EQUALS( '\0', c2.data()[ c2.size() ], "c2 not null terminated");
	ASSERT_EQUALS( '\0', c3.data()[ c3.size() ], "c3 not null terminated");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine002Copying_FromChar()
{
	BEGIN_TEST_METHOD( "TestTwine002Copying_FromChar" )

	twine c1( 'a' );
	twine c2( 'b' );
	twine c3( 'c' );

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), 1, "c1.size() != 1" );
	ASSERT_EQUALS( c2.size(), 1, "c2.size() != 1" );
	ASSERT_EQUALS( c3.size(), 1, "c3.size() != 1" );

	// Ensure that the contents match
	ASSERT_EQUALS( 'a', c1.data()[0], "c1 != 'a'" );
	ASSERT_EQUALS( 'b', c2.data()[0], "c2 != 'b'" );
	ASSERT_EQUALS( 'c', c3.data()[0], "c3 != 'c'" );

	// Ensure null termination:
	ASSERT_EQUALS( '\0', c1.data()[ c1.size() ], "c1 not null terminated");
	ASSERT_EQUALS( '\0', c2.data()[ c2.size() ], "c2 not null terminated");
	ASSERT_EQUALS( '\0', c3.data()[ c3.size() ], "c3 not null terminated");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

void TestTwine002Copying_FromXmlNodePtr()
{
	BEGIN_TEST_METHOD( "TestTwine002Copying_FromXmlNodePtr" )

	const xmlChar* t1 = (const xmlChar*)"ShortValue";
	const xmlChar* t2 = (const xmlChar*)"LongValue That will not be less than 32 chars.";
	const xmlChar* t3 = (const xmlChar*)"a";

	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode( doc, NULL, (const xmlChar*)"SLibTwineTesting", NULL);
	xmlNodePtr root = xmlDocGetRootElement(doc);
	xmlSetProp(root, (const xmlChar*)"t1", t1);
	xmlSetProp(root, (const xmlChar*)"t2", t2);
	xmlSetProp(root, (const xmlChar*)"t3", t3);

	twine c1( root, "t1" );
	twine c2( root, "t2" );
	twine c3( root, "t3" );

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), strlen((const char*)t1), "c1.size() != strlen(t1)" );
	ASSERT_EQUALS( c2.size(), strlen((const char*)t2), "c2.size() != strlen(t2)" );
	ASSERT_EQUALS( c3.size(), strlen((const char*)t3), "c3.size() != strlen(t3)" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), (char*)t1, "c1.data() == t1" );
	ASSERT_NOTEQUALS( c2.data(), (char*)t2, "c2.data() == t2" );
	ASSERT_NOTEQUALS( c3.data(), (char*)t3, "c3.data() == t3" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), t1, c1.size() ), "c1 != t1" );
	ASSERT_EQUALS( 0, memcmp( c2(), t2, c2.size() ), "c2 != t2" );
	ASSERT_EQUALS( 0, memcmp( c3(), t3, c3.size() ), "c3 != t3" );

	// Ensure null termination:
	ASSERT_EQUALS( '\0', c1.data()[ c1.size() ], "c1 not null terminated");
	ASSERT_EQUALS( '\0', c2.data()[ c2.size() ], "c2 not null terminated");
	ASSERT_EQUALS( '\0', c3.data()[ c3.size() ], "c3 not null terminated");

	// Useful macros:
	// ASSERT_EQUALS(a, b, "a is not equal to b, but it should be.")
	// ASSERT_NOTEQUALS(a, b, "a is equal to b, but it shouldn't be.")
	// ASSERT_TRUE(a, "a should be true, but it isn't.")
	// ASSERT_FALSE(a, "a should be false, but it isn't.")

	END_TEST_METHOD
}

