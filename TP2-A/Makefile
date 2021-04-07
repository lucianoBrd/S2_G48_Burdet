CC ?= gcc

VOBJS ?= vehicule_checker_student.o utils.o
DOBJS ?= dashboard.o utils.o

.SUFFIXES: .c .o

VEHICULE = vehicule_checker_student
DASHBOARD = dashboard

all: $(VEHICULE) $(DASHBOARD)

$(VEHICULE): $(VOBJS)
	$(CC) -o $(VEHICULE) $(VOBJS)

$(DASHBOARD): $(DOBJS)
	$(CC) -o $(DASHBOARD) $(DOBJS)

.c.o:
	$(CC) -c $*.c
	
clean :
	-rm -f *.o $(VEHICULE) $(DASHBOARD)
	