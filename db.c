#ifndef GLOBALINCLUDED
#include "global.h"
#endif

enum 
{
DB_FILE_USERS,
DB_FILE_MAP,
DB_FILE_MARKET,
DB_FILE_TOTAL,
};

char dbFileUsersName[] = "%s/userdb";
char dbFileMapName[] = "map";
char dbFileMarketName[] = "market";

char *dbFileList[DB_FILE_TOTAL] = { dbFileUsersName, dbFileMapName, dbFileMarketName };
FILE *dbFilePtr[DB_FILE_TOTAL];


char *dbImageDirs[HTTP_DIR_TOTAL] = {
"%s",
"%s/avatars",
"%s/filetypes",
"%s/ostypes",
"%s/smilies",
};

enum 
{
DB_FILE_USER_INFO,
DB_FILE_USER_MAIN,
DB_FILE_USER_BUILD,
DB_FILE_USER_PLANETS,
DB_FILE_USER_FLEETS,
DB_FILE_USER_NEWS,
DB_FILE_USER_MARKET,
DB_FILE_USER_MAILIN,
DB_FILE_USER_MAILOUT,
DB_FILE_USER_SPECOPS,
DB_FILE_USER_RECORD,
DB_FILE_USER_FLAGS,

DB_FILE_USER_TOTAL,
};



char dbFileUserInfoName[] = "%s/user%d/info";
char dbFileUserMailInName[] = "%s/user%d/mailin";
char dbFileUserMailOutName[] = "%s/user%d/mailout";
char dbFileUserRecordName[] = "%s/user%d/record";

char dbFileUserMainName[] = "%s/user%d/main";
char dbFileUserBuildName[] = "%s/user%d/build";
char dbFileUserPlanetsName[] = "%s/user%d/planets";
char dbFileUserFleetsName[] = "%s/user%d/fleets";
char dbFileUserNewsName[] = "%s/user%d/news";
char dbFileUserMarketName[] = "%s/user%d/market";
char dbFileUserSpecOpsName[] = "%s/user%d/specops";
char dbFileUserGameFlags[] = "%s/user%d/flags";

char *dbFileUserList[DB_FILE_USER_TOTAL] = { dbFileUserInfoName, dbFileUserMainName, dbFileUserBuildName, dbFileUserPlanetsName, dbFileUserFleetsName, dbFileUserNewsName, dbFileUserMarketName, dbFileUserMailInName, dbFileUserMailOutName, dbFileUserSpecOpsName, dbFileUserRecordName, dbFileUserGameFlags };

long long int dbFileUserListDat0[] = { 0, -1, -1, 0, 0 };
int dbFileUserListDat1[] = { 0, 8 };

int dbFileUserListBase[DB_FILE_USER_TOTAL] = { 0, 0, 4, 4, 4, 40, 8, 8, 8, 4, 4, 0 };
long long int *dbFileUserListData[DB_FILE_USER_TOTAL] = { 0, 0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat1, dbFileUserListDat1, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0 };


int dbMapBInfoStatic[MAP_TOTAL_INFO];



int dbArtefactPos[ARTEFACT_NUMUSED];

int dbArtefactMax;



/*
Database format

users
  4:next user ID
  4:number of free IDs
4*X:list of free IDs

map
  4:size X
  4:size Y
  4:number of systems
  4:number of planets
  4:number of families
  4:number of players per empire
  4:reserved
 32:reserved
struct ( 20 )
  4:position ( y << 16 ) + x
  4:index first planet
  4:number of planets
  4:empire home system, -1:none
  4:reserved
struct ( 184 )
  4:system indice
  4:position, ( y << 20 ) + ( x << 8 ) + planet
  4:planet owner ID, none if < 0
  4:size
  4:flags
  4:population
  4:maxpopulation
3*4:special
 64:number of buildings
 64:number of units
  4:construction
  4:protection
  4:surrender
  4:reserved
struct ( 336 )
  4:number of players
128:players ID, 32 fixed maximum
  4:home system ID
  4:home system position ( y << 16 ) + x
 64:empire name
  4:leader ID
 32:vote index in players IDs
  4:picture mime
  4:picture time
  4:planets
  4:networth
  4:artefacts
  4:rank
 72:reserved

market
   4:num max bids
   4:list free
3 * 2 * 80 * 12 ; res*action*prices*struct
struct( 12 )
   4:total of bids
   4:first list ID
   4:last list ID
struct( 16 )
   4:previous id
   4:next id
   4:quantity
   4:owner ID


user%d/user
  4:user ID
  4:level
  4:flags
  4:reserved
 32:name
 32:password

user%d/main
 nn:dbUserMainDef

user%d/build
  4:number of builds
struct ( 20 )
  4:type, |0x10000 if units
  4:quantity
  4:time remaining
  4:planet ID
  4:planet position, ( y << 20 ) + ( x << 8 ) + planet
  4*8:Cost

user%d/planets
  4:number of planets
struct ( 20 )
  4:planet index
  4:system index
  4:location, ( y << 20 ) + ( x << 8 ) + planet
  4:flags, &1:portal
  4:reserved

user%d/fleets
  4:number of fleets
struct ( 96 )
 64:units
  4:order
  4:destination, ( y << 20 ) + ( x << 8 ) + planet
  4:destination planet ID
  4:destination system ID
  4:source, ( y << 20 ) + ( x << 8 )
  4:flags
  4:time
  8:reserved

user%d/news
  8:num
  8:list used
  8:list free
  8:news flags
  8:new num
struct ( 128 )
  8:previous news id
  8:next news id
  8:time
  8:flags
224:news data

user%d/market
  4:number of bids
  4:flags
struct ( 20 )
  4:action
  4:resource
  4:price
  4:quantity
  4:bid ID

user%d/specops
  4:number of specops
struct ( nn )
 nn:dbUserSpecOpDef


fam%d/news
  4:num
  4:list used
  4:list free
  4:news flags
  4:new num
struct ( 128 )
  4:previous news id
  4:next news id
  4:time
  4:flags
112:news data

And Mal forgot the mail file
....


*/


FILE *dbFileGenOpen( int num ) {
	char szSource[500];
	char COREDIR[256];
	
	if(num == DB_FILE_USERS) {
		sprintf( COREDIR, "%s/users", sysconfig.directory );
		sprintf(szSource, dbFileList[num], COREDIR);
	} else {
		sprintf( COREDIR, "%s/data", sysconfig.directory );
		sprintf(szSource, dbFileList[num], COREDIR);
	}
	if( dbFilePtr[num] )
		return dbFilePtr[num];
    
	if( !( dbFilePtr[num] = fopen( szSource, "rb+" ) ) ) {
		if( options.verbose ) {
			printf("DBGen: %02d, Can't open \"%s\"\n", errno, szSource );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "DBGen: %02d, Can't open \"%s\"\n", errno, szSource );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return 0;
	}

return dbFilePtr[num];
}

void dbFileGenClose( int num )
{
  if( dbFilePtr[num] )
  {
    fclose( dbFilePtr[num] );
    dbFilePtr[num] = 0;
  }
  return;
}

void dbFlush()
{
  int a;
  for( a = 0 ; a < DB_FILE_TOTAL ; a++ )
    dbFileGenClose( a );
  return;
}


FILE *dbFileUserOpen( int id, int num ) {
	char fname[1024];
	char COREDIR[256];
	FILE *file;

if((num&0xFFFF) == DB_FILE_USER_INFO) {
	sprintf( COREDIR, "%s/users", sysconfig.directory );  
	sprintf( fname, dbFileUserList[num&0xFFFF], COREDIR, id );
} else {
	sprintf( COREDIR, "%s/data", sysconfig.directory );  
  	sprintf( fname, dbFileUserList[num&0xFFFF], COREDIR, id );
}
  
if( !( file = fopen( fname, "rb+" ) ) ) {
		// mooooooo
	if( ( file = fopen( fname, "wb" ) ) ) {
		fwrite( dbFileUserListData[num&0xFFFF], 1, dbFileUserListBase[num&0xFFFF], file );
		fclose( file );
		return fopen( fname, "rb+" );
	}

	if( num < 0x10000 ) {
		if( options.verbose ) {
			printf("Error %02d, fopen %s\n", errno, fname );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Error %02d, fopen %s\n", errno, fname );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	}

	return 0;
}


return file;
}

FILE *dbFileFamOpen( int id, int num )
{
  int a, b;
  char fname[32];
  FILE *file;
  sprintf( fname, "fam%02dn%02d", id, num );
  if( !( file = fopen( fname, "rb+" ) ) )
  {
    if( ( file = fopen( fname, "wb+" ) ) )
    {
      if( num == 0 )
      {
        a = 0;
        fwrite( &a, 1, sizeof(int), file );
      }
      else
      {
        a = 0;
        for( b = 0 ; b < 4096*2/4 ; b++ )
          fwrite( &a, 1, sizeof(int), file );
      }
      fseek( file, 0, SEEK_SET );
      return file;
    }
	if( num < 0x10000 ) {
		if( options.verbose ) {
			printf("Error %02d, fopen %s\n", errno, fname );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Error %02d, fopen %s\n", errno, fname );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	}
    return 0;
  }
  return file;
}




dbUserPtr dbUserList;
dbUserPtr dbUserTable[16384];



dbUserPtr dbUserAllocate( int id )
{
  char pass[128];
  dbUserPtr user;
if( !( user = malloc( sizeof(dbUserDef) ) ) ) {
	if( options.verbose ) {
		printf("Error, malloc dbuser failed\n" );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error, malloc dbuser failed\n" );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
    return 0;
  }
  memset( user, 0, sizeof(dbUserDef) );
  user->prev = (void **)&(dbUserList);
  user->next = dbUserList;
  if( dbUserList )
    dbUserList->prev = &(user->next);
  dbUserList = user;
  user->id = id;
  user->lasttime = 0;
  dbUserRetrievePassword( id, pass );
  srand( time(0) + pass[1] + 3*pass[3] + 5*pass[5] + 7*pass[7] );
  user->session[0] = rand();
  user->session[1] = rand();
  user->session[2] = rand();
  user->session[3] = rand();
  dbUserTable[id] = user;
  return user;
}

void dbUserFree( dbUserPtr user )
{
  dbUserPtr next;
  next = user->next;
  *(user->prev) = (void *)next;
  if( next )
    next->prev = user->prev;
  free( user );
  if( (unsigned int)user->id >= 16384 )
    return;
  dbUserTable[user->id] = 0;
  return;
}


/*
users
  4:next user ID
  4:number of free IDs
4*X:list of free IDs
*/
int dbInitUsersReset()
{
  int a, last, freenum;
  //dbUserMainDef maind;
  FILE *file;

  if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
    return -3;
  last = 0;
  for( a = 0 ; a < 16384 ; a++ )
  {
    if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_MAIN ) ) )
      continue;
    last = a;
    fclose( file );
  }

  fseek( dbFilePtr[DB_FILE_USERS], 8, SEEK_SET );
  freenum = 0;
  for( a = 0 ; a <= last ; a++ )
  {
    if( ( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_MAIN ) ) )
    {
      fclose( file );
      continue;
    }
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
    freenum++;
  }

  fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
  last++;
  fwrite( &last, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
  fwrite( &freenum, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  return 1;
}



int dbInit() {
	int a, b, c;
	int array[4];
	dbUserPtr user;
	dbUserInfoDef infod;
	dbMainPlanetDef planetd;
	dbForumForumDef forumd;
	FILE *file;
	char szUsersFile[500];
	char COREDIR[256];
	
sprintf( COREDIR, "%s/data", sysconfig.directory );  
if( chdir( COREDIR ) == -1 ) {
	if( options.verbose ) {
		printf("Error %02d, db chdir, Dir: %s\n", errno, COREDIR );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, db chdir, Dir: %s\n", errno, COREDIR );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return 0;
}

if( ( dbMapRetrieveMain( dbMapBInfoStatic ) < 0 ) )
	return 0;

if( !( dbFileGenOpen( DB_FILE_MARKET ) ) ) {
	if( options.verbose )
	printf("Market database not found, creating...\n" );
	syslog(LOG_INFO, "Market database not found, creating...\n" );

	if( !( dbFilePtr[DB_FILE_MARKET] = fopen( dbFileList[DB_FILE_MARKET], "wb+" ) ) ) {
		if( options.verbose ) {
			printf("Error, could not create market database!\n" );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Error, could not create market database!\n" );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return 0;
	}

	fseek( dbFilePtr[DB_FILE_MARKET], 0, SEEK_SET );
	array[0] = 0;
	array[1] = -1;
	fwrite( array, 1, 2*sizeof(int), dbFilePtr[DB_FILE_MARKET] );

	array[0] = 0;
	array[1] = -1;
	array[2] = -1;
	for( a = 0 ; a < 6*DB_MARKET_RANGE ; a++ )
		fwrite( array, 1, 3*sizeof(int), dbFilePtr[DB_FILE_MARKET] );

	dbFileGenClose( DB_FILE_MARKET );
}


if( !( file = fopen( "forums", "rb+" ) ) ) {
	if( options.verbose )
	printf("Forum database not found, creating...\n" );
	syslog(LOG_INFO, "Forum database not found, creating...\n" );
	if( !( file = fopen( "forums", "wb+" ) ) ) {
		if( options.verbose ) {
			printf("Error, could not create forum database!\n" );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Error, could not create forum database!\n" );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return 0;
	}
	a = 0;
	fwrite( &a, 1, sizeof(int), file );
	forumd.threads = 0;
	forumd.time = 0;
	forumd.tick = 0;
	forumd.flags = 0;
	for( a = 0 ; a < dbMapBInfoStatic[MAP_EMPIRES] ; a++ ) {
		sprintf( forumd.title, "Empire %d forum", a );
		forumd.rperms = 2;
		forumd.wperms = 2;
		forumd.flags = DB_FORUM_FLAGS_FORUMFAMILY;
		dbForumAddForum( &forumd, 1, 100+a );
	}
	if( options.verbose )
	printf("Created Forums for %d Empires.\n", a-1 );
	syslog(LOG_INFO, "Created Forums for %d Empires.\n", a-1 );

}
fclose( file );


if( !( dbFileGenOpen( DB_FILE_USERS ) ) ) {
	if( options.verbose )
	printf("User database not found, creating...\n" );
	syslog(LOG_INFO, "User database not found, creating...\n" );

    // Create a path to the users file in the same way as dbFileGenOpen
	sprintf( COREDIR, "%s/users", sysconfig.directory );
	sprintf( szUsersFile, dbFileList[DB_FILE_USERS], COREDIR );
	if( !( dbFilePtr[DB_FILE_USERS] = fopen( szUsersFile, "wb+" ) ) ) {
		if( options.verbose ) {
			printf("Error, could not create user database!\n" );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Error, could not create user database!\n" );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return 0;
	}
	fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
	a = 0;
	fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
	fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
	dbFileGenClose( DB_FILE_USERS );
	return 1;
} else {
	dbInitUsersReset();
}


fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
if( fread( &b, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x01: Userbase\n" );
	syslog(LOG_ERR, "Failure reading file x01: Userbase\n"  );
}

for( a = 0 ; a < b ; a++ ) {
	if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_INFO ) ) )
		continue;
	if( fread( &c, 1, sizeof(int), file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.0\n" );
		syslog(LOG_ERR, "Failure reading file x02.0\n"  );
	}

	if( !( user = dbUserAllocate( a ) ) ) {
		fclose( file );
		continue;
	}
    
	if( fread( &user->level, 1, sizeof(int), file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.1\n" );
		syslog(LOG_ERR, "Failure reading file x02.1\n"  );
	}
	if( fread( &user->flags, 1, sizeof(int), file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.2\n" );
		syslog(LOG_ERR, "Failure reading file x02.2\n" );
	}
	if( fread( &user->reserved, 1, sizeof(int), file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.3\n" );
		syslog(LOG_ERR, "Failure reading file x02.3\n" );
	}
	if( fread( &user->name, 1, 64, file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.4\n" );
		syslog(LOG_ERR, "Failure reading file x02.4\n" );
	}
	fclose( file );
    
	if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_FLAGS ) ) )
		continue;
    
	fseek( file, sizeof(int)*2, SEEK_SET );
	if( fread( &user->flags, 1, sizeof(int), file ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x02.5\n" );
		syslog(LOG_ERR, "Failure reading file x02.5\n" );
	}
	fclose(file);
	dbUserInfoRetrieve( a, &infod );
//printf("%d\n",infod.id);
//printf("%d\n",infod.rank);
	sprintf( user->faction, "%s", infod.faction );
	sprintf( user->forumtag, "%s", infod.forumtag );
	user->lasttime = infod.lasttime;
}

dbFlush();

for( a = 0 ; a < dbMapBInfoStatic[MAP_PLANETS] ; a++ ) {
	dbMapRetrievePlanet( a, &planetd );
	if( ( b = (int)artefactPrecense( &planetd ) ) < 0 )
		continue;
	dbArtefactPos[b] = planetd.position;
}
	

return 1;
}


void dbEnd() {
	dbUserPtr user, next;

for( user = dbUserList ; user ; user = next ) {
	next = user->next;
	dbUserFree( user );
}

return;
}




// Quick map search

int dbMapFindSystem( int x, int y ) {
	dbMainSystemDef systemd;
	int a, position = ( y << 16 ) + x;

for( a = 0 ; a < dbMapBInfoStatic[MAP_SYSTEMS] ; a++ ) {
	dbMapRetrieveSystem( a, &systemd );
	if( systemd.position == position )
		return a;
}

return -1;
}

int dbMapFindValid( int x, int y ) {
	int binfo[MAP_TOTAL_INFO];
	
dbMapRetrieveMain( binfo );
if( (unsigned int)x >= binfo[MAP_SIZEX] )
	return 0;
if( (unsigned int)y >= binfo[MAP_SIZEY] )
	return 0;

return 1;
}




// Users functions
int dbUserSearch( char *name ) {
	dbUserPtr user;

for( user = dbUserList ; user ; user = user->next ) {
	if( !( ioCompareExact( name, user->name ) ) )
		continue;

	return user->id;
}

return -1;
}

int dbUserSearchFaction( char *name ) {
	dbUserPtr user;

for( user = dbUserList ; user ; user = user->next ) {
	if( !( ioCompareExact( name, user->faction ) ) )
		continue;

	return user->id;
}

return -1;
}

dbUserPtr dbUserLinkID( int id ) {

if( (unsigned int)id >= 16384 )
	return 0;

return dbUserTable[id];
}


int dbUserAdd( char *name, char *faction, char *forumtag ) {
	int a, id, freenum;
	int *user_hashes;
	int num_users=0;
	int *user_ptr;
	char dname[532], fname[532], uname[532];
	char COREDIR[512];
	dbUserDescDef descd;
	dbUserPtr h_user;
	dbUserPtr user;
	FILE *file;


if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
	return -3;

fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
if( fread( &freenum, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x03.0\n" );
	syslog(LOG_ERR, "Failure reading file x03.0\n" );
}


if( !( freenum ) ) {
	fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
	if( fread( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x03.1\n" );
		syslog(LOG_ERR, "Failure reading file x03.1\n" );
	}
} else {
	a = freenum - 1;
	fseek( dbFilePtr[DB_FILE_USERS], 8 + ( a << 2 ), SEEK_SET );
	if( fread( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] ) < 1 ) {
	 	if( options.verbose )
			printf("Failure reading file x03.2\n" );
		syslog(LOG_ERR, "Failure reading file x03.2\n" );
	}
}
	
user = dbUserAllocate( id );
  
//create both folder for player
sprintf( dname, "%s/data/user%d", sysconfig.directory, id );
sprintf( uname, "%s/users/user%d", sysconfig.directory, id );
  
mkdir( dname, S_IRWXU );
mkdir( uname, S_IRWXU );
  
//Create a db Database in the db other server
for( a = DB_FILE_USER_TOTAL-2 ;  ; a-- ) {
	sprintf( COREDIR, "%s/data", sysconfig.directory );
  	sprintf( fname, dbFileUserList[a], COREDIR, id );
    
	if( !( file = fopen( fname, "wb+" ) )) {
		dbUserFree( user );
		rmdir( dname );
		rmdir( uname );
		if( options.verbose ) {
			printf("Data: %02d, fopen dbuseradd\n", errno );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "Data: %02d, fopen dbuseradd\n", errno );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return -3;
	}
	if( a == 0 )
		break;
	if( dbFileUserListBase[a] )
		fwrite( dbFileUserListData[a], 1, dbFileUserListBase[a], file );
	fclose( file );
}

fwrite( &id, 1, sizeof(int), file );
a = 0;
fwrite( &a, 1, sizeof(int), file );
fwrite( &a, 1, sizeof(int), file );
fwrite( &a, 1, sizeof(int), file );
memset( fname, 0, 64 );
sprintf( fname, "%s", name );
fwrite( fname, 1, 64, file );
fclose( file );
 
  
//Create a user Database in the db 10Min server
for( a = DB_FILE_USER_TOTAL-2 ;  ; a-- ) {
	sprintf( COREDIR, "%s/users", sysconfig.directory );
  	sprintf( fname, dbFileUserList[a], COREDIR, id );

	if( !( file = fopen( fname, "wb+" ) )) {
		dbUserFree( user );
		rmdir( dname );
		rmdir( uname );
		if( options.verbose ) {
			printf("User: %02d, fopen dbuseradd\n", errno );
			printf("Error description is : %s\n",strerror(errno) );
		}
		syslog(LOG_ERR, "User: %02d, fopen dbuseradd\n", errno );
		syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
		return -3;
	}
	if( a == 0 )
		break;
	if( dbFileUserListBase[a] ) {
//		printf("write base of %s\n", fname);
		fwrite( dbFileUserListData[a], 1, dbFileUserListBase[a], file );
	}
	fclose( file );
}
	
fwrite( &id, 1, sizeof(int), file );
a = 0;
fwrite( &a, 1, sizeof(int), file );
fwrite( &a, 1, sizeof(int), file );
fwrite( &a, 1, sizeof(int), file );
memset( fname, 0, 64 );
sprintf( fname, "%s", name );
fwrite( fname, 1, 64, file );
fclose( file );
	
if( !( freenum ) ) {
	fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
	a = id + 1;
	fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
} else {
	a = freenum - 1;
	fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
	fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
}

descd.desc[0] = 0;
dbUserDescSet( id, &descd );

//preserve user hashes so they are not logged out
for(h_user=dbUserList;h_user;h_user=h_user->next) {
	num_users++;
}

user_hashes=(int*)malloc(num_users*5*sizeof(int));
user_ptr=user_hashes;

for(h_user=dbUserList;h_user;h_user=h_user->next) {
	user_ptr[0]=h_user->id;
	user_ptr[1]=h_user->session[0];
	user_ptr[2]=h_user->session[1];
	user_ptr[3]=h_user->session[2];
	user_ptr[4]=h_user->session[3];
	user_ptr+=5;
}

dbEnd();
dbInit();

//restore the hashes
user_ptr=user_hashes;
for(h_user=dbUserList;h_user;h_user=h_user->next) {
	if(user_ptr[0]!=h_user->id) {
		if( options.verbose )
		printf("WARNING: can't restore user hashes, id mismatch (user %d, stored %d)\n",h_user->id,user_ptr[0] );
		syslog(LOG_INFO, "WARNING: can't restore user hashes, id mismatch (user %d, stored %d)\n",h_user->id,user_ptr[0] );
		continue;
	}
	h_user->session[0]=user_ptr[1];
	h_user->session[1]=user_ptr[2];
	h_user->session[2]=user_ptr[3];
	h_user->session[3]=user_ptr[4];
	user_ptr+=5;
}
free(user_hashes);


return id;
}


int dbUserRemove( int id ) {
	int a;
	char COREDIR[512];
	char dname[532], fname[532];
	dbUserPtr user;


if( !( user = dbUserLinkID( id ) ) )
	return 0;
if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
	return 0;

dbUserFree( user );

fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
if( fread( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x04\n" );
	syslog(LOG_ERR, "Failure reading file x04\n" );
}
a++;
fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
fseek( dbFilePtr[DB_FILE_USERS], ( a + 1 ) << 2, SEEK_SET );
fwrite( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

for( a = 0 ; a < DB_FILE_USER_TOTAL-1 ; a++ ) {
	sprintf( COREDIR, "%s/users", sysconfig.directory );
	sprintf( fname, dbFileUserList[a], COREDIR, id );
	unlink( fname );
}

for( a = 0 ; a < DB_FILE_USER_TOTAL-1 ; a++ ) {
	sprintf( COREDIR, "%s/data", sysconfig.directory );
	sprintf( fname, dbFileUserList[a], COREDIR, id );
	unlink( fname );
}

sprintf( dname, "%s/users/user%d", sysconfig.directory, id );
rmdir( dname );
sprintf( dname, "%s/data/user%d", sysconfig.directory, id );
rmdir( dname );


return 1;
}


int dbUserSave( int id, dbUserPtr user ) {
	FILE *file;
  
if( !( file = dbFileUserOpen( id, DB_FILE_USER_INFO ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbsetname\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbsetname\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fwrite( &user->id, 1, sizeof(int), file );
fwrite( &user->level, 1, sizeof(int), file );
fseek(file, sizeof(int), SEEK_CUR);
fwrite( &user->reserved, 1, sizeof(int), file );
fwrite( user->name, 1, 64, file );
fclose( file );

if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLAGS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbuserflags\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbuserflags\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fseek(file, 2*sizeof(int), SEEK_CUR);
fwrite( &user->flags, 1, sizeof(int), file );
fclose(file);

return 1;
}

int dbUserSetPassword( int id, char *pass ) {
	char fname[128];
	FILE *file;
  
if( !( file = dbFileUserOpen( id, DB_FILE_USER_INFO ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbsetpassword\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbsetpassword\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fseek( file, 16+65, SEEK_SET );
memset( fname, 0, 128 );
sprintf( fname, "%s", hashencrypt(pass) );
fwrite( fname, 1, 128, file );
fclose( file );

return 1;
}

int dbUserRetrievePassword( int id, char *pass ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_INFO ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbretrievepassword\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbretrievepassword\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fseek( file, 16+65, SEEK_SET );
if( fread( pass, 1, 128, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x05\n" );
	syslog(LOG_ERR, "Failure reading file x05\n" );
}

fclose( file );

return 1;
}

int dbUserLinkDatabase( void *cnt, int id ) {
	dbUserPtr user;
	svConnectionPtr cnt2 = cnt;

if( id < 0 ) {
	cnt2->dbuser = 0;
	return 1;
}

if( !( user = dbUserLinkID( id ) ) )
	return -2;

cnt2->dbuser = user;
user->lasttime = time( 0 );

return 1;
}




// session functions

int dbSessionSet( dbUserPtr user, char *hash, int *session )
{
  int a, b;
  if( !( user ) )
    return -3;
  b = rand() & 0xFFFF;
  if( hash )
    srand( time(0) + b + 3*hash[0] + 7*hash[2] + 11*hash[4] + 13*hash[6] + 17*hash[8] );
  else
    srand( time(0) + b );
  for( a = 0 ; a < 4 ; a++ )
    user->session[a] = rand() & 0xFFFF;
  memcpy( session, user->session, 4*sizeof(int) );
  return 1;
}

int dbSessionRetrieve( dbUserPtr user, int *session ) {

if( !( user ) )
	return -3;

memcpy( session, user->session, 4*sizeof(int) );

return 1;
}




// user main data functions

int dbUserMainSet( int id, dbUserMainPtr maind ) {
	dbUserPtr user;
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAIN ) ) )
    return -3;

fwrite( maind, 1, sizeof(dbUserMainDef), file );
fclose( file );

if( !( user = dbUserLinkID( id ) ) )
	return -3;
sprintf( user->faction, "%s", maind->faction ); //FIXME: Move to infod
sprintf( user->forumtag, "%s", maind->forumtag ); //FIXME: Move to infod

return 1;
}

int dbUserMainRetrieve( int id, dbUserMainPtr maind ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAIN ) ) )
	return -3;

memset( maind, 0, sizeof(dbUserMainDef) );
if( fread( maind, 1, sizeof(dbUserMainDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x06\n" );
	syslog(LOG_ERR, "Failure reading file x06\n" );
}

fclose( file );

return 1;
};





// user build functions -- Need to change the calling of this function.

int dbUserBuildAdd( int id, int type, long long int *cost, int quantity, int time, int plnid, int plnloc ) {
	int pos, i;
	FILE *file;
	dbUserBuildDef buildp;

if( !( memset( &buildp, 0, sizeof(dbUserBuildDef) ) ) ) {
	return -3;
}
if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
	return -3;  

buildp.type = type;
buildp.quantity = quantity;
buildp.time = time;
buildp.plnid = plnid;
buildp.plnpos = plnloc;


for(i=0;buildp.cost[i];i++)
	buildp.cost[i] = cost[i];

if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x07\n" );
	syslog(LOG_ERR, "Failure reading file x07\n" );
}

fseek( file, 4+(pos*sizeof(dbUserBuildDef)), SEEK_SET );
fwrite( &buildp, 1, sizeof(dbUserBuildDef), file );
fseek( file, 0, SEEK_SET );
pos++;
fwrite( &pos, 1, sizeof(int), file );
fclose( file );

return pos;
}


int dbUserBuildRemove( int id, int bldid ) {
	int a, num, data[sizeof(dbUserBuildDef)];
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
	return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x08.0\n" );
	syslog(LOG_ERR, "Failure reading file x08.0\n" );
}

if( (unsigned int)bldid >= num ) {
	fclose( file );
	return -2;
}

if( bldid+1 < num ) {
	fseek( file, 4+(num*sizeof(dbUserBuildDef))-sizeof(dbUserBuildDef), SEEK_SET );
	if( fread( data, 1, sizeof(dbUserBuildDef), file ) < 1 ) {
 		if( options.verbose )
			printf("Failure reading file x08.1\n" );
		syslog(LOG_ERR, "Failure reading file x08.1\n" );
	}
	fseek( file, 4+(bldid*sizeof(dbUserBuildDef)), SEEK_SET );
	fwrite( data, 1, sizeof(dbUserBuildDef), file );
}

fseek( file, 0, SEEK_SET );
a = num-1;
fwrite( &a, 1, sizeof(int), file );
fclose( file );

return 1;
}


int dbUserBuildList( int id, dbUserBuildPtr *build ) {
	int a, num;
	dbUserBuildPtr buildp;
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
	return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x09.0\n" );
	syslog(LOG_ERR, "Failure reading file x09.0\n" );
}

if( !( buildp = malloc( num*sizeof(dbUserBuildDef) ) ) ) {
	fclose( file );
	return -1;
}

for( a = 0 ; a < num ; a++ ) {
	fseek( file, 4+(a*sizeof(dbUserBuildDef)), SEEK_SET );
	if( fread( &buildp[a], 1, sizeof(dbUserBuildDef), file ) < 1 ) {
 		if( options.verbose )
			printf("Failure reading file x09.1\n" );
		syslog(LOG_ERR, "Failure reading file x09.1\n" );
	}
}
fclose( file );
*build = buildp;

return num;
}


void sortlist ( int num, int *list1, int *list2 )
{
  int b, c, smaller;
  int *list3;
  
  if( !( list3 = malloc( (num)*sizeof(int) ) ) )
    return;
  
  for( b = 0 ; b < num ; b++ )
    {
      smaller = 0;
//      list3[0] = list1[0];
      for ( c = 0 ; c < num; c++ )
	{
	  if ( b != c )
	    {
	      if ( ( list2[b] > list2[c] ) || ( ( list2[b] == list2[c]) && ( list1[b] > list1[c] ) ) )
		smaller++;
	    }
	}
      list3[smaller] = list1[b];
    }
  
  for( b = 0 ; b < num ; b++ )
    list1[b] = list3[b];

//syslog(LOG_ERR, "Dit is void sortlist\n" );
  free (list3);
  return;

}

void sortlist2 ( int num, int *list1, int *list2, int *list3 )
{
  int b, c, smaller;
  int *list4;
  
  if( !( list4 = malloc( (num)*sizeof(int) ) ) )
    return;
  
  for( b = 0 ; b < num ; b++ )
    {
      smaller = 0;
      //list4[0] = list1[0];
      for ( c = 0 ; c < num; c++ )
	{
	  if ( b != c )
	    {
	      if ( ( list2[b] > list2[c] ) || ( ( list2[b] == list2[c]) && ( list3[b] > list3[c] ) ) || ( ( list2[b] == list2[c]) && ( list3[b] == list3[c] ) && ( list1[b] > list1[c] ) ) )
		smaller++;
	    }
	}
      list4[smaller] = list1[b];
    }
  
  for( b = 0 ; b < num ; b++ )
    list1[b] = list4[b];
  
//syslog(LOG_DEBUG, "dees is void sortlist2 \n" );
  free (list4);
  return;
}

int dbUserBuildListReduceTime( int id, dbUserBuildPtr *build ) {
	int a, num;
	FILE *file;
	dbUserBuildPtr buildp;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
	return -3;

if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x10.0\n" );
	syslog(LOG_ERR, "Failure reading file x10.0\n" );
}

if( !( buildp = malloc( num*sizeof(dbUserBuildDef) ) ) ) {
	fclose( file );
	return -1;
}

for( a = 0 ; a < num ; a++ ) {
	fseek( file, 4+(a*sizeof(dbUserBuildDef)), SEEK_SET );
	if( fread( &buildp[a], 1, sizeof(dbUserBuildDef), file ) < 1 ) {
 		if( options.verbose )
			printf("Failure reading file x10.1\n" );
		syslog(LOG_ERR, "Failure reading file x10.1\n" );
	}
	buildp[a].time--;
	fseek( file, 4+(a*sizeof(dbUserBuildDef)), SEEK_SET );
	fwrite( &buildp[a], 1, sizeof(dbUserBuildDef), file );
}
fclose( file );
*build = buildp;

return num;
}

int dbUserBuildEmpty( int id ) {
	int a;
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
	return -3;

a = 0;
fwrite( &a, 1, sizeof(int), file );
fclose( file );

return 1;
}






// user planets functions

int dbUserPlanetNumber( int id )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x11\n" );
	syslog(LOG_ERR, "Failure reading file x11\n" );
}
  fclose( file );
  return pos;
}

int dbUserPlanetAdd( int id, int plnid, int sysid, int plnloc, int flags )
{
  int a, pos;
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  dbMainPlanetDef Planetd;
  FILE *file;
  

	dbMapRetrievePlanet(plnid, &Planetd);
	if(artefactPrecense(&Planetd) == 6)
  {
  	dbMapRetrieveEmpire( id, &empired );
  	for( a = 0 ; a < empired.numplayers ; a++ )
    {
  		if( dbUserMainRetrieve( empired.player[a], &maind ) < 0 )
        continue;
      dbUserMainSet( empired.player[a], &maind );
    }
	}
	
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x12\n" );
	syslog(LOG_ERR, "Failure reading file x12\n" );
}
  fseek( file, 4+(pos*20), SEEK_SET );
  fwrite( &plnid, 1, sizeof(int), file );
  fwrite( &sysid, 1, sizeof(int), file );
  fwrite( &plnloc, 1, sizeof(int), file );
  fwrite( &flags, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return pos;
}

int dbUserPlanetRemove( int id, int plnid )
{
  int a, b, num, data[5];
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  dbMainPlanetDef Planetd;
  FILE *file;
  

	dbMapRetrievePlanet(plnid, &Planetd);
	if(artefactPrecense(&Planetd) == 6)
  {
  	dbMapRetrieveEmpire( id, &empired );
  	for( a = 0 ; a < empired.numplayers ; a++ )
    {
  		if( dbUserMainRetrieve( empired.player[a], &maind ) < 0 )
        continue;
      dbUserMainSet( empired.player[a], &maind );
    }
	}
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x13.0\n" );
	syslog(LOG_ERR, "Failure reading file x13.0\n" );
}
  if( num >= 2 )
  {
    fseek( file, 4+(num*20)-20, SEEK_SET );
    if( fread( data, 1, 20, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x13.1\n" );
	syslog(LOG_ERR, "Failure reading file x13.1\n" );
}
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    if( fread( &b, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x13.2\n" );
	syslog(LOG_ERR, "Failure reading file x13.2\n" );
}
    if( b != plnid )
      continue;
    if( ( num >= 2 ) && ( a+1 < num ) )
    {
      fseek( file, 4+(a*20), SEEK_SET );
      fwrite( data, 1, 20, file );
    }
    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}

int dbUserPlanetSetFlags( int id, int plnid, int flags )
{
  int num, pos;
  int *list;
  FILE *file;
  if( ( num = dbUserPlanetListIndices( id, &list ) ) < 0 )
    return -3;
  for( pos = 0 ; pos < num ; pos++ )
  {
    if( pos == num )
    {
      free( list );
      return -3;
    }
    if( list[pos] == plnid )
      break;
  }
  free( list );
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fseek( file, 4+(pos*20)+12, SEEK_SET );
  fwrite( &flags, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserPlanetListCoords( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x14.0\n" );
	syslog(LOG_ERR, "Failure reading file x14.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+8, SEEK_SET );
    if( fread( &listp[a], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x14.1\n" );
	syslog(LOG_ERR, "Failure reading file x14.1\n" );
}
  }
  fclose( file );
  *list = listp;
  return num;
}

int dbUserPlanetListIndices( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x15.0\n" );
	syslog(LOG_ERR, "Failure reading file x15.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    if( fread( &listp[a], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x15.1\n" );
	syslog(LOG_ERR, "Failure reading file x15.1\n" );
}
  }
  fclose( file );
  *list = listp;
  return num;
}

int dbUserPlanetListSystems( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x16.0\n" );
	syslog(LOG_ERR, "Failure reading file x16.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+4, SEEK_SET );
    if( fread( &listp[a], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x16.1\n" );
	syslog(LOG_ERR, "Failure reading file x16.1\n" );
}
  }
  fclose( file );
  *list = listp;
  return num;
}



int dbUserPlanetListFull ( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x17.0\n" );
	syslog(LOG_ERR, "Failure reading file x17.0\n" );
}
  if( !( listp = malloc( num*4*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    if( fread( &listp[a*4], 1, 4*sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x17.1\n" );
	syslog(LOG_ERR, "Failure reading file x17.1\n" );
}
  }
  fclose( file );
  *list = listp;
  return num;
}


int dbUserPlanetListIndicesSorted( int id, int **list, int sort )
{
  int a, num, b, d;
  int *listp, *list2p, *list3p;
  dbMainPlanetDef planetd;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x18.0\n" );
	syslog(LOG_ERR, "Failure reading file x18.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
    {
      fclose( file );
      return -1;
    }
  
  if( !( num ) )
    {
      free (listp);
      fclose( file );
      return -1;
    }
  for( a = 0 ; a < num ; a++ )
    {
      fseek( file, 4+(a*20), SEEK_SET );
      if( fread( &listp[a], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x18.1\n" );
	syslog(LOG_ERR, "Failure reading file x18.1\n" );
}
    }
  fclose( file );
  if( !( list2p = malloc( (num)*sizeof(int) ) ) )
    {
      free (listp);
      return -1;
    }
  if( !( list3p = malloc( (num)*sizeof(int) ) ) )
    {
      free (listp);
      free (list2p);
      return -1;
    }
  if (sort == 1)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.size;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 2)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= d + planetd.construction;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 3)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= (int)(100 * ( cmdGetBuildOvercost( planetd.size, d + planetd.construction ) -1 ) );
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 4)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.population;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 5)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.protection;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 6)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.special[0];
	  list3p[b] = planetd.special[1];	  
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else if (sort == 10)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -( ( planetd.position >> 8 ) & 0xFFF );
	  list3p[b] = -( planetd.position >> 20 );
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else if (sort == 11)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.size;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 12)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= -d - planetd.construction;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 13)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= -(int)(100 * ( cmdGetBuildOvercost( planetd.size, d + planetd.construction ) -1 ) );
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 14)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.population;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 15)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.protection;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 16)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.special[0];
	  list3p[b] = -planetd.special[1];	  
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = ( ( planetd.position >> 8 ) & 0xFFF );
	  list3p[b] = ( planetd.position >> 20 );
	}
      sortlist2( num, listp, list2p, list3p );
    }
  

  *list = listp;
  free (list2p);
  free (list3p);
  return num;
}



// user planets/portals functions

int dbUserPortalsList( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x19.0\n" );
	syslog(LOG_ERR, "Failure reading file x19.0\n" );
}
  if( !( listp = malloc( 3*num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    if( fread( &listp[b], 1, 3*sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x19.1\n" );
	syslog(LOG_ERR, "Failure reading file x19.1\n" );
}
    if( fread( &flags, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x19.2\n" );
	syslog(LOG_ERR, "Failure reading file x19.2\n" );
}
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b += 3;
  }
  fclose( file );
  *list = listp;
  return (b/3);
}

int dbUserPortalsListCoords( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x20.0\n" );
	syslog(LOG_ERR, "Failure reading file x20.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+8, SEEK_SET );
    if( fread( &listp[b], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x20.1\n" );
	syslog(LOG_ERR, "Failure reading file x20.1\n" );
}
    if( fread( &flags, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x20.2\n" );
	syslog(LOG_ERR, "Failure reading file x20.2\n" );
}
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b++;
  }
  fclose( file );
  *list = listp;
  return b;
}

int dbUserPortalsListIndices( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x21.0\n" );
	syslog(LOG_ERR, "Failure reading file x21.0\n" );
}
  if( !( listp = malloc( num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    if( fread( &listp[b], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x21.1\n" );
	syslog(LOG_ERR, "Failure reading file x21.1\n" );
}
    if( fread( &flags, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x21.2\n" );
	syslog(LOG_ERR, "Failure reading file x21.2\n" );
}
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b++;
  }
  fclose( file );
  *list = listp;
  return b;
}






// user fleets functions

int dbUserFleetAdd( int id, dbUserFleetPtr fleetd )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x22.0\n" );
	syslog(LOG_ERR, "Failure reading file x22.0\n" );
}
  fseek( file, 4+(pos*sizeof(dbUserFleetDef)), SEEK_SET );
  fwrite( fleetd, 1, sizeof(dbUserFleetDef), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbUserFleetRemove( int id, int fltid )
{
  int a, num, data[24];
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x23.0\n" );
	syslog(LOG_ERR, "Failure reading file x23.0\n" );
}
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  if( fltid+1 < num )
  {
    fseek( file, 4+(num*sizeof(dbUserFleetDef))-sizeof(dbUserFleetDef), SEEK_SET );
    if( fread( data, 1, sizeof(dbUserFleetDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x23.1\n" );
	syslog(LOG_ERR, "Failure reading file x23.1\n" );
}
    fseek( file, 4+(fltid*sizeof(dbUserFleetDef)), SEEK_SET );
    fwrite( data, 1, sizeof(dbUserFleetDef), file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserFleetList( int id, dbUserFleetPtr *fleetd )
{
  int a, num;
  dbUserFleetPtr fleetp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x24.0\n" );
	syslog(LOG_ERR, "Failure reading file x24.0\n" );
}
  if( !( fleetp = malloc( num*sizeof(dbUserFleetDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*sizeof(dbUserFleetDef)), SEEK_SET );
    if( fread( &fleetp[a], 1, sizeof(dbUserFleetDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x24.1\n" );
	syslog(LOG_ERR, "Failure reading file x24.1\n" );
}
  }
  fclose( file );
  *fleetd = fleetp;
  return num;
}

int dbUserFleetSet( int id, int fltid, dbUserFleetPtr fleetd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x25.0\n" );
	syslog(LOG_ERR, "Failure reading file x25.0\n" );
}
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(fltid*sizeof(dbUserFleetDef)), SEEK_SET );
  fwrite( fleetd, 1, sizeof(dbUserFleetDef), file );
  fclose( file );
  return num;
}

int dbUserFleetRetrieve( int id, int fltid, dbUserFleetPtr fleetd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x26.0\n" );
	syslog(LOG_ERR, "Failure reading file x26.0\n" );
}
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(fltid*sizeof(dbUserFleetDef)), SEEK_SET );
  if( fread( fleetd, 1, sizeof(dbUserFleetDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x26.1\n" );
	syslog(LOG_ERR, "Failure reading file x26.1\n" );
}
  fclose( file );
  return num;
}





// user news functions

int dbUserNewsAdd( int id, long long int *data, long long int flags )
{
  long long int a, num, lused, lfree, numnext, lcur, lnext, cflags;
  FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewsadd\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewsadd\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}
  
if( fread( &num, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.0\n" );
	syslog(LOG_ERR, "Failure reading file x27.0\n" );
}
if( fread( &lused, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.1\n" );
	syslog(LOG_ERR, "Failure reading file x27.1\n" );
}
if( fread( &lfree, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.2\n" );
	syslog(LOG_ERR, "Failure reading file x27.2\n" );
}

if( fread( &cflags, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.3\n" );
	syslog(LOG_ERR, "Failure reading file x27.3\n" );
}
  cflags |= flags;
  fseek( file, 24, SEEK_SET );
  fwrite( &cflags, 1, sizeof(long long int), file );

if( fread( &numnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.4\n" );
	syslog(LOG_ERR, "Failure reading file x27.4\n" );
}

  if( lfree != -1 )
  {
    fseek( file, 40+lfree*DB_USER_NEWS_SIZE+8, SEEK_SET );
    if( fread( &lnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x27.5\n" );
	syslog(LOG_ERR, "Failure reading file x27.5\n" );
}
    fseek( file, 16, SEEK_SET );
    fwrite( &lnext, 1, sizeof(long long int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 32, SEEK_SET );
    fwrite( &numnext, 1, sizeof(long long int), file );
  }

  fseek( file, 40+lcur*DB_USER_NEWS_SIZE, SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(long long int), file );

  fwrite( &lused, 1, sizeof(long long int), file );

  fwrite( data, 1, DB_USER_NEWS_BASE*sizeof(long long int), file );
  if( lused != -1 )
  {
    fseek( file, 40+lused*DB_USER_NEWS_SIZE, SEEK_SET );
    fwrite( &lcur, 1, sizeof(long long int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &lcur, 1, sizeof(long long int), file );

  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(long long int), file );

  fclose( file );
  return 1;
}

long long int dbUserNewsGetFlags( int id ) {
	long long int flags;
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewsflags\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewsflags\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fseek( file, 24, SEEK_SET );
if( fread( &flags, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x28.0\n" );
	syslog(LOG_ERR, "Failure reading file x28.0\n" );
}
fclose( file );

return flags;
}

int dbUserNewsList( int id, long long int **data )
{
  long long int a, b, c, num, lused, lfree;
  FILE *file;
  long long int *datap;
  *data = 0;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewslist\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewslist\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}
if( fread( &num, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x29.0\n" );
	syslog(LOG_ERR, "Failure reading file x29.0\n" );
}
if( fread( &lused, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x29.1\n" );
	syslog(LOG_ERR, "Failure reading file x29.1\n" );
}
if( fread( &lfree, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x29.2\n" );
	syslog(LOG_ERR, "Failure reading file x29.2\n" );
}

  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, b += (DB_USER_NEWS_BASE) )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE+8, SEEK_SET );
    if( fread( &a, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x29.3\n" );
	syslog(LOG_ERR, "Failure reading file x29.3\n" );
}
    if( fread( &datap[b], 1, DB_USER_NEWS_BASE*sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x29.4\n" );
	syslog(LOG_ERR, "Failure reading file x29.4\n" );
}
  }
  *data = datap;

  fclose( file );
  return num;
}

// I hate doing linked lists with files!! The one who fully understand this function on the first read gets a cookie.
long long int dbUserNewsListUpdate( int id, long long int **data, long long int time )
{
  long long int a, b, c, d, num, numnew, lused, lfree, lprev, lnext;
  FILE *file;
  long long int *datap;
  *data = 0;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernews\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernews\n", errno );
	return -3;
}
if( fread( &num, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.0\n" );
	syslog(LOG_ERR, "Failure reading file x30.0\n" );
}
if( fread( &lused, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.1\n" );
	syslog(LOG_ERR, "Failure reading file x30.1\n" );
}
if( fread( &lfree, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.2\n" );
	syslog(LOG_ERR, "Failure reading file x30.2\n" );
}
  a = 0;
  fwrite( &a, 1, sizeof(long long int), file );
  numnew = num;

  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, a = lnext )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
    if( fread( &lprev, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.3\n" );
	syslog(LOG_ERR, "Failure reading file x30.3\n" );
}
    if( fread( &lnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.4\n" );
	syslog(LOG_ERR, "Failure reading file x30.4\n" );
}
    if( fread( &datap[b], 1, 2*sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.5\n" );
	syslog(LOG_ERR, "Failure reading file x30.5\n" );
}
    if( !( datap[b+1] & CMD_NEWS_FLAGS_NEW ) )
    {
      numnew--;
      if( datap[b+0]+CMD_NEWS_EXPIRE_TIME >= time )
        continue;
      num--;

      if( lprev != -1 )
        fseek( file, 40+lprev*DB_USER_NEWS_SIZE+8, SEEK_SET );
      else
        fseek( file, 8, SEEK_SET );
      fwrite( &lnext, 1, sizeof(long long int), file );
      if( lnext != -1 )
      {
        fseek( file, 40+lnext*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &lprev, 1, sizeof(long long int), file );
      }

      fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
      d = -1;
      fwrite( &d, 1, sizeof(long long int), file );
      fwrite( &lfree, 1, sizeof(long long int), file );
      if( lfree != -1 )
      {
        fseek( file, 40+lfree*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &a, 1, sizeof(long long int), file );
      }
      fseek( file, 16, SEEK_SET );
      lfree = a;
      fwrite( &lfree, 1, sizeof(long long int), file );

      fseek( file, 0, SEEK_SET );
      fwrite( &num, 1, sizeof(long long int), file );
    }
    fseek( file, 40+a*DB_USER_NEWS_SIZE+16+8, SEEK_SET );
    datap[b+1] &= 0xFFFFFFFF - CMD_NEWS_FLAGS_NEW;
    fwrite( &datap[b+1], 1, sizeof(long long int), file );
    if( fread( &datap[b+2], 1, (DB_USER_NEWS_BASE-2)*sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x30.6\n" );
	syslog(LOG_ERR, "Failure reading file x30.6\n" );
}
    b += DB_USER_NEWS_BASE;
  }
  *data = datap;

  fclose( file );
  return numnew;
}

int dbUserNewsEmpty( int id ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewsempty\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewsempty\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

fwrite( dbFileUserListData[DB_FILE_USER_NEWS], 1, dbFileUserListBase[DB_FILE_USER_NEWS], file );
fclose( file );

return 1;
}





// fam news functions

int dbFamNewsAdd( int id, long long int *data )
{
  long long int a, num, lused, lfree, numnext, lcur, lnext;
  FILE *file;
  char fname[32];
  sprintf( fname, "fam%dnews", id );
if( !( file = fopen( fname, "rb+" ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewsadd\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewsadd\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
  	return -3;
}
  
if( fread( &num, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x31.0\n" );
	syslog(LOG_ERR, "Failure reading file x31.0\n" );
}
if( fread( &lused, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x31.1\n" );
	syslog(LOG_ERR, "Failure reading file x31.1\n" );
}
if( fread( &lfree, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x31.2\n" );
	syslog(LOG_ERR, "Failure reading file x31.2\n" );
}
  fseek( file, 32, SEEK_SET );
if( fread( &numnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x31.3\n" );
	syslog(LOG_ERR, "Failure reading file x31.3\n" );
}

  if( lfree != -1 )
  {
    fseek( file, 40+lfree*DB_USER_NEWS_SIZE+8, SEEK_SET );
    if( fread( &lnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x31.4\n" );
	syslog(LOG_ERR, "Failure reading file x31.4\n" );
}
    fseek( file, 16, SEEK_SET );
    fwrite( &lnext, 1, sizeof(long long int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 32, SEEK_SET );
    fwrite( &numnext, 1, sizeof(long long int), file );
  }

  fseek( file, 40+lcur*DB_USER_NEWS_SIZE, SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(long long int), file );

  fwrite( &lused, 1, sizeof(long long int), file );

  fwrite( data, 1, DB_USER_NEWS_BASE*sizeof(long long int), file );
  if( lused != -1 )
  {
    fseek( file, 40+lused*DB_USER_NEWS_SIZE, SEEK_SET );
    fwrite( &lcur, 1, sizeof(long long int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &lcur, 1, sizeof(long long int), file );

  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(long long int), file );

  fclose( file );
  return 1;
}

int dbFamNewsList( int id, long long int **data, int time )
{
  long long int a, b, c, d, num, lused, lfree, lprev, lnext;
  FILE *file;
  long long int *datap;
  char fname[32];
  *data = 0;
  sprintf( fname, "fam%dnews", id );

if( !( file = fopen( fname, "rb+" ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, fopen dbusernewslist\n", errno );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, fopen dbusernewslist\n", errno );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}

if( fread( &num, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.0\n" );
	syslog(LOG_ERR, "Failure reading file x35.0\n" );
}
if( fread( &lused, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.1\n" );
	syslog(LOG_ERR, "Failure reading file x35.1\n" );
}
if( fread( &lfree, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.2\n" );
	syslog(LOG_ERR, "Failure reading file x35.2\n" );
}
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, a = lnext )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
    if( fread( &lprev, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.3\n" );
	syslog(LOG_ERR, "Failure reading file x35.3\n" );
}
    if( fread( &lnext, 1, sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.4\n" );
	syslog(LOG_ERR, "Failure reading file x35.4\n" );
}
    if( fread( &datap[b], 1, 1*sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.5\n" );
	syslog(LOG_ERR, "Failure reading file x35.5\n" );
}
    if( datap[b+0]+CMD_NEWS_EXPIRE_TIME < time )
    {
      num--;
      if( lprev != -1 )
        fseek( file, 40+lprev*DB_USER_NEWS_SIZE+8, SEEK_SET );
      else
        fseek( file, 8, SEEK_SET );
      fwrite( &lnext, 1, sizeof(long long int), file );
      if( lnext != -1 )
      {
        fseek( file, 40+lnext*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &lprev, 1, sizeof(long long int), file );
      }

      fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
      d = -1;
      fwrite( &d, 1, sizeof(long long int), file );
      fwrite( &lfree, 1, sizeof(long long int), file );
      if( lfree != -1 )
      {
        fseek( file, 40+lfree*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &a, 1, sizeof(long long int), file );
      }
      fseek( file, 16, SEEK_SET );
      lfree = a;
      fwrite( &lfree, 1, sizeof(long long int), file );

      fseek( file, 0, SEEK_SET );
      fwrite( &num, 1, sizeof(long long int), file );
    }
    fseek( file, 40+a*DB_USER_NEWS_SIZE+16+8, SEEK_SET );
    if( fread( &datap[b+1], 1, (DB_USER_NEWS_BASE-1)*sizeof(long long int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x35.6\n" );
	syslog(LOG_ERR, "Failure reading file x35.6\n" );
}
    b += DB_USER_NEWS_BASE;
  }
  *data = datap;

  fclose( file );
  return num;
}






// map functions

int dbMapRetrieveMain( int *binfo ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
if( fread( binfo, 1, sizeof(dbMainMapDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x36.0\n" );
	syslog(LOG_ERR, "Failure reading file x36.0\n" );
}

return 1;
}

int dbMapSetMain( int *binfo ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
fwrite( binfo, 1, sizeof(dbMainMapDef), file );

return 1;
}


int dbMapSetSystem( int sysid, dbMainSystemPtr systemd ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if( (unsigned int)sysid >= dbMapBInfoStatic[MAP_SYSTEMS] )
	return -3;

fseek( file, sizeof(dbMainMapDef)+(sysid*sizeof(dbMainSystemDef)), SEEK_SET );
fwrite( systemd, 1, sizeof(dbMainSystemDef), file );

return 1;
}

int dbMapRetrieveSystem( int sysid, dbMainSystemPtr systemd ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if( (unsigned int)sysid >= dbMapBInfoStatic[MAP_SYSTEMS] )
	return -3;

fseek( file, sizeof(dbMainMapDef)+(sysid*sizeof(dbMainSystemDef)), SEEK_SET );
if( fread( systemd, 1, sizeof(dbMainSystemDef), file ) < 1 ) {
	if( options.verbose )
		printf("Failure reading file x37.0\n" );
	syslog(LOG_ERR, "Failure reading file x37.0\n" );
}

return 1;
}

int dbMapSetPlanet( int plnid, dbMainPlanetPtr planetd ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if( (unsigned int)plnid >= dbMapBInfoStatic[MAP_PLANETS] )
	return -3;

fseek( file, sizeof(dbMainMapDef)+(dbMapBInfoStatic[MAP_SYSTEMS]*sizeof(dbMainSystemDef))+(plnid*sizeof(dbMainPlanetDef)), SEEK_SET );
fwrite( planetd, 1, sizeof(dbMainPlanetDef), file );

return 1;
}

int dbMapRetrievePlanet( int plnid, dbMainPlanetPtr planetd ) {
	FILE *file;

if( plnid == -1)
	return -3;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if( (unsigned int)plnid >= dbMapBInfoStatic[MAP_PLANETS] )
	return -3;

fseek( file, sizeof(dbMainMapDef)+(dbMapBInfoStatic[MAP_SYSTEMS]*sizeof(dbMainSystemDef))+(plnid*sizeof(dbMainPlanetDef)), SEEK_SET );
if( fread( planetd, 1, sizeof(dbMainPlanetDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x38.0\n" );
	syslog(LOG_ERR, "Failure reading file x38.0\n" );
}

return 1;
}


int dbMapSetEmpire( int famid, dbMainEmpirePtr empired ) {
	FILE *file;
//	dbUserPtr user;


if(!( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if((unsigned int)famid >= dbMapBInfoStatic[MAP_EMPIRES])
	return -3;
/*
//---------------------
if ( empired->numplayers == 1) {
	empired->leader = empired->player[0];
	if(( user = dbUserLinkID( empired->leader ) )) {
		user->flags &= 0xFFFF;
		user->flags |= cmdUserFlags[CMD_FLAGS_LEADER] | CMD_USER_FLAGS_ACTIVATED;
		dbUserSave( empired->leader, user);
	}
}
//-----------------------
*/

fseek( file, sizeof(dbMainMapDef)+(dbMapBInfoStatic[MAP_SYSTEMS]*sizeof(dbMainSystemDef))+(dbMapBInfoStatic[MAP_PLANETS]*sizeof(dbMainPlanetDef))+(famid*sizeof(dbMainEmpireDef)), SEEK_SET );
fwrite( empired, 1, sizeof(dbMainEmpireDef), file );

return 1;
}

int dbMapRetrieveEmpire( int famid, dbMainEmpirePtr empired ) {
	FILE *file;

if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
	return -3;

if( (unsigned int)famid >= dbMapBInfoStatic[MAP_EMPIRES] )
	return -3;

fseek( file, sizeof(dbMainMapDef)+(dbMapBInfoStatic[MAP_SYSTEMS]*sizeof(dbMainSystemDef))+(dbMapBInfoStatic[MAP_PLANETS]*sizeof(dbMainPlanetDef))+(famid*sizeof(dbMainEmpireDef)), SEEK_SET );
if( fread( empired, 1, sizeof(dbMainEmpireDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x39.0\n" );
	syslog(LOG_ERR, "Failure reading file x39.0\n" );
}

return 1;
}




/*
relation
  4:tick
  4:type
  4:empire
  4:flags
*/

int dbEmpireRelsAdd( int id, int *rel )
{
  int pos;
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x40.0\n" );
	syslog(LOG_ERR, "Failure reading file x40.0\n" );
}
  fseek( file, 4+(pos*sizeof(dbEmpireRelationsDef)), SEEK_SET );
  fwrite( rel, 1, sizeof(dbEmpireRelationsDef), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbEmpireRelsRemove( int id, int relid )
{
  int a, num, data[4];
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x41.0\n" );
	syslog(LOG_ERR, "Failure reading file x41.0\n" );
}
  if( (unsigned int)relid >= num )
  {
    fclose( file );
    return -2;
  }
  if( relid+1 < num )
  {
    fseek( file, 4+(num*sizeof(dbEmpireRelationsDef))-sizeof(dbEmpireRelationsDef), SEEK_SET );
    if( fread( data, 1, sizeof(dbEmpireRelationsDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x41.1\n" );
	syslog(LOG_ERR, "Failure reading file x41.1\n" );
}
    fseek( file, 4+(relid*sizeof(dbEmpireRelationsDef)), SEEK_SET );
    fwrite( data, 1, sizeof(dbEmpireRelationsDef), file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbEmpireRelsList( int id, int **rel )
{
  int num;
  FILE *file;
  int *relp;
  if( (unsigned int)id >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x42.0\n" );
	syslog(LOG_ERR, "Failure reading file x42.0\n" );
}
  if( !( relp = malloc( num*sizeof(dbEmpireRelationsDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  if( ( fread( relp, 1, num*sizeof(dbEmpireRelationsDef), file ) < 1 ) && ( num ) ) {
 	if( options.verbose )
		printf("Failure reading file x42.1\n" );
	syslog(LOG_ERR, "Failure reading file x42.1\n" );
}
  fclose( file );
  *rel = relp;
  return num;
}

int dbEmpireRelsGet( int id, int relid, int *rel )
{
  int num;
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x43.0\n" );
	syslog(LOG_ERR, "Failure reading file x43.0\n" );
}
  if( (unsigned int)relid >= num )
  {
    fclose( file );
    return -1;
  }
  fseek( file, 4+relid*sizeof(dbEmpireRelationsDef), SEEK_SET );
  if( fread( rel, 1, sizeof(dbEmpireRelationsDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x43.1\n" );
	syslog(LOG_ERR, "Failure reading file x43.1\n" );
}
  fclose( file );
  return num;
}


/*
market
   4:num max bids
   4:list free

3 * 2 * 80 * 12 ; res*action*prices*struct
struct( 12 )
   4:total of bids
   4:first list ID
   4:last list ID
struct( 16 )
   4:previous id
   4:next id
   4:quantity
   4:owner ID


bid :
4:action (0buy,1sell)
4:ressource
4:price
4:quantity
4:user ID
*/

#define DB_MARKET_BIDSOFF ( 8 + 3*2*DB_MARKET_RANGE*12 )

int dbMarketReset()
{
  int a, array[3];
  FILE *file;
  if( !( file = fopen( dbFileList[DB_FILE_MARKET], "wb+" ) ) )
    return 0;
  fseek( file, 0, SEEK_SET );
  array[0] = 0;
  array[1] = -1;
  fwrite( array, 1, 2*sizeof(int), file );
  array[0] = 0;
  array[1] = -1;
  array[2] = -1;
  for( a = 0 ; a < 6*DB_MARKET_RANGE ; a++ )
    fwrite( array, 1, 3*sizeof(int), file );
  fclose( file );
  return 1;
}

int dbMarketFull( int *list )
{
  int a;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < 3*2*DB_MARKET_RANGE ; a++ )
  {
    fseek( file, 8+a*12, SEEK_SET );
    if( fread( &list[a], 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x44.0\n" );
	syslog(LOG_ERR, "Failure reading file x44.0\n" );
}
  }
  return 1;
}


int dbMarketReplaceFull( int *list )
{
  int a;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < 3*2*DB_MARKET_RANGE ; a++ )
  {
    fseek( file, 8+a*12, SEEK_SET );
    fwrite( &list[a], 1, sizeof(int), file );
  }
  return 1;
}



int dbMarketAdd( int *bid )
{
  int a, offs, num, lcur, lfree, databid[4];
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  if( fread( mhead, 1, 12, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x45.0\n" );
	syslog(LOG_ERR, "Failure reading file x45.0\n" );
}

  fseek( file, 0, SEEK_SET );
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x45.1\n" );
	syslog(LOG_ERR, "Failure reading file x45.1\n" );
}
  if( fread( &lfree, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x45.2\n" );
	syslog(LOG_ERR, "Failure reading file x45.2\n" );
}

  if( lfree != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+lfree*16 + 4, SEEK_SET );
    if( fread( &a, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x45.3\n" );
	syslog(LOG_ERR, "Failure reading file x45.3\n" );
}
    fseek( file, 4, SEEK_SET );
    fwrite( &a, 1, sizeof(int), file );
    lcur = lfree;
  }
  else
  {
    lcur = num;
    num++;
    fseek( file, 0, SEEK_SET );
    fwrite( &num, 1, sizeof(int), file );
  }

  databid[0] = mhead[2];
  databid[1] = -1;
  databid[2] = bid[DB_MARKETBID_QUANTITY];
  databid[3] = bid[DB_MARKETBID_USERID];

  mhead[0] += bid[DB_MARKETBID_QUANTITY];
  if( mhead[1] == -1 )
    mhead[1] = lcur;
  else
  {
    fseek( file, DB_MARKET_BIDSOFF+mhead[2]*16 + 4, SEEK_SET );
    fwrite( &lcur, 1, sizeof(int), file );
  }
  mhead[2] = lcur;

  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fwrite( databid, 1, 16, file );

  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  return lcur;
}

int dbMarketRemove( int *bid, int lcur )
{
  int offs, lfree, databid[4];
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  if( fread( mhead, 1, 12, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x46.0\n" );
	syslog(LOG_ERR, "Failure reading file x46.0\n" );
}
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  if( fread( databid, 1, 16, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x46.1\n" );
	syslog(LOG_ERR, "Failure reading file x46.1\n" );
}

  if( databid[0] != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+databid[0]*16+4, SEEK_SET );
    fwrite( &databid[1], 1, sizeof(int), file );
  }
  if( databid[1] != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+databid[1]*16, SEEK_SET );
    fwrite( &databid[0], 1, sizeof(int), file );
  }

  mhead[0] -= databid[2];
  if( mhead[1] == lcur )
    mhead[1] = databid[1];
  if( mhead[2] == lcur )
    mhead[2] = databid[0];
  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  fseek( file, 4, SEEK_SET );
  if( fread( &lfree, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x46.2\n" );
	syslog(LOG_ERR, "Failure reading file x46.2\n" );
}

  databid[0] = -1;
  databid[1] = lfree;
  databid[2] = -1;
  databid[3] = -1;
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fwrite( databid, 1, 16, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &lcur, 1, sizeof(int), file );

  return lcur;
}

int dbMarketListStart( int *bid )
{
  int a, offs;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs + 4, SEEK_SET );
  if( fread( &a, 1, 4, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x47.0\n" );
	syslog(LOG_ERR, "Failure reading file x47.0\n" );
}

  return a;
}

int dbMarketListNext( int lcur, int *result )
{
  int databid[4];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  if( fread( databid, 1, 16, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x48.0\n" );
	syslog(LOG_ERR, "Failure reading file x48.0\n" );
}
  result[0] = databid[2];
  result[1] = databid[3];
  return databid[1];
}

int dbMarketSetQuantity( int *bid, int lcur, int quantity, int loss )
{
  int offs;
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  if( fread( mhead, 1, 12, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x49.0\n" );
	syslog(LOG_ERR, "Failure reading file x49.0\n" );
}
  mhead[0] -= loss;
  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  fseek( file, DB_MARKET_BIDSOFF+lcur*16 + 8, SEEK_SET );
  fwrite( &quantity, 1, 4, file );

  return 0;
}




/*
user%d/market
  4:number of bids
  4:flags
struct ( 20 )
  4:action
  4:resource
  4:price
  4:quantity
  4:bid ID

*/

int dbUserMarketReset( int id )
{
  int a;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserMarketAdd( int id, int bidid, int action, int resource, int price, int quantity )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x50.0\n" );
	syslog(LOG_ERR, "Failure reading file x50.0\n" );
}
  fseek( file, 8+(pos*20), SEEK_SET );
  fwrite( &action, 1, sizeof(int), file );
  fwrite( &resource, 1, sizeof(int), file );
  fwrite( &price, 1, sizeof(int), file );
  fwrite( &quantity, 1, sizeof(int), file );
  fwrite( &bidid, 1, sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserMarketList( int id, int **list )
{
  int num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x51.0\n" );
	syslog(LOG_ERR, "Failure reading file x51.0\n" );
}
  if( !( listp = malloc( num*5*sizeof(int) ) ) )
    return -1;
  fseek( file, 8, SEEK_SET );
  if( ( fread( listp, 1, num*5*sizeof(int), file ) < 1 ) && ( num ) ) {
 	if( options.verbose )
		printf("Failure reading file x51.1\n" );
	syslog(LOG_ERR, "Failure reading file x51.1\n" );
}
  fclose( file );
  *list = listp;
  return num;
}

int dbUserMarketQuantity( int id, int bidid, int quantity )
{
  int a, b, num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x52.0\n" );
	syslog(LOG_ERR, "Failure reading file x52.0\n" );
}
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 8+(a*20)+16, SEEK_SET );
    if( fread( &b, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x52.1\n" );
	syslog(LOG_ERR, "Failure reading file x52.1\n" );
}
    if( b != bidid )
      continue;
    fseek( file, 8+(a*20)+12, SEEK_SET );
    fwrite( &quantity, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}

int dbUserMarketRemove( int id, int bidid )
{
  int a, b, num, data[5];
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x53.0\n" );
	syslog(LOG_ERR, "Failure reading file x53.0\n" );
}
  if( num >= 2 )
  {
    fseek( file, 8+(num*20)-20, SEEK_SET );
    if( fread( data, 1, 20, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x53.1\n" );
	syslog(LOG_ERR, "Failure reading file x53.1\n" );
}
  }
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 8+(a*20)+16, SEEK_SET );
    if( fread( &b, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x53.2\n" );
	syslog(LOG_ERR, "Failure reading file x53.2\n" );
}
    if( b != bidid )
      continue;
    if( ( num >= 2 ) && ( a+1 < num ) )
    {
      fseek( file, 8+(a*20), SEEK_SET );
      fwrite( data, 1, 20, file );
    }
    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}







/*
forums
  4:number
struct ( X )
N:X:dbForumForumDef

forum%d/threads
  4:num
  4:list used
  4:list free
  4:new num
struct ( X )
  X;dbForumForumDef
struct ( X )
  4:previous
  4:next
  X:dbForumThreadDef

forum%d/thread%d
  4:number
  4:offset
struct ( X )
  X:dbForumThreadDef
struct ( X+Y )
  4:offset
  X:dbForumPostInDef
  Y:text
*/

int dbForumListForums( int perms, dbForumForumPtr *forums )
{
  int num;
  FILE *file;
  dbForumForumPtr forumsp;
  char szSource[500];
  
  sprintf(szSource, "%s/forums", sysconfig.pubforum);
  if( !( file = fopen( szSource, "rb+" ) ))
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x54.0\n" );
	syslog(LOG_ERR, "Failure reading file x54.0\n" );
}
  if( !( forumsp = malloc( num * sizeof(dbForumForumDef) ) ) )
    return -3;
  if( (fread( forumsp, 1, num*sizeof(dbForumForumDef), file ) < 1 ) && ( num ) ) {
 	if( options.verbose )
		printf("Failure reading file x54.1\n" );
	syslog(LOG_ERR, "Failure reading file x54.1\n" );
}
  *forums = forumsp;
  fclose( file );
  return num;
}


int dbForumListThreads( int forum, int base, int end, dbForumForumPtr forumd, dbForumThreadPtr *threads )
{
  int a, b, c, d, num, lused, lfree, numnext;
  FILE *file;
  char fname[556];
  dbForumThreadPtr threadsp;

  *threads = 0;
  if( base < 0 )
    return -3;
  if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
  
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.0\n" );
	syslog(LOG_ERR, "Failure reading file x55.0\n" );
}
if( fread( &lused, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.1\n" );
	syslog(LOG_ERR, "Failure reading file x55.1\n" );
}
if( fread( &lfree, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.2\n" );
	syslog(LOG_ERR, "Failure reading file x55.2\n" );
}
if( fread( &numnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.3\n" );
	syslog(LOG_ERR, "Failure reading file x55.3\n" );
}
if( fread( forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.4\n" );
	syslog(LOG_ERR, "Failure reading file x55.4\n" );
}

  if( end < num )
    num = end;
  if( base > num )
  {
    fclose( file );
    return -3;
  }

  if( !( threadsp = malloc( num * sizeof(dbForumThreadDef) ) ) )
    return -3;
  if( base == num )
  {
    fclose( file );
    *threads = threadsp;
    return 0;
  }
  for( a = lused, b = 0 ; b < base ; b++ )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + a * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    if( fread( &a, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.5\n" );
	syslog(LOG_ERR, "Failure reading file x55.5\n" );
}
  }
  for( d = 0 ; b < num ; b++, d++ )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + a * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    c = a;
    if( fread( &a, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.6\n" );
	syslog(LOG_ERR, "Failure reading file x55.6\n" );
}
    if( fread( &threadsp[d], 1, sizeof(dbForumThreadDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x55.7\n" );
	syslog(LOG_ERR, "Failure reading file x55.7\n" );
}
    threadsp[d].id = c;
  }

  *threads = threadsp;
  fclose( file );


  return d;
}


int dbForumListPosts( int forum, int thread, int base, int end, dbForumThreadPtr threadd, dbForumPostPtr *posts )
{
  int a, b, offset, num;
  char fname[556];
  FILE *file;
  dbForumPostPtr postsp;

  *posts = 0;
  if( base < 0 )
    return -3;
  if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.0\n" );
	syslog(LOG_ERR, "Failure reading file x56.0\n" );
}

  fseek( file, 8, SEEK_SET );
if( fread( threadd, 1, sizeof(dbForumThreadDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.1\n" );
	syslog(LOG_ERR, "Failure reading file x56.1\n" );
}

  if( end < num )
    num = end;
  if( base >= num )
  {
    fclose( file );
    return -3;
  }

  if( !( postsp = malloc( num * sizeof(dbForumPostDef) ) ) )
    return -3;
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < base ; a++ )
  {
    fseek( file, offset, SEEK_SET );
    if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.2\n" );
	syslog(LOG_ERR, "Failure reading file x56.2\n" );
}
  }

  for( b = 0 ; a < num ; a++, b++ )
  {
    fseek( file, offset, SEEK_SET );
    if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.3\n" );
	syslog(LOG_ERR, "Failure reading file x56.3\n" );
}
    if( fread( &postsp[b].post, 1, sizeof(dbForumPostInDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.4\n" );
	syslog(LOG_ERR, "Failure reading file x56.4\n" );
}
    postsp[b].text = 0;
    if( (unsigned int)((postsp[b].post).length) >= 65536 )
    {
      (postsp[b].post).flags |= DB_FORUM_FLAGS_POSTERROR;
      continue;
    }
    if( !( postsp[b].text = malloc( (postsp[b].post).length + 1 ) ) )
    {
      (postsp[b].post).flags |= DB_FORUM_FLAGS_POSTERROR;
      continue;
    }
    if( fread( postsp[b].text, 1, (postsp[b].post).length, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x56.5\n" );
	syslog(LOG_ERR, "Failure reading file x56.5\n" );
}
    postsp[b].text[ (postsp[b].post).length ] = 0;
  }

  *posts = postsp;
  fclose( file );
  return b;
}




int dbForumRetrieveForum( int forum, dbForumForumPtr forumd )
{
  FILE *file;
  char fname[556];
  
  if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fseek( file, 16, SEEK_SET );
  if( fread( forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x57.0\n" );
	syslog(LOG_ERR, "Failure reading file x57.0\n" );
}
  fclose( file );
  return 1;
}



int dbForumAddForum( dbForumForumPtr forumd, int type, int nid )
{
  int a, num;
  FILE *file;
  char fname[532];
  num = nid;
  if( !( type ) )
  {
  	sprintf( fname, "%s/forums", sysconfig.pubforum);
    if( !( file = fopen( fname, "rb+" ) ))
      return -3;
    if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x58.0\n" );
	syslog(LOG_ERR, "Failure reading file x58.0\n" );
}
    fseek( file, 4+num*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( forumd, 1, sizeof(dbForumForumDef), file );
    num++;
    fseek( file, 0, SEEK_SET );
    fwrite( &num, 1, sizeof(int), file );
    num--;
    fclose( file );
  }
	
	if(num > 100)
  	sprintf( fname, "forum%d", num );
  else
  	sprintf( fname, "%s/forum%d", sysconfig.pubforum, num );
if( mkdir( fname, S_IRWXU ) == -1 ) {
	if( options.verbose ){
		printf("Error %02d, mkdir(%s)\n", errno, fname );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, mkdir(%s)\n", errno, fname );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
}
	
	if(num > 100)
  	sprintf( fname, "forum%d/threads", num );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, num );
  if( !( file = fopen( fname, "wb" ) ))
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

  return num;
}

int dbForumRemoveForum( int forum )
{
  int a, b, num;
  FILE *file;
  DIR *dirdata;
  struct dirent *direntry;
  char fname[256];
  char *frcopy;
  dbForumForumDef forumd;

	if(forum > 100)
  	a = sprintf( fname, "%s/data/forum%d", sysconfig.directory, forum );
  else
  	a = sprintf( fname,  "%s/forum%d", sysconfig.pubforum, forum );
  if( !( dirdata = opendir( fname ) ) ) {
	if( options.verbose ) {
		printf("Error %02d, opendir(%s)\n", errno, fname );
		printf("Error description is : %s\n",strerror(errno) );
	}
	syslog(LOG_ERR, "Error %02d, opendir(%s)\n", errno, fname );
	syslog(LOG_ERR, "Error description is : %s\n",strerror(errno) );
	return -3;
  }
  fname[a] = '/';
  while( ( direntry = readdir( dirdata ) ) )
  {
    sprintf( &fname[a+1], "%s", direntry->d_name );
    unlink( fname );
  }
  closedir( dirdata );
  
  if(forum > 100)
  	sprintf( fname, "forum%d", forum );
  else
  	sprintf( fname, "%s/forum%d", sysconfig.pubforum, forum );
  rmdir( fname );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", sysconfig.pubforum );	
	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x59.0\n" );
	syslog(LOG_ERR, "Failure reading file x59.0\n" );
}
  if( forum >= num )
  {
    fclose( file );
    return 0;
  }
  fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
  if( fread( &forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x59.1\n" );
	syslog(LOG_ERR, "Failure reading file x59.1\n" );
}
  forumd.flags |= DB_FORUM_FLAGS_FORUMUNUSED;
  fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  a = num - ( forum + 1 );
  if( a )
  {
    b = a * sizeof(dbForumForumDef);
    frcopy = malloc( b );
    fseek( file, 4+(forum+1)*sizeof(dbForumForumDef), SEEK_SET );
    if( fread( frcopy, 1, b, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x59.2\n" );
	syslog(LOG_ERR, "Failure reading file x59.2\n" );
}
    fseek( file, 4+(forum+0)*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( frcopy, 1, b, file );
    free( frcopy );
  }
  num--;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
  fclose( file );

  return num;
}



int dbForumAddThread( int forum, dbForumThreadPtr threadd )
{
  int a, num, lused, lfree, numnext, lcur, lnext;
  FILE *file;
  char fname[256];
  dbForumForumDef forumd;

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.0\n" );
	syslog(LOG_ERR, "Failure reading file x60.0\n" );
}
if( fread( &lused, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.1\n" );
	syslog(LOG_ERR, "Failure reading file x60.1\n" );
}
if( fread( &lfree, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.2\n" );
	syslog(LOG_ERR, "Failure reading file x60.2\n" );
}
if( fread( &numnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.3\n" );
	syslog(LOG_ERR, "Failure reading file x60.3\n" );
}
if( fread( &forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.4\n" );
	syslog(LOG_ERR, "Failure reading file x60.4\n" );
}

  if( lfree != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lfree * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    if( fread( &lnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.5\n" );
	syslog(LOG_ERR, "Failure reading file x60.5\n" );
}
    fseek( file, 8, SEEK_SET );
    fwrite( &lnext, 1, sizeof(int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 12, SEEK_SET );
    fwrite( &numnext, 1, sizeof(int), file );
  }
  forumd.time = threadd->time;
  forumd.tick = threadd->tick;
  forumd.threads++;

  fseek( file, 16+sizeof(dbForumForumDef) + lcur * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &lused, 1, sizeof(int), file );
  threadd->flags &= 0xFFFFFFFF - DB_FORUM_FLAGS_THREADFREE;
  fwrite( threadd, 1, sizeof(dbForumThreadDef), file );

  if( lused != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lused * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    fwrite( &lcur, 1, sizeof(int), file );
  }

  fseek( file, 0, SEEK_SET );
  num++;
  fwrite( &num, 1, sizeof(int), file );
  fwrite( &lcur, 1, sizeof(int), file );
  fseek( file, 16, SEEK_SET );
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", sysconfig.pubforum );	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x60.6\n" );
	syslog(LOG_ERR, "Failure reading file x60.6\n" );
}
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );
	
	if(forum > 100)
		sprintf( fname, "forum%d/thread%d", forum, lcur );
	else
		sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, lcur );

  if( !( file = fopen( fname, "wb+" ) ))
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  a = 8 + sizeof(dbForumThreadDef);
  fwrite( &a, 1, sizeof(int), file );
  fwrite( threadd, 1, sizeof(dbForumThreadDef), file );
  fclose( file );

  return lcur;
}

int dbForumRemoveThread( int forum, int thread )
{
  int a, num, lused, lfree, numnext, lprev, lnext;
  FILE *file;
  char fname[256];
  dbForumForumDef forumd;
  dbForumThreadDef threadd;

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.0\n" );
	syslog(LOG_ERR, "Failure reading file x61.0\n" );
}
if( fread( &lused, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.1\n" );
	syslog(LOG_ERR, "Failure reading file x61.1\n" );
}
if( fread( &lfree, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.2\n" );
	syslog(LOG_ERR, "Failure reading file x61.2\n" );
}
if( fread( &numnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.3\n" );
	syslog(LOG_ERR, "Failure reading file x61.3\n" );
}
if( fread( &forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.4\n" );
	syslog(LOG_ERR, "Failure reading file x61.4\n" );
}
  if( (unsigned int)thread >= numnext )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  if( fread( &lprev, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.5\n" );
	syslog(LOG_ERR, "Failure reading file x61.5\n" );
}
  if( fread( &lnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.6\n" );
	syslog(LOG_ERR, "Failure reading file x61.6\n" );
}
  if( fread( &threadd, 1, sizeof(dbForumThreadDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.7\n" );
	syslog(LOG_ERR, "Failure reading file x61.7\n" );
}
  if( ( threadd.flags & DB_FORUM_FLAGS_THREADFREE ) )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  threadd.flags |= DB_FORUM_FLAGS_THREADFREE;
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &lfree, 1, sizeof(int), file );
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  if( lprev != -1 )
    fseek( file, 16+sizeof(dbForumForumDef) + lprev * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
  else
    fseek( file, 4, SEEK_SET );
  fwrite( &lnext, 1, sizeof(int), file );
  if( lnext != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lnext * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    fwrite( &lprev, 1, sizeof(int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &thread, 1, sizeof(int), file );

  fseek( file, 0, SEEK_SET );
  num--;
  fwrite( &num, 1, sizeof(int), file );

  fseek( file, 16, SEEK_SET );
  forumd.threads--;
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, thread );
  unlink( fname );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", sysconfig.pubforum );	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.8\n" );
	syslog(LOG_ERR, "Failure reading file x61.8\n" );
}
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    if( fread( &forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x61.9\n" );
	syslog(LOG_ERR, "Failure reading file x61.9\n" );
}
    forumd.threads--;
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );

  return num;
}


int dbForumAddPost( int forum, int thread, dbForumPostPtr postd )
{
  int a, num, offset, lused, lprev, lnext;
  FILE *file;
  char fname[556];
  dbForumThreadDef threadd;
  dbForumForumDef forumd;
	
	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.0\n" );
	syslog(LOG_ERR, "Failure reading file x62.0\n" );
}
  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.1\n" );
	syslog(LOG_ERR, "Failure reading file x62.1\n" );
}

if( fread( &threadd, 1, sizeof(dbForumThreadDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.2\n" );
	syslog(LOG_ERR, "Failure reading file x62.2\n" );
}
  fseek( file, 8, SEEK_SET );
  threadd.time = postd->post.time;
  threadd.tick = postd->post.tick;
  threadd.posts++;
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  fseek( file, offset, SEEK_SET );
  a = offset + ( 4 + sizeof(dbForumPostInDef) + (postd->post).length );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &(postd->post), 1, sizeof(dbForumPostInDef), file );
  fwrite( postd->text, 1, (postd->post).length, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &a, 1, sizeof(int), file );

  fclose( file );

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fseek( file, 4, SEEK_SET );
if( fread( &lused, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.3\n" );
	syslog(LOG_ERR, "Failure reading file x62.3\n" );
}

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
if( fread( &lprev, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.4\n" );
	syslog(LOG_ERR, "Failure reading file x62.4\n" );
}
if( fread( &lnext, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.5\n" );
	syslog(LOG_ERR, "Failure reading file x62.5\n" );
}
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  if( lused != thread )
  {
    if( lprev != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lprev * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
      fwrite( &lnext, 1, sizeof(int), file );
    }
    if( lnext != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lnext * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
      fwrite( &lprev, 1, sizeof(int), file );
    }
    fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    a = -1;
    fwrite( &a, 1, sizeof(int), file );
    fwrite( &lused, 1, sizeof(int), file );
    if( lused != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lused * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
      fwrite( &thread, 1, sizeof(int), file );
    }
    fseek( file, 4, SEEK_SET );
    fwrite( &thread, 1, sizeof(int), file );
  }

  fclose( file );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", sysconfig.pubforum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.6\n" );
	syslog(LOG_ERR, "Failure reading file x62.6\n" );
}
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    if( fread( &forumd, 1, sizeof(dbForumForumDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x62.7\n" );
	syslog(LOG_ERR, "Failure reading file x62.7\n" );
}
    forumd.time = threadd.time;
    forumd.tick = threadd.tick;
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );

  return 1;
}

int dbForumRemovePost( int forum, int thread, int post )
{
  int a, num, offset, offset2;
  FILE *file;
  char fname[556];
  dbForumThreadDef threadd;
  dbForumPostDef postd;

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.0\n" );
	syslog(LOG_ERR, "Failure reading file x63.0\n" );
}
if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.1\n" );
	syslog(LOG_ERR, "Failure reading file x63.1\n" );
}
  if( (unsigned int)post >= num )
  {
    fclose( file );
    return -3;
  }

if( fread( &threadd, 1, sizeof(dbForumThreadDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.2\n" );
	syslog(LOG_ERR, "Failure reading file x63.2\n" );
}
  fseek( file, 8, SEEK_SET );
  threadd.posts--;
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  fseek( file, 8+sizeof(dbForumThreadDef), SEEK_SET );
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < num ; a++ )
  {
    if( a != post )
    {
      if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.3\n" );
	syslog(LOG_ERR, "Failure reading file x63.3\n" );
}
      fseek( file, offset, SEEK_SET );
      continue;
    }

    if( fread( &offset2, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.4\n" );
	syslog(LOG_ERR, "Failure reading file x63.4\n" );
}
    for( a++ ; a < num ; a++ )
    {
      fseek( file, offset2, SEEK_SET );
      if( fread( &offset2, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.5\n" );
	syslog(LOG_ERR, "Failure reading file x63.5\n" );
}
      if( fread( &postd.post, 1, sizeof(dbForumPostInDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.6\n" );
	syslog(LOG_ERR, "Failure reading file x63.6\n" );
}
      if( !( postd.text = malloc( (postd.post).length ) ) )
        (postd.post).length = 0;
      if( fread( postd.text, 1, (postd.post).length, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x63.7\n" );
	syslog(LOG_ERR, "Failure reading file x63.7\n" );
}

      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbForumPostInDef) + (postd.post).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &postd.post, 1, sizeof(dbForumPostInDef), file );
      fwrite( postd.text, 1, (postd.post).length, file );
      if( postd.text )
        free( postd.text );
    }

    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );

		if(forum > 100)
    	sprintf( fname, "forum%d/threads", forum );
    else
    	sprintf( fname, "%s/forum%d/threads", sysconfig.pubforum, forum );
    if( !( file = fopen( fname, "rb+" ) ))
      return -3;
    fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ) + 8, SEEK_SET );
    fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );
    fclose( file );

    return num;
  }

  return -3;
}

int dbForumEditPost( int forum, int thread, int post, dbForumPostPtr postd )
{
  int a, b, num, offset;
  FILE *file;
  char fname[556];
  dbForumThreadDef threadd;
  dbForumPostPtr posts;
/*
  if( dbForumListPosts( forum, thread, post+1, 65536, &threadd, &posts ) < 0 )
    return -3;
*/
  posts = 0;
  dbForumListPosts( forum, thread, post+1, 65536, &threadd, &posts );

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", sysconfig.pubforum, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x64.0\n" );
	syslog(LOG_ERR, "Failure reading file x64.0\n" );
}
if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x64.1\n" );
	syslog(LOG_ERR, "Failure reading file x64.1\n" );
}
  if( (unsigned int)post >= num )
  {
    if( posts )
      free( posts );
    fclose( file );
    return -3;
  }

  fseek( file, 8+sizeof(dbForumThreadDef), SEEK_SET );
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < num ; a++ )
  {
    if( a != post )
    {
      if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x64.2\n" );
	syslog(LOG_ERR, "Failure reading file x64.2\n" );
}
      fseek( file, offset, SEEK_SET );
      continue;
    }

    fseek( file, offset, SEEK_SET );
    offset += 4 + sizeof(dbForumPostInDef) + (postd->post).length;
    fwrite( &offset, 1, sizeof(int), file );
    fwrite( &postd->post, 1, sizeof(dbForumPostInDef), file );
    fwrite( postd->text, 1, (postd->post).length, file );

    for( b = 0, a++ ; a < num ; a++, b++ )
    {
      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbForumPostInDef) + (posts[b].post).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &posts[b].post, 1, sizeof(dbForumPostInDef), file );
      fwrite( posts[b].text, 1, (posts[b].post).length, file );
      if( posts[b].text )
        free( posts[b].text );
    }
    if( posts )
      free( posts );

    fseek( file, 4, SEEK_SET );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );
    return num;
  }

  if( posts )
    free( posts );
  return -3;
}




int dbMailList( int id, int type, int base, int end, dbMailPtr *mails, int *rtnum )
{
  int a, b, offset, num;
  FILE *file;
  dbMailPtr mailsp;
  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x65.0\n" );
	syslog(LOG_ERR, "Failure reading file x65.0\n" );
}
	if( rtnum )
    *rtnum = num;
  if( end < num )
    num = end;
  if( base > num )
  {
    fclose( file );
    return -3;
  }

  if( !( mailsp = malloc( num * sizeof(dbMailDef) ) ) )
    return -3;
  if( base == num )
  {
    fclose( file );
    *mails = mailsp;
    return 0;
  }
  offset = 8;
  for( a = 0 ; a < base ; a++ )
  {
    fseek( file, offset, SEEK_SET );
    if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x65.1\n" );
	syslog(LOG_ERR, "Failure reading file x65.1\n" );
}
  }

  for( b = 0 ; a < num ; a++, b++ )
  {
    fseek( file, offset, SEEK_SET );
    if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x65.2\n" );
	syslog(LOG_ERR, "Failure reading file x65.2\n" );
}
    if( fread( &mailsp[b].mail, 1, sizeof(dbMailInDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x65.3\n" );
	syslog(LOG_ERR, "Failure reading file x65.3\n" );
}
    mailsp[b].text = 0;
    if( (unsigned int)((mailsp[b].mail).length) >= 65536 )
      continue;
    if( !( mailsp[b].text = malloc( (mailsp[b].mail).length + 1 ) ) )
      continue;
    if( fread( mailsp[b].text, 1, (mailsp[b].mail).length, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x65.4\n" );
	syslog(LOG_ERR, "Failure reading file x65.4\n" );
}
    mailsp[b].text[ (mailsp[b].mail).length ] = 0;
  }

  *mails = mailsp;
  fclose( file );

  return b;
}

int dbMailAdd( int id, int type, dbMailPtr maild )
{
  int a, num, offset;
  FILE *file;
  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x66.0\n" );
	syslog(LOG_ERR, "Failure reading file x66.0\n" );
}
  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x66.1\n" );
	syslog(LOG_ERR, "Failure reading file x66.1\n" );
}

  fseek( file, offset, SEEK_SET );
  a = offset + ( 4 + sizeof(dbMailInDef) + (maild->mail).length );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &(maild->mail), 1, sizeof(dbMailInDef), file );
  fwrite( maild->text, 1, (maild->mail).length, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbMailRemove( int id, int type, int message )
{
  int a, num, offset, offset2;
  FILE *file;
  dbMailDef maild;

  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.0\n" );
	syslog(LOG_ERR, "Failure reading file x67.0\n" );
}
if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.1\n" );
	syslog(LOG_ERR, "Failure reading file x67.1\n" );
}
  if( (unsigned int)message >= num )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 8, SEEK_SET );
  offset = 8;
  for( a = 0 ; a < num ; a++ )
  {
    if( a != message )
    {
      if( fread( &offset, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.2\n" );
	syslog(LOG_ERR, "Failure reading file x67.2\n" );
}
      fseek( file, offset, SEEK_SET );
      continue;
    }

    if( fread( &offset2, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.3\n" );
	syslog(LOG_ERR, "Failure reading file x67.3\n" );
}
    for( a++ ; a < num ; a++ )
    {
      fseek( file, offset2, SEEK_SET );
      if( fread( &offset2, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.4\n" );
	syslog(LOG_ERR, "Failure reading file x67.4\n" );
}
      if( fread( &maild.mail, 1, sizeof(dbMailInDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.5\n" );
	syslog(LOG_ERR, "Failure reading file x67.5\n" );
}
      if( !( maild.text = malloc( (maild.mail).length ) ) )
        (maild.mail).length = 0;
      if( fread( maild.text, 1, (maild.mail).length, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x67.6\n" );
	syslog(LOG_ERR, "Failure reading file x67.6\n" );
}

      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbMailInDef) + (maild.mail).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &maild.mail, 1, sizeof(dbMailInDef), file );
      fwrite( maild.text, 1, (maild.mail).length, file );
      if( maild.text )
        free( maild.text );
    }

    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );
    return num;
  }

  return -3;
}

int dbMailEmpty( int id, int type )
{
  FILE *file;
  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  fwrite( dbFileUserListData[DB_FILE_USER_MAILIN+type], 1, dbFileUserListBase[DB_FILE_USER_MAILIN+type], file );
  fclose( file );
  return 1;
}








int dbUserSpecOpAdd( int id, dbUserSpecOpPtr specopd )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  if( fread( &pos, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x68.0\n" );
	syslog(LOG_ERR, "Failure reading file x68.0\n" );
}
  fseek( file, 4+(pos*sizeof(dbUserSpecOpDef)), SEEK_SET );
  fwrite( specopd, 1, sizeof(dbUserSpecOpDef), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbUserSpecOpRemove( int id, int specopid )
{
  int a, num;
  dbUserSpecOpDef data;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x69.0\n" );
	syslog(LOG_ERR, "Failure reading file x69.0\n" );
}
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  if( specopid+1 < num )
  {
    fseek( file, 4+(num*sizeof(dbUserSpecOpDef))-sizeof(dbUserSpecOpDef), SEEK_SET );
    if( fread( &data, 1, sizeof(dbUserSpecOpDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x69.1\n" );
	syslog(LOG_ERR, "Failure reading file x69.1\n" );
}
    fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
    fwrite( &data, 1, sizeof(dbUserSpecOpDef), file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserSpecOpList( int id, dbUserSpecOpPtr *specopd )
{
  int num;
  dbUserSpecOpPtr specopp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x70.0\n" );
	syslog(LOG_ERR, "Failure reading file x70.0\n" );
}
  if( !( specopp = malloc( num*sizeof(dbUserSpecOpDef) ) ) )
  {
    fclose( file );
    return -1;
  }
if( ( fread( specopp, 1, num*sizeof(dbUserSpecOpDef), file ) < 1 ) && ( num ) ) {
 	if( options.verbose )
		printf("Failure reading file x70.1\n" );
	syslog(LOG_ERR, "Failure reading file x70.1\n" );
}
  fclose( file );
  *specopd = specopp;
  return num;
}

int dbUserSpecOpSet( int id, int specopid, dbUserSpecOpPtr specopd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x71.0\n" );
	syslog(LOG_ERR, "Failure reading file x71.0\n" );
}
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
  fwrite( specopd, 1, sizeof(dbUserSpecOpDef), file );
  fclose( file );
  return num;
}

int dbUserSpecOpRetrieve( int id, int specopid, dbUserSpecOpPtr specopd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
if( fread( &num, 1, sizeof(int), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x72.0\n" );
	syslog(LOG_ERR, "Failure reading file x72.0\n" );
}
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
if( fread( specopd, 1, sizeof(dbUserSpecOpDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x72.1\n" );
	syslog(LOG_ERR, "Failure reading file x72.1\n" );
}
  fclose( file );
  return num;
}

int dbUserSpecOpEmpty( int id )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fwrite( dbFileUserListData[DB_FILE_USER_SPECOPS], 1, dbFileUserListBase[DB_FILE_USER_SPECOPS], file );
  fclose( file );
  return 1;
}










int dbUserInfoSet( int id, dbUserInfoPtr infod ) {
	FILE *file;
	dbUserPtr user;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_INFO ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
fwrite( infod, 1, sizeof(dbUserInfoDef), file );
fclose( file );

if( !( user = dbUserLinkID( id ) ) )
	return -3;

sprintf( user->faction, "%s", infod->faction );
sprintf( user->forumtag, "%s", infod->forumtag );


return 1;
}

int dbUserInfoRetrieve( int id, dbUserInfoPtr infod ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_INFO ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
if( fread( infod, 1, sizeof(dbUserInfoDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x73.0\n" );
	syslog(LOG_ERR, "Failure reading file x73.0\n" );
}
fclose( file );

return 1;
}


int dbUserDescSet( int id, dbUserDescPtr descd ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
fwrite( descd, 1, sizeof(dbUserDescDef), file );
fclose( file );

return 1;
}

int dbUserDescRetrieve( int id, dbUserDescPtr descd ) {
	FILE *file;

if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
	return -3;

fseek( file, 0, SEEK_SET );
if( fread( descd, 1, sizeof(dbUserDescDef), file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x74.0\n" );
	syslog(LOG_ERR, "Failure reading file x74.0\n" );
}
fclose( file );

return 1;
}



int dbUserRecordAdd( int id, dbUserRecordPtr recordd )
{
  int num = 0;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
if( fread( &num, 1, 4, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x75.0\n" );
	syslog(LOG_ERR, "Failure reading file x75.0\n" );
}
  num++;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
  fwrite( &num, 1, 4, file );
  fseek( file, sizeof(dbUserDescDef)+4 + ( num - 1 ) * sizeof(dbUserRecordDef), SEEK_SET );
  fwrite( recordd, 1, sizeof(dbUserRecordDef), file );
  fclose( file );
  return num;
}

int dbUserRecordList( int id, dbUserRecordPtr *records )
{
  int num = 0;
  dbUserRecordPtr recordp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
if( fread( &num, 1, 4, file ) < 1 ) {
 	if( options.verbose )
		printf("Failure reading file x76.0\n" );
	syslog(LOG_ERR, "Failure reading file x76.0\n" );
}
  if( !( recordp = malloc( num * sizeof(dbUserRecordDef) ) ) )
    return -3;
if( ( fread( recordp, 1, num * sizeof(dbUserRecordDef), file ) < 1 ) && ( num ) ) {
 	if( options.verbose )
		printf("Failure reading file x76.1\n" );
	syslog(LOG_ERR, "Failure reading file x76.1\n" );
}
  fclose( file );
  *records = recordp;
  return num;
}







