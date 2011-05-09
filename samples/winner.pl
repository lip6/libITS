#!/usr/bin/perl -w
use Getopt::Std;
use strict;
use File::Basename;

if ($#ARGV <= 0)
{
    print STDERR "syntax: graphdata.pl [-m|-l|-a] column files...

  -m   output min/max summary
  -l   output min/max summary in LaTeX form
  -l   output min/max summary in LaTeX form for ATVA
";
    exit(2);
}

my %verdict = ( 0 => "empty", 1 => "non-empty", 2 => "unknown" );


my $opt_m = 0;
my $opt_l = 0;
my $opt_a = 0;
if ($ARGV[0] eq '-m')
{
    $opt_m = 1;
    shift @ARGV;
} elsif ($ARGV[0] eq '-l')
{
    $opt_m = 1;
    $opt_l = 1;
    shift @ARGV;
} elsif ($ARGV[0] eq '-a')
{
    $opt_m = 1;
    $opt_l = 1;
    $opt_a = 1;
    shift @ARGV;
}

my $opt_c = (shift @ARGV) - 3;

my $max = 0;

my $count = 0;
my $fail = 0;

my $head = <>;
my @head = split(',', $head);
shift @head;
shift @head;
shift @head;

my %result;

my %Max;
my %Min;
my %Fail;
my %total;

while (<>)
{
    next if ($_ =~ /^\w*$/) or ($_ eq $head);

    chomp;

    my @res = split(',',$_);
    my $meth = shift @res;

    # ignore FSOWCTY and SOP for ATVA
    next if ($opt_a && (($meth eq 'FSOWCTY') || ($meth eq 'SOP')));

    my $model = basename(shift @res);
    my $formula = shift @res;

    # Sometimes the formula is erroneously empty.  Skip it.
    next if $formula =~ 's/^\s*("")?\s*$/';

    #next if $res[3] !~ /^\s*[$opt_v]\s*$/;
    if (defined $res[$opt_c] && $res[$opt_c] !~ /^\s*$/) {
	if ($res[$opt_c] > $max) {
	    $max = $res[$opt_c];
	}
    } else {
      $res[$opt_c] = -1;
    }

    $result{"$model,$formula"}{$meth} = [@res];
    $Max{$meth} = 0;
    $Min{$meth} = 0;
    $Max{$meth} = { 0 => 0, 1 => 0, 2 => 0 };
    $Min{$meth} = { 0 => 0, 1 => 0, 2 => 0 };
    $Fail{$meth} = { 0 => 0, 1 => 0, 2 => 0};
    $total{$meth} = { 0 => 0, 1 => 0, 2 => 0};
}

my @allmeths = sort(keys(%Max));

my $skipped = 0;
my $read = 0;
my $toolow = 0;

foreach my $key (keys %result)
{
    my $min_ = 999999999999999999999999;
    my $max_ = 0;  # including failures
    my $fmax_ = 0; # excluding failures
    my $verdict = 999999999999999999999999;

    $read++;

    my $allhere = 1;
    for my $meth (@allmeths)
    {
	next if exists $result{$key}{$meth};
	$allhere = 0;
	# print "skipped $key with only @{[keys %{$result{$key}}]}\n";
	$skipped++;
	last;
    }
    next unless $allhere;

    for my $meth (keys %{$result{$key}})
    {
	my $t = $result{$key}{$meth};
	my $v = int($t->[3]);
	my $val = $t->[$opt_c];
	if ((! defined $val) || $val =~ /^\s*$/ || $val == -1 || $v == 2) {
	    $val = 3 * $max;
	    $v = 2;
	} else {
	    $fmax_ = $val if ($val > $fmax_);
	}
	$min_ = $val if ($val < $min_);
	$max_ = $val if ($val > $max_);
	$verdict = $v if $verdict > $v;
    }

    if ($fmax_ < 0.1)
    {
	++$toolow;
	next;
    }


    for my $meth (keys %{$result{$key}})
    {
	my $t = $result{$key}{$meth};
	my $v = int($t->[3]);
	my $val = $t->[$opt_c];
	my $score = 120;
	if ((! defined $val) || $val =~ /^\s*$/ || $val == -1 || $v == 2) {
	    $val = 3 * $max;
	    $Fail{$meth}{$verdict}++;
	} else {
	    $score = 100 * $val / $fmax_;
	}
	$Max{$meth}{$verdict}++ if ($val >= $max_);
	$Min{$meth}{$verdict}++ if ($val == $min_);
	$total{$meth}{$verdict}++;
	if (!$opt_m)
	{
	    print "$meth,$verdict,$score\n";
	}
    }
}


exit unless $opt_m;


@allmeths = qw(BCZ99 FSOWCTY FSEL SOG SOP SLAP SLAP-FST);
@allmeths = qw(BCZ99 FSEL SOG SLAP SLAP-FST) if $opt_a;

if ($opt_l)
{
    print "&&";
    print "& \\multicolumn{2}{c}{$_} " foreach (@allmeths);
    print "\\\\\n";
    for my $v (0, 1)
    {
	print "";
	if ($v) { printf "\\cline{2-%d}\n%-13s", 3+2*@allmeths, "& non empty "; }
	else { printf "\\hline\n%-13s", "& empty "; }
	print "& Win   ";
	for my $meth (@allmeths)
	{
	    if (exists $Min{$meth})
	    {
		printf "& %6d & %6s ", $Min{$meth}{$v}, "(" . int(100*$Min{$meth}{$v}/$total{$meth}{$v}) . "\\%)";
	    }
	    else
	    {
		printf "& %6s & %6s ", "", "";
	    }


	}
	print "\\\\\n";
	printf "&%12s", "($total{SLAP}{$v} cases)";
	print "& Lose  ";
	for my $meth (@allmeths)
	{
	    if (exists $Max{$meth})
	    {
		printf "& %6d & %6s ", $Max{$meth}{$v}, "(" . int(100*$Max{$meth}{$v}/$total{$meth}{$v}) . "\\%)";
	    }
	    else
	    {
		printf "& %6s & %6s ", "", "";
	    }
	}
	print "\\\\\n&            & Fail  ";
	for my $meth (@allmeths)
	{
	    if (exists $Fail{$meth})
	    {
		printf "& %6d & %6s ", $Fail{$meth}{$v}, "(" . int(100*$Fail{$meth}{$v}/$total{$meth}{$v}) . "\\%)";
	    }
	    else
	    {
		printf "& %6s & %6s ", "", "";
	    }
	}
	print "\\\\\n";
    }
    exit;
}

my $meth;
my $v;
format TEXTOUT =
@<<<<<<<<<< @###### @##% @###### @##% @###### @##% @#######
$meth, $Min{$meth}{$v}, 100*$Min{$meth}{$v}/$total{$meth}{$v}, $Max{$meth}{$v}, 100*$Max{$meth}{$v}/$total{$meth}{$v}, $Fail{$meth}{$v}, 100*$Fail{$meth}{$v}/$total{$meth}{$v},$total{$meth}{$v}
.

print "$skipped incomplete experiments skipped, out of $read read.\n"
  if $skipped;
print "$toolow experiments skipped because of values too low.\n"
  if $toolow;
my $one = (keys %Fail)[0];
print "$Fail{$one}{2} experiments skipped because they failed with all techniques.\n" if $Fail{$one}{2};

print "
No counterexamples
             ----Min----  ----Max----  ----Fail---    Total\n";

$~ = 'TEXTOUT';
$v = 0;
for $meth (sort { $Min{$b}{$v} <=> $Min{$a}{$v} } (keys %Min))
{
    write;
#    printf "%11s %10s", $m, "$Min{$meth}{0}/$total{$meth}{0}\n";
}

print "
With counterexamples
             ----Min----  ----Max----  ----Fail---    Total\n";
$v = 1;
for $meth (sort { $Min{$b}{$v} <=> $Min{$a}{$v} } (keys %Min))
{
    write;
#    printf "%11s %10s", $m, "$Min{$meth}{0}/$total{$meth}{0}\n";
}
