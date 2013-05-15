/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

#include "TestTwine001Allocation.cpp"
#include "TestTwine002Copying.cpp"
#include "TestTwine003Assignment.cpp"
#include "TestTwine004Appending.cpp"
#include "TestTwine005Split.cpp"
#include "TestTwine006Convert.cpp"
#include "TestTwine007ArrayAccess.cpp"
#include "TestTwine008Cast.cpp"
#include "TestTwine009Compare.cpp"
#include "TestTwine010CheckSize.cpp"

void TestTwine000()
{
	m_test_category = "SLib::twine Testing";

	TestTwine001Allocation();
	TestTwine002Copying();
	TestTwine003Assignment();
	TestTwine004Appending();
	TestTwine005Split();
	TestTwine006Convert();
	TestTwine007ArrayAccess();
	TestTwine008Cast();
	TestTwine009Compare();
	TestTwine010CheckSize();
}

