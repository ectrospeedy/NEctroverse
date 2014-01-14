int64_t cmdTickProduction[CMD_BLDG_NUMUSED];

void cmdTickGenRanks() {
	int a, b, c, d, first, num, artmax, wa, wnum;
	FILE *file, *filep;
	ConfigArrayPtr settings[2];
	dbUserMainDef maind;
	dbMainEmpirePtr empirep;
	dbMainEmpireDef empired;
	dbUserMainPtr mainp;
	dbUserPtr user;
	int *stats, *rels;
	int artefacts[ARTEFACT_NUMUSED], artsnum;
	char COREDIR[PATH_MAX];



  artmax = 0;
  settings[0] = GetSetting( "Directory" );
  sprintf( COREDIR, "%s/rankings/round%dfamranks.txt", settings[0]->string_value, ticks.round );
  memset( artefacts, 0, ARTEFACT_NUMUSED*sizeof(int) );
  if( !( file = fopen( COREDIR, "wb" ) ) )
    return;
  sprintf( COREDIR, "%s/rankings/round%dfamranksplain.txt", settings[0]->string_value, ticks.round);
  if( !( filep =fopen( COREDIR, "wb" ) ) )
  {
    fclose( file );
    return;
  }
  if( !( stats = malloc( 6*dbMapBInfoStatic[MAP_EMPIRES]*sizeof(int) + sizeof(dbMainEmpireDef)*dbMapBInfoStatic[MAP_EMPIRES] ) ) )
  {
    fclose( file );
    return;
  }
  empirep = (void *)&stats[6*dbMapBInfoStatic[MAP_EMPIRES]];
  memset( stats, 0, 6*dbMapBInfoStatic[MAP_EMPIRES]*sizeof(int) );
settings[0] = GetSetting( "Admin Empire Number" );
settings[1] = GetSetting( "Admin Empire Ommit" );
  for( b = c = num = 0 ; b < dbMapBInfoStatic[MAP_EMPIRES] ; b++ )
  {
	if( (b == (int)settings[1]->num_value) && ( (int)settings[1]->num_value ) )
		continue;
    if( dbMapRetrieveEmpire( b, &empirep[b] ) < 0 )
      continue;
    if( !( empirep[b].numplayers ) )
      continue;

settings[0] = GetSetting( "End Wars" );
if( (int)settings[0]->num_value > 0 ) {
	if( ( wnum = dbEmpireRelsList( b, &rels ) ) >= 0 ) {
		wnum <<= 2;
		for( wa = 0 ; wa < wnum ; wa += 4 ) {
			if( rels[wa+1] == CMD_RELATION_WAR ) {
				if( (rels[wa] + (int)settings[0]->num_value) <= ticks.number ) {
					cmdExecDelRelation( b, wa / 4 );
				}
			}
		}
		free( rels );
	} else {
		error( "Getting relations list for Empire:%d", b );
	}
}

    stats[c+0] = b;
// calc NW, planets and empire artefacts
    for( a = 0 ; a < empirep[b].numplayers ; a++ )
    {
      if( dbUserMainRetrieve( empirep[b].player[a], &maind ) < 0 )
        continue;
      stats[c+1] += maind.planets;
      stats[c+2]++;
      stats[c+3] += (int)maind.networth;
    }
    empirep[b].planets = stats[c+1];
    empirep[b].networth = stats[c+3];
    dbMapSetEmpire( b, &empirep[b] );
    num++;

// set artefacts for empire members
    for( a = 0 ; a < empirep[b].numplayers ; a++ )
    {
      if( dbUserMainRetrieve( empirep[b].player[a], &maind ) < 0 )
        continue;
      maind.artefacts = empirep[b].artefacts;
      dbUserMainSet( empirep[b].player[a], &maind );
    }

    c += 6;
  }

// empire rankings
  first = 0;
  stats[4] = -1;
  stats[5] = -1;
  for( b = 1, c = 6 ; b < num ; b++, c += 6 )
  {
    if( ( stats[c+1] > stats[first+1] ) || ( ( stats[c+1] == stats[first+1] ) && ( stats[c+3] > stats[first+3] ) ) )
    {
      stats[first+4] = c;
      stats[c+5] = first;
      stats[c+4] = -1;
      first = c;
      continue;
    }
    a = stats[first+5];
    d = first;
    for( ; ; )
    {
      if( a == -1 )
      {
        stats[d+5] = c;
        stats[c+4] = d;
        stats[c+5] = -1;
        break;
      }
      if( ( stats[c+1] > stats[a+1] ) || ( ( stats[c+1] == stats[a+1] ) && ( stats[c+3] > stats[a+3] ) ) )
      {
        stats[c+4] = d;
        stats[c+5] = a;
        stats[d+5] = c;
        stats[a+4] = c;
        break;
      }
      d = a;
      a = stats[a+5];
    }
  }
  if( !( num ) )
    first = -1;
  fprintf( file, "<table cellspacing=\"4\"><tr><td align=\"right\">Rank</td><td>Name</td><td>Planets</td><td>Players</td><td>Networth</td></tr>" );
  for( a = first, b = 1 ; a != -1 ; b++ )
  {
    fprintf( file, "<tr><td align=\"right\" nowrap>" );



/*
    for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
    {
      if( empirep[stats[a+0]].artefacts & d )
        fprintf( file, "<img src=\"%s\">", artefactImage[c] );
    }
*/
    artsnum = 0;
    for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
    {
      if( empirep[stats[a+0]].artefacts & d )
      {
        artsnum++;
        artefacts[c] = 1;
      }
    }

if( artsnum > artmax )
	artmax = artsnum;

if( artsnum == ARTEFACT_NUMUSED ) {
        if ( dbMapBInfoStatic[MAP_ARTITIMER] == -1 ) {
	        settings[0] = GetSetting( "Auto Victory" );
	        if( (int)settings[0]->num_value > 0 ) {
        	        dbMapBInfoStatic[MAP_ARTITIMER] = ticks.number + (int)settings[0]->num_value;
        	        dbMapBInfoStatic[MAP_TIMEMPIRE] = stats[a+0];
	                dbMapSetMain( dbMapBInfoStatic );
                }
        } else if ( ( dbMapBInfoStatic[MAP_TIMEMPIRE] == stats[a+0] ) && ( dbMapBInfoStatic[MAP_ARTITIMER] <= ticks.number ) ) {
                ticks.status = 0;
                ticks.locked = 1;
        }
} else if( dbMapBInfoStatic[MAP_TIMEMPIRE] == stats[a+0] ) {
        dbMapBInfoStatic[MAP_ARTITIMER] = -1;
        dbMapBInfoStatic[MAP_TIMEMPIRE] = -1;
        dbMapSetMain( dbMapBInfoStatic );
}

    if( ( 3*artsnum >= ARTEFACT_NUMUSED ) || ( (3*dbArtefactMax)/2 >= ARTEFACT_NUMUSED ) )
    {
      for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
      {
        if( empirep[stats[a+0]].artefacts & d )
          fprintf( file, "<img src=\"files?type=image&name=artifacts/%s\">", artefactImage[c] );
      }
    }

	fprintf( file, " %d</td><td><a href=\"empire?id=%d\">", b, stats[a+0] );
  	if( empirep[stats[a+0]].name[0] )
      fprintf( file, "%s #%d", empirep[stats[a+0]].name, stats[a+0] );
    else
      fprintf( file, "Empire #%d", stats[a+0] );
    fprintf( file, "</a></td><td align=\"center\">%d</td><td align=\"center\">%d</td><td align=\"center\">%d</td></tr>\n", stats[a+1], stats[a+2], stats[a+3] );

    /* Plain Text */
    fprintf( filep, "%d:%d:%d:%d:%d:", b, stats[a+0], stats[a+1], stats[a+2], stats[a+3] );
    if( empirep[stats[a+0]].name[0] )
      fprintf( filep, "%s #%d", empirep[stats[a+0]].name, stats[a+0] );
    else
      fprintf( filep, "Empire #%d", stats[a+0] );
    fprintf( filep, "\n" );

    // moo
    empirep[stats[a+0]].rank = b;
    dbMapSetEmpire( stats[a+0], &empirep[stats[a+0]] );




    a = stats[a+5];
  }
  fprintf( file, "</table>" );

  fprintf( file, "<br>" );
  artsnum = 0;
for( c = 0 ; c < ARTEFACT_NUMUSED ; c++ )
    artsnum |= artefacts[c];



if( artsnum ) {
	if( !(dbMapBInfoStatic[MAP_ARTITIMER] == -1 ) ) {
		if( dbMapRetrieveEmpire( dbMapBInfoStatic[MAP_TIMEMPIRE], &empired ) < 0 )
		      return;
		if( empired.name[0] ) {
			if( (dbMapBInfoStatic[MAP_ARTITIMER] - ticks.number) <= 0 )
				fprintf( file, "<br><br><b>All Artefacts held by: %s</b><br>", empired.name );
			else
				fprintf( file, "<br><br><b>All Artefacts held by: %s<br>Round will end in %d weeks!</b><br>", empired.name, dbMapBInfoStatic[MAP_ARTITIMER] - ticks.number );
		} else {
			if( (dbMapBInfoStatic[MAP_ARTITIMER] - ticks.number) <= 0 )
				fprintf( file, "<br><br><b>All Artefacts held by: Empire #%d</b><br>", dbMapBInfoStatic[MAP_TIMEMPIRE] );
			else
				fprintf( file, "<br><br><b>All Artefacts held by: Empire #%d<br>Round will end in %d weeks!</b><br>", dbMapBInfoStatic[MAP_TIMEMPIRE], dbMapBInfoStatic[MAP_ARTITIMER] - ticks.number );
		}
	} else {
		fprintf( file, "<br><br><b>Artefacts found</b><br>" );
	}
	fprintf( file, "<table><tr><td>" );
	for( c = 0 ; c < ARTEFACT_NUMUSED ; c++ ) {
		if( artefacts[c] )
			fprintf( file, "<img src=\"files?type=image&name=artifacts/%s\"> %s<br>", artefactImage[c], artefactDescription[c] );
	}
	fprintf( file, "</td></tr></table><br>" );
}
dbArtefactMax = artmax;

  free( stats );
  fclose( file );
  fclose( filep );


// player rankings
  settings[0] = GetSetting( "Directory" );
  sprintf( COREDIR, "%s/rankings/round%dranks.txt", settings[0]->string_value, ticks.round );
  if( !( file = fopen( COREDIR, "wb" ) ) )
    return;
  sprintf( COREDIR, "%s/rankings/round%dranksplain.txt", settings[0]->string_value, ticks.round );
  if( !( filep = fopen( COREDIR, "wb" ) ) )
  {
    fclose( file );
    return;
  }
  for( user = dbUserList, num = 0 ; user ; user = user->next )
    num++;
  if( !( num ) || !( stats = malloc( 6*num*sizeof(int) + sizeof(dbUserMainDef)*num ) ) )
  {
    fclose( file );
    return;
  }
  mainp = (void *)&stats[6*num];
  memset( stats, 0, 6*num*sizeof(int) );
settings[0] = GetSetting( "Admin Empire Number" );
settings[1] = GetSetting( "Admin Empire Ommit" );
  for( b = c = 0, user = dbUserList ; user ; user = user->next )
  {
    if( !( bitflag( user->flags, CMD_USER_FLAGS_ACTIVATED ) ) )
	{
	  //printf("user %d not activated\n", user->id );
      continue;
	}
    if( dbUserMainRetrieve( user->id, &mainp[b] ) < 0 ) {
	error( "Error retreiving maind id: %d", user->id );
      continue;
	}
	if( (mainp[b].empire == (int)settings[0]->num_value) && ((int)settings[1]->num_value) )
		continue;
    stats[c+0] = user->id;
    stats[c+1] = mainp[b].planets;
    stats[c+2] = mainp[b].networth;
    stats[c+3] = b;
    b++;
    c += 6;
  }
  num = b;

  first = 0;
  stats[4] = -1;
  stats[5] = -1;
  for( b = 1, c = 6 ; b < num ; b++, c += 6 )
  {
    if( ( stats[c+1] > stats[first+1] ) || ( ( stats[c+1] == stats[first+1] ) && ( stats[c+2] > stats[first+2] ) ) )
    {
      stats[first+4] = c;
      stats[c+5] = first;
      stats[c+4] = -1;
      first = c;
      continue;
    }
    a = stats[first+5];
    d = first;
    for( ; ; )
    {
      if( a == -1 )
      {
        stats[d+5] = c;
        stats[c+4] = d;
        stats[c+5] = -1;
        break;
      }
      if( ( stats[c+1] > stats[a+1] ) || ( ( stats[c+1] == stats[a+1] ) && ( stats[c+2] > stats[a+2] ) ) )
      {
        stats[c+4] = d;
        stats[c+5] = a;
        stats[d+5] = c;
        stats[a+4] = c;
        break;
      }
      d = a;
      a = stats[a+5];
    }
  }
  if( !( num ) )
    first = -1;
  fprintf( file, "<table cellspacing=\"4\"><tr><td>Rank</td><td>Faction</td><td>Empire</td><td>Planets</td><td>Networth</td></tr>" );
  for( a = first, b = 1 ; a != -1 ; b++ )
  {
    fprintf( file, "<tr><td align=\"right\">%d</td><td><a href=\"player?id=%d\">%s</a></td><td><a href=\"empire?id=%d\">empire #%d</a></td><td align=\"center\">%d</td><td align=\"center\">%lld</td></tr>", b, stats[a+0], mainp[stats[a+3]].faction, mainp[stats[a+3]].empire, mainp[stats[a+3]].empire, mainp[stats[a+3]].planets, (long long)mainp[stats[a+3]].networth );

    fprintf( filep, "%d:%d:%d:%d:%lld:%s\n", b, stats[a+0], mainp[stats[a+3]].empire, mainp[stats[a+3]].planets, (long long)mainp[stats[a+3]].networth, mainp[stats[a+3]].faction );



    // moo
    mainp[stats[a+3]].rank = b;
    dbUserMainSet( stats[a+0], &mainp[stats[a+3]] );



    a = stats[a+5];
  }
  fprintf( file, "</table>" );

  free( stats );
  fclose( file );
  fclose( filep );



  return;
}





void cmdTickInit(){

(void)pthread_mutex_lock( &mutex );

RANDOMIZE_SEED;

return;
}


void cmdTickEnd() {

cmdTickGenRanks();

savetickconfig();

(void)pthread_mutex_unlock( &mutex );

#if IRCBOT_SUPPORT
	ConfigArrayPtr setting;
	char buffer[2][512];
setting = GetSetting( "IRC Enable" );
if( setting->num_value ) {
	setting = GetSetting( "IRC Announce" );
	if( ( setting->num_value ) && ( ticks.status ) ) {
		snprintf( buffer[0], sizeof(buffer), "Game has Ticked -- Week %d, Year %d (Tick #%d)", ticks.number % 52, ticks.number / 52, ticks.number );
		setting = GetSetting( "IRC Channel" );
		snprintf( buffer[1], 512, "#%s", setting->string_value );
		irc_cmd_notice( sysconfig.irc_session, buffer[1], buffer[0] );
	}
}
#endif

dbRegisteredInfo[DB_TOTALS_USERS_REGISTERED] = ticks.uregist;
dbRegisteredInfo[DB_TOTALS_USERS_ACTIVATED] = ticks.uactive;
dbRegisteredInfo[DB_TOTALS_USERS_ONLINE] = ticks.uonline;

return;
}




int cmdTickPlanets( int usrid, dbUserMainPtr mainp ) {
	int a, b, num, nump, nInfection;
	int *buffer;
	int *portals;
	int64_t population;
	dbMainPlanetDef planetd;
	dbMainEmpireDef empired;
	dbUserFleetPtr fleetd;
	dbUserSpecOpPtr specopd;

ticks.debug_pass = 0 + 10000;


memset( mainp->totalbuilding, 0, (CMD_BLDG_NUMUSED+1)*sizeof(int64_t) );
memset( mainp->totalunit, 0, CMD_UNIT_NUMUSED*sizeof(int64_t) );
memset( cmdTickProduction, 0, CMD_BLDG_NUMUSED*sizeof(int64_t) );


nInfection = 0;
if( ( b = dbUserSpecOpList( usrid, &specopd ) ) >= 0 ) {
	for(a = 0; a < b; a++) {
		if (specopd[a].type == (CMD_OPER_BIOINFECTION|0x10000)) {
			nInfection++;
		}
  	}
  	if( specopd )
		free( specopd );
}

if( ( num = dbUserPlanetListIndices( usrid, &buffer ) ) < 0 )
	return 0;

portals = 0;
nump = 0;

if( ( nump = dbUserPortalsListCoords( usrid, &portals ) ) < 0 ) {
	free( buffer );
	return 0;
}

ticks.debug_pass = 1 + 10000;
population = 0;

for( a = 0 ; a < num ; a++ ) {
	dbMapRetrievePlanet( buffer[a], &planetd );
	planetd.maxpopulation = (float)( ( planetd.size * CMD_POPULATION_SIZE_FACTOR ) + ( planetd.building[CMD_BUILDING_CITIES] * CMD_POPULATION_CITIES ) );

		//ARTI CODE Super Stacker
	/*	if(mainp->artefacts & ARTEFACT_*_BIT)
			planetd.maxpopulation = (float)( ( planetd.size * CMD_POPULATION_SIZE_FACTOR ) + ( planetd.building[CMD_BUILDING_CITIES] * (CMD_POPULATION_CITIES+1000) ) );
	*/
	ticks.debug_pass = 2 + 10000;

	//No more pop grow bonus it will count as upkeep reducer multiplier
	//Planet grow pop is 2% each tick -- of current population, not max
	planetd.population += ceil( planetd.population * 0.02 * pow(0.75, (float)nInfection) );
	if( planetd.population > planetd.maxpopulation )
		planetd.population = planetd.maxpopulation;


	if( ( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) )
		planetd.protection = 100;
	else
      		planetd.protection = (int)( 100.0 * battlePortalCalc( ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, portals, nump, mainp->totalresearch[CMD_RESEARCH_PORTALS] ) );


	if( planetd.construction < 0 ) {
		error( "Warning : negative construction count : %d", planetd.construction );
		planetd.construction = 0;
	}


	ticks.debug_pass = 3 + 10000;


	/* CRAP */
	for( b = 0 ; b < CMD_BLDG_NUMUSED ; b++ ) {
		if( planetd.building[b] < 0 ) {
				error( "Warning : negative building count : %d", planetd.building[b] );
				planetd.building[b] = 0;
			}
	}

	dbMapSetPlanet( buffer[a], &planetd );

	ticks.debug_pass = 4 + 10000;

	population += planetd.population;

	for( b = 0 ; b < CMD_BLDG_NUMUSED ; b++ ) {
		mainp->totalbuilding[b] += planetd.building[b];
		cmdTickProduction[b] += planetd.building[b];
	}

	for( b = 0 ; b < CMD_UNIT_NUMUSED ; b++ )
		mainp->totalunit[b] += planetd.unit[b];

	if( ( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) )
		mainp->totalbuilding[CMD_BLDG_NUMUSED]++;


	ticks.debug_pass = 5 + 10000;

	if( planetd.special[1] ) {
		if( planetd.special[0] == 0 ) {
			cmdTickProduction[CMD_BUILDING_SOLAR] += ( planetd.special[1] * planetd.building[CMD_BUILDING_SOLAR] ) / 100;
		} else if( planetd.special[0] == CMD_BONUS_ENERGY ) {
			cmdTickProduction[CMD_BUILDING_MINING] += ( planetd.special[1] * planetd.building[CMD_BUILDING_MINING] ) / 100;
		} else if( planetd.special[0] == CMD_BONUS_CRYSTAL ) {
			cmdTickProduction[CMD_BUILDING_CRYSTAL] += ( planetd.special[1] * planetd.building[CMD_BUILDING_CRYSTAL] ) / 100;
		} else if( planetd.special[0] == CMD_BONUS_ECTROLIUM ) {
			cmdTickProduction[CMD_BUILDING_REFINEMENT] += ( planetd.special[1] * planetd.building[CMD_BUILDING_REFINEMENT] ) / 100;
		} else if( planetd.special[0] == CMD_BONUS_FISSION ) {
			cmdTickProduction[CMD_BUILDING_FISSION] += ( planetd.special[1] * planetd.building[CMD_BUILDING_FISSION] ) / 100;
		}
	}


	ticks.debug_pass = 6 + 10000;


	if( ( b = (int)artefactPrecense( &planetd ) ) < 0 )
		continue;

	if( dbMapRetrieveEmpire( mainp->empire, &empired ) < 0 )
		continue;

	empired.artefacts |= 1 << b;
	dbMapSetEmpire( mainp->empire, &empired );


	ticks.debug_pass = 7 + 10000;
}

mainp->planets = num;
mainp->ressource[CMD_RESSOURCE_POPULATION] = population;


ticks.debug_pass = 8 + 10000;

free( buffer );

ticks.debug_pass = 9 + 10000;

if( portals )
	free( portals );

ticks.debug_pass = 10 + 10000;

if( ( num = dbUserFleetList( usrid, &fleetd ) ) < 0 )
	return 0;

ticks.debug_pass = 11 + 10000;

for( a = 0 ; a < num ; a++ ) {
	for( b = 0 ; b < CMD_UNIT_NUMUSED ; b++ )
		mainp->totalunit[b] += fleetd[a].unit[b];
}

ticks.debug_pass = 12 + 10000;

free( fleetd );

ticks.debug_pass = 13 + 10000;


return 1;
}





int cmdTick() {
	int a, c, d, e, num, specopnum, opvirus /*,cmd[3], i*/;
	float fb, phdecay;
	double fa, fc;
	int64_t newd[DB_USER_NEWS_BASE],/* nIllusion,*/ b;
	//int nChicks = 0, penalty;
	int marketbid[DB_MARKETBID_NUMUSED];
	int bidresult[2];
	int *plist;
	time_t now;
	dbUserPtr user;
	dbUserMainDef maind;
	dbUserBuildPtr build;
	dbMainPlanetDef planetd;
	dbUserFleetDef fleetd;
	dbUserFleetPtr fleetp;
	dbUserSpecOpPtr specopd;
	dbMainEmpireDef empired;
	ConfigArrayPtr settings;


ticks.uonline = 0;
ticks.uactive = 0;
ticks.uregist = 0;
ticks.debug_pass = 0;
ticks.debug_id = 0;
time(&now);

for( a = 0 ; a < dbMapBInfoStatic[MAP_EMPIRES] ; a++ ) {
	if( dbMapRetrieveEmpire( a, &empired ) < 0 )
		continue;
	ticks.debug_id = a;
	empired.artefacts = 0;
	//Add decay to empire funds, decay (if any) removed before deposits from this tick.
	empired.infos[CMD_RESSOURCE_ENERGY] = fmax( 0.0, ( CMD_ENERGY_DECAY * empired.fund[CMD_RESSOURCE_ENERGY] ) );
	empired.infos[CMD_RESSOURCE_CRYSTAL] = fmax( 0.0, (  CMD_CRYSTAL_DECAY * empired.fund[CMD_RESSOURCE_CRYSTAL] ) );
	empired.fund[CMD_RESSOURCE_ENERGY] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_ENERGY] - empired.infos[CMD_RESSOURCE_ENERGY] ) );
	empired.fund[CMD_RESSOURCE_CRYSTAL] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_CRYSTAL] - empired.infos[CMD_RESSOURCE_CRYSTAL] ) );
	dbMapSetEmpire( a, &empired );
}


ticks.debug_pass = 1;


if( ( dbMapRetrieveMain( dbMapBInfoStatic ) < 0 ) ) {
	error( "Tick error: Retriving Map Info!" );
}

for( user = dbUserList ; user ; user = user->next ) {
	ticks.uregist++;

	if( !( bitflag( user->flags, CMD_USER_FLAGS_ACTIVATED ) ) || ( bitflag( user->flags, CMD_USER_FLAGS_FROZEN ) ) )
		continue;

	ticks.uactive++;
	ticks.debug_id = user->id;
	ticks.uonline += ( (now - user->lasttime) < (SESSION_TIME / 4) );

	if( dbUserMainRetrieve( user->id, &maind ) < 0 ) {
		error( "Tick error: Retriving User %d", user->id );
		continue;
	}
	if( maind.empire < 0 )
		continue;

	ticks.debug_pass = 2;


	if( ( specopnum = dbUserSpecOpList( user->id, &specopd ) )  < 0 ) {
		error( "Tick error: SpecOps User %d", user->id );
		continue;
	}
	opvirus = 0;
	/*
	//WAR  ILLUSION we recalcul each tick
	for(i=0;i<specopnum;i++) {
		if (specopd[i].type == (CMD_SPELL_WARILLUSIONS | 0x1000)) {
			fa = 0.4 + (1.2/255.0) * (float)( rand() & 255 );
			nChicks = maind.totalunit[CMD_UNIT_WIZARD];
			nIllusion = ( fa * cmdRace[maind.raceid].unit[CMD_UNIT_WIZARD] * (float)nChicks * ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_WELFARE] ) / cmdPsychicopDifficulty[CMD_SPELL_WARILLUSIONS] );
			penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[CMD_SPELL_WARILLUSIONS] );
			if( penalty )
		    	nIllusion = (float)nIllusion / ( 1.0 + 0.01*(float)penalty );
			fa = 100.0 * (float)nIllusion / (float)maind.networth;
    			a = (int)( fa * 4.5 );
    			a += a * rand()%20;
    			if (a<0)
    				a = 0;
    			specopd[i].vars[0] = a;
		} else if (specopd[i].type == (CMD_SPELL_DARKWEB | 0x1000)) {
			fa = 0.4 + (1.2/255.0) * (float)( rand() & 255 );
			nChicks = maind.totalunit[CMD_UNIT_WIZARD];
			nIllusion = ( fa * cmdRace[maind.raceid].unit[CMD_UNIT_WIZARD] * (float)nChicks * ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_WELFARE] ) / cmdPsychicopDifficulty[CMD_SPELL_DARKWEB] );
			penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[CMD_SPELL_DARKWEB] );
			if( penalty )
			    	nIllusion = (float)nIllusion / ( 1.0 + 0.01*(float)penalty );
			fa = 100.0 * (float)nIllusion / (float)maind.networth;
			a = (int)( fa * 3.5 );
    			if (a<0)
    				a = 0;
    			specopd[i].vars[0] = a;
		}
	}
	*/
	for( a = specopnum-1 ; a >= 0 ; a-- ) {
		if( specopd[a].type == ( CMD_OPER_NETWORKVIRUS | 0x10000 ) )
			opvirus++;
	}


	ticks.debug_pass = 3;


	num = dbUserBuildListReduceTime( user->id, &build );
	for( a = num-1 ; a >= 0 ; a-- ) {
		memset(&newd, 0, sizeof(int64_t)*DB_USER_NEWS_BASE);
		newd[0] = ticks.number;
		newd[1] = CMD_NEWS_FLAGS_NEW;
		if( build[a].time > 0 )
			continue;
		if( !( build[a].type >> 16 ) ) {
			dbMapRetrievePlanet( build[a].plnid, &planetd );
			if( build[a].type == CMD_BLDG_NUMUSED ) {
				// portal
				planetd.flags &= 0xFFFFFFFF - CMD_PLANET_FLAGS_PORTAL_BUILD;
				planetd.flags |= CMD_PLANET_FLAGS_PORTAL;
				dbUserPlanetSetFlags( user->id, build[a].plnid, planetd.flags );
				planetd.construction--;
			} else {
				planetd.building[ build[a].type ] += build[a].quantity;
				planetd.construction -= build[a].quantity;
			}
			dbMapSetPlanet( build[a].plnid, &planetd );
			newd[2] = CMD_NEWS_BUILDING;
			newd[5] = build[a].plnid;
			newd[6] = build[a].plnpos;
		} else {
			if( !( dbUserFleetRetrieve( user->id, 0, &fleetd ) ) )
				continue;
			fleetd.unit[ build[a].type & 0xFFFF ] += build[a].quantity;
			if( !( dbUserFleetSet( user->id, 0, &fleetd ) ) )
				continue;
			newd[2] = CMD_NEWS_UNIT;
		}
		dbUserBuildRemove( user->id, a );
		newd[3] = build[a].type;
		newd[4] = build[a].quantity;
		cmdUserNewsAdd( user->id, newd, CMD_NEWS_FLAGS_BUILD );
	}
	free( build );


	ticks.debug_pass = 4;

	// calc total of buildings, units, artefacts
	cmdTickPlanets( user->id, &maind );

	ticks.debug_pass = 5;


	// add research

	for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ ) {
		fa = ( (maind.allocresearch[a]) * ( 500*cmdTickProduction[CMD_BUILDING_RESEARCH] + maind.fundresearch ) ) / 10000.0;
		if( cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_POPRESEARCH )
			fa += ( (maind.allocresearch[a]) * maind.ressource[CMD_RESSOURCE_POPULATION] ) / ( 400.0 * 100.0 );

	/*		//ARTI CODE Foohon Ancestry
			if(maind.artefacts & ARTEFACT_*_BIT)
				fa += ( (double)(maind.allocresearch[a]) * (double)maind.ressource[CMD_RESSOURCE_POPULATION] ) / ( 400.0 * 100.0 );
	*/


			maind.research[a] += cmdRace[maind.raceid].researchpoints[a] * fa;

			maind.research[a] = fmax( 0.0, maind.research[a]);
	}
	maind.fundresearch = (int64_t)( 0.9 * maind.fundresearch );

	ticks.debug_pass = 6;

	// SK: because of the network backbone arti, we need to calculate Tech research first
	//    int addedFromTech = 0;

	// calculate total research for tech

          //research maximum
          fa = cmdRace[maind.raceid].researchmax[CMD_RESEARCH_TECH];

      /*      //      ARTI CODE Divine Stone
                    if(maind.artefacts & ARTEFACT_*_BIT)
                            fa -= 25;
     */

        b = fa * ( 1.0 - exp( maind.research[CMD_RESEARCH_TECH] / ( -10.0 * maind.networth ) ) );
        if( b > maind.totalresearch[CMD_RESEARCH_TECH] ) {
		maind.totalresearch[CMD_RESEARCH_TECH]++;
	} else if( b < maind.totalresearch[CMD_RESEARCH_TECH] ) {
		maind.totalresearch[CMD_RESEARCH_TECH]--;
	}

              //addedFromTech = b/10;


	// calculate total research
	for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ ) {
		if(a == CMD_RESEARCH_TECH)
			continue;
		//research maximum
		fa = cmdRace[maind.raceid].researchmax[a];
		// CODE_ARTI
		if( ( maind.artefacts & ARTEFACT_4_BIT ) && ( a == CMD_RESEARCH_MILITARY ) ) {
			fa += 25.0;
		}
	        // put this arti last, you need the other ones calculated before this one.
	         /*   //ARTI CODE network backbone
	        if(maind.artefacts & ARTEFACT_*_BIT)
	        {
	           // exclude tech research from having this bonus (otherwise there is no cap)
	           if( a != CMD_RESEARCH_TECH)
	               {
	                fa += addedFromTech;
	               }
	        }*/

		b = fa * ( 1.0 - exp( maind.research[a] / ( -10.0 * maind.networth ) ) );
		if( b > maind.totalresearch[a] )
			maind.totalresearch[a]++;
		else if( b < maind.totalresearch[a] )
		maind.totalresearch[a]--;
	}

	ticks.debug_pass = 7;


// calc infos
	fa = ( 12.0 * cmdTickProduction[CMD_BUILDING_SOLAR] / specopSolarCalc( user->id ) );
	if( cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_SOLARP15 ) {
		fa *= 1.15;
	}

      //ARTI CODE Ether Palace
	if( maind.artefacts & ARTEFACT_32_BIT ) {
		fa *= 1.30;
	}

	fb = ( 40.0 * cmdTickProduction[CMD_BUILDING_FISSION] );
	fa += fb;

	//ARTI CODE Ether Garden

	if(maind.artefacts & ARTEFACT_ETHER_BIT) {
		fa *= 1.10;
	}

	//ARTI CODE Ether Palace
	if(maind.artefacts & ARTEFACT_8_BIT) {
		fa *= 1.25;
	}

	fb = cmdRace[maind.raceid].resource[CMD_RESSOURCE_ENERGY] * ( 1.00 + 0.01 * maind.totalresearch[CMD_RESEARCH_ENERGY] );


	maind.infos[INFOS_ENERGY_PRODUCTION] = ( fa * fb );


	settings = GetSetting( "Stockpile" );
	fc = settings->num_value * maind.infos[INFOS_ENERGY_PRODUCTION];
	fa = CMD_ENERGY_DECAY;
	maind.infos[INFOS_ENERGY_DECAY] = fa * fmax( 0.0, maind.ressource[CMD_RESSOURCE_ENERGY] - fc );


	// meh! building upkeep
	maind.infos[INFOS_BUILDING_UPKEEP] = 0;
	for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ ) {
		if( ( a == CMD_BUILDING_SOLAR ) || ( a == CMD_BUILDING_FISSION ) ) {
			maind.infos[INFOS_BUILDING_UPKEEP] += ( cmdTickProduction[a] * cmdBuildingUpkeep[a] * fb );
   		} else {
			maind.infos[INFOS_BUILDING_UPKEEP] += ( cmdTickProduction[a] * cmdBuildingUpkeep[a] );
		}

	}


	ticks.debug_pass = 8;

	for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ ) {
		if( maind.totalunit[a] < 0 )
			maind.totalunit[a] = 0;
	}


	maind.infos[INFOS_UNITS_UPKEEP] = 0;
	for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ ) {
		maind.infos[INFOS_UNITS_UPKEEP] += ( maind.totalunit[a] * cmdUnitUpkeep[a] );
	}
	//ARTI CODE Romulan Military Outpost
	if(maind.artefacts & ARTEFACT_16_BIT) {
		maind.infos[INFOS_UNITS_UPKEEP] *= 1.5;
	}
	maind.infos[INFOS_POPULATION_REDUCTION] = ((1.0/35.0) * maind.ressource[CMD_RESSOURCE_POPULATION] * ( 1.00 + 0.01 * maind.totalresearch[CMD_RESEARCH_WELFARE] ) * cmdRace[maind.raceid].growth );

	//Population Reduction changed to include portals + units
	maind.infos[INFOS_PORTALS_UPKEEP] = fmax( 0.0, ( pow( (maind.totalbuilding[CMD_BLDG_NUMUSED]-1), 1.2736 ) * 10000.0) );
	maind.infos[INFOS_POPULATION_REDUCTION] = fmin( maind.infos[INFOS_POPULATION_REDUCTION], ( maind.infos[INFOS_BUILDING_UPKEEP] + maind.infos[INFOS_UNITS_UPKEEP] + maind.infos[INFOS_PORTALS_UPKEEP] ) );

	/*
	 * Network Virus Effect.
	 */
	for( a = 0 ; a < opvirus ; a++ ) {
		maind.infos[INFOS_POPULATION_REDUCTION] -= fmax( 0.0, ( maind.infos[INFOS_POPULATION_REDUCTION] * 0.05 ) );
		maind.infos[INFOS_BUILDING_UPKEEP] += fmax( 0.0, ( maind.infos[INFOS_BUILDING_UPKEEP] * 0.15 ) );
	}

	maind.infos[INFOS_CRYSTAL_PRODUCTION] = ( cmdRace[maind.raceid].resource[CMD_RESSOURCE_CRYSTAL] * cmdTickProduction[CMD_BUILDING_CRYSTAL]);


		//ARTI CODE Crystalline Entity | reduces crystal decay by 75%
	//	if(maind.artefacts & ARTEFACT_*_BIT)
	//	fa /= 4;
	fc = settings->num_value * maind.infos[INFOS_CRYSTAL_PRODUCTION];
	fa = CMD_CRYSTAL_DECAY;
	maind.infos[INFOS_CRYSTAL_DECAY] = fa * fmax( 0.0, ( maind.ressource[CMD_RESSOURCE_CRYSTAL] - fc ) );


    //ARTI CODE Mana Gate
		//if(maind.artefacts & ARTEFACT_MANA_BIT)
		//	maind.infos[INFOS_PORTALS_UPKEEP] /= 2;

	maind.infos[INFOS_MINERAL_PRODUCTION] = ( cmdRace[maind.raceid].resource[CMD_RESSOURCE_MINERAL] * cmdTickProduction[CMD_BUILDING_MINING]);
	maind.infos[INFOS_ECTROLIUM_PRODUCTION] = ( cmdRace[maind.raceid].resource[CMD_RESSOURCE_ECTROLIUM] * cmdTickProduction[CMD_BUILDING_REFINEMENT]);

	if ( dbMapRetrieveEmpire( maind.empire, &empired ) < 0 ) {
		error( "Tick error: Retriving empire %d", maind.empire  );
		continue;
	}
	maind.infos[INFOS_ENERGY_TAX] = fmax( 0.0, ( maind.infos[INFOS_ENERGY_PRODUCTION] * empired.taxation ) );
	maind.infos[INFOS_MINERAL_TAX] = fmax( 0.0, ( maind.infos[INFOS_MINERAL_PRODUCTION] * empired.taxation ) );
	maind.infos[INFOS_CRYSTAL_TAX] = fmax( 0.0, ( maind.infos[INFOS_CRYSTAL_PRODUCTION] * empired.taxation ) );
	maind.infos[INFOS_ECTROLIUM_TAX] = fmax( 0.0, ( maind.infos[INFOS_ECTROLIUM_PRODUCTION] * empired.taxation ) );

	maind.infos[CMD_RESSOURCE_ENERGY] = maind.infos[INFOS_ENERGY_PRODUCTION] - maind.infos[INFOS_ENERGY_DECAY] - maind.infos[INFOS_BUILDING_UPKEEP] - maind.infos[INFOS_UNITS_UPKEEP] + maind.infos[INFOS_POPULATION_REDUCTION] - maind.infos[INFOS_PORTALS_UPKEEP] - maind.infos[INFOS_ENERGY_TAX];

	maind.infos[CMD_RESSOURCE_MINERAL] = (maind.infos[INFOS_MINERAL_PRODUCTION] - maind.infos[INFOS_MINERAL_TAX]);

	maind.infos[CMD_RESSOURCE_CRYSTAL] = ( (maind.infos[INFOS_CRYSTAL_PRODUCTION] - maind.infos[INFOS_CRYSTAL_TAX] ) - maind.infos[INFOS_CRYSTAL_DECAY]);

	maind.infos[CMD_RESSOURCE_ECTROLIUM] = (maind.infos[INFOS_ECTROLIUM_PRODUCTION] - maind.infos[INFOS_ECTROLIUM_TAX]);


	empired.fund[CMD_RESSOURCE_ENERGY] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_ENERGY] + maind.infos[INFOS_ENERGY_TAX] ) );
	empired.fund[CMD_RESSOURCE_MINERAL] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_MINERAL] + maind.infos[INFOS_MINERAL_TAX] ) );
	empired.fund[CMD_RESSOURCE_CRYSTAL] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_CRYSTAL] + maind.infos[INFOS_CRYSTAL_TAX] ) );
	empired.fund[CMD_RESSOURCE_ECTROLIUM] = fmax( 0.0, ( empired.fund[CMD_RESSOURCE_ECTROLIUM] + maind.infos[INFOS_ECTROLIUM_TAX] ) );

	if( dbMapSetEmpire( maind.empire, &empired ) < 0 ) {
		error( "Tick error: Setting Empire #%d Fund!", maind.empire );
	}


	//ARTI CODE Mineral enhancement
	if(maind.artefacts & ARTEFACT_64_BIT) {
		maind.infos[CMD_RESSOURCE_MINERAL] *= 1.50;
	}
/*	//ARTI CODE Ectrolim enhancement
		if(maind.artefacts & ARTEFACT_xxxx_BIT)
	{
		maind.infos[CMD_RESSOURCE_ECTROLIUM] *= 1.50;
	}	*/
	ticks.debug_pass = 9;


// fleets decay?
    a = 65536*2;
    b = -1;


    if( ( maind.ressource[CMD_RESSOURCE_ENERGY] + maind.infos[CMD_RESSOURCE_ENERGY] ) < 0 )
    {
      a = -65536*3;
      b = CMD_UNIT_GHOST;
    }

// readiness
    for( c = 0 ; c < 3 ; c++ )
    {
      if( a < 0 )
      {
        if( maind.readiness[c] > ( -65536*200 - a ) )
          maind.readiness[c] += a;
        else if( maind.readiness[c] > -65536*200 )
          maind.readiness[c] = -65536*200;
      }
      else
        maind.readiness[c] += a;


/* CODE_ARTEFACT
    if( ( maind.artefacts & ARTEFACT_*_BIT ) && ( c != 0 ) )
    {
      if( maind.readiness[c] > 65536*75 ) //alter this to increase or decrease the readiness, currently decreased to 75%
        maind.readiness[c] = 65536*75;	//alter this to increase or decrease the readiness, currently decreased to 75%
	}
    else
    {
      if( maind.readiness[c] > 65536*100 )
        maind.readiness[c] = 65536*100;
   }
 CODE_ARTEFACT*/

	if( maind.readiness[c] > 65536*100 )
		maind.readiness[c] = 65536*100;


    if( maind.readiness[c] < -65536*500 )
        maind.readiness[c] = -65536*500;

    }


ticks.debug_pass = 10;


// calculate phantoms decay rate
    phdecay = 0.20;
    if( maind.totalunit[CMD_UNIT_WIZARD] )
    {
      fa = (double)maind.totalunit[CMD_UNIT_PHANTOM] / (double)maind.totalunit[CMD_UNIT_WIZARD];
      if( fa < 0.05 )
        phdecay = 0.01;
      else
      {
        fa = (double)pow( (double)( (1.0/0.05) * fa ), 2.4 );
        phdecay = 0.01*fa;
        if( phdecay > 0.20 )
          phdecay = 0.20;
      }
    }

// fleets
    dbUserMainSet( user->id, &maind );
    fleetp = 0;
    num = dbUserFleetList( user->id, &fleetp );
    for( a = num-1 ; a >= 0 ; a-- )
    {
      e = 0;
      if( a == 0 )
      {
        if( !( dbUserFleetRetrieve( user->id, 0, &fleetp[0] ) ) )
          return 1;
      }
      for( c = b ; c >= 0 ; c-- )
      {
        d = (int)ceil( (float)fleetp[a].unit[c] * 0.02 );
        fleetp[a].unit[c] -= d;
        maind.totalunit[c] -= d;
        e |= d;
      }

      if( fleetp[a].unit[CMD_UNIT_PHANTOM] )
      {
        d = (int)ceil( (float)fleetp[a].unit[CMD_UNIT_PHANTOM] * phdecay );
        fleetp[a].unit[CMD_UNIT_PHANTOM] -= d;
        maind.totalunit[CMD_UNIT_PHANTOM] -= d;
        e |= d;
      }

      if( ( a ) && !( fleetp[a].unit[CMD_UNIT_EXPLORATION] ) && !( fleetp[a].unit[CMD_UNIT_CRUISER] ) && !( fleetp[a].unit[CMD_UNIT_CARRIER] ) && !( fleetp[a].unit[CMD_UNIT_PHANTOM] ) && !( fleetp[a].unit[CMD_UNIT_AGENT] ) && !( fleetp[a].unit[CMD_UNIT_GHOST] ) )
      {
        dbUserFleetRemove( user->id, a );
        continue;
      }
      fleetd.flags &= 0xFFFFFFFF - CMD_FLEET_FLAGS_MOVED;
      if( fleetp[a].time > 0 )
      {
        fleetd.flags |= CMD_FLEET_FLAGS_MOVED;
        fleetp[a].time--;
        dbUserFleetSet( user->id, a, &fleetp[a] );
      }
      else if( e )
        dbUserFleetSet( user->id, a, &fleetp[a] );
      if( ( a != 0 ) && ( fleetp[a].time == 0 ) )
        cmdFleetAction( &fleetp[a], user->id, a, 1 );
    }
    if( fleetp )
      free( fleetp );
    dbUserMainRetrieve( user->id, &maind );


ticks.debug_pass = 11;


// units decay on planets
    plist = 0;
    num = dbUserPlanetListIndices( user->id, &plist );
    for( a = 0 ; a < num ; a++ )
    {
      dbMapRetrievePlanet( plist[a], &planetd );
      e = 0;
      if( planetd.unit[CMD_UNIT_PHANTOM] )
      {
        planetd.unit[CMD_UNIT_PHANTOM] -= (int)ceil( phdecay * (float)(planetd.unit[CMD_UNIT_PHANTOM]) );
        e |= 1;
      }
      for( c = b ; c >= 0 ; c-- )
      {
        d = (int)ceil( (float)planetd.unit[c] * 0.02 );
        planetd.unit[c] -= d;
        maind.totalunit[c] -= d;
        e |= d;
      }
      if( e )
        dbMapSetPlanet( plist[a], &planetd );
    }
    if( plist )
      free( plist );


ticks.debug_pass = 12;


// income
    for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      maind.ressource[a] += maind.infos[a];
      if( maind.ressource[a] < 0 )
        maind.ressource[a] = 0;
    }

// networth
    a = dbUserPlanetNumber( user->id );
    maind.networth = ( 800 * a );
    for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
      maind.networth += maind.totalunit[a] * cmdUnitStats[a][CMD_UNIT_STATS_NETWORTH];
    for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
    {
     	maind.networth += 8 * maind.totalbuilding[a];
    }

    maind.networth += (int64_t)(0.004 * maind.ressource[CMD_RESSOURCE_POPULATION]);

    for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
      maind.networth += (0.001 * maind.research[a]);

// spec ops
    for( a = specopnum-1 ; a >= 0 ; a-- )
    {
      specopd[a].time--;
      if( specopd[a].time <= 0 )
      {
      	//remove planet beacon flag on the planet
      	if(specopd[a].type == CMD_OPER_PLANETBEACON)
      	{
      		dbMapRetrievePlanet(specopd[a].plnid, &planetd);
    			planetd.flags ^= CMD_PLANET_FLAGS_BEACON;
    			dbMapSetPlanet(specopd[a].plnid, &planetd);
      	}

        dbUserSpecOpRemove( user->id, a );
        continue;
      }
      dbUserSpecOpSet( user->id, a, &specopd[a] );
    }

    if( specopd )
      free( specopd );


ticks.debug_pass = 13;

    dbUserMainSet( user->id, &maind );
  }


ticks.debug_pass = 14;


// bids energy decay
  marketbid[DB_MARKETBID_ACTION] = 0;
  for( marketbid[DB_MARKETBID_RESSOURCE] = 0 ; marketbid[DB_MARKETBID_RESSOURCE] < 3 ; marketbid[DB_MARKETBID_RESSOURCE]++ )
  {
    for( marketbid[DB_MARKETBID_PRICE] = 0 ; marketbid[DB_MARKETBID_PRICE] < DB_MARKET_RANGE ; marketbid[DB_MARKETBID_PRICE]++ )
    {
      b = dbMarketListStart( marketbid );
      while( (unsigned int)b < 0x1000000 )
      {
        c = dbMarketListNext( b, bidresult );
        a = (int)ceil( CMD_ENERGY_DECAY * (float)bidresult[0] );
        if( (bidresult[0]-a) > 0 )
        {
          dbUserMarketQuantity( bidresult[1], b, bidresult[0] - a );
          dbMarketSetQuantity( marketbid, b, bidresult[0] - a, a );
        }
        else
        {
          dbUserMarketRemove( bidresult[1], b );
          dbMarketRemove( marketbid, b );
        }
        b = c;
      }
    }
  }


ticks.debug_pass = 15;


// bids crystal decay
  marketbid[DB_MARKETBID_ACTION] = 1;
  marketbid[DB_MARKETBID_RESSOURCE] = CMD_RESSOURCE_CRYSTAL-1;
  for( marketbid[DB_MARKETBID_PRICE] = 0 ; marketbid[DB_MARKETBID_PRICE] < DB_MARKET_RANGE ; marketbid[DB_MARKETBID_PRICE]++ )
  {
    b = dbMarketListStart( marketbid );
    while( (unsigned int)b < 0x1000000 )
    {
      c = dbMarketListNext( b, bidresult );
      a = (int)ceil( CMD_CRYSTAL_DECAY * (float)bidresult[0] );

      if( (bidresult[0]-a) > 0 )
      {
        dbUserMarketQuantity( bidresult[1], b, bidresult[0] - a );
        dbMarketSetQuantity( marketbid, b, bidresult[0] - a, a );
      }
      else
      {
        dbUserMarketRemove( bidresult[1], b );
        dbMarketRemove( marketbid, b );
      }
      b = c;
    }
  }


ticks.debug_pass = 16;


  return 1;
}


