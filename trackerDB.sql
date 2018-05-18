CREATE TABLE `uomodelfaro` (
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `lat` float DEFAULT NULL,
  `lon` float DEFAULT NULL,
  `speed` float DEFAULT NULL,
  `course` float DEFAULT NULL,
  PRIMARY KEY (`timestamp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

