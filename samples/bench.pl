#!/usr/bin/perl

use Getopt::Std;
use Cwd;
use File::Basename;
use File::Find;

###############
my $nusmv= "~/NuSMV/NuSMV-2.4.3-x86_64-linux-gnu/bin/NuSMV";
my $default_checksog="../src/sog-its";
#############

getopt ('mdetba');

#parsing the file list description
my $dir = $opt_d or die "Specify target directory -d please! \n$usage" if (!defined $opt_a);

# command line for tool pnddd
my $cwd =  getcwd or die "Can't get cwd, used to decide where to do output.\n";


my $DEFAULT_TIMEOUT = 120;
# howto use this tool
my $usage="Usage:\n bench.pl -d [directory base] \n"
  . "\n other options : \n"
  . " -e pathTochecksogexe (default : currentDir/$default_checksog \n"
  . " -t timeout (default $DEFAULT_TIMEOUT seconds)";

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

use strict 'vars';   # After obtaining $opt_xxx, which is not a local variable


sub workonfile {
  my $ff = $_ ;
  #  Work on all .net source files
  if ( -f $ff && $ff =~/\.net$/  ) {
    print "work on file $ff\n";
    my @formulas ;
    my $formff = dirname($ff)."/formula.ltl";
      print "using formula file : $formff";
    if ( -r $formff ) {

      push @formulas, $formff;
    }
    $formff = "$ff.ltl";
    if ( -r $formff ) {
      push @formulas, $formff;
    }
    foreach  my $formulaff (@formulas) {
      open IN,$formulaff or die "Bad formula file name";
      while (my $line = <IN>) {
	chomp $line;
	# ../check-sog -Fformula -c -e invoice.cami.net 1
	my $call = "$checksog_exe -f\"$line\" -c  $ff";
	
	print STDERR $call."\n";
	system($call);
      }
    }
  }
}



#main block

find(\&workonfile,$dir);
