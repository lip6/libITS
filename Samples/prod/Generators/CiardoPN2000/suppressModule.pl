#!/usr/bin/perl

use Getopt::Std;

getopt ('f');

my $usage="Usage:\n suppressModule.pl   -f prod_style_model_file \n Suppresses module information from a net descritption file \n"; 

my $ff = $opt_f or die $usage;

use  strict 'vars';

open (FF, "< $ff") or die "couldn\'t open file $ff for treatment\n" ;
my $ff2 = $ff;
$ff2 =~ s/\.net$/nm\.net/ ;
open (OUT,"> $ff2") or die "couldn't create new file $ff2\n";

while (<FF>) {
  my $line = $_;
  $line =~ s/\#place\s+\(\d+\)/\#place/;
  print OUT $line;
}
close FF;
close OUT;

#unlink $ff;
#system("\\mv $ff.tmp $ff");

