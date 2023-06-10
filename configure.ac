AC_PREREQ([2.69])
AC_INIT([memory-capper], [1.0])

AC_CONFIG_SRCDIR([main.c])

# See https://www.gnu.org/software/automake/manual/html_node/Requirements.html
AC_CONFIG_FILES([Makefile
                 memory-capper.service])

# Store the auxiliary build tools (e.g., install-sh, config.sub, config.guess)
# in this dir (build-aux)
AC_CONFIG_AUX_DIR([build-aux])

# Init automake, and specify this program use relaxed structures.
# i.e. this program doesn't follow the gnu coding standards, and doesn't have
# ChangeLog, COPYING, AUTHORS, INSTALL, README etc. files.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C compiler
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
# We can add more checks in this section

m4_include([m4/features.m4])
m4_include([m4/systemd.m4])

AX_AVAILABLE_SYSTEMD()
AM_CONDITIONAL(CONFIG_SYSTEMD_INIT, [test x"$systemdisinit" = xy])
AM_CONDITIONAL(CONFIG_SYSTEMD_DEV, [test x"$systemddev" = xy])

# Generate the output
AC_OUTPUT
