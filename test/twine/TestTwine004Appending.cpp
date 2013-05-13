/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

void TestTwine004Appending_FromTwine();
void TestTwine004Appending_FromTwineStar();
void TestTwine004Appending_FromCharStar();
void TestTwine004Appending_FromChar();
void TestTwine004Appending_FromSizeT();
void TestTwine004Appending_FromIntPtrT();
void TestTwine004Appending_FromFloat();

void TestTwine004Appending()
{

	TestTwine004Appending_FromTwine();
	TestTwine004Appending_FromTwineStar();
	TestTwine004Appending_FromCharStar();
	TestTwine004Appending_FromChar();
	TestTwine004Appending_FromSizeT();
	TestTwine004Appending_FromIntPtrT();
	TestTwine004Appending_FromFloat();

}

void TestTwine004Appending_FromTwine()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromTwine" )

	twine t1("SomethingShort");
	twine t2("Something That will exceed the 32 byte small internal size buffer.");
	twine t3;

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), t1.size() + 6, "c1.size() != t1.size() + 6" );
	ASSERT_EQUALS( c2.size(), t2.size() + 6, "c2.size() != t2.size() + 6" );
	ASSERT_EQUALS( c3.size(), t3.size() + 6, "c3.size() != t3.size() + 6" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), t1.data(), "c1.data() == t1.data()" );
	ASSERT_NOTEQUALS( c2.data(), t2.data(), "c2.data() == t2.data()" );
	ASSERT_NOTEQUALS( c3.data(), t3.data(), "c3.data() == t3.data()" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, t1(), t1.size() ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, t2(), t2.size() ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, t3(), t3.size() ), "c3 != t3" );

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

void TestTwine004Appending_FromTwineStar()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromTwineStar" )

	twine* t1 = new twine("SomethingShort");
	twine* t2 = new twine("Something That will exceed the 32 byte small internal size buffer.");
	twine* t3 = new twine();

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), t1->size() + 6, "c1.size() != t1.size() + 6" );
	ASSERT_EQUALS( c2.size(), t2->size() + 6, "c2.size() != t2.size() + 6" );
	ASSERT_EQUALS( c3.size(), t3->size() + 6, "c3.size() != t3.size() + 6" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), t1->data(), "c1.data() == t1.data()" );
	ASSERT_NOTEQUALS( c2.data(), t2->data(), "c2.data() == t2.data()" );
	ASSERT_NOTEQUALS( c3.data(), t3->data(), "c3.data() == t3.data()" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, t1->c_str(), t1->size() ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, t2->c_str(), t2->size() ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, t3->c_str(), t3->size() ), "c3 != t3" );

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

void TestTwine004Appending_FromCharStar()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromCharStar" )

	const char* t1 = "SomethingShort";
	const char* t2 = "Something That will exceed the 32 byte small internal size buffer.";
	const char* t3 = "";

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), strlen(t1) + 6, "c1.size() != strlen(t1) + 6" );
	ASSERT_EQUALS( c2.size(), strlen(t2) + 6, "c2.size() != strlen(t2) + 6" );
	ASSERT_EQUALS( c3.size(), strlen(t3) + 6, "c3.size() != strlen(t3) + 6" );

	// Ensure that they are using different pointers now (data is really copied)
	ASSERT_NOTEQUALS( c1.data(), t1, "c1.data() == t1" );
	ASSERT_NOTEQUALS( c2.data(), t2, "c2.data() == t2" );
	ASSERT_NOTEQUALS( c3.data(), t3, "c3.data() == t3" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, t1, strlen(t1) ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, t2, strlen(t2) ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, t3, strlen(t3) ), "c3 != t3" );

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

void TestTwine004Appending_FromChar()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromChar" )

	const char t1 = 'a';
	const char t2 = 'b';
	const char t3 = 'c';

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), 7, "c1.size() != 7" );
	ASSERT_EQUALS( c2.size(), 7, "c2.size() != 7" );
	ASSERT_EQUALS( c3.size(), 7, "c3.size() != 7" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 'a', c1()[6], "c1[6] != 'a'" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 'b', c2()[6], "c2[6] != 'b'" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 'c', c3()[6], "c3[6] != 'c'" );

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

void TestTwine004Appending_FromSizeT()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromSizeT" )

	size_t t1 = 12345;
	size_t t2 = 3456789;
	size_t t3 = 0;

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), 11, "c1.size() != 11" );
	ASSERT_EQUALS( c2.size(), 13, "c2.size() != 13" );
	ASSERT_EQUALS( c3.size(), 7, "c3.size() != 7" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, "12345", 5 ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, "3456789", 7 ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, "0", 1 ), "c3 != t3" );

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

void TestTwine004Appending_FromIntPtrT()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromIntPtrT" )

	intptr_t t1 = 12345;
	intptr_t t2 = 3456789;
	intptr_t t3 = 0;

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), 11, "c1.size() != 11" );
	ASSERT_EQUALS( c2.size(), 13, "c2.size() != 13" );
	ASSERT_EQUALS( c3.size(), 7, "c3.size() != 7" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, "12345", 5 ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, "3456789", 7 ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, "0", 1 ), "c3 != t3" );


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

void TestTwine004Appending_FromFloat()
{
	BEGIN_TEST_METHOD( "TestTwine004Appending_FromFloat" )

	float t1 = 12345.678;
	char t1c[64];
	float t2 = 34.56789;
	char t2c[64];
	float t3 = 0.0;
	char t3c[64];

	// Do it ourselves so we can check the results
	sprintf(t1c, "%f", t1);
	sprintf(t2c, "%f", t2);
	sprintf(t3c, "%f", t3);

	twine c1 = "prefix";
	twine c2 = "prefix";
	twine c3 = "prefix";
	c1 += t1;
	c2 += t2;
	c3 += t3;

	if(m_log_steps){
		printf("++ Float (%f) Twine (%s)\n", t1, c1() );
		printf("++ Float (%f) Twine (%s)\n", t2, c2() );
		printf("++ Float (%f) Twine (%s)\n", t3, c3() );
	}

	// Ensure that their sizes match
	ASSERT_EQUALS( c1.size(), strlen(t1c) + 6, "c1.size() != strlen(t1c) + 6" );
	ASSERT_EQUALS( c2.size(), strlen(t2c) + 6, "c2.size() != strlen(t2c) + 6" );
	ASSERT_EQUALS( c3.size(), strlen(t3c) + 6, "c3.size() != strlen(t3c) + 6" );

	// Ensure that the contents match
	ASSERT_EQUALS( 0, memcmp( c1(), "prefix", 6 ), "c1[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c1() + 6, t1c, strlen(t1c) ), "c1 != t1" );

	ASSERT_EQUALS( 0, memcmp( c2(), "prefix", 6 ), "c2[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c2() + 6, t2c, strlen(t2c) ), "c2 != t2" );

	ASSERT_EQUALS( 0, memcmp( c3(), "prefix", 6 ), "c3[0-5] != 'prefix'" );
	ASSERT_EQUALS( 0, memcmp( c3() + 6, t3c, strlen(t3c) ), "c3 != t3" );


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

