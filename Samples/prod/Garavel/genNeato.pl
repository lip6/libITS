#!/usr/bin/perl

use Getopt::Std;

getopt ('f');

my $usage="Usage:\n genTrans.pl   -f transitions-file \n Creates prod style description of transitions \n"; 

my $ff = $opt_f or die $usage;

use  strict 'vars';

open (FF, "< $ff") or die "couldn\'t open file $ff for treatment\n" ;
my $ff2 = "model.neato";
open (OUT,"> $ff2") or die "couldn't create new file $ff2\n";

print OUT "graph g { \n";

while (<FF>) {
  my $line = $_;
  $line =~ /\s+(\S+)\s+:\s+(.+)->(.+)/;
  my $trans = $1;
  my $inplace = $2;
  my $outplace = $3;
  print OUT "         $trans  [shape=\"box\"]; \n";
  foreach my $place (split " ",$inplace) {
    print OUT "$place -- $trans [dir=forward] ; \n";
  }
  foreach my $place (split " ",$outplace) {
    print OUT "$trans -- $place [dir=forward] ; \n";
  }

}
print OUT "} \n";
close FF;
close OUT;

