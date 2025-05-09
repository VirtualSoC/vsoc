/*
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#ifndef QEMU_MAIN_H
#define QEMU_MAIN_H

#ifdef __APPLE__
extern char **qemu_argv;
extern int qemu_argc;
#endif

int qemu_main(int argc, char **argv, char **envp);

#endif /* QEMU_MAIN_H */
