
int cmdExecNewUser( char *name, char *pass, char *faction )
{
  int a, b;
  dbUserInfoDef newuser;
  dbUserMainDef maind;

	cmdErrorString = 0;
	if( !( cmdCheckName( name ) ) )
	{
		sprintf( cmdErrorBuffer, "Your name must be between 3 and %d characters long and contain only alphanumeric and a low number of space characters. The first character must be a letter.", USER_NAME_MAX-1 );
		cmdErrorString = cmdErrorBuffer;
		return -2;
	}
	if( !( cmdCheckName( pass ) ) )
	{
		sprintf( cmdErrorBuffer, "Your password must be between 3 and %d characters long and contain only alphanumeric and a low number of space characters. The first character must be a letter.", USER_NAME_MAX-1 );
		cmdErrorString = cmdErrorBuffer;
		return -2;
	}
	if( !( cmdCheckName( faction ) ) )
	{
		sprintf( cmdErrorBuffer, "Your faction name must be between 3 and %d characters long and contain only alphanumeric and a low number of space characters. The first character must be a letter.", USER_NAME_MAX-1 );
		cmdErrorString = cmdErrorBuffer;
		return -2;
	}

  if( dbUserSearch( name ) >= 0 )
  {
    cmdErrorString = "This user name is already in use!";
    return -2;
  }
  if( dbUserSearchFaction( faction ) >= 0 )
  {
    cmdErrorString = "This faction name is already in use!";
    return -2;
  }

memset( &newuser, 0, sizeof(dbUserInfoDef) );
strncpy(newuser.name, name, USER_NAME_MAX );
strncpy(newuser.faction, faction, USER_NAME_MAX );
strncpy(newuser.password, hashencrypt(pass), USER_PASS_MAX );
strncpy(newuser.forumtag, "Player", USER_FTAG_MAX );
b = time( 0 );
newuser.createtime = b;
newuser.lasttime = b;
a = dbUserAdd( &newuser );

memcpy( &maind, &cmdUserMainDefault, sizeof(dbUserMainDef) );
strncpy( maind.faction, faction, sizeof(maind.faction) );

if( ( dbUserMainSet( a, &maind ) < 0 ) ) {
	cmdUserDelete( a );
	return -2;
}


return a;
}

int cmdExecNewUserEmpire( int id, int famnum, char *fampass, int raceid, int level )
{
  int a, b, c, x, y;
  int binfo[MAP_TOTAL_INFO], *fam;
  char epass[USER_PASS_MAX];
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  dbMainSystemDef systemd;
  dbMainPlanetDef planetd;
  dbUserPtr user;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -1;
  if( !( raceid < CMD_RACE_NUMUSED ) )
    return -1;
    
  maind.raceid = raceid;

  cmdErrorString = 0;
  if( !( dbMapRetrieveMain( binfo ) ) )
    return -1;
  
  //This is for random emp
  if( famnum == -1 )
  {
    fam = calloc( dbMapBInfoStatic[MAP_CAPACITY], sizeof(int) );
    for( a = b = 0 ; a < binfo[MAP_EMPIRES] ; a++ )
    {
      fam[a] = 0;
      cmdExecGetFamPass( a, epass );
      if( epass[0] )
        continue;
      dbEmpireGetInfo( a, &empired );
      if( !( binfo[MAP_EMEMBERS] - empired.numplayers ) )
        continue;
      fam[a] = 1 + ( empired.numplayers << 16 );
      b += fam[a];
    }
    if( !( b ) )
    {
      cmdErrorString = "The galaxy is full!";
      return -1;
    }
    c = rand() % b;
    for( a = 0 ; ; a++ )
    {
      c -= fam[a];
      if( c < 0 )
        break;
    }
    maind.empire = a;
    free( fam );
  }
  else
  {
  	//This the user specified what emp and have the right to do it
    if( famnum >= binfo[MAP_EMPIRES] )
    {
      cmdErrorString = "The empire doesn't exist!";
      return -1;
    }
    cmdExecGetFamPass( famnum, epass );

    if( ( epass[0] ) && !( checkencrypt( fampass, epass ) ) )
    {
      cmdErrorString = "The empire password is incorrect!";
      return -1;
    }
    maind.empire = famnum;
  }

  b = 100 / CMD_RESEARCH_NUMUSED;
  for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
    maind.allocresearch[a] = b;
  maind.allocresearch[0] += 100 - ( b * CMD_RESEARCH_NUMUSED );

  if( dbEmpireGetInfo( maind.empire, &empired ) < 0 )
  {
    cmdErrorString = "The empire doesn't exist!";
    return -1;
  }
  if( empired.numplayers >= binfo[MAP_EMEMBERS] )
  {
    cmdErrorString = "This empire is full!";
    return -1;
  }

  empired.player[empired.numplayers] = id;
  empired.numplayers++;
  dbEmpireSetInfo( maind.empire, &empired );

  dbMapRetrieveSystem( empired.homeid, &systemd );
  y = systemd.position >> 16;
  x = systemd.position & 0xFFFF;

  for( a = 0 ; ; a++ )
  {
    if( a == systemd.numplanets )
    {
      cmdErrorString = "This empire is full!";
      return -1;
    }
    b = systemd.indexplanet + a;
    if( dbMapRetrievePlanet( b, &planetd ) < 0 )
      return -1;
    if( planetd.owner == -1 )
      break;
  }
  maind.home = ( y << 20 ) + ( x << 8 ) + a;
  if( dbUserPlanetAdd( id, b, empired.homeid, maind.home, CMD_PLANET_FLAGS_PORTAL ) < 0 )
    return -1;

  planetd.maxpopulation = planetd.size * CMD_POPULATION_SIZE_FACTOR;
  planetd.flags |= CMD_PLANET_FLAGS_HOME | CMD_PLANET_FLAGS_PORTAL;
  planetd.owner = id;
  planetd.protection = 100;
  planetd.population = 5000;
  memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int64_t) );
  memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
  planetd.building[CMD_BUILDING_SOLAR] = 50;
  planetd.building[CMD_BUILDING_MINING] = 20;
  planetd.building[CMD_BUILDING_CRYSTAL] = 5;
  planetd.building[CMD_BUILDING_REFINEMENT] = 10;
  if( dbMapSetPlanet( b, &planetd ) < 0 )
    return -1;
  maind.planets = 1;

  if( !( user = dbUserLinkID( id ) ) )
    return -1;
  bitflag_remove( &user->flags, CMD_USER_FLAGS_KILLED );
  bitflag_remove( &user->flags, CMD_USER_FLAGS_DELETED );
  bitflag_remove( &user->flags, CMD_USER_FLAGS_NEWROUND );
  bitflag_add( &user->flags, CMD_USER_FLAGS_ACTIVATED );
  if( dbUserSave( id, user ) < 0 )
    return -2;

  if( ( dbUserFleetAdd( id, &cmdUserFleetDefault ) < 0 ) || ( cmdTotalsCalculate( id, &maind ) == NO ) || ( dbUserMainSet( id, &maind ) < 0 ) )
    return -2;

  return YES;
}


int cmdExecUserDeactivate( int id, int flags )
{
  int a, b, c, num;
  int *buffer;
  dbUserMainDef maind, main2d;
  dbUserInfoDef infod;
  dbMainPlanetDef planetd;
  dbMainSystemDef systemd;
  dbMainEmpireDef empired, empire2d;
  dbUserFleetPtr fleetp;
  dbUserSpecOpPtr specopd;
  dbUserPtr user;
  dbUserRecordDef recordd;
  ConfigArrayPtr setting;
  int i, j, k;
	int *rel;
	
  if( !( user = dbUserLinkID( id ) ) )
    return -1;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -1;
  if( dbUserInfoRetrieve( id, &infod ) < 0 )
    return -1;
  if( maind.empire != -1 )
    dbEmpireGetInfo( maind.empire, &empired );

  if( ( flags == CMD_USER_FLAGS_NEWROUND ) && ( bitflag( user->flags, CMD_USER_FLAGS_ACTIVATED ) ) && ( maind.empire != -1 ) )
  {
    recordd.roundid = ticks.round;
    setting = GetSetting( "Round Flag" );
    memcpy( recordd.roundflag, setting->string_value, USER_NAME_MAX );
    recordd.planets = maind.planets;
    recordd.networth = maind.networth;
    memcpy( recordd.faction, maind.faction, USER_NAME_MAX );
    memcpy( recordd.forumtag, infod.forumtag, USER_FTAG_MAX );
    recordd.empire = maind.empire;
    recordd.famplanets = empired.planets;
    recordd.famnetworth = empired.networth;
    memcpy( recordd.famname, empired.name, USER_NAME_MAX );
    recordd.rank = maind.rank;
    recordd.famrank = empired.rank;
    for( a = c = 0, b = 1 ; a < ARTEFACT_NUMUSED ; a++, b <<= 1 )
    {
      if( empired.artefacts & b )
        c++;
    }
    recordd.artefacts = c;
    
    //Find ally
	  if( ( j = dbEmpireRelsList( maind.empire, &rel ) ) >= 0 )
	  {
	  	j <<= 2;
	    
	    for( i = 0 ; i < j ; i += 4 )
	    {
	      if( ( rel[i+3] & 1 ) || ( rel[i+1] != CMD_RELATION_ALLY ) )
	        continue;
	      for( k = 0 ; k < j ; k += 4 )
	      {
	        if( !( rel[k+3] & 1 ) || ( rel[k+1] != CMD_RELATION_ALLY ) || ( rel[i+2] != rel[k+2] ) )
	          continue;
	        //Found the ally if we reach this line  
	        //If the ally have all the artefacts we give double tp to this user :D
	        dbEmpireGetInfo( rel[i+2], &empire2d );
	        j = pow(2, ARTEFACT_NUMUSED)-1;
	        if(empire2d.artefacts == j)
	        	infod.tagpoints += maind.planets;
	   		}
	    }
	    free( rel );
	  }
    //Find ally end
    
    if( c == ARTEFACT_NUMUSED )
      infod.tagpoints += 3 * maind.planets;
    else
      infod.tagpoints += maind.planets;

    dbUserRecordAdd( id, &recordd );
  }

  if( ( num = dbUserPlanetListIndices( id, &buffer ) ) >= 0 )
  {
    for( a = 0 ; a < num ; a++ )
    {
      dbUserPlanetRemove( id, buffer[a] );
      dbMapRetrievePlanet( buffer[a], &planetd );
      if( planetd.owner != id )
        continue;
      planetd.flags &= CMD_PLANET_FLAGS_HOME;
      planetd.owner = -1;
      planetd.construction = 0;
      planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
      memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int64_t) );
      memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
      dbMapSetPlanet( buffer[a], &planetd );
      dbMapRetrieveSystem( planetd.system, &systemd );
      systemd.unexplored++;
      dbMapSetSystem( planetd.system, &systemd );
    }
    free( buffer );
  }

  if( ( num = dbUserMarketList( id, &buffer ) ) >= 0 )
  {
    for( a = b = 0 ; a < num ; a++, b += 5 )
    {
      dbUserMarketRemove( id, buffer[b+DB_MARKETBID_BIDID] );
      dbMarketRemove( &buffer[b], buffer[b+DB_MARKETBID_BIDID] );
    }
    free( buffer );
  }

  if( maind.empire != -1 )
  {
    for( a = 0 ; a < empired.numplayers ; a++ )
    {
      if( id != empired.player[a] )
        continue;
	for( b = 0; b < CMD_EMPIRE_POLITICS_TOTAL; b++ ) {
		if( empired.politics[b] == id )
			empired.politics[b] = -1;
	}
      b = empired.numplayers-a-1;
      if( b > 0 )
      {
        memmove( &(empired.player[a]), &(empired.player[a+1]), b*sizeof(int) );
        memmove( &(empired.vote[a]), &(empired.vote[a+1]), b*sizeof(int) );
      }
      for( ; b >= 0 ; b-- )
      {
        if( empired.vote[a+b] == a )
          empired.vote[a+b] = -1;
        else if( empired.vote[a+b] > a )
          empired.vote[a+b]--;
      }

      empired.numplayers--;
 
     //Remove pass if last player
	setting = GetSetting( "Admin Empire Number" );
	if( ( empired.numplayers < 1 ) && ( maind.empire != setting->num_value ) ) {
		memset( &empired.name, 0, USER_NAME_MAX );
		memset( &empired.password, 0, USER_PASS_MAX );
	}
	
	cmdEmpireLeader( &empired );
	dbEmpireSetInfo( maind.empire, &empired );
	      
	break;
	}
	for( a = CMD_EMPIRE_POLITICS_START; a <= CMD_EMPIRE_POLITICS_END; a++ ) {
		bitflag_remove( &user->flags, a );
	}

  }

  if( ( num = dbUserFleetList( id, &fleetp ) ) >= 0 )
  {
    for( a = num-1 ; a >= 0 ; a-- )
      dbUserFleetRemove( id, a );
    free( fleetp );
  }
  if( ( num = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
     for( a = num-1 ; a >= 0 ; a-- )
       dbUserSpecOpRemove( id, a );
     if( specopd )
	free( specopd );
  }

  dbUserBuildEmpty( id );
  dbMailEmpty( id, 0 );
  dbMailEmpty( id, 1 );
  dbUserNewsEmpty( id );

  memcpy( &main2d, &cmdUserMainDefault, sizeof(dbUserMainDef) );
  if( user->level < LEVEL_MODERATOR )
    snprintf( infod.forumtag, USER_FTAG_MAX, "%s", cmdTagFind( infod.tagpoints ) );

  strncpy( main2d.faction, maind.faction, USER_NAME_MAX );
  main2d.empire = -1;

  if( !( dbUserMainSet( id, &main2d ) ) )
    return -1;
  if( !( dbUserInfoSet( id, &infod ) ) )
    return -1;

  bitflag_remove(&user->flags, CMD_USER_FLAGS_ACTIVATED );
  if( flags ) {
  	bitflag_add(&user->flags, flags );
  }
  if( dbUserSave( id, user ) < 0 )
    return -2;

  return YES;
}


int cmdUserDelete( int id )
{
  int a, b, num;
  int *buffer;
  dbUserMainDef maind;
  dbMainPlanetDef planetd;
  dbMainSystemDef systemd;
  dbMainEmpireDef empired;

  if( ( num = dbUserPlanetListIndices( id, &buffer ) ) >= 0 )
  {
    for( a = 0 ; a < num ; a++ )
    {
      dbMapRetrievePlanet( buffer[a], &planetd );
      if( planetd.owner != id )
        continue;
      planetd.flags &= CMD_PLANET_FLAGS_HOME;
      planetd.owner = -1;
      planetd.construction = 0;
      planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
      memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int64_t) );
      memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
      dbMapSetPlanet( buffer[a], &planetd );
      dbMapRetrieveSystem( planetd.system, &systemd );
      systemd.unexplored++;
      dbMapSetSystem( planetd.system, &systemd );
    }
    free( buffer );
  }

  if( ( num = dbUserMarketList( id, &buffer ) ) >= 0 )
  {
    for( a = b = 0 ; a < num ; a++, b += 5 )
      dbMarketRemove( &buffer[b], buffer[b+DB_MARKETBID_BIDID] );
    free( buffer );
  }

  if( ( dbUserMainRetrieve( id, &maind ) >= 0 ) && ( maind.empire != -1 ) && ( dbEmpireGetInfo( maind.empire, &empired ) >= 0 ) )
  {
    for( a = 0 ; a < empired.numplayers ; a++ )
    {
      if( id != empired.player[a] )
        continue;

	for( b = 0; b < CMD_EMPIRE_POLITICS_TOTAL; b++ ) {
		if( empired.politics[b] == id )
			empired.politics[b] = -1;
	}
      b = empired.numplayers-a-1;
      if( b > 0 )
      {
        memmove( &(empired.player[a]), &(empired.player[a+1]), b*sizeof(int) );
        memmove( &(empired.vote[a]), &(empired.vote[a+1]), b*sizeof(int) );
      }
      for( ; b >= 0 ; b-- )
      {
        if( empired.vote[a+b] == a )
          empired.vote[a+b] = -1;
        else if( empired.vote[a+b] > a )
          empired.vote[a+b]--;
      }

      empired.numplayers--;
      cmdEmpireLeader( &empired );
      dbEmpireSetInfo( maind.empire, &empired );
      break;
    }
  }

  dbUserBuildEmpty( id );

  if( dbUserRemove( id ) < 0 )
    return -3;

  return YES;
}



int cmdExecChangeName( int id, char *faction )
{
  dbUserInfoDef infod;
  if( dbUserInfoRetrieve( id, &infod ) < 0 )
    return -1;
  cmdErrorString = 0;
  if( !( cmdCheckName( faction ) ) )
  {
    sprintf( cmdErrorBuffer, "Your faction name must be between 3 and %d characters long and contain only alphanumeric and a low number of space characters. The first character must be a letter.", USER_NAME_MAX-1 );
    cmdErrorString = cmdErrorBuffer;
    return -2;
  }
  if( dbUserSearchFaction( faction ) >= 0 )
  {
    cmdErrorString = "This faction name is already in use!";
    return -2;
  }
  strncpy( infod.faction, faction, USER_NAME_MAX );
  dbUserInfoSet( id, &infod );
  return YES;
}


int cmdExecChangePassword( int id, char *pass )
{
  cmdErrorString = 0;
  if( !( cmdCheckName( pass ) ) )
  {
    sprintf( cmdErrorBuffer, "Your password must be between 3 and %d characters long and contain only alphanumeric and a low number of space characters. The first character must be a letter.", USER_NAME_MAX-1 );
    cmdErrorString = cmdErrorBuffer;
    return -2;
  }
  return dbUserSetPassword( id, pass );
}

 
 


int cmdExecAddBuild( int id, int type, int quantity, int plnid, int maxbuild )
{
  int a, b, c, d, ctype;
  int64_t cost[CMD_RESSOURCE_NUMUSED];
  float fa, fb;
  float costbuild[CMD_RESSOURCE_NUMUSED];
  int64_t resbuild[CMD_RESSOURCE_NUMUSED+2];
  dbMainPlanetDef planetd;
  dbUserMainDef maind;

	memset(&cost, 0, CMD_RESSOURCE_NUMUSED*sizeof(int64_t));
  cmdErrorString = 0;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  cmdGetBuildCosts( &maind, type, resbuild );
  ctype = type & 0xFFF;
  if( resbuild[0] < 0 )
  {
    cmdErrorString = "Your technology research is too low";
    return -3;
  }
  if( type == CMD_BLDG_NUMUSED )
  {
    // portal
    if( !( dbMapRetrievePlanet( plnid, &planetd ) ) )
      return -3;
    if( planetd.owner != id )
    {
      cmdErrorString = "You do not own this planet";
      return -3;
    }
    if( planetd.flags & ( CMD_PLANET_FLAGS_PORTAL | CMD_PLANET_FLAGS_PORTAL_BUILD ) )
    {
      cmdErrorString = "There is already a portal on this planet";
      return -3;
    }
    for( a = c = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      maind.ressource[a] -= (int64_t)resbuild[a];
      if( maind.ressource[a] < 0 )
        c = 1;
      cost[a] = resbuild[a];
    }
    planetd.flags |= CMD_PLANET_FLAGS_PORTAL_BUILD;
    dbUserPlanetSetFlags( id, plnid, planetd.flags );
    b = 1;
  }
  else if( !( type >> 16 ) )
  {
    if( !( dbMapRetrievePlanet( plnid, &planetd ) ) )
      return -3;
    if( planetd.owner != id )
    {
      cmdErrorString = "You do not own this planet";
      return -3;
    }
    d = planetd.construction;
    for( a = 0 ; a < CMD_BLDG_OVERBUILD ; a++ )
      d += planetd.building[a];
    fb = ( (float)maxbuild / 100.0 ) + 1.0;
    if( fb < 1.001 )
      fb = 1.001;
    memset( costbuild, 0, CMD_RESSOURCE_NUMUSED*sizeof(float) );
    fa = 1.0;
    for( b = c = 0 ; b < quantity ; b++ )
    {
      if( !( cmdBuildingFlags[ctype] ) )
      {
        fa = cmdGetBuildOvercost( planetd.size, d+b );
        if( fa >= fb )
          break;
      }

      for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
      {
        costbuild[a] += resbuild[a] * fa;
        if( maind.ressource[a] < costbuild[a] )
        {
          if( (b<<2) < quantity )
            goto cmdExecuteL0;
          else
            c = 1;
        }
      }
    }
    for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      maind.ressource[a] -= (ceil( costbuild[a] ));
      cost[a] = ceil( costbuild[a] );
    }
  }
  else
  {
    if( quantity >= 100000000 )
    {
      sprintf( cmdErrorBuffer, "I think you should try asking for a more reasonable amount of %s", cmdUnitName[ type & 0xFFF ] );
      cmdErrorString = cmdErrorBuffer;
      return -1;
    }
    for( a = c = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      maind.ressource[a] -= (resbuild[a] * quantity);
      if( maind.ressource[a] < 0 )
        c = 1;
      cost[a] = (int64_t)resbuild[a] * quantity;
    }
    plnid = -1;
    planetd.position = -1;
    b = quantity;
  }
  if( c )
  {
    d = sprintf( cmdErrorBuffer, "You lack " );
    for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      if( maind.ressource[a] < 0 )
        d += sprintf( &cmdErrorBuffer[d], "%lld %s ", -(long long)(maind.ressource[a]), cmdRessourceName[a] );
    }
    if( !( type >> 16 ) )
      sprintf( &cmdErrorBuffer[d], "to build %d %s", b, cmdBuildingName[ ctype ] );
    else
      sprintf( &cmdErrorBuffer[d], "to build %d %s", b, cmdUnitName[ ctype ] );
    cmdErrorString = cmdErrorBuffer;
    return -1;
    cmdExecuteL0:
    sprintf( cmdErrorBuffer, "You clearly lack the resources, %s especially, to build %d %s", cmdRessourceName[a], quantity, cmdBuildingName[ ctype ] );
    cmdErrorString = cmdErrorBuffer;
    return -1;
  }
  if( !( b ) )
    return 0;
  if( !( type >> 16 ) )
  {
    planetd.construction += b;
    dbMapSetPlanet( plnid, &planetd );
  }
  a = dbUserBuildAdd( id, type, cost, b, resbuild[CMD_RESSOURCE_TIME], plnid, planetd.position );
  if( a < 0 )
    return a;
  if( dbUserMainSet( id, &maind ) < 0 )
    return -3;
  return b;
}


int cmdExecAddBid( int id, int action, int resource, int price, int quantity )
{
  int a, b, c, d;
  int marketbid[DB_MARKETBID_NUMUSED];
  int bidresult[2];
  dbUserMainDef maind, main2d;
  int64_t newd[DB_USER_NEWS_BASE];

  cmdErrorString = 0;
  if( (unsigned int)resource >= 3 )
    return -3;
  if( (unsigned int)action >= 2 )
   return -3;

  if( !( ticks.status ) )
  {
    cmdErrorString = "You can't place a bid before time started.";
    return -3;
  }

  if( !( price ) || ( (unsigned int)price >= DB_MARKET_RANGE ) )
  {
    cmdErrorString = "The price must be between 1 and 250";
    return -3;
  }
  if( quantity > (2000000000/price) )
  {
    cmdErrorString = "Your bid is too large";
    return -3;
  }
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
	
	if(maind.empire == -1)
	{
		cmdErrorString = "You are not even in an empire";
		return -3;
	}
  marketbid[DB_MARKETBID_RESSOURCE] = resource;
  newd[0] = ticks.number;
  newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[3] = resource;
  c = quantity;
  if( action == 0 )
  {
    a = price * quantity;
    if( maind.ressource[CMD_RESSOURCE_ENERGY] < a )
    {
      sprintf( cmdErrorBuffer, "You lack %lld energy to place a bid for %d %s at %d.", (long long)(a - maind.ressource[CMD_RESSOURCE_ENERGY]), quantity, cmdRessourceName[resource+1], price );
      cmdErrorString = cmdErrorBuffer;
      return -3;
    }
    maind.ressource[CMD_RESSOURCE_ENERGY] -= a;
    newd[2] = CMD_NEWS_MARKET_SOLD;

    marketbid[DB_MARKETBID_ACTION] = 1;
    for( a = 0 ; a <= price ; a++ )
    {
      marketbid[DB_MARKETBID_PRICE] = a;
      b = dbMarketListStart( marketbid );
      while( (unsigned int)b < 0x1000000 )
      {
        d = dbMarketListNext( b, bidresult );
        if( c < bidresult[0] )
        {
          dbMarketSetQuantity( marketbid, b, bidresult[0] - c, c );
          dbUserMarketQuantity( bidresult[1], b, bidresult[0] - c );
          newd[4] = c;
          c = 0;
        }
        else
        {
          dbMarketRemove( marketbid, b );
          dbUserMarketRemove( bidresult[1], b );
          newd[4] = bidresult[0];
          c -= bidresult[0];
        }

        maind.ressource[marketbid[DB_MARKETBID_RESSOURCE]+1] += newd[4];
        if( bidresult[1] != id )
        {
          dbUserMainRetrieve( bidresult[1], &main2d );
          main2d.ressource[CMD_RESSOURCE_ENERGY] += (a*newd[4]);
          dbUserMainSet( bidresult[1], &main2d );
        }
        else
          maind.ressource[CMD_RESSOURCE_ENERGY] += (a*newd[4]);
        cmdUserNewsAdd( bidresult[1], newd, CMD_NEWS_FLAGS_AID );

        if( !( c ) )
          goto cmdExecuteL1;
        b = d;
      }
    }
  }
  else
  {
    a = resource + 1;
    if( maind.ressource[a] < quantity )
    {
      sprintf( cmdErrorBuffer, "You do not have %d %s.", quantity, cmdRessourceName[a] );
      cmdErrorString = cmdErrorBuffer;
      return -3;
    }
    maind.ressource[a] -= (int64_t)quantity;
    newd[2] = CMD_NEWS_MARKET_BOUGHT;

    marketbid[DB_MARKETBID_ACTION] = 0;
    for( a = DB_MARKET_RANGE-1 ; a >= price ; a-- )
    {
      marketbid[DB_MARKETBID_PRICE] = a;
      b = dbMarketListStart( marketbid );
      while( (unsigned int)b < 0x1000000 )
      {
        d = dbMarketListNext( b, bidresult );
        if( c < bidresult[0] )
        {
          dbMarketSetQuantity( marketbid, b, bidresult[0] - c, c );
          dbUserMarketQuantity( bidresult[1], b, bidresult[0] - c );
          newd[4] = c;
          c = 0;
        }
        else
        {
          dbMarketRemove( marketbid, b );
          dbUserMarketRemove( bidresult[1], b );
          newd[4] = bidresult[0];
          c -= bidresult[0];
        }

        maind.ressource[CMD_RESSOURCE_ENERGY] += (a*newd[4]);
        if( bidresult[1] != id )
        {
          dbUserMainRetrieve( bidresult[1], &main2d );
          main2d.ressource[marketbid[DB_MARKETBID_RESSOURCE]+1] += newd[4];
          dbUserMainSet( bidresult[1], &main2d );
        }
        else
          maind.ressource[marketbid[DB_MARKETBID_RESSOURCE]+1] += newd[4];
        cmdUserNewsAdd( bidresult[1], newd, CMD_NEWS_FLAGS_AID );

        if( !( c ) )
          goto cmdExecuteL1;
        b = d;
      }
    }
  }
  cmdExecuteL1:

  dbUserMainSet( id, &maind );

  if( c )
  {
    marketbid[DB_MARKETBID_ACTION] = action;
    marketbid[DB_MARKETBID_PRICE] = price;
    marketbid[DB_MARKETBID_QUANTITY] = c;
    marketbid[DB_MARKETBID_USERID] = id;
    if( ( a = dbMarketAdd( marketbid ) ) < 0 )
    {
      cmdErrorString = "Error encountered while placing bid";
      return -3;
    }
    dbUserMarketAdd( id, a, action, resource, price, c );
  }

  return YES;
}



int cmdExecRemoveBid( int id, int bidid )
{
  int a, b, c;
  int *ibuffer;
  dbUserMainDef maind;

  cmdErrorString = 0;
  if( ( b = dbUserMarketList( id, &ibuffer ) ) < 0 )
    return -3;
  for( a = c = 0 ; a < b ; a++, c += 5 )
  {
    if( ibuffer[c+DB_MARKETBID_BIDID] != bidid )
      continue;
    if( dbUserMainRetrieve( id, &maind ) < 0 )
      return -3;
    if( !( ibuffer[c+DB_MARKETBID_ACTION] ) )
      maind.ressource[CMD_RESSOURCE_ENERGY] += (int64_t)(ibuffer[c+DB_MARKETBID_QUANTITY] * ibuffer[c+DB_MARKETBID_PRICE]);
    else
      maind.ressource[ibuffer[c+DB_MARKETBID_RESSOURCE]+1] += (int64_t)(ibuffer[c+DB_MARKETBID_QUANTITY]);
    dbUserMainSet( id, &maind );
    dbMarketRemove( &ibuffer[c], bidid );
    dbUserMarketRemove( id, bidid );
    free( ibuffer );
    return YES;
  }
  free( ibuffer );
  return -3;
}

int cmdExecGetMarket( int *market )
{
  int a, b, c;
  int marketfull[6*DB_MARKET_RANGE];

  cmdErrorString = 0;
  if( dbMarketFull( marketfull ) < 0 )
    return -3;
  for( a = 0 ; a < 12 ; a++ )
    market[a] = -1;
  for( b = c = 0 ; b < 3*4 ; b += 4 )
  {
    for( a = DB_MARKET_RANGE-1 ; a >= 0 ; a-- )
    {
      if( marketfull[c+a] )
      {
        market[b+0] = marketfull[c+a];
        market[b+1] = a;
        break;
      }
    }
    c += DB_MARKET_RANGE;
    for( a = 0 ; a < DB_MARKET_RANGE ; a++ )
    {
      if( marketfull[c+a] )
      {
        market[b+2] = marketfull[c+a];
        market[b+3] = a;
        break;
      }
    }
    c += DB_MARKET_RANGE;
  }
  return YES;
}


int cmdExecSendAid( int id, int destid, int fam, int64_t *res)
{
  int a, b;
  dbMainEmpireDef empired;
  dbUserMainDef maind, main2d;
  int64_t newd[DB_USER_NEWS_BASE];

  if( !( ticks.status ) )
  {
    cmdErrorString = "You can't send aid before the beginning of the round.";
    return -3;
  }

  cmdErrorString = 0;
  if( dbEmpireGetInfo( fam, &empired ) < 0 )
    return -3;
  b = 0;
  for( a = 0 ; a < empired.numplayers ; a++ )
  {
  	//Valid its in the emp or ally we send aid to
    if( empired.player[a] == id )
      b |= 1;
    if( empired.player[a] == destid )
      b |= 2;	
  }
  if( b != 3 )
    return -3;
  b = 0;
  for( a = 0 ; a < 4 ; a++ )
  {
    b |= res[a];
    /*if( res[a] >= 1000000000 )
      return -3;*/
  }
  if( !( b ) )
  {
    cmdErrorString = "You can't send an empty aid shipment!";
    return -3;
  }

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbUserMainRetrieve( destid, &main2d ) < 0 )
    return -3;
  for( a = 0 ; a < 4 ; a++ )
  {
    maind.ressource[a] -= (int64_t)res[a];
    if( maind.ressource[a] >= 0 )
      continue;
    sprintf( cmdErrorBuffer, "You don't have %lld %s.", (long long)res[a], cmdRessourceName[a] );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  dbUserMainSet( id, &maind );

  for( a = 0 ; a < 4 ; a++ )
  {
    main2d.ressource[a] += res[a];
    newd[4+a] = res[a];
  }
  dbUserMainSet( destid, &main2d );

  newd[0] = ticks.number;
  newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[2] = CMD_NEWS_AID;
  newd[3] = id;
  cmdUserNewsAdd( destid, newd, CMD_NEWS_FLAGS_AID );

  return YES;
}


int cmdExecAidAccess( int id, int access )
{
  //int a, b;
  dbUserMainDef maind;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  maind.benefactors[0] = access;
  dbUserMainSet( id, &maind );
  return YES;
}


int cmdExecGetAid( int id, int destid, int fam, int64_t *res )
{
  int a, b;
  dbMainEmpireDef empired;
  dbUserMainDef maind, main2d;
  dbUserPtr user2;
  int64_t newd[DB_USER_NEWS_BASE];

cmdErrorString = 0;

  if( !( ticks.status ) )
  {
    cmdErrorString = "You can't receive aid before the beginning of the round.";
    return -3;
  }

  if( dbEmpireGetInfo( fam, &empired ) < 0 )
    return -3;
  b = 0;
  for( a = 0 ; a < empired.numplayers ; a++ )
  {
    if( empired.player[a] == id )
      b |= 1;
    if( empired.player[a] == destid )
      b |= 2;
  }
  if( b != 3 )
    return -3;
  b = 0;
  for( a = 0 ; a < 4 ; a++ )
  {
    b |= res[a];
    /*if( res[a] >= 1000000000 )
      return -3;*/
  }
  if( !( b ) )
  {
    cmdErrorString = "You can't receive an empty aid shipment!";
    return -3;
  }

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbUserMainRetrieve( destid, &main2d ) < 0 )
    return -3;
  if( !( user2 = dbUserLinkID( destid ) ) )
    return -3;

  /* Check access rights - maind is giver */
  if( maind.benefactors[0] == 3 ) {
    goto access;
  } else if( ( maind.benefactors[0] == 2 ) && ( bitflag( user2->flags, CMD_USER_FLAGS_LEADER ) || bitflag( user2->flags, CMD_USER_FLAGS_DEVMINISTER ) ) ) {
    goto access;
  } else if( ( maind.benefactors[0] == 1 ) && ( bitflag( user2->flags, CMD_USER_FLAGS_LEADER ) ) ) {
    goto access;
  }
  cmdErrorString = "You are not authorized to request aid from this faction.";
  return -3;

  access:

  for( a = 0 ; a < 4 ; a++ )
  {
    maind.ressource[a] -= res[a];
    if( maind.ressource[a] >= 0 )
      continue;
    sprintf( cmdErrorBuffer, "There isn't %lld %s available.", (long long)res[a], cmdRessourceName[a] );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  dbUserMainSet( id, &maind );

  for( a = 0 ; a < 4 ; a++ )
  {
    main2d.ressource[a] += (int64_t)res[a];
    newd[4+a] = res[a];
  }
  dbUserMainSet( destid, &main2d );

  newd[0] = ticks.number;
  newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[2] = CMD_NEWS_GETAID;
  newd[3] = destid;
  cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_AID );

  return YES;
}


int cmdExecChangeVote( int id, int vote )
{
  int a, b;
  dbMainEmpireDef empired;
  dbUserMainDef maind;

  cmdErrorString = 0;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbEmpireGetInfo( maind.empire, &empired ) < 0 )
    return -3;
  for( b = 0 ; ; b++ )
  {
    if( b == empired.numplayers )
      return -3;
    if( id == empired.player[b] )
      break;
  }
  if( vote == -1 )
    empired.vote[b] = -1;
  else
  {
    for( a = 0 ; ; a++ )
    {
      if( a == empired.numplayers )
        return -3;
      if( vote == empired.player[a] )
      {
        empired.vote[b] = a;
        break;
      }
    }
  }
  cmdEmpireLeader( &empired );
  if( dbEmpireSetInfo( maind.empire, &empired ) < 0 )
    return -3;
  return YES;
}



int cmdExecChangFamName( int fam, char *name )
{
  int a;
  dbMainEmpireDef empired;

  cmdErrorString = 0;
  if( dbEmpireGetInfo( fam, &empired ) < 0 )
    return -3;
  for( a = 0 ; ( a < USER_NAME_MAX-1 ) && ( name[a] ) ; a++ )
    empired.name[a] = name[a];
  empired.name[a] = 0;

  if( dbEmpireSetInfo( fam, &empired ) < 0 )
    return -3;
  return YES;
}


int cmdExecFamMemberFlags( int id, int fam, int flags ) {
	int a, b;
	dbUserPtr user;
	dbMainEmpireDef empired, empire2d;

cmdErrorString = 0;
if( ( dbEmpireGetInfo( fam, &empired ) < 0 ) || ( dbEmpireGetInfo( fam, &empire2d ) < 0 ) )
	return -3;
if( id == empired.politics[CMD_POLITICS_LEADER] )
	return -3;
for( a = 0 ; ; a++ ) {
	if( a == empired.numplayers )
		return -3;
	if( id == empired.player[a] )
		break;
}
if( !( user = dbUserLinkID( id ) ) )
	return -3;
if( flags > CMD_USER_FLAGS_NUMUSED )
	return -3;

if( flags ) {
	for( a = CMD_EMPIRE_POLITICS_START+1; a <= CMD_EMPIRE_POLITICS_END; a++ ) {
		if( ( a != flags ) && ( bitflag( user->flags, a) ) ) {
				empired.politics[a-CMD_EMPIRE_POLITICS_START] = -1;
				bitflag_remove( &user->flags, a );
			}
	}
	empired.politics[flags-CMD_EMPIRE_POLITICS_START] = id;
	bitflag_add( &user->flags, flags );
} else {
	for( a = CMD_EMPIRE_POLITICS_START+1; a <= CMD_EMPIRE_POLITICS_END; a++ ) {
		if( bitflag( user->flags, a ) ) {
			empired.politics[a-CMD_EMPIRE_POLITICS_START] = -1;
			bitflag_remove( &user->flags, a );
		}
	}
}
dbUserSave( id, user );
/*
 * I only like one of each "Minister" per Empire, so strip others if someone is changed.
 */
if( ( flags ) && ( flags != CMD_USER_FLAGS_INDEPENDENT ) ) {
	for( b = 0 ; b <= empired.numplayers ; b++ ) {
		if( empired.player[b] == id )
			continue;
		if( !( user = dbUserLinkID( empired.player[b] ) ) )
			continue;
		bitflag_remove( &user->flags, flags );
		dbUserSave( empired.player[b], user );
	}
}

memcpy( empire2d.politics, empired.politics, CMD_POLITICS_NUMUSED*sizeof(int) );
dbEmpireSetInfo( fam, &empire2d );

return YES;
}



int cmdExecSetFamPass( int fam, char *pass ) {
	int a;
	dbMainEmpireDef empired;




cmdErrorString = 0;
if( dbEmpireGetInfo( fam, &empired ) < 0 )
	return -3;

for( a = 0 ; a < USER_PASS_MAX-1 ; a++ ) {
	if( ( empired.password[a] == 10 ) || ( empired.password[a] == 13 ) )
		break;
	empired.password[a] = pass[a];
}
empired.password[a] = 0;

if( strlen(empired.password) )
  sprintf(empired.password, "%s", hashencrypt(empired.password) );

if( dbEmpireSetInfo( fam, &empired ) < 0 )
	return -3;


return YES;
}

int cmdExecGetFamPass( int fam, char *pass ) {
	dbMainEmpireDef empired;

cmdErrorString = 0;

if( dbEmpireGetInfo( fam, &empired ) < 0 )
	return -3;

strcpy(pass, empired.password);

return YES;
}




int cmdExecFindRelation( int fam, int famtarget, int *numallies, int flags )
{
  int a, b, c, d, num;
  int *rels;

  cmdErrorString = 0;
  if( fam >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( famtarget >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;

  if( ( num = dbEmpireRelsList( fam, &rels ) ) < 0 )
    return -3;


  num <<= 2;
  for( a = b = c = d = 0 ; a < num ; a += 4 )
  {
    if( ( rels[a+1] == CMD_RELATION_ALLY ) && !( rels[a+3] ) )
      c++;
    if( rels[a+2] != famtarget )
      continue;
    if( rels[a+1] == CMD_RELATION_WAR )
    {
      if( numallies )
        *numallies = c;
      if( ( flags ) && ( rels[a+3] ) )
        continue;
      free( rels );
      return CMD_RELATION_WAR;
    }
    else if( rels[a+1] == CMD_RELATION_ALLY ) {
      b += 2 + ( rels[a+3] & 1 );
    } else if( rels[a+1] == CMD_RELATION_NAP ) {
      d += 2 + ( rels[a+3] & 1 );
    }

  }

  free( rels );

  if( numallies )
    *numallies = c;

  if( !( flags ) && ( b == 2 ) )
    return CMD_RELATION_ALLY_OFFER;
  else if( b == 5 )
    return CMD_RELATION_ALLY;
  else if( !( flags ) && ( d == 2 ) )
    return CMD_RELATION_NAP_OFFER;
  else if( d == 5 )
    return CMD_RELATION_NAP;

  return -1;
}

int cmdExecAddRelation( int fam, int type, int famtarget )
{
  int a, rel[4], nPct;
  dbMainEmpireDef empired;
  dbMainEmpireDef empire2d;

  cmdErrorString = 0;
  if( fam >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( famtarget >= dbMapBInfoStatic[MAP_EMPIRES] )
    return -3;
  if( fam == famtarget )
    return -3;

  if( cmdExecFindRelation( fam, famtarget, &a, 1 ) >= 0 )
    return -3;

  if( type == CMD_RELATION_ALLY )
  {	

  	
    if( a >= 1 )
    {
      cmdErrorString = "You can't send more than 1 alliance offer!";

      return -3;
    }
    rel[0] = ticks.number;
    rel[1] = type;
    rel[2] = fam;
    rel[3] = 1;
    if( dbEmpireRelsAdd( famtarget, rel ) < 0 )
      return -3;
  }
  else if( type == CMD_RELATION_NAP ) {
    rel[0] = ticks.number;
    rel[1] = type;
    rel[2] = fam;
    rel[3] = 1;
    if( dbEmpireRelsAdd( famtarget, rel ) < 0 )
      return -3;
  }
  else if( type == CMD_RELATION_WAR )
  {
    if( dbEmpireGetInfo( fam, &empired ) < 0 )
      return -3;
    if( dbEmpireGetInfo( famtarget, &empire2d ) < 0 )
      return -3;
		
		//60% of empire size "in planets"
		nPct = 6;
    		
    if( nPct*empired.planets >= 10*empire2d.planets )
    {
      cmdErrorString = "You can't declare war on a empire less than 60% your empire size.";
      return -3;
    }
    rel[0] = ticks.number;
    rel[1] = type;
    rel[2] = fam;
    rel[3] = 1;
    if( dbEmpireRelsAdd( famtarget, rel ) < 0 )
      return -3;
  }
  else
    return -3;
  rel[2] = famtarget;
  rel[3] = 0;
  return dbEmpireRelsAdd( fam, rel );
}

int cmdExecDelRelation( int fam, int relid )
{
  int a, num, rel[4];
  int *rels;

  cmdErrorString = 0;
  if( dbEmpireRelsGet( fam, relid, rel ) < 0 )
    return -3;
  if( ( rel[0]+26 > ticks.number ) )
  {
    cmdErrorString = "A relation can't be canceled for 26 weeks after being declared.";
    return -3;
  }
  if( rel[3] & 1 )
    return -3;
  if( ( num = dbEmpireRelsList( rel[2], &rels ) ) < 0 )
    return -3;
  num <<= 2;
  for( a = 0 ; a < num ; a += 4 )
  {
    if( a == num )
    {
      free( rels );
      return -3;
    }
    if( ( rels[a+2] == fam ) && ( rels[a+1] == rel[1] ) && ( rels[a+3] & 1 ) )
      break;
  }
  dbEmpireRelsRemove( rel[2], a >> 2 );
  free( rels );
  return dbEmpireRelsRemove( fam, relid );
}


int cmdExecSetSectsize( int id, int size )
{
  dbUserMainDef maind;

  cmdErrorString = 0;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( size < 10 )
    return -3;
  if( size > 60 )
    return -3;
  maind.config_mapsize = size;
  if( dbUserMainSet( id, &maind ) < 0 )
    return -3;
  return YES;
}





int cmdExecSendFleetInfos( int id, int plnid, int *fr )
{
  int a, b, ret;
  float fa;
  dbUserMainDef maind, main2d;
  dbMainPlanetDef planetd;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
    return -2;
  b = cmdFindDistPortal( id, ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, &a, 0 );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send a fleet from!";
    return -3;
  }
  else if( b == -3 )
    return -3;

 // CODE for artefact that decreases/increases travel speed by n%
  
 /*  if ( maind.artefacts & ARTEFACT_8_BIT)
   fa = cmdRace[maind.raceid].speed * 0.75;
   else */
   fa = cmdRace[maind.raceid].speed;
 
  ret = (int)( (float)a / fa ) >> 8;
  if( !( fr ) )
    return ret;
  if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
  {
    cmdErrorString = "It seems no one explored this planet, yet.";
    return -3;
  }
  *fr = battleReadinessLoss( &maind, &main2d );
  
  return ret;
}





int cmdExecSendFleet( int id, int x, int y, int z, int order, int64_t *sendunit )
{
  int a, b;
  float fa;
  dbUserMainDef maind;
  dbMainSystemDef systemd;
  dbUserFleetDef fleetd, fleet2d;

  cmdErrorString = 0;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  fleetd.sysid = dbMapFindSystem( x, y );
  if( ( order == CMD_FLEET_ORDER_ATTACK ) || ( order == CMD_FLEET_ORDER_STATION ) )
  {
    if( ( fleetd.sysid < 0 ) || ( dbMapRetrieveSystem( fleetd.sysid, &systemd ) < 0 ) || ( z >= systemd.numplanets ) )
    {
      cmdErrorString = "This planet doesn't exist";
      return -3;
    }
    fleetd.destid = systemd.indexplanet + z;
  }
  else if( order == CMD_FLEET_ORDER_MOVE )
  {
    if( fleetd.sysid < 0 )
      fleetd.sysid = -1;
    if( !( dbMapFindValid( x, y ) ) )
    {
      cmdErrorString = "Coordinates aren't valid";
      return -3;
    }
    fleetd.destid = -1;
  }
  else
    return -3;

  b = cmdFindDistPortal( id, x, y, &a, &fleetd.source );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send this fleet from!";
    return -3;
  }
  else if( b == -3 )
    return -3;

  
 // CODE for artefact that decreases/increases travel speed by n%
  
 /*   if ( maind.artefacts & ARTEFACT_8_BIT)
    fa = cmdRace[maind.raceid].speed * 0.75;
    else */
    fa = cmdRace[maind.raceid].speed;
              
  
  fleetd.basetime = fleetd.time = (int)((int)( (float)a / fa ) >> 8);
	
	// units check
  if( sendunit[CMD_UNIT_EXPLORATION] )
    return -3;
  a = sendunit[CMD_UNIT_BOMBER] + sendunit[CMD_UNIT_FIGHTER] + sendunit[CMD_UNIT_TRANSPORT];
  if( sendunit[CMD_UNIT_CARRIER]*100 < a )
  {
    cmdErrorString = "You are not sending enough carriers, each carrier can hold 100 fighters, bombers or transports.";
    return -3;
  }
  a = sendunit[CMD_UNIT_SOLDIER] + sendunit[CMD_UNIT_DROID] + 4*sendunit[CMD_UNIT_GOLIATH];
  if( sendunit[CMD_UNIT_TRANSPORT]*100 < a )
  {
    cmdErrorString = "You are not sending enough transports, each transport can hold 100 soldiers or droids, or 25 goliaths.";
    return -3;
  }

  if( !( dbUserFleetRetrieve( id, 0, &fleet2d ) ) )
    return -3;
  for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
  {
    if( sendunit[a] > fleet2d.unit[a] )
    {
      sprintf( cmdErrorBuffer, "You don't have as many as %lld %s in your main fleet!", (long long)sendunit[a], cmdUnitName[a] );
      cmdErrorString = cmdErrorBuffer;
      return -3;
    }
    b += sendunit[a];
    fleet2d.unit[a] -= sendunit[a];
  }
  if( !( b ) )
  {
    cmdErrorString = "You must send some units to make a fleet.";
    return -3;
  }

  if( !( dbUserFleetSet( id, 0, &fleet2d ) ) )
    return -3;
  memcpy( fleetd.unit, sendunit, CMD_UNIT_NUMUSED*sizeof(int64_t) );
  fleetd.order = order;
  fleetd.destination = ( x << 8 ) + ( y << 20 ) + ( z );
  fleetd.flags = 0;
  if( ( a = dbUserFleetAdd( id, &fleetd ) ) < 0 )
    return -3;

  if( fleetd.basetime == 0 )
    cmdFleetAction( &fleetd, id, a, 0 );
  return fleetd.basetime;
}



int cmdExecSendAgents( int id, int x, int y, int z, int order, int64_t agents )
{
  int a, b;
  float fa;
  dbUserMainDef maind;
  dbUserFleetDef fleetd, fleet2d;
  dbMainSystemDef systemd;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  fleetd.sysid = dbMapFindSystem( x, y );
  if( ( order < CMD_FLEET_ORDER_FIRSTOP ) || ( order >= CMD_FLEET_ORDER_LASTOP ) )
    return -3;
  if( ( fleetd.sysid < 0 ) || ( dbMapRetrieveSystem( fleetd.sysid, &systemd ) < 0 ) || ( z >= systemd.numplanets ) )
  {
    cmdErrorString = "This planet doesn't exist";
    return -3;
  }
  fleetd.destid = systemd.indexplanet + z;

  b = cmdFindDistPortal( id, x, y, &a, &fleetd.source );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send this fleet from!";
    return -3;
  }
  else if( b == -3 )
    return -3;

 // CODE for artefact that decreases/increases travel speed by n%

 /* if ( maind.artefacts & ARTEFACT_8_BIT)
  fa = cmdRace[maind.raceid].speed * 0.75;
  else */
  fa = cmdRace[maind.raceid].speed;
            
  
  fleetd.basetime = fleetd.time = (int)( (float)a / fa ) >> 8;

  if( !( dbUserFleetRetrieve( id, 0, &fleet2d ) ) )
    return -3;
  if( agents > fleet2d.unit[CMD_UNIT_AGENT] )
  {
    sprintf( cmdErrorBuffer, "You don't have as many as %lld %s in your main fleet!", (long long)agents, cmdUnitName[CMD_UNIT_AGENT] );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  if( agents == 0 )
  {
    sprintf( cmdErrorBuffer, "You can't send an empty fleet!" );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  fleet2d.unit[CMD_UNIT_AGENT] -= agents;

  if( !( dbUserFleetSet( id, 0, &fleet2d ) ) )
    return -3;
  memset( fleetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
  fleetd.unit[CMD_UNIT_AGENT] = agents;
  fleetd.order = order;
  fleetd.destination = ( x << 8 ) + ( y << 20 ) + ( z );
  fleetd.flags = 0;
  if( ( a = dbUserFleetAdd( id, &fleetd ) ) < 0 )
    return -3;

  if( fleetd.basetime == 0 )
    cmdFleetAction( &fleetd, id, a, 0 );
  return fleetd.basetime;
}


int cmdExecSendGhosts( int id, int x, int y, int z, int order, int64_t ghosts )
{
  int a, b;
  float fa;
  dbUserMainDef maind;
  dbMainSystemDef systemd;
  dbUserFleetDef fleetd, fleet2d;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  fleetd.sysid = dbMapFindSystem( x, y );
  if( ( order < CMD_FLEET_ORDER_FIRSTINCANT ) || ( order >= CMD_FLEET_ORDER_LASTINCANT ) )
    return -3;

  if( z != -1 )
  {
    if( ( fleetd.sysid < 0 ) || ( dbMapRetrieveSystem( fleetd.sysid, &systemd ) < 0 ) || ( z >= systemd.numplanets ) )
    {
      cmdErrorString = "This planet doesn't exist";
      return -3;
    }
    fleetd.destid = systemd.indexplanet + z;
  }
  else
  {
    if( ( x >= dbMapBInfoStatic[MAP_SIZEX] ) || ( y >= dbMapBInfoStatic[MAP_SIZEY] ) )
    {
      cmdErrorString = "These coordinates would lead your ships outside the galaxy!";
      return -3;
    }
    fleetd.destid = -1;
    z = 0;
  }

  b = cmdFindDistPortal( id, x, y, &a, &fleetd.source );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send this fleet from!";
    return -3;
  }
  else if( b == -3 )
    return -3;

 // CODE for artefact that decreases/increases travel speed by n%

 /* if ( maind.artefacts & ARTEFACT_8_BIT)
  fa = cmdRace[maind.raceid].speed * 0.75;
  else  */
  fa = cmdRace[maind.raceid].speed;
            
    
  fleetd.basetime = fleetd.time = (int)( (float)a / fa ) >> 8;
  if( !( dbUserFleetRetrieve( id, 0, &fleet2d ) ) )
    return -3;
  if( ghosts > fleet2d.unit[CMD_UNIT_GHOST] )
  {
    sprintf( cmdErrorBuffer, "You don't have as many as %lld %s in your main fleet!", (long long)ghosts, cmdUnitName[CMD_UNIT_GHOST] );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  if( ghosts == 0 )
  {
    sprintf( cmdErrorBuffer, "You can't send an empty fleet!" );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  fleet2d.unit[CMD_UNIT_GHOST] -= ghosts;

  if( !( dbUserFleetSet( id, 0, &fleet2d ) ) )
    return -3;
  memset( fleetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
  fleetd.unit[CMD_UNIT_GHOST] = ghosts;
  fleetd.order = order;
  fleetd.destination = ( x << 8 ) + ( y << 20 ) + ( z );
  fleetd.flags = 0;
  if( ( a = dbUserFleetAdd( id, &fleetd ) ) < 0 )
    return -3;

  if( fleetd.basetime == 0 )
    cmdFleetAction( &fleetd, id, a, 0 );
  return fleetd.basetime;
}




int cmdExecExploreShips( int id )
{
  int a, num, total;
  dbUserFleetPtr fleetd;
  if( ( num = dbUserFleetList( id, &fleetd ) ) <= 0 )
    return 0;
  total = 0;
  for( a = 1 ; a < num ; a++ )
  {
    if( fleetd[a].unit[CMD_UNIT_EXPLORATION] )
      total++;
  }
  free( fleetd );
  return total;
}


int cmdExecExploreInfo( int id, int plnid, int *ibuffer )
{
  int a, b;
  dbMainPlanetDef planetd;
  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
    return -2;
  if( planetd.owner != -1 )
  {
    cmdErrorString = "This planet is not available for exploration!";
    return -3;
  }
  b = cmdFindDistPortal( id, ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, &a, 0 );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send an exploration ship from!";
    return -3;
  }
  else if( b == -3 )
    return -3;
  ibuffer[0] = ( ( a / cmdUnitStats[CMD_UNIT_EXPLORATION][CMD_UNIT_STATS_SPEED] ) >> 8 ) + 1;
  return YES;
}


int cmdExploreCost( int id, dbUserMainPtr mainp ) {
	int cost;
	
cost = (( ( ( mainp->planets + cmdExecExploreShips( id ) ) << 16 ) >> 2 ) + 10*65536 );

return cost;
}

int cmdExecExplore( int id, int plnid, int *ibuffer )
{
  int a, b;
  dbUserMainDef maind;
  dbUserFleetDef fleetd, fleet2d;
  dbMainPlanetDef planetd;

  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
    return -2;
  if( planetd.owner != -1 )
  {
    cmdErrorString = "This planet is not available for exploration!";
    return -3;
  }

  if( !( dbUserFleetRetrieve( id, 0, &fleet2d ) ) )
    return -3;
  if( fleet2d.unit[CMD_UNIT_EXPLORATION] < 1 )
  {
    cmdErrorString = "You don't have any exploration ship to send away!";
    return -3;
  }

  memset( &fleetd, 0, sizeof(dbUserFleetDef) );
  b = cmdFindDistPortal( id, ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, &a, &fleetd.source );
  if( b == -2 )
  {
    cmdErrorString = "You don't have any portal to send an exploration ship from!";
    return -3;
  }
  else if( b == -3 )
    return -3;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( maind.readiness[0] < 0 )
  {
    cmdErrorString = "Your people aren't prepared to send another exploration ship!";
    return -3;
  }

	maind.readiness[0] -= cmdExploreCost( id, &maind );
	
	ibuffer[0] = ( ( a / cmdUnitStats[CMD_UNIT_EXPLORATION][CMD_UNIT_STATS_SPEED] ) >> 8 ) + 1;
	
  fleetd.time = ibuffer[0];
  fleetd.basetime = ibuffer[0];
  fleetd.unit[CMD_UNIT_EXPLORATION] = 1;
  fleetd.order = CMD_FLEET_ORDER_EXPLORE;
  fleetd.destination = planetd.position;
  fleetd.destid = plnid;
  fleetd.sysid = planetd.system;
  fleetd.flags = 0;
  if( !( dbUserFleetAdd( id, &fleetd ) ) )
    return -3;

  fleet2d.unit[CMD_UNIT_EXPLORATION]--;
  if( !( dbUserFleetSet( id, 0, &fleet2d ) ) )
    return -3;

  return dbUserMainSet( id, &maind );
}








int cmdExecChangeFleet( int id, int x, int y, int z, int order, int fltid )
{
  int a, b;
  float fa;
  dbUserMainDef maind;
  dbUserFleetDef fleetd;
  dbMainSystemDef systemd;

  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbUserFleetRetrieve( id, fltid, &fleetd ) < 0 )
  {
    cmdErrorString = "This fleet doesn't exist!";
    return -3;
  }
  cmdFleetGetPosition( &fleetd, &a, &b );
  fleetd.source = ( b << 20 ) + ( a << 8 );
  fleetd.order = order;

 // CODE for artefact that decreases/increases travel speed by n%

 /* if ( maind.artefacts & ARTEFACT_8_BIT)
  fa = cmdRace[maind.raceid].speed * 0.75;
  else */
  fa = cmdRace[maind.raceid].speed;
            


  if( order == CMD_FLEET_ORDER_RECALL )
  {
    fleetd.sysid = 0;
    fleetd.destid = cmdFindDistPortal( id, a, b, &a, &fleetd.destination );
    if( fleetd.destid == -2 )
    {
      cmdErrorString = "You don't have any portal to recall this fleet to!";
      return -3;
    }
    else if( fleetd.destid == -3 )
      return -3;

    fleetd.basetime = fleetd.time = (int)( (float)a / fa ) >> 8;

    
   	if( !( dbUserFleetSet( id, fltid, &fleetd ) ) )
      return -3;
    if( fleetd.basetime == 0 )
    {
      cmdFleetAction( &fleetd, id, fltid, 0 );
      cmdErrorString = "Fleet recalled to main fleet";
      return -1;
    }
    return fleetd.basetime;
  }
  else if(( order == CMD_FLEET_ORDER_MOVE )||( order == CMD_FLEET_ORDER_MERGE ))
  {
    cmdFindDistCurrent( a, b, x, y, &a );
if(order == CMD_FLEET_ORDER_MOVE)
	fleetd.time = 0;
    else
    	fleetd.time = 1;
    if( a )
      fleetd.time = ( (int)( (float)a / fa ) >> 8 ) + ( fleetd.flags & CMD_FLEET_FLAGS_MOVED );
    if(!fleetd.time)
    	fleetd.time++;
    fleetd.basetime = fleetd.time;
    fleetd.sysid = dbMapFindSystem( x, y );
    if( fleetd.sysid < 0 )
      fleetd.sysid = -1;
    if( !( dbMapFindValid( x, y ) ) )
    {
      cmdErrorString = "Coordinates aren't valid";
      return -3;
    }
    fleetd.destid = -1;
    fleetd.destination = ( x << 8 ) + ( y << 20 );
    fleetd.flags |= CMD_FLEET_FLAGS_MOVED;
    if( !( dbUserFleetSet( id, fltid, &fleetd ) ) )
      return -3;
    if( fleetd.basetime == 0 )
    {
      cmdFleetAction( &fleetd, id, fltid, 0 );
      return YES;
    }
    return fleetd.basetime;
  }
  else if( order == CMD_FLEET_ORDER_EXPLORE )
  {
    if( !( fleetd.unit[CMD_UNIT_EXPLORATION] ) )
      return -3;
    cmdFindDistCurrent( a, b, x, y, &a );
/*
    fleetd.time = ( (int)( (float)a / cmdUnitStats[CMD_UNIT_EXPLORATION][CMD_UNIT_STATS_SPEED] ) >> 8 ) + 1;
*/
    fleetd.time = ( (int)( (float)a / fa ) >> 8 ) + 1;

    fleetd.basetime = fleetd.time;
    fleetd.sysid = dbMapFindSystem( x, y );
    if( ( fleetd.sysid < 0 ) || ( dbMapRetrieveSystem( fleetd.sysid, &systemd ) < 0 ) || ( z >= systemd.numplanets ) )

    {
      cmdErrorString = "This planet doesn't exist";
      return -3;
    }
    fleetd.destid = systemd.indexplanet + z;
    fleetd.destination = ( x << 8 ) + ( y << 20 ) + ( z );
    fleetd.flags |= CMD_FLEET_FLAGS_MOVED;
    if( !( dbUserFleetSet( id, fltid, &fleetd ) ) )
      return -3;
    if( fleetd.basetime == 0 )
    {
      cmdFleetAction( &fleetd, id, fltid, 0 );
      cmdErrorString = "Planet explored";
      return YES;
    }
    return fleetd.basetime;
  }
  else if( ( order == CMD_FLEET_ORDER_ATTACK ) || ( order == CMD_FLEET_ORDER_STATION ) )
  {
if( fleetd.unit[CMD_UNIT_EXPLORATION] )
	return -3;
	cmdFindDistCurrent( a, b, x, y, &a );
	fleetd.time = 0;
	if( a )
		fleetd.time = ( (int)( (float)a / fa ) >> 8 ) + 1;
		fleetd.basetime = fleetd.time;
		fleetd.sysid = dbMapFindSystem( x, y );
		if( ( fleetd.sysid < 0 ) || ( dbMapRetrieveSystem( fleetd.sysid, &systemd ) < 0 ) || ( z >= systemd.numplanets ) )
			{
				cmdErrorString = "This planet doesn't exist";
				return -3;
			}
    fleetd.destid = systemd.indexplanet + z;
    fleetd.destination = ( x << 8 ) + ( y << 20 ) + ( z );
    fleetd.flags |= CMD_FLEET_FLAGS_MOVED;
    if( !( dbUserFleetSet( id, fltid, &fleetd ) ) )
      return -3;
    if( fleetd.basetime == 0 )
    {
      cmdFleetAction( &fleetd, id, fltid, 0 );
      return YES;
    }
    return fleetd.basetime;
  }
  else
    return -3;
}






int cmdExecOfferPlanet( int id, int destid, int plnid )
{
  int64_t newd[DB_USER_NEWS_BASE];
  dbMainPlanetDef planetd;

  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
    return -3;
  if( planetd.flags & CMD_PLANET_FLAGS_HOME )
    return -3;
  if( id == destid )
    return -3;

  if( destid >= 0 )
  {
    newd[0] = ticks.number;
    newd[1] = CMD_NEWS_FLAGS_NEW;
    newd[2] = CMD_NEWS_PLANET_OFFER;
    newd[3] = id;
    newd[4] = plnid;
    newd[5] = planetd.position;
    cmdUserNewsAdd( destid, newd, CMD_NEWS_FLAGS_FLEET );
  }

  planetd.surrender = destid;
  dbMapSetPlanet( plnid, &planetd );

  return YES;
}


int cmdExecTakePlanet( int id, int plnid )
{
  int a, b;
  int64_t newd[DB_USER_NEWS_BASE];
  dbMainPlanetDef planetd;
  dbUserMainDef maind, main2d ;
  dbMainSystemDef systemd;
  dbBuildPtr buildd;

  cmdErrorString = 0;
  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
    return -3;
  if( planetd.flags & CMD_PLANET_FLAGS_HOME )
    return -3;
  if( planetd.owner == id )
    return -3;
  if( planetd.surrender != id )
    return -3;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
    return -3;
  if( maind.empire != main2d.empire )
    return -1;

  if( maind.readiness[0] < -65536*100 )
  {
    cmdErrorString = "Your forces are too tired to take over this planet at the moment.";
    return -3;
  }
  a = (int)battleReadinessLoss( &maind, &main2d );
  if( a >= 20*65536 )
    a = 20*65536;
  maind.readiness[0] -= a;
  if( dbUserMainSet( id, &maind ) < 0 )
    return -3;

  newd[0] = ticks.number;
  newd[1] = 0;
  newd[2] = CMD_NEWS_PLANET_TAKEN;
  newd[3] = planetd.owner;
  newd[4] = plnid;
  newd[5] = planetd.position;
  cmdUserNewsAdd( id, newd, 0 );

  newd[0] = ticks.number;
  newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[2] = CMD_NEWS_PLANET_GIVEN;
  newd[3] = id;
  newd[4] = plnid;
  newd[5] = planetd.position;
  cmdUserNewsAdd( planetd.owner, newd, CMD_NEWS_FLAGS_FLEET );

  if( !( dbUserPlanetRemove( planetd.owner, plnid ) ) )
    return -3;

  for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
    planetd.building[a] = (int)floor( 0.9 * (float)planetd.building[a] );

  if( planetd.construction )
  {
    b = dbUserBuildList( planetd.owner, &buildd );
    for( a = b-1 ; a >= 0 ; a-- )
    {
      if( buildd[a].plnid == plnid )
        dbUserBuildRemove( planetd.owner, a );
    }
    if( buildd )
      free( buildd );
   }
  dbMapRetrieveSystem( planetd.system, &systemd );
  planetd.flags = ( bitflag( systemd.flags, MAP_SYSTEM_FLAG_MEGA ) ? CMD_PLANET_FLAGS_MEGA : 0 );
  planetd.owner = id;
  planetd.construction = 0;
  planetd.surrender = -1;
  planetd.flags &= 0xFFFFFFFF - ( CMD_PLANET_FLAGS_PORTAL | CMD_PLANET_FLAGS_PORTAL_BUILD );
  memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
  dbMapSetPlanet( plnid, &planetd );
  dbUserPlanetAdd( id, plnid, planetd.system, planetd.position, planetd.flags );

  return YES;
}



void *cmdEndofRound( ) {
	int a;
	char fname[PATH_MAX];
	dbUserPtr user;
	ConfigArrayPtr setting;
	dbMainEmpireDef empired;
	struct tm variable;

info( "Iniating Map Reset." );
ticks.status = false;
ticks.locked = true;

for( user = dbUserList ; user ; user = user->next ) {
	cmdExecUserDeactivate( user->id, CMD_USER_FLAGS_NEWROUND );
}

dbMarketReset();

sysconfig.regen = true;

while( sysconfig.shutdown == false ) {
	nanosleep((struct timespec[]){{0, ( 500000000 / 4 ) }}, NULL);
}
info( "Now Purging old Empires..." );

setting = GetSetting( "Directory" );
for( a = 0; a < dbMapBInfoStatic[MAP_EMPIRES]; a++ ) {
	if( ( dbEmpireGetInfo( a, &empired ) == YES ) && ( empired.picture > 0 ) ) {
		snprintf(fname, PATH_MAX,"%s/uploads/empire%d/pic%d", setting->string_value, a, empired.picture );
		unlink(fname);
	}
	dbForumRemoveForum( true, a );
}

snprintf( fname, sizeof(fname), "%s/data/forums", setting->string_value );
unlink( fname );

time(&now);

variable = *localtime(&now);
variable.tm_mday++;
sysconfig.start = variable;
ticks.round++;
ticks.number = 0;

if( spawn_map() == NO ) {
	critical( "Map Creation Failed" );
	return NO;
}

if( dbInit() == NO ) {
	critical( "Server Database Initation Failed, now exiting..." );
	return NO;
}

info( "Regeneration Completed, now reactivating WWW Servers." );

ticks.locked = false;
sysconfig.regen = false;

pthread_exit( &sysconfig.regen_thread );
}



