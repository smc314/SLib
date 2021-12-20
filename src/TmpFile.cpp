#include "TmpFile.h"
#include "AnException.h"
#include "EnEx.h"
using namespace SLib;

TmpFile::TmpFile()
{
	EnEx ee("TmpFile::TmpFile()");
	twine fileName; fileName.Guid(); // Our file name is a guid
	fileName = File::PathCombine( File::OurLocation(), fileName );
	create( fileName );
}

TmpFile::TmpFile(const twine& fileName)
{
	EnEx ee("TmpFile::TmpFile(const twine fileName)");
	create( fileName );
}

TmpFile::~TmpFile()
{
	EnEx ee("TmpFile::~TmpFile()");
	// closeFile blanks out the file name - make a quick copy
	twine tmpFileName( m_fileName );
	closeFile();
	Delete( tmpFileName );
}
