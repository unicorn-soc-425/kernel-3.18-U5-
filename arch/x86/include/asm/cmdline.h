#ifndef _ASM_X86_CMDLINE_H
#define _ASM_X86_CMDLINE_H

int cmdline_find_option_bool(const char *cmdline_ptr, const char *option);
<<<<<<< HEAD
=======
int cmdline_find_option(const char *cmdline_ptr, const char *option,
			char *buffer, int bufsize);
>>>>>>> v4.9.227

#endif /* _ASM_X86_CMDLINE_H */
