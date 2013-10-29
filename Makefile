#Check for colorgcc installation, use it if found.
ifeq ($(wildcard /usr/bin/colorgcc),) 
CC = gcc
else
CC = colorgcc
endif
SQLLIBS := $(shell mysql_config --libs)
SQLFLAG := $(shell mysql_config --cflags)

#The standard config needed to compile basic server, withought these it won't work.
FLAGS = $(SQLFLAG) --fast-math -Wall -fno-strict-aliasing -lpng -O3 
LIBS = $(SQLLIBS) -lcrypt -lcrypto -lssl

#Purely optional, you can remove this. It adds extra debugging headers for gdb usage.
DEFS = -ggdb

# Right then, now we know all of that... lets build something!
server: sv.o io.o db.o cmd.o html.o map.o md5.o
	$(CC) sv.o io.o db.o cmd.o html.o map.o md5.o $(DEFS) -o evserver $(FLAGS) $(LIBS)

sv.o: *.h sv.c svban.c
	$(CC) sv.c $(DEFS) -o sv.o -c $(FLAGS)

io.o: *.h io.c iohttpvars.c iohttp.c iohttpmime.c ioevm.c
	$(CC) io.c $(DEFS) -o io.o -c $(FLAGS)

db.o: *.h db.c
	$(CC) db.c $(DEFS) -o db.o -c $(FLAGS)

cmd.o: *.h cmd.c cmdexec.c cmdtick.c battle.c specop.c
	$(CC) cmd.c $(DEFS) -o cmd.o -c $(FLAGS)

map.o: *.h map.c
	$(CC) map.c $(DEFS) -o map.o -c $(FLAGS)

html.o: *.h html.c html_main.c html_admin.c html_gameplay.c html_user.c html_forum.c html_status.c
	$(CC) html.c $(DEFS) -o html.o -c $(FLAGS)

md5.o: *.h md5.c
	$(CC) md5.c $(DEFS) -o md5.o -c $(FLAGS)

#I hate to point out the ovbious, but these are just used for cleaning things up a bit.
clean:
	rm *~ -rf
	rm *.o -rf
	rm *.raw -rf
	rm optional/*.o -rf
	rm optional/*~ -rf

#Dangerous, will wipe everything... whole database the works. Nothing is spared.
#But I do a lot of clean testing, so I like to have this one handy. It doesn't build anything.
#Can also be used in unison such as "make blank server", same for above "make clean server" works.
blank: clean
	rm core -rf
	rm /tmp/evcore -rf

#Not yet in deployment. This is just my testing section.
mysql.o: optional/mysql.c optional/mysql.h *.h
	$(CC) optional/mysql.c $(DEFS) -o mysql.o -c $(FLAGS) $(LIBS)

mysqltest: mysql.o md5.o
	$(CC) mysql.o md5.o $(DEFS) -o mysqltest $(FLAGS) $(LIBS)

