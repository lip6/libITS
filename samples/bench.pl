#!/usr/bin/perl

use Getopt::Std;
use Cwd;
use File::Basename;


use strict 'vars';   # After obtaining $opt_xxx, which is not a local variable


my $nusmv= "~/NuSMV/NuSMV-2.4.3-x86_64-linux-gnu/bin/NuSMV";
my $checksog="../check-sog";
my $formulaff="./philo/formula.ltl";
my $model="./philo/philo20";

open IN,$formulaff or die "Bad formula file name";
while (my $line = <IN>) {
  chomp $line;
  # ../check-sog -Fformula -c -e invoice.cami.net 1
  my $call = "$checksog -f\"$line\" -c  $model.net 1";

  print STDERR $call."\n";
  system($call);
}



