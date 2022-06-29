# import  "/data/ythierry/workspaces/mars/fr.lip6.move.gal.benchmark/INPUTS/CircadianClock-PT-000001/CTLFireabilitySimple.gal";
#property CircadianClock-PT-000001-CTLFireabilitySimple-0 
(E(("ma">=1)U(("a">=1)&&("dr">=1))));
#property CircadianClock-PT-000001-CTLFireabilitySimple-1 
(AG((("a">=1)&&("r">=1))&&("c_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-2 
(EF(("ma">=1)&&("a_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-3 
(AX(("c">=1)&&("r_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-4 
(A((("da_a">=1)&&("ma_cap">=1))U(("da_a">=1)&&("ma_cap">=1))));
#property CircadianClock-PT-000001-CTLFireabilitySimple-5 
(EF(("da_a">=1)&&("a_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-6 
(EG("ma">=1));
#property CircadianClock-PT-000001-CTLFireabilitySimple-7 
(EX(("da">=1)&&("ma_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-8 
(AF(("mr">=1)&&("r_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-9 
(AG(("ma">=1)&&("a_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-10 
(AG("r">=1));
#property CircadianClock-PT-000001-CTLFireabilitySimple-11 
(E(((("a">=1)&&("r">=1))&&("c_cap">=1))U(("dr">=1)&&("mr_cap">=1))));
#property CircadianClock-PT-000001-CTLFireabilitySimple-12 
(AF(("dr">=1)&&("mr_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-13 
(EF(("mr">=1)&&("r_cap">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-14 
(A((("da">=1)&&("ma_cap">=1))U("r">=1)));
#property CircadianClock-PT-000001-CTLFireabilitySimple-15 
(AF(("da_a">=1)&&("a_cap">=1)));
