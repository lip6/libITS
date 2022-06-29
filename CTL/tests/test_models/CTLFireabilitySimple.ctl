# import  "/data/ythierry/workspaces/mars/fr.lip6.move.gal.benchmark/INPUTS/Philosophers-PT-000005/CTLFireabilitySimple.gal";
#property Philosophers-COL-000005-CTLFireabilitySimple-0 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-1 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-2 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-3 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-4 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-5 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-6 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-7 
(AF((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1)));
#property Philosophers-COL-000005-CTLFireabilitySimple-8 
(E((((((("Fork_1">=1)&&("Think_2">=1))||(("Fork_5">=1)&&("Think_1">=1)))||(("Think_4">=1)&&("Fork_3">=1)))||(("Fork_2">=1)&&("Think_3">=1)))||(("Fork_4">=1)&&("Think_5">=1)))U(((((("Fork_1">=1)&&("Think_2">=1))||(("Fork_5">=1)&&("Think_1">=1)))||(("Think_4">=1)&&("Fork_3">=1)))||(("Fork_2">=1)&&("Think_3">=1)))||(("Fork_4">=1)&&("Think_5">=1)))));
#property Philosophers-COL-000005-CTLFireabilitySimple-9 
(E(((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1))U(((((("Catch1_1">=1)&&("Fork_1">=1))||(("Catch1_2">=1)&&("Fork_2">=1)))||(("Fork_5">=1)&&("Catch1_5">=1)))||(("Catch1_3">=1)&&("Fork_3">=1)))||(("Catch1_4">=1)&&("Fork_4">=1)))));
#property Philosophers-COL-000005-CTLFireabilitySimple-10 
(EG(((((("Fork_1">=1)&&("Think_2">=1))||(("Fork_5">=1)&&("Think_1">=1)))||(("Think_4">=1)&&("Fork_3">=1)))||(("Fork_2">=1)&&("Think_3">=1)))||(("Fork_4">=1)&&("Think_5">=1))));
#property Philosophers-COL-000005-CTLFireabilitySimple-11 
(AF(((((("Fork_5">=1)&&("Catch2_1">=1))||(("Catch2_4">=1)&&("Fork_3">=1)))||(("Fork_4">=1)&&("Catch2_5">=1)))||(("Fork_1">=1)&&("Catch2_2">=1)))||(("Fork_2">=1)&&("Catch2_3">=1))));
#property Philosophers-COL-000005-CTLFireabilitySimple-12 
(A((((((("Think_2">=1)&&("Fork_2">=1))||(("Fork_3">=1)&&("Think_3">=1)))||(("Think_1">=1)&&("Fork_1">=1)))||(("Fork_4">=1)&&("Think_4">=1)))||(("Fork_5">=1)&&("Think_5">=1)))U((((("Eat_4">=1)||("Eat_3">=1))||("Eat_2">=1))||("Eat_1">=1))||("Eat_5">=1))));
#property Philosophers-COL-000005-CTLFireabilitySimple-13 
(A((((((("Fork_1">=1)&&("Think_2">=1))||(("Fork_5">=1)&&("Think_1">=1)))||(("Think_4">=1)&&("Fork_3">=1)))||(("Fork_2">=1)&&("Think_3">=1)))||(("Fork_4">=1)&&("Think_5">=1)))U(((((("Fork_1">=1)&&("Think_2">=1))||(("Fork_5">=1)&&("Think_1">=1)))||(("Think_4">=1)&&("Fork_3">=1)))||(("Fork_2">=1)&&("Think_3">=1)))||(("Fork_4">=1)&&("Think_5">=1)))));
#property Philosophers-COL-000005-CTLFireabilitySimple-14 
(E((((((("Catch1_1">=1)&&("Fork_1">=1))||(("Catch1_2">=1)&&("Fork_2">=1)))||(("Fork_5">=1)&&("Catch1_5">=1)))||(("Catch1_3">=1)&&("Fork_3">=1)))||(("Catch1_4">=1)&&("Fork_4">=1)))U(((((("Fork_5">=1)&&("Catch2_1">=1))||(("Catch2_4">=1)&&("Fork_3">=1)))||(("Fork_4">=1)&&("Catch2_5">=1)))||(("Fork_1">=1)&&("Catch2_2">=1)))||(("Fork_2">=1)&&("Catch2_3">=1)))));
