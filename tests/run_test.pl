#! /usr/bin/perl


print "Running test : $ARGV[0] \n";
open IN, "< $ARGV[0]";


my $title = <IN>;
chomp $title;
my $call = <IN>;
chomp $call;

my @nominal ;

while (my $line = <IN>) {
  if ($line =~ /Model ,\|S\| /) {
    $line = <IN> or die "Unexpected end of file after stats readout";
    chomp $line;
    @nominal = split (/\,/,$line);
    last;
  }
}

close IN;

# Now run the tool

my $tmpfile = "$ARGV[0].tmp";


# print "syscalling : $call \n";
print "##teamcity[testStarted name='$title']\n";

open IN, "$call |";
while (my $line = <IN>) {
#  print "read : $line";
  if ($line =~ /Model ,\|S\| /) {
    $line = <IN> or die "Unexpected end of file after stats readout";
    chomp $line;
    @tested = split (/\,/,$line);
    last;
  }
}

if ( @nominal[1] != @tested[1] ) {
  print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
#  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n";
} else {
  print "Test successful : $title \n";
}


print "##teamcity[testFinished name='$title']\n";

