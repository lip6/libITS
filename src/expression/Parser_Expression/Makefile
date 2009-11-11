################
# Programm name #
################

PROGRAM = libEXPRESS.a

##################
# Names of file  #
##################

##################### source ########################################

# Source files .c
FILES = expression.tab.cpp expression.lex.cpp RdPE.cpp

######################
# Names of directory #
######################

# Bin directory: where to put program
BINDIR = .

# Source directory: where to find .c
SRCDIR = ../../parser_RdPE

# Include directory: where to find .h
INCDIR = ../../expression ../../parser_RdPE

# Library directory: where to find library
LIBDIR = ../../expression ../../parser_RdPE

# Working directory
OBJDIR = .
###############
# extra flags #
############### 

# compiler flag
CFLAGS = -ggdb -Wall
#CFLAGS = -O3 -Wall

#################
# Program names #
#################

CC = gcc
CXX = g++
# CXX =g++
LINKER = $(CXX)

############## Automatic ########################################## 
# Every Makefile should contain
SHELL = /bin/sh

# Compute the file list for each type

CCFILES =$(filter %.c,$(FILES))
CPPFILES =$(filter %.cpp,$(FILES))

# OFILES = $(CCFILES:.c=.o) 
DFILES = $(addprefix $(OBJDIR)/,$(CCFILES:.c=.d)) $(addprefix $(OBJDIR)/,$(CPPFILES:.cpp=.d))
OFILES = $(addprefix $(OBJDIR)/,$(CCFILES:.c=.o)) $(addprefix $(OBJDIR)/,$(CPPFILES:.cpp=.o))
# Constructs path to search for 

vpath %.cpp $(addsuffix :,$(SRCDIR)).
vpath %.c $(addsuffix :,$(SRCDIR)).
vpath %.h $(addsuffix :,$(INCDIR)).
vpath $(PROGRAM) $(BINDIR)
vpath %.o .
vpath %.d
vpath %.a $(LIBDIR)


# Constructs real name options for directory

REAL_LIBDIR     = $(addprefix -L, $(LIBDIR))
REAL_INCDIR     = $(addprefix -I,$(INCDIR))

# Simple targets

.PHONY:all clean depend parsers parsersclean

all : $(PROGRAM)
	@echo "all Done"

clean :
	@echo -n "Cleaning directory..."
	@-/bin/rm -f *.d
	@-/bin/rm -f *.o
	@-/bin/rm -f *.tab.*
	@-/bin/rm -f *.lex.*
	@-/bin/rm -f $(PROGRAM)
	@-/bin/rm -f *~
	@echo clean Done

# Link object files

$(PROGRAM): $(OFILES)
	@echo -n "Linking $(PROGRAM) ..."
	ar rv $(PROGRAM) ../expression.o ../Word.o $(OFILES)

# Implicite rules
.SUFFIXES : .tab.c .lex.c .tab.d .lex.d .c .o .d .l .y .cpp

# Rule to compile .c files (handle correctly C++ // comments)

%.o : %.cpp  
	@echo -n "Compiling $< :... "
	$(CXX) -c $(CFLAGS) $(REAL_INCDIR) $(REAL_LIB) $<
	@echo "Done"

# Rule to build dependencied for each file when necessary

%.d :%.cpp 
	@echo -n "Computing dependencies for $< ... "
	@$(SHELL) -ec '$(CXX) -M -I$(<D) $(CFLAGS) $(REAL_INCDIR) $(REAL_LIB) $< | sed '\''s,$*.o,$(OBJDIR)/$*.d $(OBJDIR)/$*.o,g'\''>$@'
	@echo "Done"

%.tab.cpp: %.y
	bison -p expression -d $<
	mv $*.tab.c $*.tab.cpp

%.lex.cpp: %.l %.tab.cpp
	flex	-oexpression.lex.c -Pexpression $<
	mv $*.lex.c $*.lex.cpp

-include $(DFILES)








