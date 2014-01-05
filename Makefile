#Check for colorgcc installation, use it if found.
ifeq ($(wildcard /usr/bin/colorgcc),) 
CC = gcc
else
CC = colorgcc
endif
CONFIGS := $(shell cat config/depreciated.h)
SQLLIBS := $(shell mysql_config --libs)
SQLFLAG := $(shell mysql_config --cflags)

LIBDIR = .libs/
MODLIBS = 
#The standard config needed to compile basic server, withought these it won't work.
FLAGS = -O2 -O3 --fast-math -Wall #-Wextra
LIBS = -lcrypt -lpng


ifneq ($(findstring HTTPS_SUPPORT 1,$(CONFIGS)),)
LIBS += -lgcrypt -lgnutls
endif

ifneq ($(findstring FACEBOOK_SUPPORT 1,$(CONFIGS)),)
LIBS += -lcurl
endif

ifneq ($(findstring DEBUG_SUPPORT 1,$(CONFIGS)),)

ifneq ($(findstring MYSQL_SUPPORT 1,$(CONFIGS)),)
DEFS = -rdynamic
else
DEFS = -g -rdynamic
endif

endif

ifneq ($(findstring IRCBOT_SUPPORT 1,$(CONFIGS)),)
MODLIBS += $(LIBDIR)ircbot.o
endif

ifneq ($(findstring MYSQL_SUPPORT 1,$(CONFIGS)),)
FLAGS += $(SQLFLAG)
LIBS += $(SQLLIBS)
MODLIBS += $(LIBDIR)mysql.o
else
LIBS += -lm -pthread
endif

HEAD = *.h config/*.h

# Right then, now we know all of that... lets build something!
server: $(LIBDIR)main.o $(LIBDIR)io.o $(LIBDIR)http.o $(LIBDIR)db.o $(LIBDIR)cmd.o $(LIBDIR)html.o $(LIBDIR)map.o $(LIBDIR)extras.o $(MODLIBS)
	$(CC) $(LIBDIR)main.o $(LIBDIR)io.o $(LIBDIR)http.o $(LIBDIR)db.o $(LIBDIR)cmd.o $(LIBDIR)html.o $(LIBDIR)map.o $(LIBDIR)extras.o $(MODLIBS) $(DEFS) -o evserver $(FLAGS) $(LIBS)

run:	server
	sudo service evserver start

stop:	
	sudo service evserver stop
	
restart: stop run
	
$(LIBDIR)main.o: $(HEAD) main.c
	$(CC) main.c $(DEFS) -o $(LIBDIR)main.o -c $(FLAGS)

$(LIBDIR)io.o: $(HEAD) io.c iohttpvars.c iohttp.c iohttpmime.c
	$(CC) io.c $(DEFS) -o $(LIBDIR)io.o -c $(FLAGS)

$(LIBDIR)http.o: $(HEAD) http/*.h http/*.c 
	$(CC) http/http.c $(DEFS) -o $(LIBDIR)http.o -c $(FLAGS)

$(LIBDIR)db.o: $(HEAD) db.c
	$(CC) db.c $(DEFS) -o $(LIBDIR)db.o -c $(FLAGS)

$(LIBDIR)cmd.o: $(HEAD) cmd.c cmdexec.c cmdtick.c battle.c specop.c artefact.c
	$(CC) cmd.c $(DEFS) -o $(LIBDIR)cmd.o -c $(FLAGS)

$(LIBDIR)map.o: $(HEAD) map.c
	$(CC) map.c $(DEFS) -o $(LIBDIR)map.o -c $(FLAGS)

$(LIBDIR)html.o: $(HEAD) html/*.h html/*.c
	$(CC) html/html.c $(DEFS) -o $(LIBDIR)html.o -c $(FLAGS)

$(LIBDIR)extras.o: $(HEAD) extras/*.h extras/*.c
	$(CC) extras/extras.c $(DEFS) -o $(LIBDIR)extras.o -c $(FLAGS)

$(LIBDIR)ircbot.o: $(HEAD) ircbot/*.h ircbot/*.c
	$(CC) ircbot/ircbot.c $(DEFS) -o $(LIBDIR)ircbot.o -c $(FLAGS)

#I hate to point out the ovbious, but these are just used for cleaning things up a bit.
clean:
	rm *~ -rf
	rm .libs/*.o -rf
	rm test_modules/*~ -rf

#Dangerous, will wipe everything... whole database the works. Nothing is spared.
#But I do a lot of clean testing, so I like to have this one handy. It doesn't build anything.
#Can also be used in unison such as "make blank server", same for above "make clean server" works.
blank: clean
	rm data_core -rf
	rm /tmp/evcore -rf

#Not yet in deployment. This is just my testing section.
$(LIBDIR)mysql.o: test_modules/mysql.c test_modules/*.h *.h
	$(CC) test_modules/mysql.c $(DEFS) -o $(LIBDIR)mysql.o -c $(FLAGS) $(LIBS)

mysqltest: mysql.o extras.o
	$(CC) $(LIBDIR)mysql.o $(LIBDIR)extras.o $(DEFS) -o mysqltest $(FLAGS) $(LIBS)

