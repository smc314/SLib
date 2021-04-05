/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <algorithm>

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <XmlHelpers.h>
#include <Timer.h>
using namespace SLib;

#include "HelixSqldoSearchField.h"
using namespace Helix::Build;


HelixSqldoSearchField::HelixSqldoSearchField() 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo()");

}

HelixSqldoSearchField::HelixSqldoSearchField(xmlNodePtr elem) 
{
	EnEx ee(FL, "HelixSqldo::HelixSqldo(xmlNodePtr elem)");

	name.getAttribute( elem, "name" );
	sql.getAttribute( elem, "sql" );
	type.getAttribute( elem, "type" );
	alias1.getAttribute( elem, "alias1" );
	alias2.getAttribute( elem, "alias2" );
	alias3.getAttribute( elem, "alias3" );
	alias4.getAttribute( elem, "alias4" );
	alias5.getAttribute( elem, "alias5" );
	if(type.empty()){
		type = "Contains"; // Default to a contains search
	}
}

twine HelixSqldoSearchField::NormalizeName() const
{
	if(name.size() == 0) return ""; // Bail out early

	twine ret( name ); // make a copy to work on

	// Replace Invalid Characters with a space
	for(size_t i = 0; i < ret.size(); i++) if(isalnum( ret[i] ) == 0) ret[i] = ' ';

	// CamelCase based on spaces
	for(size_t i = 0; i < ret.size(); i++){
		if(ret[i] == ' '){
			ret.erase(i, 1);
			if(ret.size() >= i){
				ret.ucase( i );
			}
			i--; // adjust the loop counter
		}
	}

	// Uppercase the first letter
	ret.ucase( 0 );

	// If it ends with ID, convert to Id
	if(ret.endsWith( "ID" )){
		ret.lcase( ret.size() - 1 );
	}
	
	return ret;
}
