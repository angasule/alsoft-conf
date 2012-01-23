cmake_minimum_required(VERSION 2.4.7)
SET( cmake_verbose_makefile TRUE)

PROJECT( alsoft-conf )

SET( OSC_SRCS
	./src/main.cpp
	./src/main_window.cpp
	./src/confirm_close.cpp
	./src/new_parser.cpp
	./src/speaker.cpp
)

# Headers that should be moc'ed
SET( OSC_MOC_HDRS
	./src/speaker.h
	./src/main_window.h
	./src/confirm_close.h
)

SET( OSC_UIS
	./src/ui/main.ui
	./src/ui/confirm_close.ui
)

ADD_DEFINITIONS( -Wall)

FIND_PACKAGE(Qt4 COMPONENTS  QtCore QtGui REQUIRED)

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( OSC_UI_HDRS ${OSC_UIS} )

QT4_WRAP_CPP( OSC_MOC_SRCS ${OSC_MOC_HDRS} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

ADD_EXECUTABLE( alsoft-conf ${OSC_SRCS} ${OSC_MOC_SRCS} ${OSC_UI_HDRS} )

TARGET_LINK_LIBRARIES( alsoft-conf ${QT_LIBRARIES} )

INSTALL( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/alsoft-conf DESTINATION bin)
