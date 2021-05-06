CC ?= gcc

VOBJS ?= vehicule_checker_student.o utils.o
DOBJS ?= dashboard.o utils.o
ROBJS ?= road_follower.o utils.o
SOBJS ?= studentOBD2.o utils.o
UOBJS ?= UserOBD2Terminal.o utils.o
SCOBJS ?= sensorsCAN.o utils.o

.SUFFIXES: .c .o

VEHICULE = vehicule_checker_student
DASHBOARD = dashboard
FOLLOWER = road_follower
STUDENTOBD = studentOBD2
USEROBD = UserOBD2Terminal
SENSORSCAN = sensorsCAN

all: $(VEHICULE) $(DASHBOARD) $(FOLLOWER) $(STUDENTOBD) $(USEROBD) $(SENSORSCAN)

$(VEHICULE): $(VOBJS)
	$(CC) -o $(VEHICULE) $(VOBJS)

$(DASHBOARD): $(DOBJS)
	$(CC) -o $(DASHBOARD) $(DOBJS)

$(FOLLOWER): $(ROBJS)
	$(CC) -o $(FOLLOWER) $(ROBJS)

$(STUDENTOBD): $(SOBJS)
	$(CC) -o $(STUDENTOBD) $(SOBJS)

$(USEROBD): $(UOBJS)
	$(CC) -o $(USEROBD) $(UOBJS)

$(SENSORSCAN): $(SCOBJS)
	$(CC) -o $(SENSORSCAN) $(SCOBJS)

.c.o:
	$(CC) -c $*.c
	
clean :
	-rm -f *.o $(VEHICULE) $(DASHBOARD) $(FOLLOWER) $(STUDENTOBD) $(USEROBD) $(SENSORSCAN)
	