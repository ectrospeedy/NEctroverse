#ifndef CONFIGINCLUDED
#include "config.h"
#endif
#ifndef GLOBALINCLUDED
#include "global.h"
#endif


int iohttpIdentifyHex( char *num )
{
 int a, b, c;
 for( a = b = 0 ; ; b <<= 4 )
 {
  if( ( num[a] >= '0' ) && ( num[a] <= '9' ) )
   b += c = num[a] - '0';
  else if( ( num[a] >= 'a' ) && ( num[a] <= 'z' ) )
   b += c = num[a] - ('a'-10);
  else if( ( num[a] >= 'A' ) && ( num[a] <= 'Z' ) )
   b += c = num[a] - ('A'-10);
  if( ++a == 4 )
   break;
 }
 return b;
}

int iohttpIdentify( svConnectionPtr cnt, int action ) {
	int a, b, id, session[4];
	char *src;
	char *data;
	FILE *file;
	struct stat stdata;


iohttpDataPtr iohttp = cnt->iodata;


if( !( src = iohttp->cookie ) )
	goto iohttpIdentifyL0;

if( !( src = ioCompareFindWords( src, "USRID=" ) ) )
	goto iohttpIdentifyL0;

id = iohttpIdentifyHex( &src[0] );

if( dbUserLinkDatabase( cnt, id ) < 0 )
	goto iohttpIdentifyL0;

if( dbSessionRetrieve( cnt->dbuser, session ) < 0 )
	goto iohttpIdentifyL0;

for( a = 0 ; a < 4 ; a++ ) {
	b = iohttpIdentifyHex( &src[4+(a<<2)] );

	if( session[a] != b )
		goto iohttpIdentifyL0;
}

if(( action & 2 )&&(cnt->dbuser)) {
	if( !( (cnt->dbuser)->flags & CMD_USER_FLAGS_ACTIVATED ) && ( (cnt->dbuser)->level < LEVEL_MODERATOR ) ) {
		if( action & 1 ) {
			if( action & 8 )
			iohttpBase( cnt, 1|2 );
			svSendString( cnt, "This account has not been activated yet.</body></html>" );
		}
		return -1;
	}
}

if( action & 4 ) {
	if( (cnt->dbuser)->flags & CMD_USER_FLAGS_ACTIVATED ) {
		if( action & 1 ) {
			if( action & 8 )
				iohttpBase( cnt, 1|2 );
			svSendString( cnt, "This account has been activated.</body></html>" );
		}
		return -1;
	}
}


return id;

iohttpIdentifyL0:

if( action & 1 ) {

	iohttpFunc_login( cnt, 1, "If you were playing just a few seconds ago, the server program was probably updated and restarted." );

}


return -1;
}

void iohttpBase( svConnectionPtr cnt, int flags ) {
	FILE *file;
	struct stat stdata;
	char *data;
	
svSendString( cnt, "Content-Type: text/html\n\n" );
svSendString( cnt, "<html><head>");
svSendPrintf( cnt, "<title>%s</title>", SERVERNAME );
svSendPrintf( cnt, "<link rel=\"icon\" href=\"favicon.ico\">" );
if( flags & 4 )
	svSendString( cnt, "<base target=\"_blank\">" );
 
svSendString( cnt, "<style type=\"text/css\">" );

if( stat( IOHTTP_READ_DIRECTORY "/style.css", &stdata ) != -1 ) {
	if( ( data = malloc( stdata.st_size + 1 ) ) ) {
		data[stdata.st_size] = 0;
		if( ( file = fopen( IOHTTP_READ_DIRECTORY "/style.css", "rb" ) ) ) {
			fread( data, 1, stdata.st_size, file );
			svSendString( cnt, data );
			fclose( file );
		}
	free( data );
	}
}

if( flags & 1 ) {
	svSendString( cnt, "body{background-image:url(mbg.gif);" );
	if( !( flags & 2 ) )
		svSendString( cnt, "background-attachment:fixed;" );
	svSendString( cnt, "}" );
}

svSendString( cnt, "</style></head>" );
svSendString( cnt, "<body" );

if( flags & 8 )
	svSendString( cnt, " onload=\"if (window != window.top) { top.location.href=location.href }\" " );


svSendString( cnt, "><center>" );

return;
}

int iohttpHeader( svConnectionPtr cnt, int id, dbUserMainPtr mainp )
{
 int a;
 if( dbUserMainRetrieve( id, mainp ) < 0 )
 {
  svSendString( cnt, "Error while retriving user's main data</center></body></html>" );
  return 0;
 }

 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" border=\"0\" align=\"center\">" );

 svSendString( cnt, "<tr>" );
 svSendString( cnt, "<td width=\"35\" rowspan=\"2\"><img height=\"63\" src=\"i02.jpg\" width=\"35\"></td>" );
 svSendString( cnt, "<td width=\"100%\" height=\"21\" align=\"center\">" );

 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" background=\"i05.jpg\" border=\"0\" align=\"center\">" );
 svSendString( cnt, "<tr>" );
 svSendString( cnt, "<td width=\"41\"><img height=\"21\" src=\"i03.jpg\" width=\"41\"></td>" );
 svSendString( cnt, "<td background=\"i05.jpg\">" );

 if( svTickStatus )
 svSendPrintf( cnt, "<table width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Networth : %lld</b></font></td><td width=\"40%%\" align=\"center\"><font size=\"1\"><b>Tick time : %d seconds left</b></font></td><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Population : %lld0</b></font></td></tr></table>", mainp->networth, (int)( svTickTime - time(0) ), mainp->ressource[CMD_RESSOURCE_POPULATION] );
 else
 svSendPrintf( cnt, "<table width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Networth : %lld</b></font></td><td width=\"40%%\" align=\"center\"><font size=\"1\"><b>Tick time : time frozen</b></font></td><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Population : %lld0</b></font></td></tr></table>", mainp->networth, mainp->ressource[CMD_RESSOURCE_POPULATION] );

 svSendString( cnt, "</td><td width=\"78\"><img height=\"21\" src=\"i07.jpg\" width=\"78\"></td></tr></table>" );
 svSendString( cnt, "</td></tr>" );

 svSendString( cnt, "<tr><td width=\"100%\" align=\"center\">" );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" background=\"i15.jpg\" border=\"0\" align=\"center\"><tr>" );

 a = dbUserNewsGetFlags( id );
 svSendString( cnt, "<td width=\"41\"><a href=\"hq\"><img height=\"42\" title=\"mail\" src=\"i09" );
 if( a & CMD_NEWS_FLAGS_MAIL )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"41\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"40\"><a href=\"hq\"><img height=\"42\" title=\"reports\" src=\"i10" );
 if( a & CMD_NEWS_FLAGS_BUILD )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"40\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"39\"><a href=\"hq\"><img height=\"42\" title=\"economy\" src=\"i11" );
 if( a & CMD_NEWS_FLAGS_AID )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"39\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"39\"><a href=\"hq\"><img height=\"42\" title=\"fleets\" src=\"i12" );
 if( a & CMD_NEWS_FLAGS_ATTACK )
  svSendString( cnt, "a" );
 else if( a & CMD_NEWS_FLAGS_FLEET )
  svSendString( cnt, "b" );
 svSendString( cnt, ".jpg\" width=\"39\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"18\"><img height=\"42\" src=\"i13.jpg\" width=\"18\"></td>" );
 svSendString( cnt, "<td width=\"100%\" background=\"i15.jpg\" align=\"left\">" );

 svSendString( cnt, "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\"><tr>" );
 svSendString( cnt, "<td width=\"50%\" align=\"center\" nowrap><font size=\"1\"><b>" );
 svSendPrintf( cnt, "Energy: %lld<br>Mineral: %lld</b></font></td><td width=\"50%%\" align=\"center\" nowrap><font size=\"1\"><b>Crystal: %lld<br>Ectrolium: %lld</b></font>", mainp->ressource[0], mainp->ressource[1], mainp->ressource[2], mainp->ressource[3] );
 svSendString( cnt, "</td></tr></table>" );

 svSendString( cnt, "</td><td width=\"49\"><img height=\"42\" src=\"i17.jpg\" width=\"49\"></td></tr></table>" );

 svSendString( cnt, "</td></tr></table><br>" );

 return 1;
}

void iohttpFunc_frontmenu( svConnectionPtr cnt, int flags ) {

svSendString( cnt, "<center>" );
svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td align=\"center\"><img src=\"ectro_03.jpg\" width=350 height=80  alt=\"ect Top\"></td></tr>" );

svSendString( cnt, "<tr><td><table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr>" );
svSendString( cnt, "<td background=\"ectro_12.jpg\" width=\"45%\">&nbsp;</td>" );
svSendString( cnt, "<td align=\"center\" width=\"10%\"><img src=\"ectro_06.jpg\" width=\"450\" height=\"75\"></td>" );
svSendString( cnt, "<td background=\"ectro_12.jpg\" width=\"45%\">&nbsp;</td>" );
svSendString( cnt, "</tr></table></tr></td>" );

svSendString( cnt, "<tr><td align=\"center\"><img src=\"ectro_09.jpg\" width=\"660\" height=\"100\"></td></tr>" );
svSendString( cnt, "<tr><td background=\"ectro_12.jpg\" align=\"center\"><table width=\"660\" height=\"75\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_13.jpg\" align=\"right\" valign=\"middle\"><b>" );

if( !( flags == 1 ) ) {
svSendString( cnt, "<a href=\"/\">Main</a>" );
}
if( !( flags == 2 ) ) {
if( !( flags == 1 ) )
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"register\">Register</a>" );
}
if( !( flags == 3 ) ) {
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"forum\">Forums</a>" );
}
if( !( flags == 4 ) ) {
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"faq\">FAQ</a>" );
}
if( !( flags == 5 ) ) {
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"gettingstarted\">Getting Started</a>" );
}
if( !( flags == 6 ) ) {
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"halloffame\">Hall of fame</a> " );
}
if( !( flags == 7 ) ) {
svSendString( cnt, " | " );
svSendString( cnt, "<a href=\"status\">Server Status</a>" );
}

svSendString( cnt, "</b></td></tr>" );

svSendString( cnt, "</table>" );
svSendString( cnt, "</td></tr></table>" );
svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">" );

return;
}

void iohttpBodyInit( svConnectionPtr cnt, char *title, ... )
{
 char text[4096];
 va_list ap;
 va_start( ap, title );
 vsnprintf( text, 4096, title, ap );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\" background=\"i27.jpg\"><tr><td width=\"10%\"><img height=\"24\" src=\"i25.jpg\" width=\"22\"></td><td width=\"80%\" align=\"center\" nowrap><b><font face=\"verdana\" size=\"2\">" );
 svSendString( cnt, text );
 svSendString( cnt, "</font></b></td><td width=\"10%\" align=\"right\"><img height=\"24\" src=\"i30.jpg\" width=\"62\"></td></tr></table>" );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"7\" background=\"i38.jpg\">&nbsp;</td><td bgcolor=\"#0b1119\" align=\"center\"><br>" );
 return;
}

void iohttpBodyEnd( svConnectionPtr cnt )
{
 svSendString( cnt, "<br></td><td width=\"7\" background=\"i43.jpg\">&nbsp;</td></tr></table><table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"62\"><img height=\"12\" src=\"i45.jpg\" width=\"62\"></td><td width=\"100%\" background=\"i47.jpg\"><img height=\"12\" src=\"i47.jpg\" width=\"1\"></td><td width=\"62\"><img height=\"12\" src=\"i49.jpg\" width=\"62\"></td></tr></table></center><br><br><br></body></html>" );
 return;
}



void iohttpFunc_races( svConnectionPtr cnt )
{
 iohttpBase( cnt, 0 );
 svSendPrintf( cnt, "<br><b>%s races</b><br><br>", SERVERNAME );

 svSendString( cnt, "<h3>Harks</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-10% Energy production<br>+20% Research production <br>-20% Population upkeep reduction<br>+40% Attack strength<br>-10% Defence strength<br>+40% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Fighters units 20% stronger<br>Higher crystal production by 25%<br>Military Research production limit of 250%<br>Halved culture Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Network virus<br>Infiltration<br>Bio infection<br>Military sabotage<br>Nuke Planet<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Incandescence<br>Black Mist<br>War Illusions</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Portal Force Field<br>Vortex Portal<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Manticarias</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+40% Energy production<br>-10% Research production<br>-10% Population upkeep reduction<br>+40% Psychics strength<br>+20% Ghost Ships strength<br>-30% Attack strength<br>+10% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Higher solars collectors efficiency by 15%<br>Doubled culture Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Energy Transfer<br><br><i>Spells</i><br>Dark Web<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Planetary Shielding<br>Mind Control</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Foohons</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-20% Energy production<br>+50% Research production<br>+10% Population upkeep reduction<br>+10% Ghost Ships strength<br>+20% Attack strength<br>+10% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Each 4000 population produces 1 research point each week<br>Ectrolium production increased by 20%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Infiltration<br>Military Sabotage<br>High Infiltration<br>Planetary Beacon<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Psychic Assault</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Vortex Portal<br>Mind Control</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Spacebornes</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+30% Energy production<br>+10% Research production<br>+20% Population upkeep reduction<br>-30% Psychics strength<br>+30% Agents strengths<br>+20% Defence strength<br>+80% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Soldiers and droids units 10% stronger<br>Energy production Research production limit of 250%<br>Halved technology Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Network Virus<br>Bio Infection<br>Energy Transfer<br>Nuke Planet<br>Planetary Beacon<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Incandescence<br>Black Mist</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Survey System<br>Planetary Shielding<br></td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Dreamweavers</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-20% Energy production<br>+40% Research production<br>-10% Population upkeep reduction<br>+50% Psychics strength<br>+30% Ghost Ships strength<br>-30% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Doubled technology Research production <br>Higher crystal production by 25%<br>Construction Research production limit of 250%<br>Halved military Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Network Virus<br>Bio Infection<br>Energy Transfer<br>Military Sabotage<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Portal Force Field<br>Mind Control<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );
/*	
	svSendString( cnt, "<h3>Furtifons</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-10% Attack strength<br>-10% Research production<br>-10% Population upkeep reduction<br>+20% Agents strengths<br>+20% Ghost Ships strength<br>+60% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Culture Research production provides a psychic shield for planets<br>Double Operations research<br>Construction Research production limit of 250%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Energy Transfer<br>Infiltration<br>High Infiltration<br>Planetary Beacon<br><br><i>Spells</i><br>Incandescence<br>War Illusions</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Planetary Shielding<br>Portal Force Field<br>Vortex Portal</td>" );
	svSendString( cnt, "</tr></table><br><br>" );
	
	svSendString( cnt, "<h3>Samsonites</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+30% Attack strength<br>+20% Defence strength<br>+10% Energy production<br>+10% Research production<br>-10% Agents strength<br>-10% Psychics strength<br>-20% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Ignore Dark Webs effects<br>185% Research production max</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Bio Infection<br>Military Sabotage<br>Nuke Planet<br><br><i>Spells</i><br>Black Mist<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Survey System<br>Mind Control<br>Energy Surge</td>" );
	svSendString( cnt, "</tr></table><br><br>" );

	svSendString( cnt, "<h3>Ultimums</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+70% stronger for all units<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Double Research production points production <br>All Research production maximum are 250%<br>Each 4000 population produces a Research production point<br>Higher solars collectors efficiency by 15%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Network Virus<br>Infiltration<br>Bio Infection<br>Energy Transfer<br>Military Sabotage<br><br>Nuke Planet<br>High Infiltration<br>Planetary Beacon<br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Planetary Shielding<br>Portal Force Field<br>Vortex Portal<br>Mind Control<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );
*/
 svSendString( cnt, "<br><br><br>" );

/*
Psychic Assault - attemps to kill the psychics of another faction, causing psychics casualities on both sides.

Sense Artefact - Attempts to locate the nearest artefact, from a particular system
Survey System - Attempt to determine the size, resource bonus and the precense of portals for each planet of a solar system
Planetary Shielding - Create temporary shielding protecting defensive units for a specific planet
Portal Force Field - Create a force field around a specific planet, making portal travel very difficult, reducing the owner capabilities to protect it
Vortex Portal - Create a temporary portal in the targeted system from which fleets can attack and return to the main fleet
Mind Control - Take control of an enemy planet with all its infrastructure, most of the population is killed
Energy Surge - Spreads a destructive wave in an faction network, feeding on the faction's energy reserves ; destroying resource reserves, research and buildings
*/

 return;
}

void iohttpFunc_register( svConnectionPtr cnt ) {

iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 2 );

svSendString ( cnt, "<br><br><h3>Register</h3><br>" );
svSendString ( cnt, "<b>Currently disabled, due to pre-alpha stage.</b>" );

//svSendString( cnt, "<form action=\"register2\" method=\"POST\">User name<br><input type=\"text\" name=\"name\"><br><br>Password<br><input type=\"password\" name=\"pass\"><br><br>Faction name<br><input type=\"text\" name=\"faction\"><br><br><input type=\"submit\" value=\"OK\"></form>" );

iohttpFunc_endhtml( cnt );


return;
}

void iohttpFunc_register2( svConnectionPtr cnt )
{
 int a, i, id;
 int session[4];
 char *name, *pass, *faction;
 FILE *file;
 iohttpDataPtr iohttp;
 char timebuf[256];
 char COREDIR[256];
 long long int newd[DB_USER_NEWS_BASE];
 dbMailDef maild;
 char Message[] = "Congratulations! You have successfully registered your account!<br>Good luck and have fun,<br><br>- Administration";

 iohttpVarsInit( cnt );
/* name = iohttpVarsFind( "name" );
 pass = iohttpVarsFind( "pass" );
 faction = iohttpVarsFind( "faction" );*/
 iohttpVarsCut();
 if( ( name ) && ( pass ) && ( faction ) ) {
	  if( ( id = cmdExecNewUser( name, pass, faction ) ) < 0 ) {
		iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 2 );

		if( cmdErrorString )
			svSendString( cnt, cmdErrorString );
		else
			svSendString( cnt, "Error encountered while registering user" );

		svSendString( cnt, "<br><br><a href=\"register\">Try again?</a>" );
		goto iohttpFunc_register2L0;
	}
  	newd[0] = svTickNum;
	 	newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[2] = CMD_NEWS_MAIL;
  newd[3] = 0;
  newd[4] = 0; //From the admin
  newd[5] = 0; //From the admin team
  memcpy( &newd[6], "Admin", 6 );
  cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_MAIL );
  	
  	
		(maild.mail).length = strlen(Message);
		maild.text = Message;
		(maild.mail).authorid = 0;
  sprintf( (maild.mail).authorname, "Admin" );
  (maild.mail).authorempire = 0;
  (maild.mail).time = time( 0 )-(3600*SERVER_TIME_ZONE);
  (maild.mail).tick = svTickNum;
  (maild.mail).flags = 0;
  if( dbMailAdd( id, 0, &maild ) < 0 )
	syslog(LOG_ERR, "Error sending registration email\n" );

  
  if( ( dbUserLinkDatabase( cnt, id ) < 0 ) || ( dbSessionSet( cnt->dbuser, 0, session ) < 0 ) )
  {
   iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 2 );
   svSendString( cnt, "Error encountered while registering session" );
   goto iohttpFunc_register2L0;
  }
  svSendPrintf( cnt, "Set-Cookie: USRID=%04x%04x%04x%04x%04x; path=/\n", id, session[0], session[1], session[2], session[3] );
  iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 2 );

  svSendPrintf( cnt, "New user created<br>User name : %s<br>Password : %s<br>Faction name : %s<br>Account ID : %d<br>", name, pass, faction, id );

  sprintf( COREDIR, "%s/logs/register", COREDIRECTORY );
  if( ( file = fopen( COREDIR, "ab" ) ) )
  {
   //fprintf( file, "Register ID %d ( %x )\n", id, id );
   a = time(0);
   strftime( timebuf, 256, "%T, %b %d %Y;", localtime( (time_t *)&a ) );
   fprintf( file, "Time %s", timebuf );
   fprintf( file, "Name %s;", name );
   fprintf( file, "Password %s;", pass );
   fprintf( file, "Faction %s;", faction );
   fprintf( file, "IP %s;", inet_ntoa( cnt->sockaddr.sin_addr ) );
   iohttp = cnt->iodata;
   strcpy(timebuf, iohttp->user_agent);
	  for(i=0;i<strlen(timebuf);i++)
	  {
	  	if(timebuf[i] == ';')
	  		timebuf[i] = ',';
	  }
	  fprintf( file, "%s;", timebuf );
   fprintf( file, "Cookie %s;;", iohttp->cookie );
   fprintf(file, "ID : %d ( %X );\n", id, id);
   fclose( file );
  }
} else {
iohttpBase( cnt, 1|8 );
if( ( id = iohttpIdentify( cnt, 4|1 ) ) < 0 )
	return;
iohttpFunc_frontmenu( cnt, 2 );
svSendString( cnt, "This account was not activated yet." );
}

 svSendString( cnt, "<form action=\"register3\" method=\"POST\"><br><br>Empire number<br><i>Leave blank to join a random empire</i><br><input type=\"text\" name=\"empire\"><br><br>" );
 svSendString( cnt, "Empire password<br><i>Only required if defined by the leader of the empire to join.</i><br><input type=\"text\" name=\"fampass\"><br><br>" );
 svSendString( cnt, "Faction race<br><i>The race of your people define many characteristics affecting different aspects of your faction.</i> - <a href=\"races\" target=\"_blank\">See races</a><br><select name=\"race\">" );
 for( a = 0 ; a < CMD_RACE_NUMUSED-1 ; a++ )
  svSendPrintf( cnt, "<option value=\"%d\">%s", a, cmdRaceName[a] );
 svSendString( cnt, "</select><br><br>" );

 svSendString( cnt, "<input type=\"submit\" value=\"OK\"></form>" );

 svSendString( cnt, "<br><br><a href=\"famranks\" target=\"_blank\">See empire rankings</a>" );
 svSendString( cnt, "<br><a href=\"rankings\" target=\"_blank\">See faction rankings</a>" );

 iohttpFunc_register2L0:
 iohttpFunc_endhtml( cnt );

 return;
}

void iohttpFunc_register3( svConnectionPtr cnt )
{
 int a, id, raceid;
 char *empire;
 char *fampass;
 char *race;

 iohttpBase( cnt, 1|8 );
 if( ( id = iohttpIdentify( cnt, 1|4 ) ) < 0 )
  return;
iohttpFunc_frontmenu( cnt, 2 );

 iohttpVarsInit( cnt );
 empire = iohttpVarsFind( "empire" );
 fampass = iohttpVarsFind( "fampass" );
 race = iohttpVarsFind( "race" );
 iohttpVarsCut();

 if( ( empire ) && ( race ) )
 {
  for( a = 0 ; a < 31 ; a++ )
  {
   if( ( fampass[a] == 10 ) || ( fampass[a] == 13 ) )
    break;
  }
		
		if( empire[0] == 0 )
  	a = -1;
  else if( empire[0] == '#' ) 
   sscanf( &empire[1], "%d", &a );
  else
   sscanf( empire, "%d", &a );
  sscanf( race, "%d", &raceid );
  
  if( cmdExecNewUserEmpire( id, a, fampass, raceid, (cnt->dbuser)->level ) < 0 )
  {
   if( cmdErrorString )
    svSendString( cnt, cmdErrorString );
   else
    svSendString( cnt, "Error encountered while registering user" );
   svSendString( cnt, "<br><br><a href=\"/register2\">Try again</a>" );
   goto iohttpFunc_register3L0;
  }
  svSendPrintf( cnt, "<b>Account activated!</b><br>" );
  svSendString( cnt, "<br><br><br><a href=\"/main\">Log in</a>" );
iohttpFunc_endhtml( cnt );
  return;
 }
 else
  svSendString( cnt, "Incorrect query!" );

 iohttpFunc_register3L0:
 svSendString( cnt, "<br><br><a href=\"/\">Main page</a><br><br><a href=\"/login\">Log in</a>" );
iohttpFunc_endhtml( cnt );
 return;
}


void iohttpFunc_login( svConnectionPtr cnt, int flag, char *text, ... ) {
	struct stat stdata;
	char *data;	
	FILE *file;

iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 0 );


if( strlen(text) ) {
	svSendPrintf( cnt, "<br>%s", text );
	if( flag ) {
		if( stat( IOHTTP_READ_DIRECTORY "/login.txt", &stdata ) != -1 ) {
			if( ( data = malloc( stdata.st_size + 1 ) ) ) {
				data[stdata.st_size] = 0;
				if( ( file = fopen( IOHTTP_READ_DIRECTORY "/login.txt", "rb" ) ) ) {
					if( stdata.st_size > 0 ) {
						svSendString( cnt, "<br>" );
						while( fgets( data, stdata.st_size, file ) != NULL ) {
							svSendPrintf( cnt, "%s<br>", data );
						}
					}
					fclose( file );
				}
				free( data );
			}
		}
	}
	svSendString( cnt, "<br><br>" );
} else {
svSendString( cnt, "<br><h3>Login</h3><br>" );
}
svSendString( cnt, "<form action=\"main\" method=\"POST\">Name<br><input type=\"text\" name=\"name\"><br><br>Password<br><input type=\"password\" name=\"pass\"><br><br><input type=\"submit\" value=\"OK\"></form>" );

svSendString( cnt, "</center></body></html>" );

iohttpFunc_endhtml( cnt );

return;
}

void iohttpFunc_endhtml( svConnectionPtr cnt ) {


svSendString( cnt, "</td><td width=\"7%\">&nbsp;</td></tr></table>\n" );
svSendString( cnt, "</center>\n" );
svSendString( cnt, "</body></html>\n" );

return;
}

void iohttpFunc_boxstart( svConnectionPtr cnt, char *title ) {

svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\" background=\"i27.jpg\"><tr><td width=\"10%\"><img height=\"24\" src=\"i25.jpg\" width=\"22\"></td><td width=\"80%\" align=\"center\" nowrap><b><font face=\"verdana\" size=\"2\">" );
svSendPrintf( cnt, "%s", title);
svSendString( cnt, "</font></b></td><td width=\"10%\" align=\"right\"><img height=\"24\" src=\"i30.jpg\" width=\"62\"></td></tr></table><table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"7\" background=\"i38.jpg\">&nbsp;</td><td bgcolor=\"#0b1119\"><br>" );

return;
}

void iohttpFunc_boxend( svConnectionPtr cnt ) {

svSendString( cnt, "<br><br></td><td width=\"7\" background=\"i43.jpg\">&nbsp;</td></tr></table><table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"62\"><img height=\"12\" src=\"i45.jpg\" width=\"62\"></td><td width=\"100%\" background=\"i47.jpg\"><img height=\"12\" src=\"i47.jpg\" width=\"1\"></td><td width=\"62\"><img height=\"12\" src=\"i49.jpg\" width=\"62\"></td></tr></table>" );

return;
}

void iohttpFunc_front( svConnectionPtr cnt, char *text, ...  ) {
	dbUserMainDef maind;
	struct stat stdata;
	char *data;
	FILE *file;
	int id;


iohttpBase( cnt, 1|8 );

if( ( id = iohttpIdentify( cnt, 2 ) ) >= 0 ) {
	if( dbUserMainRetrieve( id, &maind ) < 0 )
	return;
} 

iohttpFunc_frontmenu( cnt, 1 );

if( strlen(text) )
	svSendPrintf( cnt, "<b>%s</b><br><br>", text );

svSendString( cnt, "<tr><td width=\"7%\">&nbsp;</td><td width=\"40%\" valign=\"top\">" );

//notices
int lines = 0;
if( stat( IOHTTP_READ_DIRECTORY "/updates.txt", &stdata ) != -1 ) {
	if( ( data = malloc( stdata.st_size + 1 ) ) ) {
		data[stdata.st_size] = 0;
		if( ( file = fopen( IOHTTP_READ_DIRECTORY "/updates.txt", "rb" ) ) ) {
			if( stdata.st_size > 0 ) {
				while( fgets( data, stdata.st_size, file ) != NULL ) {
					if(lines == 0) {
						iohttpFunc_boxstart( cnt, data );
					} else if (strlen(data) > 1 ) {
						svSendPrintf( cnt, "&nbsp;&nbsp;%s<br>", data );
					}
					if( (lines >= 1) && (strlen(data) == 1 ) ) {
						iohttpFunc_boxend( cnt );
						svSendString( cnt, "<br><br>" );
						lines = 0;
					} else {
						lines++;
					}
				}
			iohttpFunc_boxend( cnt );
			svSendString( cnt, "<br>" );
			svSendString( cnt, "<br>" );
			}
			fclose( file );
		}
	free( data );
	}
}
//end notes

svSendString( cnt, "</td><td width=\"6%\">" );
svSendString( cnt, "&nbsp;" );
svSendString( cnt, "</td><td width=\"40%\" valign=\"top\">" );

svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Log in</b></font></td></tr>" );
svSendString( cnt, "<tr><td>" );
svSendString( cnt, "<table cellspacing=\"8\"><tr><td>" );

if( (id < 0) || (strlen(text)) ) {
	svSendString( cnt, "<font size=\"2\"><form action=\"main\" method=\"POST\">Name<br><input type=\"text\" name=\"name\" size=\"24\"><br><br>Password<br><input type=\"password\" name=\"pass\" size=\"24\"><br><br><input type=\"submit\" value=\"Log in\"></form>" );
} else {
	svSendPrintf( cnt, "<br><b>You are already loged in as <i>%s</i></b><br>", cnt->dbuser->name );
	svSendString( cnt, "<br>" );
	svSendString( cnt, "<a href=\"/main\" target=\"_top\">Proceed to game</a>" );
	svSendString( cnt, "<br>" );
	svSendString( cnt, "<br>" );
	svSendString( cnt, "<a href=\"/logout\" target=\"_top\">Log out</a>" );
	svSendString( cnt, "<br>" );
	svSendString( cnt, "<br>" );
}

svSendString( cnt, "</td></tr></table>" );

if( stat( IOHTTP_READ_DIRECTORY "/todo.txt", &stdata ) != -1 ) {
	if( ( data = malloc( stdata.st_size + 1 ) ) ) {
		data[stdata.st_size] = 0;
		if( ( file = fopen( IOHTTP_READ_DIRECTORY "/todo.txt", "rb" ) ) ) {
			if( stdata.st_size > 0 ) {
			svSendString( cnt, "<i>Items on the admins to-do list :</i>" );
			svSendString( cnt, "<br>" );
			while( fgets( data, stdata.st_size, file ) != NULL ) {
				if( strlen(data) > 1 )
					svSendPrintf( cnt, "&nbsp;&#9734;&nbsp;&nbsp;%s<br>", data );
			}
			svSendString( cnt, "<br>" );
			}
			fclose( file );
		}
		free( data );
	}
}

/*
svSendString( cnt, "<iframe src=\"//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fwww.facebook.com%2Fpages%2FEctroverse%2F133044593518078&amp;send=false&amp;layout=box_count&amp;width=450&amp;show_faces=false&amp;font=segoe+ui&amp;colorscheme=dark&amp;action=like&amp;height=90\" scrolling=\"no\" frameborder=\"0\" style=\"border:none; overflow:hidden; width:450px; height:90px;\" allowTransparency=\"true\"></iframe>\n" );

svSendString( cnt, "<br>\n" );
svSendString( cnt, "<br>\n" );
svSendString( cnt, "<script type=\"text/javascript\">\n" );
svSendString( cnt, "\n" );
svSendString( cnt, "  var _gaq = _gaq || [];\n" );
svSendString( cnt, "    _gaq.push(['_setAccount', 'UA-38148306-1']);\n" );
svSendString( cnt, "      _gaq.push(['_trackPageview']);\n" );
svSendString( cnt, "\n" );      
svSendString( cnt, "        (function() {\n" );
svSendString( cnt, "            var ga = document.createElement('script'); ga.type =\n" );
svSendString( cnt, "            'text/javascript'; ga.async = true;\n" );
svSendString( cnt, "                ga.src = ('https:' == document.location.protocol ?\n" );
svSendString( cnt, "                'https://ssl' : 'http://www') +\n" );
svSendString( cnt, "                '.google-analytics.com/ga.js';\n" );
svSendString( cnt, "                    var s = document.getElementsByTagName('script')[0];\n" );
svSendString( cnt, "                    s.parentNode.insertBefore(ga, s);\n" );
svSendString( cnt, "                      })();\n" );
svSendString( cnt, "                      \n" );
svSendString( cnt, "                      </script>\n" );
svSendString( cnt, "\n" );
svSendString( cnt, "<br>\n" );
*/
iohttpFunc_endhtml( cnt );
return;
}


void iohttpFunc_faq( svConnectionPtr cnt ) {
	struct stat stdata;
	char *data;	
	FILE *file;

iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 4 );

svSendString( cnt, "<tr><td width=\"7%\">&nbsp;</td><td width=\"86%\" valign=\"top\"><table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Frequently Asked Question</b></font></td></tr>" );
svSendString( cnt, "<tr><td>" );

if( stat( IOHTTP_READ_DIRECTORY "/faq.html", &stdata ) != -1 ) {
	if( ( data = malloc( stdata.st_size + 1 ) ) ) {
		data[stdata.st_size] = 0;
		if( ( file = fopen( IOHTTP_READ_DIRECTORY "/faq.html", "rb" ) ) ) {
			fread( data, 1, stdata.st_size, file );
			svSendString( cnt, data );
			fclose( file );
		}
		free( data );
	}
}

svSendString( cnt, "</td></tr></table></td><td width=\"7%\">&nbsp;</td></tr>" );

iohttpFunc_endhtml( cnt );
return;
}


void iohttpFunc_gettingstarted( svConnectionPtr cnt ) {

iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 5 );

svSendString( cnt, "<tr><td width=\"7%\">&nbsp;</td><td width=\"86%\" valign=\"top\">" );


svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Getting started in the galaxy of Ectroverse</b></font></td></tr>" );
svSendString( cnt, "<tr><td><font size=\"2\">" );
svSendString( cnt, "<center>This page is a basic guide to new players on how to get started.<br>It is important to read this to get the best possible experience when first playing!</center>" );

svSendString( cnt, "<br><b>Creating an account:</b><br>" );
svSendString( cnt, "<a href=\"#a0\">0. Registering the account.</a><br>" );
svSendString( cnt, "<a href=\"#a1\">1. The user name and faction name.</a><br>" );
svSendString( cnt, "<a href=\"#a2\">2. Joining an empire.</a><br>" );
svSendString( cnt, "<a href=\"#a3\">3. Choosing your race.</a><br>" );
svSendString( cnt, "<a href=\"#a4\">4. Completion and logging in.</a><br>" );
svSendString( cnt, "</a><br>" );

svSendString( cnt, "<br><b>Playing the game:</b><br>" );
svSendString( cnt, "<a href=\"#b0\">0. Resources & Buildings.</a><br>" );
svSendString( cnt, "<a href=\"#b1\">1. Planets.</a><br>" );
svSendString( cnt, "<a href=\"#b2\">2. Research.</a><br>" );
svSendString( cnt, "<a href=\"#b3\">3. Military.</a><br>" );
svSendString( cnt, "</a><br>" );

svSendString( cnt, "<br><b>Tips:</b><br>" );
svSendString( cnt, "<a href=\"#c0\">0. Map generation.</a><br>" );
svSendString( cnt, "<a href=\"#c1\">1. Fleet page.</a><br>" );
svSendString( cnt, "<a href=\"#c2\">2. Account page.</a><br>" );
svSendString( cnt, "</a><br>" );

svSendString( cnt, "</td></tr></table><br><br>" );


svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Creating an account:</b></font></td></tr>" );
svSendString( cnt, "<tr><td><font size=\"2\"><br>" );

svSendString( cnt, "<a name=\"a0\"><b><i>0. Registering the account.</i></b></a><br>" );
svSendString( cnt, "The great thing about Ectroverse is that the signup is simple. You won't need to input your e-mail address and no other activation is required." );
svSendString( cnt, "<br><br>" );

svSendString( cnt, "<a name=\"a1\"><b><i>1. The user name and faction name.</i></b></a><br>" );
svSendString( cnt, "Your user name is something personal. Other players won't see this.<br>" );
svSendString( cnt, "Your Faction name however, is your handle in the game. Choose carefully.<br>" );
svSendString( cnt, "<br>" );

svSendString( cnt, "<a name=\"a2\"><b><i>2. Joining an Empire.</i></b></a><br>" );
svSendString( cnt, "Being part of an Empire is one of the most important aspects of the game.<br>" );
svSendString( cnt, "You start out small and will need other players to grow.<br>" );
svSendString( cnt, "Know anyone in the game? Ask them for their empire number and password.<br>" );
svSendString( cnt, "<br>" );
svSendString( cnt, "If you want to start an Empire of your own, look at the <a href=\"famranks\">Empire</a> rankings and pick a number not yet in the list!" );
svSendString( cnt, "<br>Want to team up with random players? Leave blank. But remember, be a teamplayer and you'll earn a rank in the Empire.<br>" );
svSendString( cnt, "<br>" );

svSendString( cnt, "<a name=\"a3\"><b><i>3. Choosing your race.</i></b></a><br>" );
svSendString( cnt, "You are well on your way to making a name for yourself.<br>" );
svSendString( cnt, "But how will people remember you? As an aggressive attacker? A proud and rich Energy provider? A self made and self sufficient powerhouse?<br>" );
svSendString( cnt, "<br>" );
svSendString( cnt, "Your race will decide which path you will walk.<br>" );
svSendString( cnt, "View the stats for each race <a href=\"races\">here</a>.<br><br>" );

svSendString( cnt, "<a name=\"a4\"><b><i>4. Completion and logging in.</i></b></a><br>" );
svSendString( cnt, "Congratulations. You have created an account, chosen an Empire to fight for and selected your race.<br>" );
svSendString( cnt, "You are now ready to explore, build and fight your way to the top.<br>" );
svSendString( cnt, "<br>" );

svSendString( cnt, "</td></tr></table><br><br>" );

svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Playing the game:</b></font></td></tr>" );
svSendString( cnt, "<tr><td><font size=\"2\"><br>" );

svSendString( cnt, "<a name=\"b0\"><b><i>0. Resources & Buildings.</i></b></a><br>" );
svSendString( cnt, "Your main concern is growth. You need to spend your resources by making resources.<br>" );
svSendString( cnt, "How do you do that? Simple. Build buildings on your planets.<br>" );
svSendString( cnt, "There are 4 resources in the galaxy. Energy, Minerals, Crystal and Ectrolium. <br>" );
svSendString( cnt, "Most buildings need 3 of the 4 resources to be built, so you need to plan your building a bit at the start.<br><br>" );

svSendString( cnt, "<a name=\"b1\"><b><i>1. Planets.</i></b></a><br>" );
svSendString( cnt, "Exploring and conquering planets is your way of getting to the top.<br>" );
svSendString( cnt, "You start out with 4 exploration ships. Send these out into the galaxy to colonise planets.<br>" );
svSendString( cnt, "Don't worry, you'll be building a lot of them soon enough. Afterall, who is pleased with <i>just</i> 4 planets....<br><br>" );

svSendString( cnt, "<a name=\"b2\"><b><i>2. Research.</i></b></a><br>" );
svSendString( cnt, "Get smarter and do it smart.<br>" );
svSendString( cnt, "Research is a very important aspect of the game.<br>" );
svSendString( cnt, "Head over to your Research tab and distribute points wisely!<br><br>" );

svSendString( cnt, "<a name=\"b3\"><b><i>3. Military.</i></b></a><br>" );
svSendString( cnt, "A big fleet makes you strong, but they also increase your Networth.<br>" );
svSendString( cnt, "A higher networth makes it harder to grow.<br>" );
svSendString( cnt, "Build up your fleet steadily, don't go overboard.<br>" );
svSendString( cnt, "And remember, you need to pay upkeep for your little Cruisers too!<br><br>" );

svSendString( cnt, "</td></tr></table><br><br>" );



svSendString( cnt, "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Tips:</b></font></td></tr>" );
svSendString( cnt, "<tr><td><font size=\"2\"><br>" );

svSendString( cnt, "<a name=\"c0\"><b><i>0. Map generation</i></b></a><br>" );
svSendString( cnt, "The galaxy map is the place where you'll find planets to explore and conquer... But it is easy to lose your way and location." );
svSendString( cnt, "<br>Luckily you can generate a map using the Generation page." );
svSendString( cnt, "<br>You can filter away planets that are already owned, that are of your Empire or are that of enemies!" );
svSendString( cnt, "<br>A usefull tool to say the least.<br><br>" );

svSendString( cnt, "<a name=\"c1\"><b><i>1. Fleet page.</i></b></a><br>" );
svSendString( cnt, "Check your fleet page. In the top right you see <b>\"Fleet Orders\"</b>, select the dropdown and chose <b>\"Wait in System\"</b> instead of the default one." );
svSendString( cnt, "<br>It is easy to forget this option, and spend 10 minutes wondering where your mighty fleet has gone!.<br><br>" );

svSendString( cnt, "<a name=\"c2\"><b><i>2. Account page.</i></b></a><br>" );
svSendString( cnt, "This will here page allows you to alter your Faction name when the game hasn't started yet." );
svSendString( cnt, "<br>Once the time starts flowing, you won't be able to change your name until the next round has started.<br><br>" );


svSendString( cnt, "</td></tr></table><br><br>" );

iohttpFunc_endhtml( cnt );

return;
}

void iohttpFunc_halloffame( svConnectionPtr cnt ) {
	struct stat stdata;
	char *data;	
	FILE *file;

iohttpBase( cnt, 1|8 );
iohttpFunc_frontmenu( cnt, 6 );

svSendString( cnt, "<tr><td width=\"7%\">&nbsp;</td><td width=\"86%\" valign=\"top\"><table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">" );
svSendString( cnt, "<tr><td background=\"ectro_16.jpg\" height=\"15\"><font color=\"#FFFFFF\" size=\"2\"><b>Hall of Fame / Server Rankings</b></font></td></tr>" );
svSendString( cnt, "<tr><td>" );

if( stat( IOHTTP_READ_DIRECTORY "/halloffame.html", &stdata ) != -1 ) {
	if( ( data = malloc( stdata.st_size + 1 ) ) ) {
		data[stdata.st_size] = 0;
		if( ( file = fopen( IOHTTP_READ_DIRECTORY "/halloffame.html", "rb" ) ) ) {
			fread( data, 1, stdata.st_size, file );
			svSendString( cnt, data );
			fclose( file );
		}
		free( data );
	}
}

svSendString( cnt, "</td></tr></table><br><br><br><br><br><br><br><br></td><td width=\"7%\">&nbsp;</td></tr>" );

iohttpFunc_endhtml( cnt );
return;
}




