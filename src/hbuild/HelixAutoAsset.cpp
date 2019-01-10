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
#include "HelixAutoAsset.h"
#include "HelixWorker.h"
using namespace Helix::Build;


HelixAutoAsset::HelixAutoAsset() 
{
	EnEx ee(FL, "HelixAutoAsset::HelixAutoAsset()");

}

HelixAutoAsset::HelixAutoAsset(const HelixAutoAsset& c)
{
	EnEx ee(FL, "HelixAutoAsset::HelixAutoAsset(const HelixAutoAsset& c)");

}

HelixAutoAsset& HelixAutoAsset::operator=(const HelixAutoAsset& c)
{
	EnEx ee(FL, "HelixAutoAsset::operator=(const HelixAutoAsset& c)");

	return *this;
}

HelixAutoAsset::HelixAutoAsset(const HelixAutoAsset&& c)
{
	EnEx ee(FL, "HelixAutoAsset::HelixAutoAsset(const HelixAutoAsset&& c)");

}

HelixAutoAsset& HelixAutoAsset::operator=(const HelixAutoAsset&& c)
{
	EnEx ee(FL, "HelixAutoAsset::operator=(const HelixAutoAsset&& c)");


	return *this;
}

HelixAutoAsset::~HelixAutoAsset()
{
	EnEx ee(FL, "HelixAutoAsset::~HelixAutoAsset()");

}

void HelixAutoAsset::Generate()
{
	EnEx ee(FL, "HelixAutoAsset::Generate");

	// Generate the Javascript data object file in all of the correct qd app folders
	for(auto& app : HelixConfig::getInstance().QxApps()){
		m_currentPackage = app;
		INFO(FL, "Processing package: %s", m_currentPackage() );
		FindAllJSFiles( "../../qd/" + m_currentPackage);
	}
}

void HelixAutoAsset::FindAllJSFiles(const twine& start)
{
	// check all of the normal files:
	vector<twine> files = File::listFiles(start);
	for(int i = 0; i < (int)files.size(); i++){
		twine child = start + "/" + files[i];
		if(child.endsWith(twine(".js"))){
			m_currentFile = child;
			ProcessFile(child);
		}
	}

	// Then recurse through all of the subdirectories:
	vector<twine> folders = File::listFolders(start);
	for(int i = 0; i < (int)folders.size(); i++){
		if(folders[i] != "." && folders[i] != ".."){
			FindAllJSFiles(start + "/" + folders[i]);
		}
	}

}

void HelixAutoAsset::ProcessFile( const twine& file_name ) 
{
	vector<twine> pathElements = twine(file_name).split("/");
	if(pathElements[4] != "source" ||
		pathElements[5] != "class"
	){
		return; // skip this file
	}
	DEBUG(FL, "Processing JavaScript File: %s\n", file_name() );

	vector<twine> lines;
	try {
		File f(file_name);
		lines = f.readLines();

		vector<twine> icons = FindIcons( lines );
		
		UpdateAssetTags( lines, icons);

	} catch (AnException& e){
		printf("Error processing file %s:\n%s\n", file_name(), e.Msg() );
		return;
	}
	
	// Now save the updated file:
	twine output;
	for(int i = 0; i < (int)lines.size(); i++){
		output += lines[i] + "\n";
	}
	File::writeToFile(file_name, output);
}

vector<twine> HelixAutoAsset::FindIcons( vector<twine>& lines ) 
{
	vector<twine> ret;
	
	for(int i = 0; i < (int)lines.size(); i++){
		if(lines[i].startsWith("#asset")){
			continue; // skip this line
		}
		size_t idx1 = lines[i].find("\"" + m_currentPackage + "/");
		if(idx1 == TWINE_NOT_FOUND){
			continue; // skip this line
		}
		bool found_template = false;
		size_t idx2 = lines[i].find(".png\"");
		if(idx2 == TWINE_NOT_FOUND){
			idx2 = lines[i].find(".gif\"");
			if(idx2 == TWINE_NOT_FOUND){
				idx2 = lines[i].find(".jpg\"");
				if(idx2 == TWINE_NOT_FOUND){
					// not a .png, .gif, or .jpg file.
					// check for a template file
					idx2 = lines[i].find("\"" + m_currentPackage + "/templates/" );
					if(idx2 == TWINE_NOT_FOUND){
						// not a template file either
						continue;
					} else {
						// found a template file.  move idx2 to the end of the string.
						idx2 = lines[i].find( '"', idx1 + 5 );
						found_template = true;
					}
				}
			}
		}
		idx1 ++; // skip the quote at the beginning
		if(!found_template){
			idx2 += 4; // skip the quote at the end
		}
		ret.push_back( lines[i].substr(idx1, idx2-idx1) );
	}
	return ret;
}

void HelixAutoAsset::UpdateAssetTags( vector<twine>& lines, vector<twine>& icons )
{
	int idx1, idx2;
	idx1 = idx2 = 0;
	
	// Find the first #asset line
	for(int i = 0; i < (int)lines.size(); i++){
		if(lines[i].startsWith("#asset(")){
			idx1 = i;
			break;
		}
	}
		
	// find the last #asset line
	for(int i = idx1 + 1; i < (int)lines.size(); i++){
		if(lines[i].startsWith("#asset(")){
			idx2 = i;
		}
	}
	
	if(idx1 == 0){
		// no #asset tags in the file.  log a warning.
		if(icons.size() != 0){
			// Find the first *** line:
			for(int i = 0; i < (int)lines.size(); i++){
				if(lines[i].startsWith("**********")){
					lines.insert(lines.begin() + i + 1, "************************************************************************ */");
					lines.insert(lines.begin() + i + 1, "");
					lines.insert(lines.begin() + i + 1, "/* ************************************************************************");
					idx1 = i+2;
					break;
				}
			}
			if(idx1 == 0){ // still zero
				WARN(FL, "Couldn't find the place to add the #asset tags\n");
				return;
			}
			
		} else {
			return; // no icons so no issue.
		}
	}
	if(idx2 == 0){
		idx2 = idx1;
	}
	
	// Get rid of all #asset tags.
	for(int i = 0; i < idx2-idx1+1; i++){
		// save it if it has a star in it.
		if(lines[ idx1 ].find('*') != TWINE_NOT_FOUND){
			size_t starIdx1 = lines[ idx1 ].find( '(' );
			size_t starIdx2 = lines[ idx1 ].find( ')' );
			starIdx1 ++;
			icons.push_back( lines[ idx1 ].substr( starIdx1, starIdx2 - starIdx1 ) );
		}
		lines.erase(lines.begin() + idx1);
	}
	
	// Now add back in the correct #asset tags for the icons we use.
	for(int i = 0; i < (int)icons.size(); i++){
		lines.insert(lines.begin() + idx1, "#asset(" + icons[i] + ")");
		idx1++;
	}
	
}
	
