# CMake File for SLib
project(libSlib LANGUAGES CXX C)

# We require at least a minimum version of cmake as 3.5
cmake_minimum_required(VERSION 3.5)

# The primary library definition
add_library(SLib 
	Base64.cpp Log.cpp SSocket.cpp Socket.cpp Thread.cpp Mutex.cpp Tools.cpp twine.cpp Date.cpp
	SmtpClient.cpp Interval.cpp EMail.cpp Timer.cpp Parms.cpp LogMsg.cpp EnEx.cpp XmlHelpers.cpp
	BlockingQueue.cpp File.cpp LogFile.cpp HttpClient.cpp ZipFile.cpp MemBuf.cpp sqlite3.c
	LogFile2.cpp ioapi.c mztools.c unzip.c zip.c
)

# Add an alias so that our library can be used inside the build tree
add_library(SLib::SLib ALIAS SLib)


target_compile_features(SLib
	PUBLIC
		cxx_std_14
)
option(BUILD_SHARED_LIBS "Build shared library" ON)

if(WIN32)
	target_link_libraries(SLib PRIVATE ???)
else() # Linux or Mac
	target_compile_options(SLib
		PUBLIC
			-O3 -Wall # All warnings and 3rd level optimization
	)
	target_include_directories(SLib
		PUBLIC
			/usr/include/libxml2
			/usr/include/curl
	)
	target_link_libraries(SLib 
		PUBLIC 
			ssl crypto pthread resolv xml2 z curl
	)
endif()

if( "${CMAKE_SYSTEM_NAME}" MATCHES "Linux" )
	target_include_directories(SLib
		PUBLIC
			/usr/include
	)
	target_link_libraries(SLib 
		PUBLIC 
			rt
	)
elseif(APPLE)
	target_include_directories(SLib
		PUBLIC
			/usr/local/opt/openssl/include
	)
	target_link_libraries(SLib 
		PUBLIC 
			iconv
	)
endif()

add_executable(LogDump LogDump.cpp)
target_link_libraries(LogDump SLib::SLib)

add_executable(SLogDump SLogDump.cpp)
target_link_libraries(SLogDump SLib::SLib)

add_executable(SqlShell  SqlShell.c)
target_link_libraries(SqlShell SLib::SLib)

#
# Installation Setup
#
set(CMAKE_INSTALL_PREFIX "../../")
set(SYSTEM_DIR_INSTALL FALSE)
set(INSTALL_ROOT ".")
set(INSTALL_CMAKE "cmake")
set(INSTALL_EXAMPLES ".")
set(INSTALL_DATA ".")
set(INSTALL_BIN "bin")
set(INSTALL_SHARED "lib")
set(INSTALL_LIB "lib")
set(INSTALL_INCLUDE "include")
install(FILES 
	AnException.h HttpClient.h ObjQueue.h ZipFile.h sqlite3.h
	AutoXMLChar.h Interval.h Parms.h crypt.h sqlite3ext.h
	Base64.h Lock.h Pool.h Pool.cpp dptr.h suvector.h
	BlockingQueue.h Log.h SSocket.h hpdf_config.win64.h twine.h
	Date.h LogFile.h SmtpClient.h ioapi.h unzip.h
	EMail.h LogFile2.h Socket.h iowin32.h xmlinc.h
	EnEx.h LogMsg.h Thread.h memptr.h zip.h
	File.h MemBuf.h Timer.h mztools.h
	GSocket.h MsgQueue.h Tools.h smtp.h
	Hash.h Mutex.h XmlHelpers.h sptr.h
	DESTINATION ${INSTALL_INCLUDE} COMPONENT dev)
install(TARGETS SLib EXPORT SLib-targets LIBRARY DESTINATION ${INSTALL_SHARED})
install(TARGETS LogDump RUNTIME DESTINATION ${INSTALL_BIN})
install(TARGETS SLogDump RUNTIME DESTINATION ${INSTALL_BIN})
install(TARGETS SqlShell RUNTIME DESTINATION ${INSTALL_BIN})
