OS_NAME := $(shell uname)
OS_VER  := $(shell uname -r)

DEFAULT_MAK_DIR := common/examples

ifeq ($(OS_NAME), SunOS)
include $(DEFAULT_MAK_DIR)/solaris.default.mak
TARGET_OS_VERSION := $(shell echo $(OS_VER) | sed -n -e "s/5\.\([0-9]\)/\1/p")
ifeq ($(TARGET_OS_VERSION), 6)
TARGET_OS_VERSION := 2.6
endif
ifeq ($(SUNWS), on)
COMPILER_TOOL := sun
COMPILER_TOOL_VERSION := 5
endif
endif

ifeq ($(OS_NAME), Linux)
ifeq ($(MVISTA), on)
include $(DEFAULT_MAK_DIR)/mvista.default.mak
else
ifeq ($(UCLINUX), on)
include $(DEFAULT_MAK_DIR)/uclinux.default.mak
else
include $(DEFAULT_MAK_DIR)/redhat.default.mak
TARGET_OS_VERSION := redhat-$(shell cat /etc/redhat-release | awk '{print $$5}')
COMPILER_TOOL_VERSION := $(shell gcc -v 2>&1 | sed -n -e "s/gcc version \([0-9]\.[0-9]\).*/\1/p")
endif
endif
endif

ifeq ($(OS_NAME), UnixWare)
include $(DEFAULT_MAK_DIR)/unixware.default.mak
endif

ifeq ($(OS_NAME), OSF1)
include $(DEFAULT_MAK_DIR)/tru64.default.mak
endif

ifeq ($(OS_NAME), HP-UX)
include $(DEFAULT_MAK_DIR)/hpux.default.mak
endif

# --------------- CygWin: VxWorks, Symbian, Mopi, Nucleus, pSOS, OSE, Win32 -------------

ifeq ($(findstring CYGWIN,$(OS_NAME)), CYGWIN)

# VxWorks
ifneq ($(TORNADO_VERSION), )
include $(DEFAULT_MAK_DIR)/vxworks.gnu.default.mak
TARGET_OS_VERSION := $(TORNADO_VERSION)
endif

# Symbian
ifneq ($(SYMBIAN_ROOT), )
include $(DEFAULT_MAK_DIR)/symbian.default.mak
ifeq ($(MWCC), on)
COMPILER_TOOL := metrowerks
COMPILER_TOOL_VERSION := 2.4.7
endif
endif

# MOPI
# note that MOPI section MUST com before Nucleus section, since MOPI sets up nucleus environment too
ifneq ($(MOPI_ROOT), )
include $(DEFAULT_MAK_DIR)/mopi.ads.arm.default.mak
endif

# Nucleus
ifneq ($(NUCLEUS_ROOT), )
ifneq ($(DIABLIB), )
include $(DEFAULT_MAK_DIR)/nucleus.diab.ppc.default.mak # PPC
else
include $(DEFAULT_MAK_DIR)/nucleus.ads.arm.default.mak # ARM
endif
endif

# pSOS
ifneq ($(PSS_ROOT), )
include $(DEFAULT_MAK_DIR)/psos.diab.ppc.default.mak
endif

# OSE
ifneq ($(OSE_ROOT), )
include $(DEFAULT_MAK_DIR)/ose.ghs.ppc.default.mak
endif

# INTEGRITY
ifneq ($(INTEGRITY_ROOT), )
include $(DEFAULT_MAK_DIR)/integrity.ghs.default.mak
endif

# Microsoft Visual C
ifneq ($(MSVC_ROOT), )
include $(DEFAULT_MAK_DIR)/win32.default.mak
endif

endif # CYGWIN (EMBEDDED)

$(warning *** TARGET_OS: $(TARGET_OS)($(TARGET_OS_VERSION)), COMPILER_TOOL: $(COMPILER_TOOL)($(COMPILER_TOOL_VERSION)) ***)