# import  "/data/ythierry/workspaces/mars/fr.lip6.move.gal.benchmark/INPUTS/FMS-PT-005/CTLCardinality.gal";
#property FMS-PT-005-CTLCardinality-0 
(AF(EF(!("P1wP2"<="P2"))));
#property FMS-PT-005-CTLCardinality-1 
(EX((("M1">=2)||(("P2s"<="P3M2")||("P1M1"<="P12wM3")))&&(AG("P12wM3"<="P2wP1"))));
#property FMS-PT-005-CTLCardinality-2 
(AG(((!("P3M2"<="M3"))||(("P12M3"<="M1")||("P2s">=1)))&&(("P3M2">=1)||(("P12"<="P2wP1")||("P1wM1">=2)))));
#property FMS-PT-005-CTLCardinality-3 
(!(EF((("P1s"<="P2M2")&&("P1wP2">=1))&&("P12M3">=3))));
#property FMS-PT-005-CTLCardinality-4 
(AG((EG("P2d"<="P1M1"))||(EF("P12M3"<="M3"))));
#property FMS-PT-005-CTLCardinality-5 
(((AG("P12"<="P12"))&&(E(("P2"<="P2wM2")U("P1s"<="P2"))))&&(AF(AF("P12s"<="P12M3"))));
#property FMS-PT-005-CTLCardinality-6 
(AG(((("P12wM3"<="M3")||("P12"<="M1"))||("P12">=2))||(!(("P1wP2"<="M1")||("M3"<="P2")))));
#property FMS-PT-005-CTLCardinality-7 
(AG(EF(!("P12s"<="P1wP2"))));
#property FMS-PT-005-CTLCardinality-8 
(EF(!(EX("P12"<="P1"))));
#property FMS-PT-005-CTLCardinality-9 
(E((!("P12wM3">=2))U(AG("M3"<="P2wP1"))));
#property FMS-PT-005-CTLCardinality-10 
(EF(AF(("P1d">=3)&&("P2">=3))));
#property FMS-PT-005-CTLCardinality-11 
(AG(AX(!("M3">=3))));
#property FMS-PT-005-CTLCardinality-12 
(!(EF("P12wM3">=3)));
#property FMS-PT-005-CTLCardinality-13 
(E((AX("P3s"<="P3M2"))U(("P2M2">=3)&&("P12s"<="P12s"))));
#property FMS-PT-005-CTLCardinality-14 
(AX(AG(("M3">=1)||("M1">=1))));
#property FMS-PT-005-CTLCardinality-15 
(E(((("P1wM1"<="P12M3")&&("M2">=3))||("P12"<="P3"))U(EG("P1wP2">=3))));
