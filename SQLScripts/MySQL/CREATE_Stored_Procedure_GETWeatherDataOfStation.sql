DELIMITER $$
CREATE DEFINER=`<USERNAME>`@`%` PROCEDURE `get_WeatherDataOfStation`(in stationID bigint,in beg datetime, in ende datetime)
BEGIN
select * FROM T_StationData WHERE IDNO_Station = stationid AND INSERT_TS>beg AND insert_ts < ende;
END$$
DELIMITER ;
