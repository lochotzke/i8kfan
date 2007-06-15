#include <mach/mach_types.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#ifdef DEBUG
static int verbose = 0;
#else
static int verbose = 0;
#endif
#define dbg_printf if(verbose) printf

#define I8K_SMM_FN_STATUS       0x0025
#define I8K_SMM_POWER_STATUS    0x0069
#define I8K_SMM_SET_FAN         0x01a3
#define I8K_SMM_GET_FAN         0x00a3
#define I8K_SMM_GET_SPEED       0x02a3
#define I8K_SMM_GET_TEMP        0x10a3
#define I8K_SMM_GET_DELL_SIG1   0xfea3
#define I8K_SMM_GET_DELL_SIG2   0xffa3
#define I8K_SMM_BIOS_VERSION    0x00a6

#define I8K_FAN_MULT            30
#define I8K_MAX_TEMP            127

#define I8K_FAN_PRIMARY         0
#define I8K_FAN_SECONDARY       1
#define I8K_FAN_OFF             0
#define I8K_FAN_LOW             1
#define I8K_FAN_HIGH            2
#define I8K_FAN_MAX             I8K_FAN_HIGH

#define I8K_POWER_AC            0x05
#define I8K_POWER_BATTERY       0x01
#define I8K_AC                  1
#define I8K_BATTERY             0

#define INIT_REGS               SMMRegisters regs = { 0, 0, 0, 0, 0, 0 }

typedef struct {
    unsigned int eax;
    unsigned int ebx __attribute__ ((packed));
    unsigned int ecx __attribute__ ((packed));
    unsigned int edx __attribute__ ((packed));
    unsigned int esi __attribute__ ((packed));
    unsigned int edi __attribute__ ((packed));
} SMMRegisters;

static void dump_regs(SMMRegisters *regs)
{
    printf("eax = 0x%08x (%d)\n", regs->eax, regs->eax);
    printf("ebx = 0x%08x (%d)\n", regs->ebx, regs->ebx);
    printf("ecx = 0x%08x (%d)\n", regs->ecx, regs->ecx);
    printf("edx = 0x%08x (%d)\n", regs->edx, regs->edx);
    printf("esi = 0x%08x (%d)\n", regs->esi, regs->esi);
    printf("edi = 0x%08x (%d)\n", regs->edi, regs->edi);
}

static int i8k_smm(SMMRegisters *regs)
{
    int rc;
    int eax = regs->eax;
    
    asm("pushl %%eax\n\t" \
        "movl 0(%%eax),%%edx\n\t" \
        "push %%edx\n\t" \
        "movl 4(%%eax),%%ebx\n\t" \
        "movl 8(%%eax),%%ecx\n\t" \
        "movl 12(%%eax),%%edx\n\t" \
        "movl 16(%%eax),%%esi\n\t" \
        "movl 20(%%eax),%%edi\n\t" \
        "popl %%eax\n\t" \
        "out %%al,$0xb2\n\t" \
        "out %%al,$0x84\n\t" \
        "xchgl %%eax,(%%esp)\n\t" \
        "movl %%ebx,4(%%eax)\n\t" \
        "movl %%ecx,8(%%eax)\n\t" \
        "movl %%edx,12(%%eax)\n\t" \
        "movl %%esi,16(%%eax)\n\t" \
        "movl %%edi,20(%%eax)\n\t" \
        "popl %%edx\n\t" \
        "movl %%edx,0(%%eax)\n\t" \
        "lahf\n\t" \
        "shrl $8,%%eax\n\t" \
        "andl $1,%%eax\n" \
        : "=a" (rc)
        : "a" (regs)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
    
    if ((rc != 0) || ((regs->eax & 0xffff) == 0xffff) || (regs->eax == eax)) {
        return -1;
    }
    
    return 0;
}

/*
 * Read the bios version. Return the version as an integer corresponding
 * to the ascii value, for example "A17" is returned as 0x00413137.
 */
static int i8k_get_bios_version(void)
{
    INIT_REGS;
    int rc;

    regs.eax = I8K_SMM_BIOS_VERSION;
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
    }

    return regs.eax;
}

/*
 * Read the power status.
 */
static int i8k_get_power_status(void)
{
    INIT_REGS;
    int rc;

    regs.eax = I8K_SMM_POWER_STATUS;
    if ((rc=i8k_smm(&regs)) < 0) {
        return rc;
    }

    return regs.eax & 0xff; // 0 = No Batt, 3 = No AC, 1 = Charging, 5 = Full.
}

/*
 * Read the fan speed in RPM.
 */
static int i8k_get_fan_speed(int fan)
{
    INIT_REGS;
    int rc;

    regs.eax = I8K_SMM_GET_SPEED;
    regs.ebx = fan & 0xff;
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
    }

    return (regs.eax & 0xffff) * I8K_FAN_MULT;
}

static int i8k_get_fan0_speed(void)
{
    return i8k_get_fan_speed(I8K_FAN_PRIMARY);
}

static int i8k_get_fan1_speed(void)
{
    return i8k_get_fan_speed(I8K_FAN_SECONDARY);
}

/*
 * Read the fan status.
 */
static int i8k_get_fan_status(int fan)
{
    INIT_REGS;
    int rc;

    regs.eax = I8K_SMM_GET_FAN;
    regs.ebx = fan & 0xff;
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
    }

    return (regs.eax & 0xff);
}

static int i8k_get_fan0_status(void)
{
    return i8k_get_fan_status(I8K_FAN_PRIMARY);
}

static int i8k_get_fan1_status(void)
{
    return i8k_get_fan_status(I8K_FAN_SECONDARY);
}

/*
 * Set the fan speed (off, low, high). Returns the new fan status.
 */
static int i8k_set_fan(int fan, int speed)
{
    INIT_REGS;
    int rc;

    speed = (speed < 0) ? 0 : ((speed > I8K_FAN_MAX) ? I8K_FAN_MAX : speed);

    regs.eax = I8K_SMM_SET_FAN;
    regs.ebx = (fan & 0xff) | (speed << 8);
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
    }

    return (i8k_get_fan_status(fan));
}

static int i8k_set_fan0(int speed)
{
    return i8k_set_fan(I8K_FAN_PRIMARY, speed);
}

static int i8k_set_fan1(int speed)
{
    return i8k_set_fan(I8K_FAN_SECONDARY, speed);
}

/*
 * Read the CPU temperature in Celcius.
 */
static int i8k_get_cpu_temp(void)
{
    INIT_REGS;
    int rc;
    int temp;
    
    regs.eax = I8K_SMM_GET_TEMP;
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
	}
	
    temp = regs.eax & 0xff;
    return temp;
}

static int i8k_get_dell_sig_aux(int fn)
{
    INIT_REGS;
    int rc;
    
    regs.eax = fn;
    if ((rc=i8k_smm(&regs)) < 0) {
	    return rc;
    }
    
    if ((regs.eax == 1145651527) && (regs.edx == 1145392204)) {
	    return 0;
    } else {
	    return -1;
    }
}

static int i8k_get_dell_signature(void)
{
    int rc;

    if (((rc=i8k_get_dell_sig_aux(I8K_SMM_GET_DELL_SIG1)) < 0) && 
        ((rc=i8k_get_dell_sig_aux(I8K_SMM_GET_DELL_SIG2)) < 0)) {
	    return rc;
    }

    return 0;
}

/*static int sysctl_i8k_power_status SYSCTL_HANDLER_ARGS
{
    int error = 0, retval;
    
    if (req->newptr) {
        printf("sysctl_i8k_power_status: power status is read-only\n");
    } else {
        retval = i8k_get_power_status();
        printf("sysctl_i8k_power_status: %d\n", retval);
        error = SYSCTL_OUT(req, &retval, sizeof(int));
    }
    
    return error;
}

static int sysctl_i8k_cpu_temp SYSCTL_HANDLER_ARGS
{
    int error = 0, retval;
    
    if (req->newptr) {
        printf("sysctl_i8k_cpu_temp: CPU temperature is read-only\n");
    } else {
        retval = i8k_get_cpu_temp();
        printf("sysctl_i8k_cpu_temp: %d\n", retval);
        error = SYSCTL_OUT(req, &retval, sizeof(int));
    }
    
    return error;
}*/

static int sysctl_i8k_fan0_status SYSCTL_HANDLER_ARGS
{
    int error = 0, retval;

    if (req->newptr) {
        int speed_wanted;
        error = SYSCTL_IN(req, &speed_wanted, sizeof(int));
        if (error)
            return error;
        retval = i8k_set_fan0(speed_wanted);
        dbg_printf("sysctl_i8k_fan0: setting speed to %d\n", retval);
        error = SYSCTL_OUT(req, &retval, sizeof(int));
    } else {
        retval = i8k_get_fan0_status();
        dbg_printf("sysctl_i8k_fan0_status: %d\n", retval);
        error = SYSCTL_OUT(req, &retval, sizeof(int));
    }

    return error;
}

static int sysctl_i8k_fan1_status SYSCTL_HANDLER_ARGS
{
    int error = 0, retval;

    if (req->newptr) {
        int speed_wanted;
        error = SYSCTL_IN(req, &speed_wanted, sizeof(int));
        if (error)
            return error;
        retval = i8k_set_fan1(speed_wanted);
        dbg_printf("sysctl_i8k_fan1: setting speed to %d\n", retval);
        //error = SYSCTL_OUT(req, &retval, sizeof(int));
    } else {
        retval = i8k_get_fan1_status();
        dbg_printf("sysctl_i8k_fan1_status: %d\n", retval);
        error = SYSCTL_OUT(req, &retval, sizeof(int));
    }

    return error;
}

#define SYSCTL_i8k_rd_function(name, handler, description)                       \
        static int sysctl_i8k_##name SYSCTL_HANDLER_ARGS                         \
        {                                                                        \
            int error = 0, retval;                                               \
            if (req->newptr) {                                                   \
                dbg_printf("sysctl_i8k_" #name ": " #description " is read-only\n"); \
            } else {                                                             \
                retval = handler();                                              \
                dbg_printf("sysctl_i8k_" #name ": %d\n", retval);                    \
                error = SYSCTL_OUT(req, &retval, sizeof(int));  }                \
            return error;                                                        \
        }                                                                        \
        SYSCTL_PROC(_i8k, OID_AUTO, name, CTLTYPE_INT | CTLFLAG_RD, 0, 0,        \
                &sysctl_i8k_##name, "I", #description);

SYSCTL_DECL(_i8k);
SYSCTL_NODE(, OID_AUTO, i8k, CTLFLAG_RW, 0, "I8k Statistics");
SYSCTL_i8k_rd_function(power_status, i8k_get_power_status, "Power status");
SYSCTL_i8k_rd_function(cpu_temp,     i8k_get_cpu_temp,     "CPU temperature");
SYSCTL_i8k_rd_function(bios_version, i8k_get_bios_version, "BIOS version");
SYSCTL_i8k_rd_function(fan0_speed,   i8k_get_fan0_speed,   "Fan 0 speed");
SYSCTL_i8k_rd_function(fan1_speed,   i8k_get_fan1_speed,   "Fan 1 speed");

SYSCTL_PROC(_i8k, OID_AUTO, fan0_status, CTLTYPE_INT | CTLFLAG_RW, 0, 0,
    &sysctl_i8k_fan0_status, "I", "Fan 0 status");
SYSCTL_PROC(_i8k, OID_AUTO, fan1_status, CTLTYPE_INT | CTLFLAG_RW, 0, 0,
    &sysctl_i8k_fan1_status, "I", "Fan 1 status");

//SYSCTL_PROC(_i8k, OID_AUTO, power_status, CTLTYPE_INT | CTLFLAG_RD, 0, 0,
//    &sysctl_i8k_power_status, "I", "Power Status");
//SYSCTL_PROC(_i8k, OID_AUTO, cpu_temp, CTLTYPE_INT | CTLFLAG_RD, 0, 0,
//    &sysctl_i8k_cpu_temp, "I", "CPU Temperature");

static struct sysctl_oid *i8k_sysctl_list[] =
{
    &sysctl__i8k_power_status,
    &sysctl__i8k_cpu_temp,
    &sysctl__i8k_bios_version,
    &sysctl__i8k_fan0_speed,
    &sysctl__i8k_fan1_speed,
    &sysctl__i8k_fan0_status,
    &sysctl__i8k_fan1_status,
    (struct sysctl_oid *)0
};


kern_return_t I8kfan_start (kmod_info_t * ki, void * d)
{
    int rc, i;
    
    if ((rc=i8k_get_dell_signature()) < 0) {
        dbg_printf("Unable to get Dell SMM signature!\n");
        return KERN_FAILURE;
    }
    
    sysctl_register_oid(&sysctl__i8k);
    for (i = 0; i8k_sysctl_list[i]; i++) {
        sysctl_register_oid(i8k_sysctl_list[i]);
    }

    dbg_printf("I8kfan loaded!\n");

    return KERN_SUCCESS;
}

kern_return_t I8kfan_stop (kmod_info_t * ki, void * d)
{
    int i;
    
    sysctl_unregister_oid(&sysctl__i8k);
    for (i = 0; i8k_sysctl_list[i]; i++) {
        sysctl_unregister_oid(i8k_sysctl_list[i]);
    }
    
	dbg_printf("I8kfan unloaded!\n");

    return KERN_SUCCESS;
}
