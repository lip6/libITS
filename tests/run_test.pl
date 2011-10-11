#! /usr/bin/perl


print "Running test : $ARGV[0] \n";
open IN, "< $ARGV[0]";


my $title = <IN>;
chomp $title;
my $call = <IN>;
chomp $call;

my @refverdicts = ();

while (my $line = <IN>) {
  if ($line =~ /.*accepting run.*/ )  {
    chomp $line;
    my $rverdict = $line;
    push (@refverdicts,$rverdict);
  }
}

close IN;

# Now run the tool

my $tmpfile = "$ARGV[0].tmp";


# print "syscalling : $call \n";
print "##teamcity[testStarted name='$title']\n";

my @verdicts = ();

open IN, "($call) |";
while (my $line = <IN>) {
  if ($line =~ /.*accepting run.*/ )  {
    chomp $line;
    my $verdict = $line;
    push (@verdicts,$verdict);
  }
}


foreach my $i  (0..$#refverdicts ) {
  if ( @refverdicts[$i] ne @verdicts[$i] ) {
    print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='formula $i : @refverdicts[$i]' actual='@verdict[$i]'] \n";
  } else {
    print "Test of formula $i successful.\n";
  }
}


print "##teamcity[testFinished name='$title']\n";

