#!/bin/sh
#
# Script that attempts to detect the CPU/platform automatically.
# This script is modeled after the equivalent routines as
# distributed with the ATLAS configuration system.
#

# Find utilities
uname_bin="`which uname 2>/dev/null`"
if test -x "${uname_bin}" ; then
    uname_p="`\"${uname_bin}\" -p 2>/dev/null`"
    uname_i="`\"${uname_bin}\" -i 2>/dev/null`"
    uname_m="`\"${uname_bin}\" -m 2>/dev/null`"
    uname_s="`\"${uname_bin}\" -s 2>/dev/null`"
else
    uname_bin=""
    uname_p=""
    uname_i=""
    uname_m=""
    uname_s=""
fi

psrinfo_bin="`which psrinfo 2>/dev/null`"
if test ! -x "${psrinfo_bin}" ; then
    psrinfo_bin=/usr/sbin/psrinfo
    if test ! -x "${psrinfo_bin}" ; then
	psrinfo_bin=""
    fi
fi

pinfo_bin="`which pinfo 2>/dev/null`"
if test ! -x "${pinfo_bin}" ; then
    pinfo_bin=/usr/sbin/pinfo
    if test ! -x "${pinfo_bin}" ; then
	pinfo_bin=""
    fi
fi

#---------------------------------------------------------------------
# Canonicalize OS name

OS="${uname_s-UNKNOWN}"
case "${OS}" in
    *Linux*) OS=Linux ;;
    *FreeBSD*) OS=FreeBSD ;;
    *Darwin*) OS=Darwin ;;
    *SunOS*) OS=SunOS ;;
    *OSF1*) OS=OSF1 ;;
    *IRIX*) OS=IRIX ;;
    *AIX*) OS=AIX ;;
    *cygwin*) OS=cygwin ;;
    *WIN*) OS=Windows ;;
    *HP-UX*) OS=HPUX ;;
esac

#---------------------------------------------------------------------
VENDOR=UNKNOWN
CPU=UNKNOWN
CLOCK=UNKNOWN
CACHE=UNKNOWN

# --- Linux ---
if test x"${OS}" = xLinux && test -f /proc/cpuinfo ; then
    model_name="`grep \"^model name[[:space:]]*:\" /proc/cpuinfo | tail -1`"
    if test -z "${model_name}" ; then
	model_name="`grep \"^model[[:space:]]*:\" /proc/cpuinfo | tail -1`"
    fi
    if test -z "${model_name}" ; then
	model_name="`grep \"^cpu model[[:space:]]*:\" /proc/cpuinfo | tail -1`"
    fi
    case "${model_name}" in
	*Opteron*) VENDOR=AMD ; CPU=Opteron ;;
	*Pentium*\ M*) VENDOR=Intel ; CPU=Pentium_M ;;
	*XEON* | *Xeon* | *Pentium*\ 4*) VENDOR=Intel ; CPU=Pentium_4 ;;
	*Pentium*\ III*) VENDOR=Intel ; CPU=Pentium_III ;;
	*Pentium*\ II*) VENDOR=Intel ; CPU=Pentium_II ;;
	*Pentium*Pro*) VENDOR=Intel ; CPU=Pentium_Pro ;;
	*Itanium\ 2*) VENDOR=Intel ; CPU=Itanium_2 ;;
	*Itanium*) VENDOR=Intel ; CPU=Itanium ;;
	*EV4*) VENDOR=DEC ; CPU=Alpha21064 ;;
	*EV5*) VENDOR=DEC ; CPU=Alpha21164 ;;
	*EV6*) VENDOR=DEC ; CPU=Alpha21264 ;;
	*Efficeon*) VENDOR=TransMeta ; CPU=Efficeon ;;
	*Athlon\ HX*) VENDOR=AMD ; CPU=AthlonHX ;;
	*Athlon*64*) VENDOR=AMD ; CPU=Athlon64 ;;
	*Athlon*) VENDOR=AMD ; CPU=Athlon ;;
	*AMD-K7*) VENDOR=AMD ; CPU=K7 ;;
    esac
    if test x"${CPU}" = xUNKNOWN ; then
	model_name="`grep \"^family[[:space:]]*:\" /proc/cpuinfo | tail -1`"
	case "${model_name}" in
	    *Itanium\ 2*) VENDOR=Intel ; CPU=Itanium_2 ;;
	    *Itanium*) VENDOR=Intel ; CPU=Itanium_2 ;;
	esac
    fi
    if test x"${CPU}" = xUNKNOWN ; then
	model_name="`grep \"^cpu[[:space:]]*:\" /proc/cpuinfo | tail -1`"
	case "${model_name}" in
	    *POWER5*) VENDOR=IBM ; CPU=Power5 ;;
	    *POWER4*) VENDOR=IBM ; CPU=Power4 ;;
	    *POWER3*) VENDOR=IBM ; CPU=Power3 ;;
	    *POWER2*) VENDOR=IBM ; CPU=Power2 ;;
	    *604e*) VENDOR=IBM ; CPU=PowerPC604e ;;
	    *604*) VENDOR=IBM ; CPU=PowerPC604 ;;
	    *G4* | *7400* | *7410* | *7455*) VENDOR=IBM ; CPU=G4 ;;
	    *PPC970FX*) VENDOR=IBM ; CPU=G5 ;;
	esac
    fi

    cpu_MHz="`grep \"^cpu MHz[[:space:]]*:\" /proc/cpuinfo | tail -1 | awk '{print $4;}'`"
    case "${cpu_MHz}" in
	[0-9]*[.0-9]) CLOCK="${cpu_MHz}-MHz" ;;
    esac
    if test x"${CLOCK}" = xUNKNOWN ; then
	clock_rate="`grep \"^cpu[[:space:]]*:\" /proc/cpuinfo | tail -1 | awk '{print $3;}'`"
	case "${clock_rate}" in
	    [0-9]*[.0-9]*MHz)
	    CLOCK="`echo \"${clock_rate}\" | sed 's,MHz,,'`-MHz" ;;
	    [0-9]*[.0-9]*GHz)
	    CLOCK="`echo \"${clock_rate}\" | sed 's,GHz,,'`-GHz" ;;
	esac
    fi

    cache_size="`grep \"^cache size[[:space:]]*:\" /proc/cpuinfo | tail -1 | awk '{print $4\"-\"$5;}'`"
    case "${cache_size}" in
	[0-9]*-?B) CACHE="${cache_size}" ;;
    esac
fi

# FreeBSD/Darwin
if test x"${CPU}" = xUNKNOWN && \
	(test x"${OS}" = xFreeBSD || test x"${OS}" = xDarwin) \
; then
    sysctl_bin="`which sysctl 2>/dev/null`"
    if test -x "${sysctl_bin}" ; then
	sysctl_out="`\"${sysctl_bin}\" hw.model`"
	case "${sysctl_out}" in
	    *PowerMac*4* | *PowerMac*3,[356]*) VENDOR=IBM ; CPU="G4" ;;
	    *PowerMac*7,2*) VENDOR=IBM ; CPU="G5" ;;
	    *433au*) VENDOR=DEC ; CPU=Alpha21164 ;;
	    *XP1000*) VENDOR=DEC ; CPU=Alpha21264 ;;
	    *Pentium\ Pro*) VENDOR=Intel ; CPU=Pentium_Pro ;;
	    *Pentium\ III*) VENDOR=Intel ; CPU=Pentium_III ;;
	    *Pentium\ II*) VENDOR=Intel ; CPU=Pentium_II ;;
	    *Athlon*) VENDOR=AMD ; CPU=Athlon ;;
	    *AMD-K7*) VENDOR=AMD ; CPU=K7 ;;
	    *32\ bit\ Hammer*) VENDOR=AMD ; CPU=Hammer32 ;;
	    *64\ bit\ Hammer*) VENDOR=AMD ; CPU=Hammer64 ;;
	    *Pentium/P55C*) VENDOR=Intel ; CPU=Pentium_MMX ;;
	    *Pentium*) VENDOR=Intel ; CPU=Pentium ;;
	esac
    fi
fi

# --- SunOS ---
if test x"${OS}" = xSunOS && test -x "${psrinfo_bin}" ; then
    proc_info="`\"${psrinfo_bin}\" -v`"
    case "${proc_info}" in
	*sparcv9\ processor*)
	VENDOR=Sun ; CPU=sparcv9 ;;
	*sparcv8\ processor*)
	VENDOR=Sun ; CPU=sparcv8 ;;
    esac
    case "${proc_info}" in
	*[.0-9]*\ MHz*)
	CLOCK="`\"${psrinfo_bin}\" -v | grep MHz | tail -1 | awk '{for(i=1;i<=NF;i++){if($i==\"MHz,\") print $(i-1);}}'`"
	if test x"${CPU}" = xsparcv9 && test "${CLOCK}" -gt 800 ; then
	    CPU="UltraSPARC_III"
	else
	    CPU="UltraSPARC"
	fi
	CLOCK="${CLOCK}-MHz"
	;;
	*[.0-9]*\ GHz*)
	CLOCK="`\"${psrinfo_bin}\" -v | grep GHz | tail -1 | awk '{for(i=1;i<=NF;i++){if($i==\"GHz,\") print $(i-1);}}'`"
	if test x"${CPU}" = xsparcv9 ; then
	    CPU="UltraSPARC_III"
	fi
	CLOCK="${CLOCK}-GHz"
	;;
    esac
fi

# OSF1 (DEC)
if test x"${OS}" = xOSF1 ; then
    case "${uname_m}" in
	*alpha*) 
	    VENDOR=DEC
	    proc_info=""
	    if test -x "${pinfo_bin}" ; then
		proc_info="`\"${pinfo_bin}\" -v | grep EV`"
	    fi
	    if test -z "${proc_info}" && test -x "${psrinfo_bin}" ; then
		proc_info="`\"${psrinfo_bin}\" -v | grep EV`"
	    fi
	    case "${proc_info}" in
		*EV6*) CPU=Alpha21264 ;;
		*EV5*) CPU=Alpha21164 ;;
		*EV4*) CPU=Alpha21064 ;;
	    esac
	    ;;
    esac
fi

# IRIX (SGI)
# From an ATLAS thread:
#   IP21 should be R8000
#   IP22 should be R4400 or R5000
#   IP25 should be R10000
#   IP26 should be R8000
#   IP27 should be R10000 or R12000
#   IP28 should be R10000
#   IP30 should be R10000 or R12000 or R14000 (dunno about R16000)
#   IP32 should be R5000, R10000, or R12000 (dunno regarding R14000/R16000)
#   IP35 should be R14000 or R16000
#
if test x"${OS}" = xIRIX ; then
    VENDOR=SGI
    case "${uname_m}" in
	*IP21*) CPU=IP21 ;; # R8000
	*IP22*) CPU=IP22 ;; # R4400 or R5000
	*IP25*) CPU=IP25 ;; # R10k
	*IP26*) CPU=IP26 ;; # R8000
	*IP27*) CPU=IP27 ;; # R10k or R12k
	*IP28*) CPU=IP28 ;; # R10k
	*IP30*) CPU=IP30 ;; # R10k, R12k, R14k
	*IP32*) CPU=IP32 ;; # R5000, R10k, R14k
	*IP35*) CPU=IP35 ;; # R14k or R16k
    esac
fi

if test x"${OS}" = xAIX && test -x ./getcpu ; then
    VENDOR=IBM
    CPU=`./getcpu`
fi

if test x"${OS}" = xHPUX ; then
    case "${uname_m}" in
	*9000/735*) VENDOR=HP ; CPU=9K735 ;;
    esac
    if test x"${CPU}" = xUNKNOWN ; then
	file_out="`file /stand/vmunix`"
	case "${file_out}" in
	    *PA-RISC\ 2.0*) VENDOR=HP ; CPU=PA20 ;;
	esac
    fi
fi

# Default test -- use 'uname'
if test x"${CPU}" = xUNKNOWN && test -x "${uname_bin}" ; then
    CPU="${uname_p}"
    if test -z "${CPU}" ; then
	CPU="${uname_i}"
    fi
fi

echo "${VENDOR}--${CPU}--${CLOCK}--${CACHE}"

# eof
