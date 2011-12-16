#!/usr/bin/perl
#
# cnf_rename_vars.pl:
#        This perl script renames the variables in the clause according 
#        to order file.
#
# Created: 5 June. 2001, Fadi Aloul (c)
#
#

if(@ARGV != 4) {
  die "USAGE: cnf_rename_vars.pl vars_file order_file output_file\n\t- Var seperated by space or new line in order_file\n\n";
}

open(VAR, $ARGV[0]) || die ("Cant open $argv[0]");
open(CNF, $ARGV[1]) || die ("Cant open $argv[1]");
open(OUT, ">$ARGV[2]") || die ("Cant open $argv[2]");


@new_var = ();


#
# Setup new variables
#

$counter = 1;

my @vars={};

while(<VAR>) {
  chomp;
  push @vars, $_;
}

#
# Read CNF variable & rename, still need to
# rename not-mentioned variables
#
print OUT "#TYPE ". @ARGV[3]. "\n";
while(<CNF>) {
  print OUT "$vars[$_]\n";
}
print OUT "#END\n";

close OUT;
close CNF;
close VAR;
