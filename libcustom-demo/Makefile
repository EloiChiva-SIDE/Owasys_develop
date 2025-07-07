#!make
#Environment
OWA300_ENV=/opt/crosstool/setup_owa300_env
OWA4_ENV=/opt/crosstool/setup_owa4x_env
OWA5_ENV=/opt/crosstool/setup_owa5x_env
MKDIR = mkdir
CP = cp
DPKG = dpkg-deb

CND_DISTDIR=dist
CND_BUILDDIR=build
CND_INSTALLDIR=install
#Package installation version
DPKG_VERSION = 1
# Name of the project
NAME=libcustom-demo
VERSION=1.0.2
LIB_VERSION=$(VERSION)
OBJECT=$(NAME).so
CFLAGS= -Wall -Wno-format-truncation `pkg-config --cflags dbus-1` -Wl,--no-undefined
# To include new directories there must be added with the format -I<includeDir>
INCLUDE=-Iinc
SOURCE=src/*.c
# Link Libraries and Options
LIBS = -lm -lhiredis -lev `pkg-config --libs dbus-1` -ldl -l:libpollux.so.1
STRIP_OPTION= -s
ISLIBRARY= --shared -fPIC
GDB=-g
#Definitions
# GIT_HASH=$(shell git rev-parse --short HEAD)
# GIT_BRANCH=$(shell git rev-parse --abbrev-ref HEAD)
# GIT_CHANGES=$(shell git diff -- '*.c' '*.h' --stat)

# ifeq (,$(GIT_CHANGES))
#     GIT_STATUS=$(GIT_BRANCH)_$(GIT_HASH)
# else
#     GIT_STATUS=$(GIT_BRANCH)_$(GIT_HASH)_dirty
# endif
# DEFINE=-D_MATH -DGIT_STATUS=\"${GIT_STATUS}\"
DEFINE= -DNAME=\"${NAME}\" -DVERSION=\"${VERSION}\"
MSG_BUILD = '**** Building $(OBJECT)'

#Build entries
owa300: build_owa300
owa4: build_owa4x
owa5: build_owa5x
Linux: build_pc

# Build tasks
build_owa300:
	@echo
	@echo $(MSG_BUILD) owa300
	${MKDIR} -p ${CND_DISTDIR}/owa300
	. $(OWA300_ENV); \
	echo "**** Compiler CC="$${CC}; \
	$${CC} $(CFLAGS) $(DEFINE) -DOWA300 -o$(CND_DISTDIR)/owa300/$(OBJECT) $(INCLUDE) $(SOURCE) -O2 -s $(LIBS) $(LDLIBSOPTIONS) $(ISLIBRARY);
	@echo $(MSG_BUILD) owa300 Done!!

build_owa4x:
	@echo
	@echo $(MSG_BUILD) owa4x
	${MKDIR} -p ${CND_DISTDIR}/owa4x
	. $(OWA4_ENV); \
	echo "**** Compiler CC="$${CC}; \
	$${CC} $(CFLAGS) $(DEFINE) -DOWA4X -o$(CND_DISTDIR)/owa4x/$(OBJECT) $(INCLUDE) $(SOURCE) -O2 -s $(LIBS) $(LDLIBSOPTIONS) $(ISLIBRARY);
	@echo $(MSG_BUILD) owa4x Done!!

build_owa5x:
	@echo
	@echo $(MSG_BUILD) owa5x
	${MKDIR} -p ${CND_DISTDIR}/owa5x
	. $(OWA5_ENV); \
	echo "**** Compiler CC="$${CC}; \
	$${CC} $(CFLAGS) $(DEFINE) -DOWA5X -o$(CND_DISTDIR)/owa5x/$(OBJECT) $(INCLUDE) $(SOURCE) -O2 -s $(LIBS) $(LDLIBSOPTIONS) $(ISLIBRARY);
	@echo $(MSG_BUILD) owa5x Done!!

build_pc:
	@echo
	@echo $(MSG_BUILD) linux
	${MKDIR} -p ${CND_DISTDIR}/linux
	$(CC) $(CFLAGS) $(DEFINE) $(GDB) -DPC -o$(CND_DISTDIR)/linux/$(OBJECT) $(INCLUDE) $(SOURCE) $(PC_LIBS) $(LIBS) $(LDLIBSOPTIONS) $(ISLIBRARY)
	@echo $(MSG_BUILD) linux Done!!
	
clean:
	rm -Rf dist/*

clean:
	rm -Rf $(CND_DISTDIR)

.PHONY: owa300 owa4 owa5 Linux clean all deb .deb-copyfiles .deb-owa300 .deb-owa4x .deb-owa5x

all: owa300 owa4 owa5 Linux

deb: .deb-copyfiles .deb-owa300 .deb-owa4x .deb-owa5x
	@echo '**** deb packages Created!!'

deb-pc: .deb-copyfiles .deb-pc
	@echo '**** deb linux package Created!!'


.deb-copyfiles: 
	@echo
	@echo '**** Building deb package'
	@echo
	@echo '**** Creating directories'

	${MKDIR} -p ${CND_DISTDIR}/deb/usr/local/lib
	${MKDIR} -p ${CND_DISTDIR}/deb/usr/share/doc/owasys/$(NAME)
	@echo "**** Copying common files"
	${CP} -r ${CND_INSTALLDIR}/etc ${CND_DISTDIR}/deb
	${CP} ${CND_INSTALLDIR}/changelog ${CND_DISTDIR}/deb/usr/share/doc/owasys/$(NAME)
	${CP} ${CND_INSTALLDIR}/copyright ${CND_DISTDIR}/deb/usr/share/doc/owasys/$(NAME)

.deb-owa300:
	@echo
	@echo '**** Building deb package for owa300'
	@echo
	${CP} -r ${CND_INSTALLDIR}/DEBIAN ${CND_DISTDIR}/deb
	@chmod a+x ${CND_DISTDIR}/deb/DEBIAN/postinst
	@sed -i -e 's/_version/$(LIB_VERSION)/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@sed -i -e 's/_arch/armhf/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@echo '**** Creating owa300 binaries'
	${CP} ${CND_DISTDIR}/owa300/$(OBJECT) ${CND_DISTDIR}/deb/usr/local/lib
	@echo '**** Creating deb package'
	${DPKG} -Zgzip --build ${CND_DISTDIR}/deb ${CND_DISTDIR}/$(NAME)-owa300-$(LIB_VERSION)-$(DPKG_VERSION)_armhf.deb
	@echo '**** Done!!'

.deb-owa4x:
	@echo
	@echo '**** Building deb package for owa4X'
	@echo
	${CP} -r ${CND_INSTALLDIR}/DEBIAN ${CND_DISTDIR}/deb
	@chmod a+x ${CND_DISTDIR}/deb/DEBIAN/postinst	
	@sed -i -e 's/_version/$(LIB_VERSION)/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@sed -i -e 's/_arch/armhf/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@echo '**** Creating owa4X binaries'
	${CP} ${CND_DISTDIR}/owa4x/$(OBJECT) ${CND_DISTDIR}/deb/usr/local/lib
	@echo '**** Creating deb package'
	${DPKG} -Zgzip --build ${CND_DISTDIR}/deb ${CND_DISTDIR}/$(NAME)-owa4x-$(LIB_VERSION)-$(DPKG_VERSION)_armhf.deb
	@echo '**** Done!!'

.deb-owa5x:
	@echo
	@echo '**** Building deb package for owa5X'
	@echo
	${CP} -r ${CND_INSTALLDIR}/DEBIAN ${CND_DISTDIR}/deb	
	@chmod a+x ${CND_DISTDIR}/deb/DEBIAN/postinst
	@sed -i -e 's/_version/$(LIB_VERSION)/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@sed -i -e 's/_arch/arm64/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@echo '**** Creating owa5X binaries'
	${CP} ${CND_DISTDIR}/owa5x/$(OBJECT) ${CND_DISTDIR}/deb/usr/local/lib
	@echo '**** Creating deb package'
	${DPKG} -Zgzip --build ${CND_DISTDIR}/deb ${CND_DISTDIR}/$(NAME)-owa5x-$(LIB_VERSION)-$(DPKG_VERSION)_arm64.deb
	@echo '**** Done!!'

.deb-pc:
	@echo
	@echo '**** Building deb package for amd64'
	@echo
	${CP} -r ${CND_INSTALLDIR}/DEBIAN ${CND_DISTDIR}/deb	
	@chmod a+x ${CND_DISTDIR}/deb/DEBIAN/postinst
	@sed -i -e 's/_version/$(LIB_VERSION)/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@sed -i -e 's/_arch/amd64/g' ${CND_DISTDIR}/deb/DEBIAN/control
	@echo '**** Creating amd64 binaries'
	${CP} ${CND_DISTDIR}/linux/$(OBJECT) ${CND_DISTDIR}/deb/usr/local/lib
	@echo '**** Creating deb package'
	${DPKG} -Zgzip --build ${CND_DISTDIR}/deb ${CND_DISTDIR}/$(NAME)-$(LIB_VERSION)-$(DPKG_VERSION)_amd64.deb
	@echo '**** Done!!'
