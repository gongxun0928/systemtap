/* Included once by translate.cxx c_unparser::emit_common_header ()
   Defines all common fields and probe flags for struct context.
   Available to C-based probe handlers as fields of the CONTEXT ptr.  */

/* Used to indicate whether a probe context is in use.
   Tested in the code entering the probe setup by common_probe_entry_prologue
   and cleared by the common_probe_entry_epilogue code. When an early error
   forces a goto probe_epilogue then needs an explicitly atomic_dec() first.
   All context busy flags are tested on module unload to prevent unloading
   while some probe is still running.  */
atomic_t busy;

/* The fully-resolved probe point associated with a currently running probe
   handler, including alias and wild-card expansion effects.
   aka stap_probe->pp.  Setup by common_probe_entryfn_prologue.
   Used in warning/error messages and accessible by pp() tapset function.  */
const char *probe_point;

/* The script-level probe point associated with a currently running probe
   handler, including  wild-card expansion effects as per 'stap -l'.
   Guarded by STP_NEED_PROBE_NAME as setup in pn() tapset function.  */
#ifdef STP_NEED_PROBE_NAME
const char *probe_name;
#endif

/* The kind of probe this is.  One of the _STP_PROBE_HANDLER_ constants.
   Used to determined what other fields are setup and how.  Often the
   probe context fields depend on how the probe handler is triggered
   and what information it gets passed.  */
int probe_type;

/* Number of "actions" this probe handler is still allowed to do.
   Setup in common_probe_entryfn_prologue to either MAXACTION or
   Checked by code generated by c_unparser::record_actions (), which will
   set last_error in case this goes to zero and then jumps to out.
   MAXACTION_INTERRUPTIBLE.  Note special case in enter_all_profile_probes.  */
int actionremaining;

/* The current nesting of a function. Needed to determine which "level" of
   locals to use. See the recursion_info traversing_visitor for how the
   maximum is calculated.  Locals of a function are stored at
   c->locals[c->nesting], see c_unparser::emit_function ().  */
int nesting;

/* A place to format error messages into if some error occurs, last_error
   will then be pointed here.  */
string_t error_buffer;

/* Only used when stap script uses tokenize.stp tapset.  */
#ifdef STAP_NEED_CONTEXT_TOKENIZE
string_t tok_str;
char *tok_start;
char *tok_end;
#endif

/* NB: last_error is used as a health flag within a probe.
   While it's 0, execution continues
   When it's "something", probe code unwinds, _stp_error's, sets error state */
const char *last_error;
/* Last statement (token) executed. Often set together with last_error. */
const char *last_stmt;

/* status of pt_regs regs field.  _STP_REGS_ flags.  */
int regflags;
/* Set when probe handler gets pt_regs handed to it. This can be either
   the kernel registers or the user space registers.  The regflags field
   will indicate which and whether the user registers are complete.  */
struct pt_regs *regs;

/* unwaddr is caching unwound address in each probe handler on ia64. */
#if defined __ia64__
unsigned long *unwaddr;
#endif

/* Individual Probe State (ips).
   A union since only one can be active at a time.  */
union {

  /* kretprobe state. */
  struct {
    struct kretprobe_instance *pi;
    /* int64_t count in pi->data, the rest is string_t.
       See the kretprobe.stp tapset.  */
    int pi_longs;
  } krp;

  /* State for mark_derived_probes.  */
  struct {
    va_list *mark_va_list;
    const char *marker_name;
    const char *marker_format;
  } kmark;

  /* State for tracepoint probes. */
  const char *tracepoint_name;

  /* uretprobe state */
  struct uretprobe_instance *ri;

  /* State for procfs probes, see tapset-procfs.cxx.  */
  void *procfs_data;
} ips;


/* Only used when stap script uses the i386 or x86_64 register.stp tapset. */
#ifdef STAP_NEED_REGPARM
int regparm;
#endif

/* Only used for overload processing. */
#ifdef STP_OVERLOAD
cycles_t cycles_base;
cycles_t cycles_sum;
#endif

/* Current state of the unwinder (as used in the unwind.c dwarf unwinder). */
#if defined(STP_NEED_UNWIND_DATA)
struct unwind_context uwcontext;
#endif
