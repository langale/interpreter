TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt

isEmpty(ADDRESS_SANITIZER) {
	ADDRESS_SANITIZER = "NO"
}

QMAKE_CXXFLAGS += -fPIC -fopenmp
QMAKE_CXXFLAGS +=			\
	-Wall					\
	-Wextra					\ # reasonable and standard
	-Wshadow				\ # warn if a variable declaration shadows one from
							\ # a parent context
	-Wnon-virtual-dtor		\ # warn if a class with virtual functions has
							\ # non-virtual destructors
	-Wold-style-cast		\ # warn for c-style casts
	-Wcast-align			\ # warn for potential performance problem casts
	-Wunused				\ # warn on anything being unused
	-Woverloaded-virtual	\ # warn if a virtual is overloaded (not overridden)
	-Wpedantic				\ # warn if non-standard C++ is used
	-Wconversion			\ # warn on type conversions that may lose data
	-Wsign-conversion		\ # warn on sign conversions
	-Wnull-dereference		\ # warn if a null dereference is detected
	-Wdouble-promotion		\ # warn if float is implicitly promoted to double
	-Wformat=2				\ # warn on security issues around functions that
							\ # format output
	-Wduplicated-cond		\ # warn if if-then-else chan has duplicated conditions
	-Wduplicated-branches	\ # warn if if-then-else have duplicated code
	-Wlogical-op			\ # warn about logical operations being used where
							\ # bitwise were probably prefered
	-Wuseless-cast			\ # warn if you perform a cast to the same type
	-Wrestrict

QMAKE_CXXFLAGS_DEBUG += -DDEBUG -D_GLIBCXX_DEBUG -Og -g
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -UDEBUG -DNDEBUG -fstrict-aliasing -O3

QMAKE_LFLAGS += -fPIC -O3 -Wl,-O3
QMAKE_LFLAGS_RELEASE += -DNDEBUG -UDEBUG
QMAKE_LFLAGS_DEBUG += -DDEBUG -D_GLIBCXX_DEBUG

equals(ADDRESS_SANITIZER, "YES") {
	# not to be used in combination with valgrind
	QMAKE_CXXFLAGS += -fsanitize=address
	LIBS += -lasan
}

ALE_DIR = $$(HOME)/Documents/projects/ale-dev/ale/
THIS_DIR = $$(HOME)/Documents/projects/ale-dev/interpreter/

CONFIG(debug, debug|release) {
	LIBS += -L$${ALE_DIR}/ale-debug -lale
	PRE_TARGETDEPS += $${ALE_DIR}/ale-debug/libale.so

	LIBS += -L$${THIS_DIR}/intlib-debug -lintlib
	PRE_TARGETDEPS += $${THIS_DIR}/intlib-debug/libintlib.so
}
CONFIG(release, debug|release) {
	LIBS += -L$${ALE_DIR}/ale-release -lale
	PRE_TARGETDEPS += $${ALE_DIR}/ale-release/libale.so

	LIBS += -L$${THIS_DIR}/intlib-release -lintlib
	PRE_TARGETDEPS += $${THIS_DIR}/intlib-release/libintlib.so
}

INCLUDEPATH += $${ALE_DIR}
INCLUDEPATH += $${THIS_DIR}

DEPENDPATH += $${ALE_DIR}

# project files
SOURCES += \
	main.cpp
