EXE_BASENAME=atm
TARGETSRC= atmmain.cpp atm.cpp network.cpp trans.cpp
TARGETOBJ=$(TARGETSRC:.cpp=.obj)

CC=cl.exe
CFLAGS=/nologo /W4 /WX /Ob1 /EHsc /std:c++17
LINK=link.exe
LFLAGS=/nologo
OUT_DIR=.\build
MAKE=nmake.exe /nologo

!IFDEF DEBUG
BUILDTYPE=debug
CFLAGS = $(CFLAGS) /DDEBUG /Zi /Fd$(OUT_DIR)\$(EXE_BASENAME).pdb
LFLAGS = $(LFLAGS) /DEBUG
!ELSE
BUILDTYPE=release
CFLAGS = $(CFLAGS) /DNDEBUG
LFLAGS = $(LFLAGS) /release
!ENDIF

# Building ATM requires this define.
CFLAGS = $(CFLAGS) /DATM_SIDE

.SUFFIXES: .cpp .obj

usage:
    @echo Usage:
    @echo.
    @echo "nmake <target>"
    @echo.
    @echo "Supported Targets:"
    @echo.
    @echo "debug      Debug Version of $(EXE_BASENAME)"
    @echo "release    Release Version of $(EXE_BASENAME)"
    @echo "clean      Remove *.obj and executable"
    @echo.

debug:
    @$(MAKE) /f Makefile DEBUG=1 bldtarget

release:
    @$(MAKE) /f Makefile RELEASE=1 bldtarget

bldtarget: preproc $(TARGETOBJ)
    $(LINK) $(LFLAGS) /out:$(OUT_DIR)\$(EXE_BASENAME).exe $(OUT_DIR)\$(BUILDTYPE)\*.obj

preproc: bldmsg
    @if not exist $(OUT_DIR)\$(BUILDTYPE) mkdir $(OUT_DIR)\$(BUILDTYPE)

bldmsg:
    @echo Building $(EXE_BASENAME) - $(BUILDTYPE)
    @echo.

.cpp.obj :
   $(CC) $(CFLAGS) /Fo$(OUT_DIR)\$(BUILDTYPE)\$@ /c $<

clean:
    @if exist $(OUT_DIR)\debug\*.obj del $(OUT_DIR)\debug\*.obj
    @if exist $(OUT_DIR)\release\*.obj del $(OUT_DIR)\release\*.obj
    @if exist $(OUT_DIR)\$(EXE_BASENAME).exe $(OUT_DIR)\$(EXE_BASENAME).exe
    @echo Clean complete
