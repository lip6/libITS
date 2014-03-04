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


foreach my $i  (0..$#refverdicts ) {
  if ( @refverdicts[$i] ne @verdicts[$i] ) {
    print "##teamcity[testFailed name='$title' message='regression detected' details='formula $i' expected='@refverdicts[$i]' actual='@verdicts[$i]'] \n";
  } else {
    print "Test of formula $i successful.\n";
  }
}


print "##teamcity[testFinished name='$title']\n";

