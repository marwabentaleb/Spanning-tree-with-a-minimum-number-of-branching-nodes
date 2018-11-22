SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio125/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio125/concert
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ 


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD 

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEXINCDIR   = $(CPLEXDIR)/include
CONCERTINCDIR = $(CONCERTDIR)/include
CCFLAGS = $(CCOPT) -I$(CONCERTINCDIR) -I$(CPLEXINCDIR) 
CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -pthread


# ------------------------------------------------------------
EXEC=model_poly

all:$(EXEC)

$(EXEC): $(EXEC).o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o $(EXEC) $(EXEC).o $(CCLNFLAGS)

$(EXEC).o: $(EXEC).cpp
	$(CCC) -c $(CCFLAGS) $(EXEC).cpp -o $(EXEC).o

clean:
	rm -rf *.o

EXEC1=model_exp

all:$(EXEC1)

$(EXEC1): $(EXEC1).o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o $(EXEC1) $(EXEC1).o $(CCLNFLAGS)

$(EXEC1).o: $(EXEC1).cpp
	$(CCC) -c $(CCFLAGS) $(EXEC1).cpp -o $(EXEC1).o

EXEC2=heuristic
all:$(EXEC2)

$(EXEC2): $(EXEC2).o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o $(EXEC2) $(EXEC2).o $(CCLNFLAGS)

$(EXEC2).o: $(EXEC2).cpp
	$(CCC) -c $(CCFLAGS) $(EXEC2).cpp -o $(EXEC2).o




