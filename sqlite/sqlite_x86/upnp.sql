PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE [upnp] (
  [enable] INT(1) DEFAULT (0), 
  [netType] INT(1) DEFAULT (0), 
  [auto] INT(1) DEFAULT (0), 
  [updateTime] INT(1) DEFAULT (30), 
  [serverAddr] CHAR(128), 
  [cmdPort] INT(1) DEFAULT (0), 
  [dataPort] INT(1) DEFAULT (0), 
  [webPort] INT(1) DEFAULT (0), 
  [resPort] INT(1) DEFAULT (0), 
  [cmdStat] INT(1) DEFAULT (0), 
  [dataStat] INT(1) DEFAULT (0), 
  [webStat] INT(1) DEFAULT (0), 
  [resStat] INT(1) DEFAULT (0));
INSERT INTO "upnp" VALUES(0,0,0,60,NULL,5000,554,80,0,0,0,0,0);
COMMIT;
