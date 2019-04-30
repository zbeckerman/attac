// List.cpp: implementation of the CTWGame class.
//

#include "stdafx.h"
#include "ADO.h"
#include "JumpgateServer.h"
#include "TWGame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CTWGame::CTWGame()
{
	//these are allocated in a new thread so do it here
	::CoInitialize(NULL);

	twgsKey=NULL;
	Game=NULL;
	Title=NULL;
	GameOp=NULL;
	Email=NULL;
	ICQ=NULL;
	NextBang=NULL;
	Revision=NULL;
	GoldEnabled=NULL;
	MBBSCompatibility=NULL;
	StartDay=NULL;
	DefinedStartDay=NULL;
	InternalAliens=NULL;
	InternalFerrengi=NULL;
	ClosedGame=NULL;
	ShowStardock=NULL;
	ShipDelayMode=NULL;
	EtherDelayMode=NULL;
	MaxCommands=NULL;
	TurnBase=NULL;
	TimeOnline=NULL;
	InactiveTime=NULL;
	LastBustClearDay=NULL;
	InitialFighters=NULL;
	InitialCredits=NULL;
	InitialHolds=NULL;
	NewPlayerPlanets=NULL;
	DaysTillDeletion=NULL;
	ColonistRegenRate=NULL;
	MaxPlanetSector=NULL;
	MaxCorpMembers=NULL;
	FedSpaceShipLimit=NULL;
	PhotonMissileDuration=NULL;
	PhotonMissilesDisabled=NULL;
	DaysRunning=NULL;
	TradewarsVersion=NULL;
	CloakFailPercent=NULL;
	DebrisLossPercent=NULL;
	TradePercent=NULL;
	StealBuy=NULL;
	ProductionRate=NULL;
	MaxProductionRegen=NULL;
	MultiplePhotons=NULL;
	ClearBustDays=NULL;
	StealFactor=NULL;
	RobFactor=NULL;
	RobDelay=NULL;
	PortProductionMax=NULL;
	RadiationLifetime=NULL;
	FighterLockDecay=NULL;
	Users=NULL;
	Ships=NULL;
	Ports=NULL;
	Planets=NULL;
	MaxCourseLength=NULL;
	TournamentMode=NULL;
	DaysToEnter=NULL;
	MaxTimesBlownUp=NULL;
	Sectors=NULL;
	ActivePlayers=NULL;
	ActiveAliens=NULL;
	PercentPlayersGood=NULL;
	PercentAliensGood=NULL;
	ActivePorts=NULL;
	PortValue=NULL;
	ActivePlanets=NULL;
	PercentPlanetCitadels=NULL;
	ActiveShips=NULL;
	ActiveCorps=NULL;
	ActiveFigs=NULL;
	ActiveMines=NULL;
	Notes=NULL;
	LastUpdatedWhen=NULL;
	LastUpdatedBy=NULL;
	GameActive=NULL;
	RandomFactor=NULL;
	PasswordGame=NULL;
	PasswordEditor=NULL;
	DaysTilDeletion=NULL;
	TavernAnnouncement=NULL;
	LimpetRemoval=NULL;
	ReregisterShip=NULL;
	CitadelTransportUnit=NULL;
	CitadelTransportUpgrade=NULL;
	GenesisTorpedo=NULL;
	ArmidMine=NULL;
	LimpetMine=NULL;
	Beacon=NULL;
	TypeITWarp=NULL;
	TypeIITWarp=NULL;
	TWarpUpgrade=NULL;
	PsychicProbe=NULL;
	PlanetScanner=NULL;
	AtomicDetonator=NULL;
	Corbomite=NULL;
	EtherProbe=NULL;
	PhotonMissile=NULL;
	CloakingDevice=NULL;
	MineDisruptor=NULL;
	HolographicScanner=NULL;
	DensityScanner=NULL;
	FerrengiHomeBase=NULL;
	Stardock=NULL;
	SecondPortZeroLocation=NULL;
	ThirdPortZeroLocation=NULL;

}

CTWGame::~CTWGame()
{
	pfree((void**)&twgsKey);
	pfree((void**)&Game);
	pfree((void**)&Title);
	pfree((void**)&GameOp);
	pfree((void**)&Email);
	pfree((void**)&ICQ);
	pfree((void**)&NextBang);
	pfree((void**)&Revision);
	pfree((void**)&GoldEnabled);
	pfree((void**)&MBBSCompatibility);
	pfree((void**)&StartDay);
	pfree((void**)&DefinedStartDay);
	pfree((void**)&InternalAliens);
	pfree((void**)&InternalFerrengi);
	pfree((void**)&ClosedGame);
	pfree((void**)&ShowStardock);
	pfree((void**)&ShipDelayMode);
	pfree((void**)&EtherDelayMode);
	pfree((void**)&MaxCommands);
	pfree((void**)&TurnBase);
	pfree((void**)&TimeOnline);
	pfree((void**)&InactiveTime);
	pfree((void**)&LastBustClearDay);
	pfree((void**)&InitialFighters);
	pfree((void**)&InitialCredits);
	pfree((void**)&InitialHolds);
	pfree((void**)&NewPlayerPlanets);
	pfree((void**)&DaysTillDeletion);
	pfree((void**)&ColonistRegenRate);
	pfree((void**)&MaxPlanetSector);
	pfree((void**)&MaxCorpMembers);
	pfree((void**)&FedSpaceShipLimit);
	pfree((void**)&PhotonMissileDuration);
	pfree((void**)&PhotonMissilesDisabled);
	pfree((void**)&DaysRunning);
	pfree((void**)&TradewarsVersion);
	pfree((void**)&CloakFailPercent);
	pfree((void**)&DebrisLossPercent);
	pfree((void**)&TradePercent);
	pfree((void**)&StealBuy);
	pfree((void**)&ProductionRate);
	pfree((void**)&MaxProductionRegen);
	pfree((void**)&MultiplePhotons);
	pfree((void**)&ClearBustDays);
	pfree((void**)&StealFactor);
	pfree((void**)&RobFactor);
	pfree((void**)&RobDelay);
	pfree((void**)&PortProductionMax);
	pfree((void**)&RadiationLifetime);
	pfree((void**)&FighterLockDecay);
	pfree((void**)&Users);
	pfree((void**)&Ships);
	pfree((void**)&Ports);
	pfree((void**)&Planets);
	pfree((void**)&MaxCourseLength);
	pfree((void**)&TournamentMode);
	pfree((void**)&DaysToEnter);
	pfree((void**)&MaxTimesBlownUp);
	pfree((void**)&Sectors);
	pfree((void**)&ActivePlayers);
	pfree((void**)&ActiveAliens);
	pfree((void**)&PercentPlayersGood);
	pfree((void**)&PercentAliensGood);
	pfree((void**)&ActivePorts);
	pfree((void**)&PortValue);
	pfree((void**)&ActivePlanets);
	pfree((void**)&PercentPlanetCitadels);
	pfree((void**)&ActiveShips);
	pfree((void**)&ActiveCorps);
	pfree((void**)&ActiveFigs);
	pfree((void**)&ActiveMines);
	pfree((void**)&Notes);
	pfree((void**)&LastUpdatedWhen);
	pfree((void**)&LastUpdatedBy);
	pfree((void**)&GameActive);
	pfree((void**)&RandomFactor);
	pfree((void**)&PasswordGame);
	pfree((void**)&PasswordEditor);
	pfree((void**)&DaysTilDeletion);
	pfree((void**)&TavernAnnouncement);
	pfree((void**)&LimpetRemoval);
	pfree((void**)&ReregisterShip);
	pfree((void**)&CitadelTransportUnit);
	pfree((void**)&CitadelTransportUpgrade);
	pfree((void**)&GenesisTorpedo);
	pfree((void**)&ArmidMine);
	pfree((void**)&LimpetMine);
	pfree((void**)&Beacon);
	pfree((void**)&TypeITWarp);
	pfree((void**)&TypeIITWarp);
	pfree((void**)&TWarpUpgrade);
	pfree((void**)&PsychicProbe);
	pfree((void**)&PlanetScanner);
	pfree((void**)&AtomicDetonator);
	pfree((void**)&Corbomite);
	pfree((void**)&EtherProbe);
	pfree((void**)&PhotonMissile);
	pfree((void**)&CloakingDevice);
	pfree((void**)&MineDisruptor);
	pfree((void**)&HolographicScanner);
	pfree((void**)&DensityScanner);
	pfree((void**)&FerrengiHomeBase);
	pfree((void**)&Stardock);
	pfree((void**)&SecondPortZeroLocation);
	pfree((void**)&ThirdPortZeroLocation);

	//these are allocated in a new thread so do it here
	::CoUninitialize();


}

char* CTWGame::insertIntoDatabase() {

	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;
	char *tempValue=NULL;

	if (this->twgsKey==NULL)
		return false;


	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		sprintf (sqlBuffer, "insert into JUMPGATE_GAMES ( [twgsKey], [Game], [Title], [GameOp], [Email],[ICQ],[NextBang],[Revision],[GoldEnabled],[MBBSCompatibility],");
		strcat (sqlBuffer, "[StartDay],[DefinedStartDay],[InternalAliens],[InternalFerrengi],[ClosedGame],[ShowStardock],[ShipDelayMode],[EtherDelayMode],");
		strcat (sqlBuffer, "[MaxCommands],[TurnBase],[TimeOnline],[InactiveTime],[LastBustClearDay],[InitialFighters],[InitialCredits],[InitialHolds],[NewPlayerPlanets],[DaysTilDeletion],");
		strcat (sqlBuffer, "[ColonistRegenRate],[MaxPlanetSector],[MaxCorpMembers],[FedSpaceShipLimit],[PhotonMissileDuration],[CloakFailPercent],[DebrisLossPercent],[TradePercent],[StealBuy],[ProductionRate],");
		strcat (sqlBuffer, "[MaxProductionRegen],[MultiplePhotons],[ClearBustDays],[StealFactor],[RobFactor],[RobDelay],[PortProductionMax],[RadiationLifetime],[FighterLockDecay],[Users],[Ships],[Ports],[Planets],");
		strcat (sqlBuffer, "[MaxCourseLength],[TournamentMode],[DaysToEnter],[MaxTimesBlownUp],[Sectors],[ActivePlayers],[ActiveAliens],[PercentPlayersGood],[PercentAliensGood],[ActivePorts],[PortValue],[ActivePlanets],");
		strcat (sqlBuffer, "[PercentPlanetCitadels],[ActiveShips],[ActiveCorps],[ActiveFigs],[ActiveMines],[Notes],[Stardock],[PhotonMissilesDisabled],[DaysRunning],[TradewarsVersion],[LastUpdatedWhen],[LastUpdatedBy] ) values (");


		//key
		tempValue = fixSQLValue (this->twgsKey);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Game);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Title);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->GameOp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Email);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ICQ);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");
		
		//key
		tempValue = fixSQLValue (this->NextBang);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Revision);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->GoldEnabled);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MBBSCompatibility);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->StartDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->DefinedStartDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InternalAliens);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InternalFerrengi);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ClosedGame);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ShowStardock);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ShipDelayMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->EtherDelayMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxCommands);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->TurnBase);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->TimeOnline);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InactiveTime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->LastBustClearDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InitialFighters);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InitialCredits);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->InitialHolds);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->NewPlayerPlanets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->DaysTilDeletion);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ColonistRegenRate);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxPlanetSector);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxCorpMembers);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->FedSpaceShipLimit);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PhotonMissileDuration);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->CloakFailPercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->DebrisLossPercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->TradePercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->StealBuy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ProductionRate);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxProductionRegen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MultiplePhotons);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ClearBustDays);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->StealFactor);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->RobFactor);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->RobDelay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PortProductionMax);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->RadiationLifetime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->FighterLockDecay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Users);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Ships);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Ports);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Planets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxCourseLength);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->TournamentMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->DaysToEnter);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->MaxTimesBlownUp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Sectors);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActivePlayers);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");
		
		//key
		tempValue = fixSQLValue (this->ActiveAliens);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PercentPlayersGood);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PercentAliensGood);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActivePorts);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PortValue);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActivePlanets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PercentPlanetCitadels);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActiveShips);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActiveCorps);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActiveFigs);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);
	
		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->ActiveMines);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Notes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->Stardock);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->PhotonMissilesDisabled);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->DaysRunning);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->TradewarsVersion);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->LastUpdatedWhen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//key
		tempValue = fixSQLValue (this->LastUpdatedBy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		//end parentesis
		strcat (sqlBuffer, ");");


		FILE *fp1;

		fp1 = fopen ("c:\\abc.txt","w");
		if (fp1 != NULL) {
			fprintf (fp1, "%s", sqlBuffer);
			fclose (fp1);
		}


		if (cAdo->Execute(sqlBuffer)==0) {
			char *tmp = cAdo->GetErrorMessage();
			MessageBox(NULL, tmp,"Jumpgate Server",MB_ICONERROR);
			pfree((void**)&tmp);
		}

	}

	cAdo->Disconnect();

	return NULL;
	
}

char* CTWGame::updateInDatabase () {
	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;
	char *tempValue=NULL;

	if (this->twgsKey==NULL)
		return false;


	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		sprintf (sqlBuffer, "update JUMPGATE_GAMES set ");

		strcat (sqlBuffer, " [Title]=");

		//key
		tempValue = fixSQLValue (this->Title);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [GameOp]=");

		//key
		tempValue = fixSQLValue (this->GameOp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Email]=");

		//key
		tempValue = fixSQLValue (this->Email);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ICQ]=");

		//key
		tempValue = fixSQLValue (this->ICQ);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [NextBang]=");
		
		//key
		tempValue = fixSQLValue (this->NextBang);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Revision]=");

		//key
		tempValue = fixSQLValue (this->Revision);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [GoldEnabled]=");

		//key
		tempValue = fixSQLValue (this->GoldEnabled);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MBBSCompatibility]=");

		//key
		tempValue = fixSQLValue (this->MBBSCompatibility);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [StartDay]=");

		//key
		tempValue = fixSQLValue (this->StartDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [DefinedStartDay]=");

		//key
		tempValue = fixSQLValue (this->DefinedStartDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InternalAliens]=");

		//key
		tempValue = fixSQLValue (this->InternalAliens);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InternalFerrengi]=");

		//key
		tempValue = fixSQLValue (this->InternalFerrengi);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ClosedGame]=");

		//key
		tempValue = fixSQLValue (this->ClosedGame);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ShowStardock]=");

		//key
		tempValue = fixSQLValue (this->ShowStardock);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ShipDelayMode]=");

		//key
		tempValue = fixSQLValue (this->ShipDelayMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [EtherDelayMode]=");

		//key
		tempValue = fixSQLValue (this->EtherDelayMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxCommands]=");

		//key
		tempValue = fixSQLValue (this->MaxCommands);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [TurnBase]=");

		//key
		tempValue = fixSQLValue (this->TurnBase);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [TimeOnline]=");

		//key
		tempValue = fixSQLValue (this->TimeOnline);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InactiveTime]=");

		//key
		tempValue = fixSQLValue (this->InactiveTime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [LastBustClearDay]=");

		//key
		tempValue = fixSQLValue (this->LastBustClearDay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InitialFighters]=");

		//key
		tempValue = fixSQLValue (this->InitialFighters);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InitialCredits]=");

		//key
		tempValue = fixSQLValue (this->InitialCredits);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [InitialHolds]=");

		//key
		tempValue = fixSQLValue (this->InitialHolds);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [NewPlayerPlanets]=");

		//key
		tempValue = fixSQLValue (this->NewPlayerPlanets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [DaysTilDeletion]=");

		//key
		tempValue = fixSQLValue (this->DaysTilDeletion);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ColonistRegenRate]=");

		//key
		tempValue = fixSQLValue (this->ColonistRegenRate);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxPlanetSector]=");

		//key
		tempValue = fixSQLValue (this->MaxPlanetSector);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxCorpMembers]=");

		//key
		tempValue = fixSQLValue (this->MaxCorpMembers);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [FedSpaceShipLimit]=");

		//key
		tempValue = fixSQLValue (this->FedSpaceShipLimit);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PhotonMissileDuration]=");

		//key
		tempValue = fixSQLValue (this->PhotonMissileDuration);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [CloakFailPercent]=");

		//key
		tempValue = fixSQLValue (this->CloakFailPercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [DebrisLossPercent]=");

		//key
		tempValue = fixSQLValue (this->DebrisLossPercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [TradePercent]=");

		//key
		tempValue = fixSQLValue (this->TradePercent);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [StealBuy]=");

		//key
		tempValue = fixSQLValue (this->StealBuy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ProductionRate]=");

		//key
		tempValue = fixSQLValue (this->ProductionRate);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxProductionRegen]=");

		//key
		tempValue = fixSQLValue (this->MaxProductionRegen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MultiplePhotons]=");

		//key
		tempValue = fixSQLValue (this->MultiplePhotons);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ClearBustDays]=");

		//key
		tempValue = fixSQLValue (this->ClearBustDays);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [StealFactor]=");

		//key
		tempValue = fixSQLValue (this->StealFactor);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [RobFactor]=");

		//key
		tempValue = fixSQLValue (this->RobFactor);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [RobDelay]=");

		//key
		tempValue = fixSQLValue (this->RobDelay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PortProductionMax]=");

		//key
		tempValue = fixSQLValue (this->PortProductionMax);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [RadiationLifetime]=");

		//key
		tempValue = fixSQLValue (this->RadiationLifetime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [FighterLockDecay]=");

		//key
		tempValue = fixSQLValue (this->FighterLockDecay);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Users]=");

		//key
		tempValue = fixSQLValue (this->Users);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Ships]=");

		//key
		tempValue = fixSQLValue (this->Ships);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Ports]=");

		//key
		tempValue = fixSQLValue (this->Ports);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Planets]=");

		//key
		tempValue = fixSQLValue (this->Planets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxCourseLength]=");

		//key
		tempValue = fixSQLValue (this->MaxCourseLength);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [TournamentMode]=");

		//key
		tempValue = fixSQLValue (this->TournamentMode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [DaysToEnter]=");

		//key
		tempValue = fixSQLValue (this->DaysToEnter);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [MaxTimesBlownUp]=");

		//key
		tempValue = fixSQLValue (this->MaxTimesBlownUp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Sectors]=");

		//key
		tempValue = fixSQLValue (this->Sectors);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActivePlayers]=");

		//key
		tempValue = fixSQLValue (this->ActivePlayers);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActiveAliens]=");
		
		//key
		tempValue = fixSQLValue (this->ActiveAliens);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PercentPlayersGood]=");

		//key
		tempValue = fixSQLValue (this->PercentPlayersGood);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PercentAliensGood]=");

		//key
		tempValue = fixSQLValue (this->PercentAliensGood);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActivePorts]=");

		//key
		tempValue = fixSQLValue (this->ActivePorts);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PortValue]=");

		//key
		tempValue = fixSQLValue (this->PortValue);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActivePlanets]=");

		//key
		tempValue = fixSQLValue (this->ActivePlanets);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PercentPlanetCitadels]=");

		//key
		tempValue = fixSQLValue (this->PercentPlanetCitadels);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActiveShips]=");

		//key
		tempValue = fixSQLValue (this->ActiveShips);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActiveCorps]=");

		//key
		tempValue = fixSQLValue (this->ActiveCorps);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ActiveFigs]=");

		//key
		tempValue = fixSQLValue (this->ActiveFigs);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);
	
		strcat (sqlBuffer, ", [ActiveMines]=");

		//key
		tempValue = fixSQLValue (this->ActiveMines);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Notes]=");

		//key
		tempValue = fixSQLValue (this->Notes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Stardock]=");

		//key
		tempValue = fixSQLValue (this->Stardock);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [PhotonMissilesDisabled]=");

		//key
		tempValue = fixSQLValue (this->PhotonMissilesDisabled);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [DaysRunning]=");

		//key
		tempValue = fixSQLValue (this->DaysRunning);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [TradewarsVersion]=");

		//key
		tempValue = fixSQLValue (this->TradewarsVersion);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [LastUpdatedWhen]=");

		//key
		tempValue = fixSQLValue (this->LastUpdatedWhen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [LastUpdatedBy]=");

		//key
		tempValue = fixSQLValue (this->LastUpdatedBy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		//end parentesis
		strcat (sqlBuffer, " WHERE [twgsKey]=");

		//key
		tempValue = fixSQLValue (this->twgsKey);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		//end parentesis
		strcat (sqlBuffer, " AND [Game]=");

		//key
		tempValue = fixSQLValue (this->Game);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);


		FILE *fp1;

		fp1 = fopen ("c:\\ghi.txt","w");
		if (fp1 != NULL) {
			fprintf (fp1, "%s", sqlBuffer);
			fclose (fp1);
		}


		if (cAdo->Execute(sqlBuffer)==0) {
			char *tmp = cAdo->GetErrorMessage();
			MessageBox(NULL, tmp,"Jumpgate Server",MB_ICONERROR);
			pfree((void**)&tmp);
		}

	}

	cAdo->Disconnect();

	return NULL;
}


bool CTWGame::existsInDatabase () {
	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;


	if (this->twgsKey==NULL || this->Game==NULL)
		return false;

	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		char *replaceSQLTWGS = fixSQLValue (this->twgsKey);
		char *replaceSQLGame = fixSQLValue (this->Game);
		sprintf (sqlBuffer, "select count(*) AS theCounter from JUMPGATE_GAMES where twgsKey=%s and Game=%s", replaceSQLTWGS, replaceSQLGame);
		pfree((void **) &replaceSQLTWGS);
		pfree((void **) &replaceSQLGame);

		RecPtr recordPointer = cAdo->ExecuteWithReturn(sqlBuffer);
		if (recordPointer!=NULL) {
			long counter = recordPointer->Fields->GetItem("theCounter")->GetValue();
			if (counter>0) {
				returnValue=true;
			}
		}
	}
	
	cAdo->Disconnect();

	return returnValue;
}

bool CTWGame::insertOrUpdateData() {
	if (existsInDatabase()==true) {
		updateInDatabase();
	} else {
		insertIntoDatabase();
	}
	
	return true;
}