// List.hinterface for the CTWGame class.
//

#if !defined(AFX_LIST_H__661E_A76D_EB38C9EE2028__INCLUDED_)
#define AFX_LIST_H__661E_A76D_EB38C9EE2028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTWGame
{
protected:

public:
	CTWGame();
	virtual ~CTWGame();

	char* insertIntoDatabase();
	char* updateInDatabase ();
	bool existsInDatabase ();
	bool insertOrUpdateData();

	char *twgsKey;
	char *Game;
	char *Title;
	char *GameOp;
	char *Email;
	char *ICQ;
	char *NextBang;
	char *Revision;
	char *GoldEnabled;
	char *MBBSCompatibility;
	char *StartDay;
	char *DefinedStartDay;
	char *InternalAliens;
	char *InternalFerrengi;
	char *ClosedGame;
	char *ShowStardock;
	char *ShipDelayMode;
	char *EtherDelayMode;
	char *MaxCommands;
	char *TurnBase;
	char *TimeOnline;
	char *InactiveTime;
	char *LastBustClearDay;
	char *InitialFighters;
	char *InitialCredits;
	char *InitialHolds;
	char *NewPlayerPlanets;
	char *DaysTillDeletion;
	char *ColonistRegenRate;
	char *MaxPlanetSector;
	char *MaxCorpMembers;
	char *FedSpaceShipLimit;
	char *PhotonMissileDuration;
	char *PhotonMissilesDisabled;
	char *DaysRunning;
	char *TradewarsVersion;
	char *CloakFailPercent;
	char *DebrisLossPercent;
	char *TradePercent;
	char *StealBuy;
	char *ProductionRate;
	char *MaxProductionRegen;
	char *MultiplePhotons;
	char *ClearBustDays;
	char *StealFactor;
	char *RobFactor;
	char *RobDelay;
	char *PortProductionMax;
	char *RadiationLifetime;
	char *FighterLockDecay;
	char *Users;
	char *Ships;
	char *Ports;
	char *Planets;
	char *MaxCourseLength;
	char *TournamentMode;
	char *DaysToEnter;
	char *MaxTimesBlownUp;
	char *Sectors;
	char *ActivePlayers;
	char *ActiveAliens;
	char *PercentPlayersGood;
	char *PercentAliensGood;
	char *ActivePorts;
	char *PortValue;
	char *ActivePlanets;
	char *PercentPlanetCitadels;
	char *ActiveShips;
	char *ActiveCorps;
	char *ActiveFigs;
	char *ActiveMines;
	char *Notes;
	char *LastUpdatedWhen;
	char *LastUpdatedBy;
	char *GameActive;
	char *RandomFactor;
	char *PasswordGame;
	char *PasswordEditor;
	char *DaysTilDeletion;
	char *TavernAnnouncement;
	char *LimpetRemoval;
	char *ReregisterShip;
	char *CitadelTransportUnit;
	char *CitadelTransportUpgrade;
	char *GenesisTorpedo;
	char *ArmidMine;
	char *LimpetMine;
	char *Beacon;
	char *TypeITWarp;
	char *TypeIITWarp;
	char *TWarpUpgrade;
	char *PsychicProbe;
	char *PlanetScanner;
	char *AtomicDetonator;
	char *Corbomite;
	char *EtherProbe;
	char *PhotonMissile;
	char *CloakingDevice;
	char *MineDisruptor;
	char *HolographicScanner;
	char *DensityScanner;
	char *FerrengiHomeBase;
	char *Stardock;
	char *SecondPortZeroLocation;
	char *ThirdPortZeroLocation;

};

#endif // !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_)
