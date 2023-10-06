DELIMITER $$
CREATE DEFINER=`<USERNAME>`@`%` PROCEDURE `new_Weatherdata`(in station varchar(100), in hum real, in temp real, in pres real)
BEGIN
CALL `<DBNAME>`.`new_station`(station);
SET @v1:=(SELECT IDNO FROM T_Stations WHERE StationName = station LIMIT 1);
INSERT INTO `<DBNAME>`.`T_StationData`
(`IDNO_Station`,
`Humidity`,
`Temperature`,
`Pressure`)
VALUES
((SELECT IDNO FROM T_Stations WHERE StationName = station LIMIT 1),
hum,
temp,
pres);
UPDATE `<DBNAME>`.`T_Stations`
SET
`Last_Active` = CURRENT_TIMESTAMP
WHERE `IDNO` = @v1;
 
END$$
DELIMITER ;
