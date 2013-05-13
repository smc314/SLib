/* **************************************************************************** */
/* This is an SLib test that is included in our SLibTest application.  This code  */
/* is included directly in SLibTest.cpp, so there is no need for additional     */
/* headers, etc.  Refer to SLibTest.cpp for the global variables that you have  */
/* access to.                                                                   */
/* Please ensure that this test is declared properly in SLibTest.h, and invoked */
/* inside SLibTest.cpp in the RunOneTable method - or wherever appropriate.     */
/* **************************************************************************** */

#include "TestDate001Allocation.cpp"
#include "TestDate002Copying.cpp"
#include "TestDate003Updating.cpp"

void TestDate000()
{
	m_test_category = "SLib::Date Testing";

	TestDate001Allocation();
	TestDate002Copying();
	TestDate003Updating();

}

