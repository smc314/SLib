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
#include <suvector.h>
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

bool HelixConfig::UseDebug()
{
	return XmlHelpers::getBoolAttr( xmlDocGetRootElement( m_config ), "Debug" );
}

twine HelixConfig::CoreFolder()
{
	twine ret( xmlDocGetRootElement( m_config ), "CoreFolder" );
	return ret;
}

bool HelixConfig::SkipPdfGen()
{
	return XmlHelpers::getBoolAttr( xmlDocGetRootElement( m_config ), "SkipPdfGen" );
}

vector<twine> HelixConfig::QxApps()
{
	vector<twine> ret;
	auto apps = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "QxApps" );
	if(apps == nullptr) return ret;
	for(auto app : XmlHelpers::FindChildren( apps, "App" )){
		twine appName( app, "name" );
		if(XmlHelpers::getBoolAttr( app, "noApi" ) == true) continue; // Skip these
		ret.push_back( appName );
	}
	return ret;
}

vector<twine> HelixConfig::QxAppsAll()
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

vector<twine> HelixConfig::LogicRepos()
{
	suvector<twine> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Logics" );
	if(node == nullptr) return ret;
	for(auto l : XmlHelpers::FindChildren( node, "Logic" )){
		twine repo( l, "repo" );
		if(repo.empty() == false){
			ret.push_back( repo );
		}
	}
	return ret;
}

twine HelixConfig::LogicRepo( const twine& logic )
{
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Logics" );
	if(node == nullptr) return "";
	auto child = XmlHelpers::FindChildWithAttribute( node, "Logic", "name", logic() );
	if(child == nullptr){ // No Logic with this name
		return "";
	}
	twine repo( child, "repo" );
	return repo;
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

vector<xmlNodePtr> HelixConfig::Deploy()
{
	vector<xmlNodePtr> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Deploy" );
	if(node == nullptr) return ret;
	return XmlHelpers::FindChildren( node, "Install" );
}

void HelixConfig::IncludeTest( bool tf )
{
	m_include_test = tf;
}

bool HelixConfig::IncludeTest( )
{
	return m_include_test;
}

vector<twine> HelixConfig::ApacheIncludes()
{
	vector<twine> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Apache" );
	if(node == nullptr) return ret;

#ifdef _WIN32
	twine platform = "WIN32";
#elif __APPLE__
	twine platform = "MAC";
#elif __linux__
	twine platform = "LINUX";
#else
	throw AnException(0, FL, "Unknown build platform");
#endif

	auto incs = XmlHelpers::FindChildren( node, "Include" );
	for(auto inc : incs){
		twine incName( inc, "name" );
		twine platName( inc, "platform" );
		if(platName == platform && incName.empty() == false){
			ret.push_back(incName);
		}
	}
	return ret;
}

vector<twine> HelixConfig::ApacheLibs()
{
	vector<twine> ret;
	auto node = XmlHelpers::FindChild( xmlDocGetRootElement( m_config ), "Apache" );
	if(node == nullptr) return ret;

#ifdef _WIN32
	twine platform = "WIN32";
#elif __APPLE__
	twine platform = "MAC";
#elif __linux__
	twine platform = "LINUX";
#else
	throw AnException(0, FL, "Unknown build platform");
#endif

	auto libs = XmlHelpers::FindChildren( node, "Lib" );
	for(auto lib : libs){
		twine libName( lib, "name" );
		twine platName( lib, "platform" );
		if(platName == platform && libName.empty() == false){
			ret.push_back(libName);
		}
	}
	return ret;
}

