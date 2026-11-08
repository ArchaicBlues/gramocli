PROG = gramocli


SRCS = checkfile.c 	\
        gramofile.c \
        signpr_main.c \
        signpr_general.c signpr_median.c signpr_wav.c	\
	secshms.c signpr_cmf.c signpr_mean.c signpr_doubmed.c	\
	tracksplit.c \
		signpr_cmf2.c signpr_rms.c \
        signpr_exper.c endian.c signpr_l1fit.c


OBJS = $(SRCS:.c=.o)
SHELL = /bin/sh

CC = gcc

#Debug see https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
#sudo apt-get install build-essential gdb
#https://code.visualstudio.com/docs/cpp/config-linux
LDFLAGS = 


########## CHOOSE YOUR ARCHITECTURE:    (NOTE: also see bplaysrc/Makefile!)

# For Linux (and maybe others), use these, Codeoptimierung per -O1,-O2,-O3, debugg info mit -g compilieren
CFLAGS = -Wall -O2 -DTURBO_MEDIAN -DTURBO_BUFFER 
DEPS = $(OBJS)
LIBS = -lm
COPY_A = -a

$(PROG): $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(PROG) $(LIBS) 
	@echo ''
	@echo ''

#$(MAKE) is a recursive invocation of make itself, forwarding the -t, -n and -q options. 
#This makes sense: you want the nested make invocations to run with these options as well.
.PHONY: clean
clean:
	-rm -f *.o *.d
	
.PHONY: indent
indent:
	indent *.c *.h


#SHELL is the Makefile variable that sets the preferred shell to use. 
#The default on is /bin/sh, so if you need Bash features, you might have to set this variable.
%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
		      | sed '\''s/\($*\)\.o/& $@/g'\'' > $@'
#
# 'some.o: some.c other.h'   ==> 'some.o some.d: some.c other.h'  => OK

include $(SRCS:.c=.d)

