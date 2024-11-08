TEMPLATE = lib

VERSION_MAJOR = 99.99
VERSION_BUILD = 99
VERSION = $${VERSION_MAJOR}.$${VERSION_BUILD}

CONFIG += c++20
CONFIG -= app_bundle
CONFIG -= qt
QT -= core gui

isEmpty(ADDRESS_SANITIZER) {
	ADDRESS_SANITIZER = "NO"
}

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

ALE_DIR = $$(HOME)/Documents/projects/ale-dev/ale
CONFIG(debug, debug|release) {
	LIBS += -L$${ALE_DIR}/ale-debug -lale
	PRE_TARGETDEPS += $${ALE_DIR}/ale-debug/libale.so
}
CONFIG(release, debug|release) {
	LIBS += -L$${ALE_DIR}/ale-release -lale
	PRE_TARGETDEPS += $${ALE_DIR}/ale-release/libale.so
}

INCLUDEPATH += $${ALE_DIR}

THIS_DIR = $$(HOME)/Documents/projects/ale-dev/interpreter
INCLUDEPATH += $${THIS_DIR}

DEPENDPATH += $${ALE_DIR}

# project files
HEADERS += \
	detail/any_arithmetic.hpp \
	detail/any_comparison.hpp \
	detail/any_to_bool.hpp \
	memory/memory.hpp \
	memory/scope.hpp \
	memory/subscope.hpp \
	program.hpp

SOURCES += \
	ast/n_ary_nodes/arithmetic_node.cpp \
	ast/n_ary_nodes/comma_separated_group_node.cpp \
	ast/n_ary_nodes/comparison_node.cpp \
	ast/n_ary_nodes/logical_node.cpp \
	ast/n_ary_nodes/subscope_modifier_node.cpp \
	ast/n_ary_nodes/subscripted_variable_node.cpp \
	ast/ternary_nodes/if_else_node.cpp \
	ast/binary_nodes/assignation_node.cpp \
	ast/binary_nodes/declaration_node.cpp \
	ast/binary_nodes/variable_sequence_node.cpp \
	ast/binary_nodes/while_loop_node.cpp \
	ast/unary_nodes/negation_node.cpp \
	ast/unary_nodes/negative_node.cpp \
	ast/unary_nodes/positive_node.cpp \
	ast/zero_ary_nodes/false_node.cpp \
	ast/zero_ary_nodes/literal_node.cpp \
	ast/zero_ary_nodes/true_node.cpp \
	ast/zero_ary_nodes/variable_node.cpp \
	detail/any_arithmetic.cpp \
	detail/any_arithmetic_addition.cpp \
	detail/any_arithmetic_division.cpp \
	detail/any_arithmetic_exponentiation.cpp \
	detail/any_arithmetic_modulus.cpp \
	detail/any_arithmetic_multiplication.cpp \
	detail/any_arithmetic_subtraction.cpp \
	detail/any_comparison.cpp \
	detail/any_comparison_equal.cpp \
	detail/any_comparison_greater.cpp \
	detail/any_comparison_greater_equal.cpp \
	detail/any_comparison_less.cpp \
	detail/any_comparison_less_equal.cpp \
	detail/any_comparison_not_equal.cpp \
	memory/memory.cpp \
	memory/scope.cpp \
	memory/subscope.cpp \
	program.cpp
