# Init
AC_INIT([pmurs], [0.0.1], [i.ucar86@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
CFLAGS+="-pedantic -Wall -std=gnu99"

# Checks for programs
AC_PROG_CC

# Checks for header files
AC_CHECK_HEADERS([stdlib.h],, [AC_MSG_ERROR([<stdlib.h> required])])
AC_CHECK_HEADERS([unistd.h],, [AC_MSG_ERROR([<unistd.h> required])])
AC_CHECK_HEADERS([string.h],, [AC_MSG_ERROR([<string.h> required])])
AC_CHECK_HEADERS([arpa/inet.h],, [AC_MSG_ERROR([<arpa/inet.h> required])])
AC_CHECK_HEADERS([netdb.h],, [AC_MSG_ERROR([<netdb.h> required])])
AC_CHECK_HEADERS([netinet/in.h],, [AC_MSG_ERROR([<netinet/in.h> required])])
AC_CHECK_HEADERS([sys/socket.h],, [AC_MSG_ERROR([<sys/socket.h> required])])


# Checks for library functions
AC_CHECK_FUNCS([dup2],, [AC_MSG_ERROR([dup2 required])])
AC_CHECK_FUNCS([memset],, [AC_MSG_ERROR([memset required])])
AC_CHECK_FUNCS([socket],, [AC_MSG_ERROR([socket required])])
AC_FUNC_FORK

# Defines
AC_ARG_VAR([TARGET_SHELL], [shell in the target machine (default: /bin/sh)])
test -z $TARGET_SHELL && TARGET_SHELL=/bin/sh
AC_DEFINE_UNQUOTED([TARGET_SHELL], ["$TARGET_SHELL"], [shell in the target machine])
AC_ARG_VAR([HEARTBEAT], [heartbeat packet payload (default: no payload)])
AC_DEFINE_UNQUOTED([HEARTBEAT], ["$HEARTBEAT"], [heartbeat packet payload])

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT
