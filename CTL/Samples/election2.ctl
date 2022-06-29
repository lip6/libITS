AF( second_negociation_phase_ID0 = 1);
!EG(!second_negociation_phase_ID0 = 1);
AF( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1);
AF( second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1);AG( second_negociation_phase_ID0 = 1);
!EF(!second_negociation_phase_ID0 = 1);
AG( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1);
AG( second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1);A( init_ID0 = 1 U second_negociation_phase_ID0 = 1 );
!((E(!(second_negociation_phase_ID0=1) U (!(init_ID0=1) * !(second_negociation_phase_ID0=1))) + EG(!(second_negociation_phase_ID0=1))));
!E( !init_ID0 = 1 U !second_negociation_phase_ID0 = 1 );
A( init_ID0 = 1 * init_ID1 = 1 U second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
A( init_ID0 = 1 + init_ID1 = 1  U second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1 );first_negociation_phase_ID0 = 1;
second_negociation_phase_ID0 = 1;
EG( second_negociation_phase_ID0 = 1);
EG( first_negociation_phase_ID0 = 1);
EG( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1);
EG( second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1);! AF(first_negociation_phase_ID0 = 1);
EG(!(first_negociation_phase_ID0 = 1));

! EF(first_negociation_phase_ID0 = 1);
AG(!(first_negociation_phase_ID0 = 1));

!AX(first_negociation_phase_ID0 = 1);
EX(!(first_negociation_phase_ID0 = 1));

AF(first_negociation_phase_ID0 = 1);
A(TRUE U first_negociation_phase_ID0 = 1);

EF(first_negociation_phase_ID0 = 1);
E(TRUE U first_negociation_phase_ID0 = 1);

AG(first_negociation_phase_ID0 = 1);
first_negociation_phase_ID0 = 1 * AX (AG(first_negociation_phase_ID0 = 1));

EG(first_negociation_phase_ID0 = 1);
first_negociation_phase_ID0 = 1 * EX(EG(first_negociation_phase_ID0 = 1));

EF(first_negociation_phase_ID0 = 1);
first_negociation_phase_ID0 = 1 + EX(EF(first_negociation_phase_ID0 = 1));(init_ID0=1 -> first_negociation_phase_ID0 = 1);
(init_ID0=1 -> first_negociation_phase_ID0 = 0);
(EX(init_ID0=1) -> first_negociation_phase_ID0 = 1);
(init_ID0=1 -> EX(first_negociation_phase_ID0 = 1));
(EX(init_ID0=1) -> EX(first_negociation_phase_ID0 = 1));EX(init_ID0=1);
EF(init_ID0=1);!EX(TRUE);
!EX(!(FALSE));EX(TRUE);
(TRUE);
(FALSE);
!(TRUE);
!(FALSE);EX(second_negociation_phase_ID0 = 1);
EX(second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1 );
EX( (second_negociation_phase_ID0 = 1) * (second_negociation_phase_ID1 = 1));EF( second_negociation_phase_ID0 = 1);
EF( second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1 );
EF( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );E( init_ID0 = 1 U second_negociation_phase_ID0 = 1 );
( init_ID0 = 1 * init_ID1 = 1 );
( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
EX( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
EF( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
(( init_ID0 = 1 * init_ID1 = 1 ) * ( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 ));
( init_ID0 = 1 * init_ID1 = 1 ) *  EX( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
E( init_ID0 = 1 * init_ID1 = 1 U second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );
(( init_ID0 = 1 * init_ID1 = 1 ) *  EX( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 )) + ( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 ) ;
E( init_ID0 = 1 + init_ID1 = 1  U second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1 );
EX(second_negociation_phase_ID0 = 1);
AX( second_negociation_phase_ID0 = 1);
!EX(!second_negociation_phase_ID0 = 1);
AX( second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1);
AX( second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1);
AX(!(second_negociation_phase_ID0 = 1 + second_negociation_phase_ID1 = 1));
AX( second_negociation_phase_ID0 = 1 * init_ID1 = 1);E( init_ID0 = 1 * init_ID1 = 1 U second_negociation_phase_ID0 = 1 * second_negociation_phase_ID1 = 1 );