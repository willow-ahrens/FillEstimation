/**
 *  \file getcpu.c
 *  \brief Get the CPU architecture using a run-time probe.
 */

#include <config/config.h>
#include <stdio.h>

/* --------------------------------------------------------------
 * AIX systems
 */
#if HAVE_SYS_SYSTEMCFG_H
#include <sys/systemcfg.h>

/* The following code snippet taken from CONFIG/archinfo_aix.c
 * distributed with ATLAS 3.7.10.
 */
/*
 * ifdef guards for older systemcfg.h's:
 */
#ifndef POWER_620
#define POWER_620 0x0040
#endif
#ifndef POWER_630
#define POWER_630 0x0080
#endif
#ifndef POWER_A35
#define POWER_A35 0x0100
#endif
#ifndef POWER_RS64II
#define POWER_RS64II 0x0200
#endif
#ifndef POWER_RS64III
#define POWER_RS64III 0x0400
#endif
#ifndef IA64
#define IA64 0x0003
#endif
#ifndef IA64_M1
#define IA64_M1 0x0001
#endif
#ifndef IA64_M2
#define IA64_M2 0x0002
#endif

int
main (int argc, char *argv[])
{
  const char *cpu = NULL;
  switch (_system_configuration.architecture)
    {
    case POWER_RS:
    case POWER_PC:
      switch (_system_configuration.implementation)
	{
	case POWER_RS1:
	  cpu = "Power1";
	  break;
	case POWER_RSC:
	  cpu = "PowerRSC";
	  break;
	case POWER_RS2:
	  cpu = "Power2";
	  break;
	case POWER_601:
	  cpu = "PowerPC601";
	  break;
	case POWER_603:
	  cpu = "PowerPC603";
	  break;
	case POWER_604:
	  if (_system_configuration.dcache_size == 32 * 1024)
	    cpu = "PowerPC604e";
	  else
	    cpu = "PowerPC604";
	  break;
	case POWER_620:
	  cpu = "PowerPC620";
	  break;
	case POWER_630:
	  cpu = "Power3";
	  break;
	case POWER_A35:
	  cpu = "PowerPCA35";
	  break;
	case POWER_RS64II:
	  cpu = "PowerRS64III";
	  break;
#if defined(POWER_4)
	case POWER_4:
	  cpu = "Power4";
	  break;
#endif
#if defined(POWER_5)
	case POWER_5:
	  cpu = "Power5";
	  break;
#endif
	default:
	  if (__power_rs ())
	    cpu = "PowerUNKNOWN";
	  else if (__power_pc ())
	    cpu = "PowerPCUNKNOWN";
	  break;
	}
      break;
    case IA64:
      switch (_system_configuration.implementation)
	{
	case IA64_M1:
	  cpu = "Itanium";
	  break;
	case IA64_M2:
	  cpu = "Itanium_2";
	  break;
	default:
	  cpu = "IA64";
	  break;
	}
    }
  printf ("%s", cpu == NULL ? "UNKNOWN" : cpu);
  return 0;
}

/* --------------------------------------------------------------
 *  Default: do nothing
 */
#else
int
main (int argc, char *argv[])
{
  printf ("UNKNOWN");
  return 0;
}
#endif

/* eof */
