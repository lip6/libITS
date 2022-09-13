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
	# unfortunately, chomp is not enough, because we sometimes run the script
	# from mingW, where Perl interpretation (Unix style) is wrong.
	$line =~ s/[\012\015]//g ; 
    my $rverdict = $line;
    push (@refverdicts,$rverdict);
  }
}

close IN;

# Now run the tool

my $tmpfile = "$ARGV[0].tmp";


print "##teamcity[testStarted name='$title']\n";

my @verdicts = ();

# print "syscalling : $call \n";
open IN, "($call) |";
while (my $line = <IN>) {
  print $line;
  if ($line =~ /.*accepting run.*/ )  {
	# unfortunately, chomp is not enough, because we sometimes run the script
	# from mingW, where Perl interpretation (Unix style) is wrong.
	chomp $line;
	$line =~ s/[\012\015]//g ; 
    my $verdict = $line;
    push (@verdicts,$verdict);
#	print "added verdict : $verdict \n";
  } else {
#	print "NO \n";
  }
}

# retrieve the exit value of the test
my $failure;
if ($? == 0) {
  $failure = 0;
} else {
  $failure = 1;
}

my $returnvalue = 0;

foreach my $i  (0..$#refverdicts ) {
  if ( @refverdicts[$i] ne @verdicts[$i] ) {
    print "##teamcity[testFailed name='$title' message='regression detected' details='formula $i' expected='@refverdicts[$i]' actual='@verdicts[$i]'] \n";
    $returnvalue = 1;
  } elsif ( $failure ) {
    print "@outputs\n";
    print "\n##teamcity[testFailed name='$title' message='test did not exit properly' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
    print "Expected :  @nominal[1]  Obtained :  @tested[1] \n"; 
    my $exitval = $? >> 8;
    print "test exited with value $exitval\n";
    $returnvalue = 99;
  } else {
    print "Test of formula $i successful.\n";
    print "Control Values/Obtained : \n$title\n@refverdicts\n@verdicts\n";
  }
}



print "##teamcity[testFinished name='$title']\n";

exit $returnvalue;

