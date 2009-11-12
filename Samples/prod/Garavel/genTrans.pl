#!/usr/bin/perl

use Getopt::Std;

getopt ('f');

my $usage="Usage:\n genTrans.pl   -f transitions-file \n Creates prod style description of transitions \n"; 

my $ff = $opt_f or die $usage;

use  strict 'vars';

open (FF, "< $ff") or die "couldn\'t open file $ff for treatment\n" ;
my $ff2 = "trans.net";
open (OUT,"> $ff2") or die "couldn't create new file $ff2\n";

while (<FF>) {
  my $line = $_;
  $line =~ /\s+(\S+)\s+:\s+(.+)->(.+)/;
  my $inplace = $2;
  my $outplace = $3;
  print OUT "#trans $1\n";
  print OUT "in {";
  foreach my $place (split " ",$inplace) {
    print OUT "$place:<..>;";
  }
  print OUT "}\n";
  print OUT "out {";
  foreach my $place (split " ",$outplace) {
    print OUT "$place:<..>;";
  }
  print OUT "}\n";
  print OUT "#endtr \n"
}
close FF;
close OUT;

