#! /usr/bin/perl

my $tnum = 1;


while (my $line = <STDIN>) {
  my $datafile = "test_$tnum.data";
  chomp $line;
  my $title = $line;
  $line = <STDIN> or die "badly formatted test file; we expect a sequence of : title \\n test \\n \n";
  chomp $line;
  open OUT, "> $datafile";
  print OUT  "$title\n";
  print OUT  "$line\n\n";
  
  my @tested;
  my $call = "$line";
  print "##teamcity[testStarted name='$title']\n";
  open IN, "($call) |" or die "An exception was raised when attempting to run "+$call+"\n";
	while (my $line = <IN>) {
		print OUT "$line";
  	if ($line =~ /Model ,\|S\| /) {
    	$line = <IN> or die "Unexpected end of file after stats readout";
    	chomp $line;
    	@tested = split (/\,/,$line);
  	}
	}
	close IN;
	close OUT;
	print "##teamcity[buildStatisticValue key='testDuration_$title' value='@tested[2]']\n";
  print "##teamcity[buildStatisticValue key='testMemory_$title' value='@tested[3]']\n";
  print "##teamcity[testFinished name='$title']\n";
  $tnum++;
}

system 'echo "Data collected on (machine/date):" > data.info';
system "(uname -a ; date) >> data.info";
system 'echo "Configure run with :" >> data.info';
system "head -8 ../config.log >> data.info";
system 'echo "Using revision :" >> data.info';
system 'svn info >> data.info';
