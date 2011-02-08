#! /usr/bin/perl


print "Running test : $ARGV[0] \n";
open IN, "< $ARGV[0]";


my $title = <IN>;
chomp $title;
my $call = <IN>;
chomp $call;

my @nominal ;
my @refverdicts = ();

while (my $line = <IN>) {
  if ($line =~ /reachable,/) {
    chomp $line;
    @nominal = split (/\,/,$line);
  } elsif ($line =~ /^Formula is (\w+) \!/ )  {
    my $verdict = ($1 == TRUE);
    push (@refverdicts,$verdict);
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
  if ($line =~ /reachable,/) {
    chomp $line;
    @tested = split (/\,/,$line);
  } elsif ($line =~ /^Formula is (\w+) \!/ )  {
    my $verdict = ($1 == TRUE);
    push (@verdicts,$verdict);
  }
}


if ( @nominal[1] != @tested[1] ) {
  print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
#  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n";
} else {
  print "Test successful : $title -> reachable state space size \n";
}

foreach my $i  (0..$#refverdicts ) {
  if ( @refverdicts[$i] != @verdicts[$i] ) {
     print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='formula $i : @refverdicts[$i]' actual='@verdict[$i]'] \n";
     last;
  } else {
    print "Test of formula $i successful.\n";
  }
}


print "##teamcity[testFinished name='$title']\n";

