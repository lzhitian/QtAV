# qmake library building template pri file
# Copyright (C) 2011-2013 Wang Bin <wbsecg1@gmail.com>
# Shanghai, China.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
############################## HOW TO ##################################
# Suppose the library name is XX
# Usually what you need to change are: staticlink, LIB_VERSION, NAME and DLLDESTDIR.
# And rename xx-buildlib and LIBXX_PRI_INCLUDED
# the contents of libXX.pro is:
#    TEMPLATE = lib
#    QT -= gui
#    CONFIG *= xx-buildlib
#    STATICLINK = 1 #optional. default is 0, i.e. dynamically link
#    PROJECTROOT = $$PWD/..
#    include(libXX.pri)
#    preparePaths($$OUT_PWD/../out)
#    HEADERS = ...
#    SOURCES = ...
#    ...
# the content of other pro using this library is:
#    TEMPLATE = app
#    PROJECTROOT = $$PWD/..
#    STATICLINK = 1 #or 0
#    include(dir_of_XX/libXX.pri)
#    preparePaths($$OUT_PWD/../out)
#    HEADERS = ...
#    SOURCES = ...
#

NAME = QtAV
!isEmpty(LIB$$upper($$NAME)_PRI_INCLUDED): {
	error("lib$${NAME}.pri already included")
	unset(NAME)
}
eval(LIB$$upper($$NAME)_PRI_INCLUDED = 1)

LIB_VERSION = 1.2.4 #0.x.y may be wrong for dll
isEmpty(STATICLINK): STATICLINK = 0  #1 or 0. use static lib or not

TEMPLATE += fakelib
PROJECT_TARGETNAME = $$qtLibraryTarget($$NAME)
TEMPLATE -= fakelib

isEmpty(PROJECTROOT): PROJECTROOT = $$PWD/..
include($${PROJECTROOT}/common.pri)
preparePaths($$OUT_PWD/../out)
CONFIG += depend_includepath #?

PROJECT_SRCPATH = $$PWD
PROJECT_LIBDIR = $$qtLongName($$BUILD_DIR/lib)
#for system include path
*msvc* {
} else {
    QMAKE_CXXFLAGS += -isystem $$PROJECT_SRCPATH -isystem $$PROJECT_SRCPATH/..
}
INCLUDEPATH *= $$PROJECT_SRCPATH $$PROJECT_SRCPATH/.. $$PROJECT_SRCPATH/$$NAME
DEPENDPATH *= $$PROJECT_SRCPATH
QMAKE_LFLAGS_RPATH += #will append to rpath dir

#eval() ?
!contains(CONFIG, $$lower($$NAME)-buildlib) {
	#The following may not need to change
	CONFIG *= link_prl
        LIBS *= -L$$PROJECT_LIBDIR -l$$qtLibName($$NAME)
	isEqual(STATICLINK, 1) {
		PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtStaticLib($$NAME)
	} else {
		win32 {
			PRE_TARGETDEPS *= $$PROJECT_LIBDIR/$$qtSharedLib($$NAME, $$LIB_VERSION)
		} else {
			PRE_TARGETDEPS *= $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)
# $$[QT_INSTALL_LIBS] and $$DESTDIR will be auto added to rpath
# Current (sub)project dir is auto added to the first value as prefix. e.g. QMAKE_RPATHDIR = .. ==> -Wl,-rpath,ROOT/.. 
# Executable dir search: ld -z origin, g++ -Wl,-R,'$ORIGIN', in makefile -Wl,-R,'$$ORIGIN'
# Working dir search: "."
# TODO: for macx. see qtcreator/src/rpath.pri. search exe dir first(use QMAKE_LFLAGS = '$$RPATH_FLAG' $$QMAKE_LFLAGS)
			unix:!macx {
				QMAKE_RPATHDIR *= $$PROJECT_LIBDIR:\'\$\$ORIGIN\':\'\$\$ORIGIN/lib\':.
				QMAKE_LFLAGS *= -Wl,-z,origin
			}
		}
	}
} else {
	#Add your additional configuration first. e.g.

#	win32: LIBS += -lUser32
# The following may not need to change

	#TEMPLATE = lib
	VERSION = $$LIB_VERSION
        TARGET = $$PROJECT_TARGETNAME ##I commented out this before, why?
	DESTDIR= $$PROJECT_LIBDIR

	CONFIG *= create_prl #
	isEqual(STATICLINK, 1) {
		CONFIG -= shared dll ##otherwise the following shared is true, why?
		CONFIG *= staticlib
	} else {
		DEFINES += Q_DLL_LIBRARY #win32-msvc*
		CONFIG *= shared #shared includes dll
	}

	shared {
		!isEqual(DESTDIR, $$BUILD_DIR/bin): DLLDESTDIR = $$BUILD_DIR/bin #copy shared lib there
		CONFIG(release, debug|release): !isEmpty(QMAKE_STRIP): QMAKE_POST_LINK = -$$QMAKE_STRIP $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)

		#copy from the pro creator creates.
		symbian {
			MMP_RULES += EXPORTUNFROZEN
			TARGET.UID3 = 0xE4CC8061
			TARGET.CAPABILITY =
			TARGET.EPOCALLOWDLLDATA = 1
			addFiles.sources = $$qtSharedLib($$NAME, $$LIB_VERSION)
			addFiles.path = !:/sys/bin
			DEPLOYMENT += addFiles
		}
	}
	unix:!symbian {
		maemo5 {
			target.path = /opt/usr/lib
		} else {
			target.path = /usr/lib
		}
		INSTALLS += target
	}
}
unix {
    LIBS += -L/usr/local/lib
    QMAKE_RPATHDIR += /usr/local/lib
}

unset(LIB_VERSION)
unset(PROJECT_SRCPATH)
unset(PROJECT_LIBDIR)
unset(PROJECT_TARGETNAME)

