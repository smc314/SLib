/* **************************************************************************

   Copyright (c): 2008 - 2013 Hericus Software, LLC

   License: The MIT License (MIT)

   Authors: Steven M. Cherry

************************************************************************** */

#include <AnException.h>
#include <EnEx.h>
#include <Log.h>
#include <XmlHelpers.h>
#include <Timer.h>
using namespace SLib;

#include "HelixConfig.h"
using namespace Helix::Build;

// Our global instance of the filesystem
static HelixConfig* m_helix_config = nullptr;

HelixConfig& HelixConfig::getInstance()
{
	EnEx ee(FL, "HelixConfig::getInstance()", true);

	if(m_helix_config == nullptr){
		m_helix_config = new HelixConfig();
	}
	return *m_helix_config;
}

HelixConfig::HelixConfig()
{
	EnEx ee(FL, "HelixConfig::HelixConfig()");

	if(!File::Exists( "hbuild.xml" )){
		throw AnException(0, FL, "hbuild.xml missing!");
	}
	m_config = xmlParseFile( "hbuild.xml" );
}

HelixConfig::~HelixConfig()
{
	EnEx ee(FL, "HelixConfig::~HelixConfig()");

}

bool HelixConfig::UseCore()
{
	return XmlHelpers::getBoolAttr( xmlDocGetRootElement( m_config ), "UseCore" );
}

twine HelixConfig::CoreFolder()
{
	twine ret( xmlDocGetRootElement( m_config ), "CoreFolder" );
	return ret;
}

vector<twine> HelixConfig::QxApps()
{
	vector<twine> ret;
	auto apps = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "QxApps" );
	if(apps == nullptr) return ret;
	for(auto app : XmlHelpers::FindChildren( apps, "App" )){
		twine appName( app, "name" );
		ret.push_back( appName );
	}
	return ret;
}

vector<twine> HelixConfig::Logics()
{
	vector<twine> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Logics" );
	if(node == nullptr) return ret;
	for(auto l : XmlHelpers::FindChildren( node, "Logic" )){
		twine lName( l, "name" );
		ret.push_back( lName );
	}
	return ret;
}

vector<twine> HelixConfig::LogicDepends(const twine& logic)
{
	vector<twine> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Logics" );
	if(node == nullptr) return ret;
	auto child = XmlHelpers::FindChildWithAttribute( node, "Logic", "name", logic() );
	if(child == nullptr){ // No Logic with this name
		return ret;
	}
	auto deps = XmlHelpers::FindChildren( child, "Depends" );
	for(auto d : deps){
		twine depName( d, "name" );
		ret.push_back( depName );
	}
	return ret;
}

vector<xmlNodePtr> HelixConfig::Installs()
{
	vector<xmlNodePtr> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Installs" );
	if(node == nullptr) return ret;
	return XmlHelpers::FindChildren( node, "Install" );
}