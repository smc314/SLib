#ifndef XMLHELPERS_H
#define XMLHELPERS_H

/*
 * Copyright (c) 2001,2002 Steven M. Cherry. All rights reserved.
 *
 * This file is a part of slib - a c++ utility library
 *
 * The slib project, including all files needed to compile
 * it, is free software; you can redistribute it and/or use it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  See file COPYING for details.
 */

#ifdef _WIN32
#	ifndef DLLEXPORT
#		define DLLEXPORT __declspec(dllexport)
#	endif
#else
#	define DLLEXPORT
#endif

#include <stdlib.h>

#include "xmlinc.h"
#include "AutoXMLChar.h"
#include "twine.h"
#include "EnEx.h"
#include "MemBuf.h"

#include <vector>
using namespace std;

namespace SLib {

/** This class is a collection of static methods designed to make looping and 
  * searching through an XML document a bit easier than the standard libxml2 methods do.
  *
  * @author Steven M. Cherry
  * @copyright 2011 Steven M. Cherry
  */
class DLLEXPORT XmlHelpers {

	public:

		/// Find a child node by node name.  Returns the first node with a matching name.
		static xmlNodePtr FindChild(const xmlNodePtr parent, const char* childNodeName) {
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::FindChild()");
			}
			if(childNodeName == NULL){
				throw AnException(0, FL, "NULL childNodeName passed to XmlHelpers::FindChild()");
			}
			size_t l = strlen(childNodeName);
			if(l == 0){
				// Empty child node name.  Just return the first one found.
				return parent->xmlChildrenNode;
			}
			// Loop through the child nodes and return the first one with a name that matches
			for(xmlNodePtr node = parent->xmlChildrenNode; node != NULL; node = node->next){
				if(node->type != XML_ELEMENT_NODE) continue;
				if(strcmp((const char*)node->name, childNodeName) == 0){
					return node;
				}
			}
			return NULL; // Not Found
		}

		/// Find a child node by node name, attribute and attribute value.
		static xmlNodePtr FindChildWithAttribute(const xmlNodePtr parent, const char* childNodeName,
			const char* attrName, const char* attrValue) 
		{
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::FindChildWithAttribute()");
			}
			if(childNodeName == NULL || strlen(childNodeName) == 0){
				throw AnException(0, FL, "NULL childNodeName passed to XmlHelpers::FindChildWithAttribute()");
			}
			if(attrName == NULL || strlen(attrName) == 0){
				throw AnException(0, FL, "NULL attrName passed to XmlHelpers::FindChildWithAttribute()");
			}
			if(attrValue == NULL || strlen(attrValue) == 0){
				throw AnException(0, FL, "NULL attrValue passed to XmlHelpers::FindChildWithAttribute()");
			}
			
			// Loop through the child nodes and return the first one with a name that matches
			for(xmlNodePtr node = parent->xmlChildrenNode; node != NULL; node = node->next){
				if(node->type != XML_ELEMENT_NODE) continue;
				if(strcmp((const char*)node->name, childNodeName) == 0){
					// Check the attribute:
					twine an(node, attrName);
					if(an.length() != 0 && an == attrValue){
						return node;
					}
				}
			}
			return NULL; // Not Found
		}

		/// Find children by node name.  Returns all of the child nodes with a matching name.
		static vector<xmlNodePtr> FindChildren(const xmlNodePtr parent, const char* childNodeName) {
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::FindChild()");
			}
			if(childNodeName == NULL){
				throw AnException(0, FL, "NULL childNodeName passed to XmlHelpers::FindChild()");
			}
			size_t l = strlen(childNodeName);
			if(l == 0){
				// Empty child node name.  Just return the first one found.
				throw AnException(0, FL, "Child Node name is empty");
			}
			vector<xmlNodePtr> ret;
			// Loop through the child nodes and return the first one with a name that matches
			for(xmlNodePtr node = parent->xmlChildrenNode; node != NULL; node = node->next){
				if(node->type != XML_ELEMENT_NODE) continue;
				if(strcmp((const char*)node->name, childNodeName) == 0){
					ret.push_back(node);
				}
			}
			return ret;
		}

		static twine getTextNodeValue(xmlNodePtr node) {
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed to XmlHelpers::getTextNodeValue()");
			}
			twine ret;
			AutoXMLChar tmp;
			if(xmlNodeIsText(node)){
				tmp = xmlNodeGetContent(node);
				ret = tmp;
			} else if(node->type == XML_ELEMENT_NODE){
				// Loop through all text sub-nodes and gather up their contents:
				for(xmlNodePtr n = node->xmlChildrenNode; n != NULL; n = n->next){
					if(xmlNodeIsText(n)){
						tmp = xmlNodeGetContent(n);
						ret += tmp;
					}
				}
			}
				
			return ret;
		}

		static void setTextNodeValue(xmlNodePtr node, twine& content) {
			xmlNodeSetContent(node, (const xmlChar*) content);
		}

		static twine getCDATASection(xmlNodePtr node) {
			EnEx ee(FL, "XmlHelpers::getCDATASection(xmlNodePtr node)");
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed to XmlHelpers::getCDATASection()");
			}
			twine ret;

			if(node->type == XML_CDATA_SECTION_NODE){
				// This is the CData node, get it's content and return;
				ret << xmlNodeGetContent( node );
				return ret;
			} else {
				// find the CData node under the given parent:
				for(xmlNodePtr n = node->xmlChildrenNode; n != NULL; n = n->next){
					if(n->type == XML_CDATA_SECTION_NODE){
						// Found the CData section.  Pull out its contents:
						ret << xmlNodeGetContent( n );
						return ret;
					}
				}
			}
			throw AnException(0, FL, "No CDATA Section found under given node.");
		}

		static void setCDATASection(xmlNodePtr parent, const twine& content) {
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::setCDATASection()");
			}
			xmlNodePtr cdata = xmlNewCDataBlock(parent->doc, (const xmlChar*)content, (int)content.size() );
			xmlAddChild(parent, cdata);
		}

		static twine getBase64( xmlNodePtr node ){
			EnEx ee(FL, "XmlHelpers::getBase64(xmlNodePtr node)");
			if(node == NULL){
				return "";
			}
			// First pick up the data in a CDATA section
			twine b64 = XmlHelpers::getCDATASection( node );

			// Then Base64 decode the data:
			b64.decode64();
			return b64;
		}

		static void setBase64( xmlNodePtr parent, const twine& content) {
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::setBase64()");
			}

			// First encode the data
			twine b64; // have to make a copy because the input is const.
			if(content.length() == 0){
				b64 = "";
			} else {
				b64 = content;
			}
			b64.encode64();

			// Then set it into a cdata block
			XmlHelpers::setCDATASection(parent, b64);
		}

		static void setBase64( xmlNodePtr parent, const MemBuf& content) {
			if(parent == NULL){
				throw AnException(0, FL, "NULL Parent passed to XmlHelpers::setBase64()");
			}

			// First encode the data
			MemBuf b64; // have to make a copy because the input is const.
			if(content.length() == 0){
				b64 = "";
			} else {
				b64 = content;
			}
			b64.encode64();
			twine tmp; tmp.set(b64(), b64.length());

			// Then set it into a cdata block
			XmlHelpers::setCDATASection(parent, tmp);
		}


		static size_t getIntAttr(xmlNodePtr node, const char* attrName){
			EnEx ee(FL, "XmlHelpers::getIntAttr(xmlNodePtr node)");
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into getIntAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into getIntAttr");
			}

			twine tmp(node, attrName);
			if(tmp.empty()){
				return 0;
			} else {
				return tmp.get_int();
			}
		}

		static void setIntAttr(xmlNodePtr node, const char* attrName, size_t val){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into setIntAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into setIntAttr");
			}

			twine tmp;
			tmp = val;
			xmlSetProp(node, (const xmlChar*)attrName, tmp);
		}
			
		static float getFloatAttr(xmlNodePtr node, const char* attrName){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into getIntAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into getIntAttr");
			}

			twine tmp(node, attrName);
			if(tmp.empty()){
				return 0;
			} else {
				return tmp.get_float();
			}
		}

		static void setFloatAttr(xmlNodePtr node, const char* attrName, float val){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into setIntAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into setIntAttr");
			}

			twine tmp;
			tmp = val;
			xmlSetProp(node, (const xmlChar*)attrName, tmp);
		}
			
		static Date getDateAttr(xmlNodePtr node, const char* attrName, const twine& dateFormat = ""){
			EnEx ee(FL, "XmlHelpers::getDateAttr(xmlNodePtr node)");
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into getDateAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into getDateAttr");
			}

			Date ret;
			twine tmp( node, attrName );
			if(tmp.empty() || tmp == "null" || tmp == "Null" || tmp == "NULL"){
				ret.SetMinValue();
			} else {
				if(dateFormat.empty()){
					ret.SetValue( tmp ); // if the format doesn't match, exceptions will be thrown here
				} else {
					ret.SetValue( tmp, dateFormat ); // if the format doesn't match exceptions will be thrown
				}
			}
			return ret;
		}

		static void setDateAttr(xmlNodePtr node, const char* attrName, const Date& date, const twine& dateFormat = ""){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into setDateAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into setDateAttr");
			}
			Date* td = (Date*)&date;

			if(date.IsMinValue()){
				return; // nothing to do
			} else if(dateFormat.empty()){
				xmlSetProp(node, (const xmlChar*)attrName, td->GetValue());
			} else {
				xmlSetProp(node, (const xmlChar*)attrName, td->GetValue(dateFormat));
			}
		}
			

		static bool getBoolAttr(xmlNodePtr node, const char* attrName){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into getBoolAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into getBoolAttr");
			}

			twine tmp(node, attrName);
			if(tmp.empty()){
				return false;
			} else {
				if(tmp == "1" ||
					tmp == "true" ||
					tmp == "True" ||
					tmp == "TRUE" ||
					tmp == "yes" ||
					tmp == "Yes" ||
					tmp == "YES" ||
					tmp == "On" ||
					tmp == "ON"
				){
					return true;
				} else {
					return false;
				}
			}
		}

		static void setBoolAttr(xmlNodePtr node, const char* attrName, bool val){
			if(node == NULL){
				throw AnException(0, FL, "NULL node passed into setIntAttr");
			}
			if(attrName == NULL){
				throw AnException(0, FL, "NULL attribute name passed into setIntAttr");
			}

			twine tmp;
			if(val){
				tmp = "true";
			} else {
				tmp = "false";
			}
			xmlSetProp(node, (const xmlChar*)attrName, tmp);
		}
			
		static twine docToStringPretty(xmlDocPtr doc){
			if(doc == NULL){
				throw AnException(0, FL, "NULL document passed into docToStringPretty");
			}

			xmlChar* buffer;
			int buf_size;
			xmlIndentTreeOutput = 1;
			xmlDocDumpFormatMemory(doc, &buffer, &buf_size, 1);
			xmlIndentTreeOutput = 0;

			twine ret;
			ret.set((char*)buffer, buf_size);
			xmlFree(buffer);

			return ret;
		}

		static twine docToString(xmlDocPtr doc){
			if(doc == NULL){
				throw AnException(0, FL, "NULL document passed into docToStringPretty");
			}

			xmlChar* buffer;
			int buf_size;
			xmlDocDumpMemory(doc, &buffer, &buf_size);

			twine ret;
			ret.set((char*)buffer, buf_size);
			xmlFree(buffer);

			return ret;
		}

}; // End Class





} // End Namespace

#endif // XMLHELPERS_H Defined
