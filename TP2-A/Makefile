CC ?= gcc

VOBJS ?= vehicule_checker_student.o utils.o
DOBJS ?= dashboard.o utils.o
ROBJS ?= road_follower.o utils.o

.SUFFIXES: .c .o

VEHICULE = vehicule_checker_student
DASHBOARD = dashboard
FOLLOWER = road_follower

all: $(VEHICULE) $(DASHBOARD) $(FOLLOWER)

$(VEHICULE): $(VOBJS)
	$(CC) -o $(VEHICULE) $(VOBJS)

$(DASHBOARD): $(DOBJS)
	$(CC) -o $(DASHBOARD) $(DOBJS)

$(FOLLOWER): $(ROBJS)
	$(CC) -o $(FOLLOWER) $(ROBJS)

.c.o:
	$(CC) -c $*.c
	
clean :
	-rm -f *.o $(VEHICULE) $(DASHBOARD) $(FOLLOWER)
	