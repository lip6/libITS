#!/usr/bin/perl -w
use Getopt::Std;
use strict;
use File::Basename;

if ($#ARGV <= 0)
{
    print STDERR "syntax: graphdata.pl [-m] column files...

  -m   output min/max summary
";
    exit(2);
}

my %verdict = ( 0 => "empty", 1 => "non-empty", 2 => "unknown" );


my $opt_m = 0;
if ($ARGV[0] eq '-m')
{
    $opt_m = 1;
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
    my $model = basename(shift @res);
    my $formula = shift @res;
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

my $nmeths = int(keys(%Max));

my $awin0=0;
my $bwin0=0;
my $awin1=0;
my $bwin1=0;
my $awin2=0;
my $bwin2=0;
my $nbvalues =0;
my $faila=0;
my $failb=0;
my $failureanotb=0;
my $failurebnota=0;
my $fullfail=0;

foreach my $key (keys %result)
{
    my $min_ = 999999999999999999999999;
    my $max_ = 0;  # including failures
    my $fmax_ = 0; # excluding failures
    my $verdict = 999999999999999999999999;

    # Skip results for which not all methods were tried.
    next unless int(keys %{$result{$key}}) == $nmeths;

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

my $meth;
my $v;
format TEXTOUT =
@<<<<<<<<<< @###### @##% @###### @##% @###### @##% @#######
$meth, $Min{$meth}{$v}, 100*$Min{$meth}{$v}/$total{$meth}{$v}, $Max{$meth}{$v}, 100*$Max{$meth}{$v}/$total{$meth}{$v}, $Fail{$meth}{$v}, 100*$Fail{$meth}{$v}/$total{$meth}{$v},$total{$meth}{$v}
.

print "No counterexamples
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
