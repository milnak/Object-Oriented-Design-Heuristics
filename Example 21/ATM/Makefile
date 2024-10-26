#define macros
EXECUTABLE_NAME = ATM.exe
DIR_SRC = .
DIR_INCLUDE = .
DIR_BIN = .\out\bin
DIR_BIN_X86 = $(DIR_BIN)\x86
DIR_INTERMEDIATE = .\out\int
DIR_INTERMEDIATE_X86 = $(DIR_INTERMEDIATE)\x86

SRC_FILES= \
  $(DIR_SRC)\Atm.cpp \
  $(DIR_SRC)\ATMMain.cpp \
  $(DIR_SRC)\Network.cpp \
  $(DIR_SRC)\Trans.cpp \

{$(DIR_SRC)}.cpp{$(DIR_INTERMEDIATE_X86)}.obj ::
	@echo Compiling...
 	cl /DATM_SIDE /c /EHsc /std:c++17 /Z7 /Fo$(DIR_INTERMEDIATE_X86)\ /I$(DIR_INCLUDE) $<

$(EXECUTABLE_NAME) : $(DIR_INTERMEDIATE_X86)\*.obj
	@echo Linking $(EXECUTABLE_NAME)...
	link /out:$(DIR_BIN_X86)\$(EXECUTABLE_NAME) $(DIR_INTERMEDIATE_X86)\*.obj

# build application
ATM: $(EXECUTABLE_NAME)

# create output directories
create_dirs:
 @if not exist $(DIR_BIN_X86) mkdir $(DIR_BIN_X86)
 @if not exist $(DIR_INTERMEDIATE_X86) mkdir $(DIR_INTERMEDIATE_X86)

# delete output directories
clean:
 @if exist $(DIR_BIN) rmdir /S /Q $(DIR_BIN)
 @if exist $(DIR_INTERMEDIATE) rmdir /S /Q $(DIR_INTERMEDIATE)

# create directories and build application
all: clean create_dirs ATM
