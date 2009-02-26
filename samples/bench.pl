#!/usr/bin/perl

use Getopt::Std;
use Cwd;
use File::Basename;
use File::Find;

###############
my $nusmv= "~/NuSMV/NuSMV-2.4.3-x86_64-linux-gnu/bin/NuSMV";
my $default_checksog="../src/sog-its";
#############

getopt ('mdetbaq');

#parsing the file list description
my $dir = $opt_d or die "Specify target directory -d please! \n$usage" if (!defined $opt_a);

# command line for tool pnddd
my $cwd =  getcwd or die "Can't get cwd, used to decide where to do output.\n";


my $DEFAULT_TIMEOUT = 120;
# howto use this tool
my $usage="Usage:\n bench.pl -d [directory base] \n"
  . "\n other options : \n"
  . " -e pathTochecksogexe (default : currentDir/$default_checksog \n"
  . " -t timeout (default $DEFAULT_TIMEOUT seconds)\n"
  . " -q disable check-sog comparison, simply compute stats for SDD sog-its implem";

my $checksog_exe;
if (defined $opt_e) {
  if ($opt_e !~ /^\//) {
    # not an absolute path
    $checksog_exe = $cwd.'/'.$opt_e;
  } else {
    $checksog_exe = $opt_e;
  }
} else {
  $checksog_exe = $cwd."/".$default_checksog ;
}

if (! -x $checksog_exe  && !defined $opt_a) {
  die "File \'$checksog_exe\' does not seem to refer to the expected checksog executable. Use option -e.\n$usage";
}

my $timeout = defined $opt_t ? $opt_t :  $DEFAULT_TIMEOUT ;

my $doComparison = 1;
if (defined $opt_q) {
  $doComparison = 0;
}

use strict 'vars';   # After obtaining $opt_xxx, which is not a local variable

my $checksogstate = $cwd."/check-sog" ;

sub workonfile {
  my $ff = $_ ;
  #  Work on all .net source files
  if ( -f $ff && $ff =~/\.net$/  ) {
    print "work on file $ff\n";

    my $totalticks = 0;
    my $totalticks2 = 0;
    my $totalstates = 0;
    my $totalstates2 = 0;
    my $totaltrans = 0;
    my $totaltrans2 = 0;

    my @formulas ;
    my $formff = dirname($ff)."/formula.ltl";
    if ( -r $formff ) {
      push @formulas, $formff;
    }
    $formff = "$ff.ltl";
    if ( -r $formff ) {
      push @formulas, $formff;
    }
    my $nbformula = 0;
    foreach  my $formulaff (@formulas) {
      open IN,$formulaff or die "Bad formula file name";
#      print "Working on formula file : $formulaff \n";
      while (my $line = <IN>) {
	chomp $line;
	$line =~ s/"/\\"/g ;
	$nbformula ++;
#	print "Working on formula : $line \n";
	# ../check-sog -Fformula -c -e invoice.cami.net 1
	my $call = "$checksog_exe -f\"$line\" -c  $ff|";
	
#	print STDERR $call."\n";
	open MYTOOL,$call;
	my $verdict = 0;
	my $nbstates =0;
	my $nbtrans =0;
	my $ticks = 0;
	while (my $outline = <MYTOOL>) {
	  print $outline;
	  if ($outline =~ /(\d+) unique states visited/) {
	    $nbstates = $1;
	  } elsif ($outline =~ /(\d+) ticks for the emptiness/) {
	    $ticks = $1;
	  } elsif ($outline =~ /(\d+) transitions explored/) {
	    $nbtrans = $1;
	  } elsif ($outline =~ /accepting run exists/ ) {
#	    print $outline;
	    $verdict = 1;
	    last;
	  }
	}
	close MYTOOL;

	my $nbstates2 =0;
	my $nbtrans2 =0;
	my $ticks2 = 0;
	my $verdict2 = 0;

	if ($doComparison == 1) {
	  my $call2 = "$checksogstate -f\"$line\" -c  $ff 5 |";
	  #	print STDERR $call2."\n";
	  open MYTOOL2,$call2;

	  while (my $outline = <MYTOOL2>) {
	    if ($outline =~ /(\d+) unique states visited/) {
	      $nbstates2 = $1;
	    } elsif ($outline =~ /(\d+) ticks for the emptiness/) {
	      $ticks2 = $1;
	    } elsif ($outline =~ /(\d+) transitions explored/) {
	      $nbtrans2 = $1;
	    } elsif ($outline =~ /accepting run exists/ ) {
	      #	    print $outline;
	      $verdict2 = 1;
	      last;
	    }
	  }
	  close MYTOOL2;

	  if ($verdict != $verdict2) {
	    print "HOUSTON, we have a problem !!";
	  }
	}

	
	$totaltrans2 += $nbtrans2;
	$totalticks2 += $ticks2 ;
	$totalstates2 += $nbstates2;
	$totaltrans += $nbtrans;
	$totalticks += $ticks ;
	$totalstates += $nbstates;
	
      }
    }
    print "Totals (SDD/BDD) for $nbformula formula : ticks : $totalticks/$totalticks2 ; States : $totalstates/$totalstates2 ; Trans : $totaltrans/$totaltrans2 \n";
  }
}



#main block

find(\&workonfile,$dir);
