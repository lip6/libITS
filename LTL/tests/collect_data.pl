#! /usr/bin/perl

my $tnum = 1;


# state : 0 out of test, 1 in test group (read ## test title), 2 in subtest (read #subtitle)
my $state=0;
# title for test
my $title="";
# subtitle
my $subtitle="";

while (my $line = <STDIN>) {
  if ($line =~ /^##/) {
    chomp $line;
    $line =~ s/#//g;
    $title = $line;
    $state = 1;
  } elsif ($line =~ /^#/) {
    if ($state == 0) {
      die "Expected a test title at line: $line \n";
    }
    chomp $line;
    $line =~ s/#//g;
    $subtitle = $line;
    $state = 2;
  } else {
    my $datafile = "test_$tnum.data";
    chomp $line;
    open OUT, "> $datafile";
    print OUT  "$title : $subtitle\n";
    print OUT  "$line\n\n";
    close OUT;
    my $call = "($line) >> $datafile";
    print "Creating test data number $tnum : $call \n" ; 
    system $call ;
    $state = 1;
    $subtitle="";
    $tnum++;
  }
}

