#! /usr/bin/perl

print "Running test : $ARGV[0] \n";
open IN, "< $ARGV[0]";


my $title = <IN>;
chomp $title;
my $call = <IN>;
chomp $call;

my $header;
my @nominal ;

while (my $line = <IN>) {
  if ($line =~ /Model ,\|S\| /) {
      $header = $line;
      $line = <IN> or die "Unexpected end of file after stats readout";
      chomp $line;
      @nominal = split (/\,/,$line);
      last;
  }
}

close IN;

# Now run the tool# 

# print "syscalling : $call \n";
print "##teamcity[testStarted name='$title']\n";

my @tested;

my @outputs = ();
my @results = `$call`;

while (my $line = shift(@results)) {
#  print "read : $line";
  push (@outputs,$line);
  if ($line =~ /Model ,\|S\| /) {
    $line = shift(@results) or die "Unexpected end of file after stats readout";
    chomp $line;
    @tested = split (/\,/,$line);
    last;
  }
}

# retrieve the exit value of the test
my $failure;
if ($? == 0) {
  $failure = 0;
} else {
  $failure = 1;
}

my $returnvalue;

if ( @nominal[1] != @tested[1] ) {
  print "@outputs\n";
  print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n";
  $returnvalue = 1;
} elsif ( $failure ) {
  print "@outputs\n";
  print "\n##teamcity[testFailed name='$title' message='test did not exit properly' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n"; 
  my $exitval = $? >> 8;
  print "test exited with value $exitval\n";
  $returnvalue = 99;
} else {
  print "##teamcity[buildStatisticValue key='testDuration' value='@tested[2]']\n";
  print "##teamcity[buildStatisticValue key='testMemory' value='@tested[3]']\n";
  print "Test successful : $title \n";
  print "Control Values/Obtained : \n$title\n@nominal\n@tested\n";
  $returnvalue = 0;
}


print "##teamcity[testFinished name='$title']\n";

exit $returnvalue;
