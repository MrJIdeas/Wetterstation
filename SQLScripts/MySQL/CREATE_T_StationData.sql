CREATE TABLE `T_StationData` (
  `INSERT_TS` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IDNO_Station` bigint NOT NULL DEFAULT '-1',
  `Humidity` double NOT NULL DEFAULT '-1',
  `Temperature` double NOT NULL DEFAULT '-1',
  `Pressure` double NOT NULL DEFAULT '-1',
  PRIMARY KEY (`INSERT_TS`,`IDNO_Station`),
  KEY `IDNO_Station_idx` (`IDNO_Station`),
  CONSTRAINT `IDNO_Station` FOREIGN KEY (`IDNO_Station`) REFERENCES `T_Stations` (`IDNO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
