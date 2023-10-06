CREATE DEFINER=`<USERNAME>`@`%` PROCEDURE `new_station`(IN stationname varchar(100))
BEGIN
IF ((SELECT COUNT(*) IDNO FROM T_Stations WHERE T_Stations.StationName = stationname)=0) THEN
    BEGIN
        INSERT INTO T_Stations (StationName) VALUES (stationname);
    END;
END IF;
END$$
DELIMITER ;
