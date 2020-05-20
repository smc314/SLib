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

#include "HelixFS.h"
#include "HelixConfig.h"
#include "HelixExtractStrings.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixExtractStrings::HelixExtractStrings() 
{
	EnEx ee(FL, "HelixExtractStrings::HelixExtractStrings()");

}

HelixExtractStrings::HelixExtractStrings(const HelixExtractStrings& c)
{
	EnEx ee(FL, "HelixExtractStrings::HelixExtractStrings(const HelixExtractStrings& c)");

}

HelixExtractStrings& HelixExtractStrings::operator=(const HelixExtractStrings& c)
{
	EnEx ee(FL, "HelixExtractStrings::operator=(const HelixExtractStrings& c)");

	return *this;
}

HelixExtractStrings::HelixExtractStrings(const HelixExtractStrings&& c)
{
	EnEx ee(FL, "HelixExtractStrings::HelixExtractStrings(const HelixExtractStrings&& c)");

}

HelixExtractStrings& HelixExtractStrings::operator=(const HelixExtractStrings&& c)
{
	EnEx ee(FL, "HelixExtractStrings::operator=(const HelixExtractStrings&& c)");


	return *this;
}

HelixExtractStrings::~HelixExtractStrings()
{
	EnEx ee(FL, "HelixExtractStrings::~HelixExtractStrings()");

}

void HelixExtractStrings::Generate()
{
	EnEx ee(FL, "HelixExtractStrings::Generate");

	m_qdRoot = "../../qd";

	// Walk the layout files in all of the correct qd app folders
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		INFO(FL, "Processing package: %s", m_currentPackage() );
		FindAllLayoutFiles( );
	}

	// Also look at all of our c++ source for static labels requiring translation
	FindAllCPPFiles();

	// Once we've gathered all of our static labels, update the label file for each of
	// our qx applications
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		UpdateStaticLabelFile();
	}
}

void HelixExtractStrings::FindAllLayoutFiles()
{
	// Layout folder starts at QD Root + Package + source/layouts
	twine start; start.format("%s/%s/source/layouts", m_qdRoot(), m_currentPackage() );
	
	// check all of the normal files:
	vector<twine> files;
	try {
		files = File::listFiles(start);
	} catch (AnException&){
		// Exceptions here simply mean the folder was empty
	}
	for(int i = 0; i < (int)files.size(); i++){
		twine child = start + "/" + files[i];
		if(child.endsWith(twine(".xml"))){
			ProcessFile(child);
		}
	}

	// Layout files are all at the top level.  Don't recurse
}

void HelixExtractStrings::FindAllCPPFiles()
{
	for(auto file : HelixFS::getInstance().FindFilesByType( ".cpp" ) ){
		ProcessCPPFile(file);
	}
}

void HelixExtractStrings::ProcessFile( const twine& file_name ) 
{
	//printf("Processing Layout File: %s\n", file_name() );

	try {
		sptr<xmlDoc, xmlFreeDoc> layout = xmlParseFile( file_name() );
		if(layout == NULL){
			printf("Error loading layout file: %s\n", file_name() );
			return;
		}

		xmlNodePtr root = xmlDocGetRootElement( layout );
		if(root == NULL){
			printf("Layout file has no root element: %s\n", file_name() );
			return;
		}

		HandleLayoutNode( root );

	} catch (AnException& e){
		printf("Error processing file %s:\n%s\n", file_name(), e.Msg() );
		return;
	}
}

void HelixExtractStrings::HandleLayoutNode( xmlNodePtr node ) 
{
	if(strcmp( (const char*)(node->name), "Button" ) == 0){
		twine label( node, "label" ); OutputStaticLabel( label );
	} else if(strcmp( (const char*)(node->name), "CKTextArea" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "Column" ) == 0){
		twine title( node, "title" ); OutputStaticLabel( title );
	} else if(strcmp( (const char*)(node->name), "DoubleCheck" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label1" ); OutputStaticLabel( label1 );
	} else if(strcmp( (const char*)(node->name), "DoubleCheck2" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label1" ); OutputStaticLabel( label1 );
	} else if(strcmp( (const char*)(node->name), "DoubleField" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine tooltip1( node, "tooltip1" ); OutputStaticLabel( tooltip1 );
		twine tooltip2( node, "tooltip2" ); OutputStaticLabel( tooltip2 );
	} else if(strcmp( (const char*)(node->name), "FieldArea" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "GroupBox" ) == 0){
		twine legend( node, "legend" ); OutputStaticLabel( legend );
	} else if(strcmp( (const char*)(node->name), "HandsOnTable" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "HorizontalBoxLayout" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "HorizontalSplitPane" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "HorizontalSplitPaneLeft" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "HorizontalSplitPaneRight" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "HtmlEmbed" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "Image" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "IFrame" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "Label" ) == 0){
		twine text( node, "text" ); OutputStaticLabel( text );
	} else if(strcmp( (const char*)(node->name), "List" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "List1Value" ) == 0){
		twine label( node, "label" ); OutputStaticLabel( label );
	} else if(strcmp( (const char*)(node->name), "List2Value" ) == 0){
		twine label( node, "label" ); OutputStaticLabel( label );
	} else if(strcmp( (const char*)(node->name), "LongLabel" ) == 0){
		twine label( node, "label" ); OutputStaticLabel( label );
	} else if(strcmp( (const char*)(node->name), "QuadCheck" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
	} else if(strcmp( (const char*)(node->name), "QuadField" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
		twine tooltip1( node, "tooltip1" ); OutputStaticLabel( tooltip1 );
		twine tooltip2( node, "tooltip2" ); OutputStaticLabel( tooltip2 );
		twine tooltip3( node, "tooltip3" ); OutputStaticLabel( tooltip3 );
		twine tooltip4( node, "tooltip4" ); OutputStaticLabel( tooltip4 );
	} else if(strcmp( (const char*)(node->name), "QuintField" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
		twine label5( node, "label5" ); OutputStaticLabel( label5 );
		twine tooltip1( node, "tooltip1" ); OutputStaticLabel( tooltip1 );
		twine tooltip2( node, "tooltip2" ); OutputStaticLabel( tooltip2 );
		twine tooltip3( node, "tooltip3" ); OutputStaticLabel( tooltip3 );
		twine tooltip4( node, "tooltip4" ); OutputStaticLabel( tooltip4 );
		twine tooltip5( node, "tooltip5" ); OutputStaticLabel( tooltip5 );
	} else if(strcmp( (const char*)(node->name), "RadioGroup2" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
	} else if(strcmp( (const char*)(node->name), "RadioGroup3" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
	} else if(strcmp( (const char*)(node->name), "RadioGroup4" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
	} else if(strcmp( (const char*)(node->name), "RadioGroupHorizontal4" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
	} else if(strcmp( (const char*)(node->name), "RadioGroup5" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
		twine label5( node, "label5" ); OutputStaticLabel( label5 );
		twine labelPrefix( node, "labelPrefix" ); OutputStaticLabel( labelPrefix );
	} else if(strcmp( (const char*)(node->name), "RadioGroup6" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine label4( node, "label4" ); OutputStaticLabel( label4 );
		twine label5( node, "label5" ); OutputStaticLabel( label5 );
		twine label6( node, "label6" ); OutputStaticLabel( label6 );
	} else if(strcmp( (const char*)(node->name), "RadioGroupBoxGroup" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "Scroll" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "SingleCheck" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
	} else if(strcmp( (const char*)(node->name), "Spacer" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "SpanField" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine tooltip1( node, "tooltip1" ); OutputStaticLabel( tooltip1 );
	} else if(strcmp( (const char*)(node->name), "StandardTable" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "TextArea" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "Tree" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "TreeVirtual" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "TripleCheck" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
	} else if(strcmp( (const char*)(node->name), "TripleField" ) == 0){
		twine label1( node, "label1" ); OutputStaticLabel( label1 );
		twine label2( node, "label2" ); OutputStaticLabel( label2 );
		twine label3( node, "label3" ); OutputStaticLabel( label3 );
		twine tooltip1( node, "tooltip1" ); OutputStaticLabel( tooltip1 );
		twine tooltip2( node, "tooltip2" ); OutputStaticLabel( tooltip2 );
		twine tooltip3( node, "tooltip3" ); OutputStaticLabel( tooltip3 );
	} else if(strcmp( (const char*)(node->name), "VerticalBoxLayout" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "VerticalSplitPane" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "VerticalSplitPaneBottom" ) == 0){
		// No static labels
	} else if(strcmp( (const char*)(node->name), "VerticalSplitPaneTop" ) == 0){
		// No static labels
	} else {
		printf("Unknown layout node: %s\n", (const char*)(node->name) );
	}

	LoopChildNodes( node );
}

void HelixExtractStrings::LoopChildNodes( xmlNodePtr node )
{
	for(xmlNodePtr child = node->xmlChildrenNode; child != NULL; child = child->next){
		if(child->type != XML_ELEMENT_NODE) continue;
		HandleLayoutNode( child );
	}
}

void HelixExtractStrings::OutputStaticLabel( const twine& staticLabel )
{
	// Skip empty labels
	if(staticLabel.empty()) return;

	m_staticLabels.push_back( staticLabel );
}

void HelixExtractStrings::ProcessCPPFile( HelixFSFile* file ) 
{
	try {
		if(file->FileName().endsWith("ExtractStrings.cpp")) return; // Skip ourself
		for(auto& line : file->Lines()){
			FindCPPStaticLabel( line );
		}
	} catch (AnException& e){
		printf("Error processing file %s:\n%s\n", file->FileName()(), e.Msg() );
		return;
	}
}

void HelixExtractStrings::FindCPPStaticLabel( const twine& line )
{
	FindCPPStaticLabelInKey( line, "tr(" );
	FindCPPStaticLabelInKey( line, "TRANS(" );
	FindCPPStaticLabelInKey( line, "translate(" );
	FindCPPStaticLabelInKey( line, "translateForReport(" );
}

void HelixExtractStrings::FindCPPStaticLabelInKey( const twine& line, const twine& key )
{
	size_t tridx = line.find(key);
	while(tridx != TWINE_NOT_FOUND){
		// Make sure that the preceding character is non-alpha
		if( tridx > 0 && isalpha( line[tridx-1] ) != 0 ) {
			// look for another tr( on this line
			tridx = line.find(key, tridx + key.size());
			continue;
		}
		size_t firstQuote = line.find("\"", tridx); // Find the first quote
		if(firstQuote == TWINE_NOT_FOUND) {
			// look for another tr( on this line
			tridx = line.find(key, tridx + key.size() );
			continue;
		}
		size_t endQuote = line.find("\"", firstQuote + 1); // Find the end quote
		// If preceeding character is backslash, then try again.
		while(endQuote != TWINE_NOT_FOUND && line[endQuote-1] == '\\'){
			endQuote = line.find("\"", endQuote + 1);
		}
		if(endQuote == TWINE_NOT_FOUND){
			// look for another tr( on this line
			tridx = line.find(key, tridx + key.size());
			continue;
		}
		
		// pull out the string between firstQuote and endQuote
		firstQuote++;
		twine cppStaticLabel = line.substr(firstQuote, endQuote - firstQuote);
		OutputStaticLabel( cppStaticLabel );

		// Now look for another tr( after this one
		tridx = line.find(key, endQuote + 1);
	}
}

void HelixExtractStrings::UpdateStaticLabelFile()
{
	// Actually, what we need to do is generate a simple Qooxdoo class with all of these strings
	// referenced.  Then the qx translation system will generate the right po files, and it will
	// internally generate references for these strings.

	twine fileName; fileName.format("%s/%s/source/class/%s/StaticLabelList.js", 
		m_qdRoot(), m_currentPackage(), m_currentPackage() );

	twine contents;
	contents.append(
		"/**\n"
		"  * Copyright (c) 2017 Hericus Software, Inc.\n"
		"  *\n"
		"  */\n"
		"qx.Class.define(\"" + m_currentPackage + ".StaticLabelList\", {\n"
		"\textend : qx.core.Object,\n"
		"\n"
		"\tconstruct : function() {\n"
		"\t\tthis.base(arguments);\n"
		"\t},\n"
		"\n"
		"\tproperties : {\n"
		"\t\tDummyString: {init : \"\", check : \"String\" }\n"
		"\t},\n"
		"\n"
		"\tmembers : {\n"
		"\n"
		"\t\t/** Our list of static labels that are defined in our internal layout files, but need\n"
		"\t\t  * to be visible somehow to qooxdoo so that it's translation system will be aware of them.\n"
		"\t\t  */\n"
		"\t\tinitStaticLabels : function() {\n"
	);

	for(size_t i = 0; i < m_staticLabels.size(); i++){
		twine line;
		line.format("\t\t\tthis.SetDummyString( ttvx.Statics.tr(\"%s\") );\n", m_staticLabels[i]() );
		contents.append( line );
	}

	contents.append(
		"\t\t}\n"
		"\t},\n"
		"\n"
		"\tdestruct : function() {\n"
		"\n"
		"\t}\n"
		"});\n"
	);

	File::writeToFile( fileName, contents );
}
