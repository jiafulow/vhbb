DROP TABLE Summary;
CREATE TABLE Summary(
Run  NUMBER NOT NULL,
Mod  VARCHAR2(50) NOT NULL,
ClusterQT   VARCHAR2(1024),
--DBBadStrips VARCHAR2(1024),	 
HotStrips   VARCHAR2(1024),	 
PedTh       VARCHAR2(1024),
PRIMARY KEY(Run,Mod)
);