#!/usr/bin/perl -w
use Getopt::Std;
use strict;

if ($#ARGV <= 1)
{
    print STDERR "syntax: graphdata.pl verdict column files...\n";
    exit(2);
}

my %verdict = ( 0 => "empty", 1 => "non-empty", 2 => "unknown" );

my $opt_v = (shift @ARGV);
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

while (<>)
{
    next if ($_ =~ /^\w*$/) or ($_ eq $head);

    chomp;

    my @res = split(',',$_);
    my $meth = shift @res;
    my $model = shift @res;
    my $formula = shift @res;
    next if $res[3] !~ /^\s*[$opt_v]\s*$/;
    if (defined $res[$opt_c] && $res[$opt_c] !~ /^\s*$/) {
	if ($res[$opt_c] > $max) {
	    $max = $res[$opt_c];
	}
    } else {
      $res[$opt_c] = -1;
    }

    $result{"$model,$formula"}{$meth} = [@res];
}

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

my %Max;
my %Min;
my %total;

foreach my $key (keys %result)
{
    my $min_ = 999999999999999999999999;
    my $max_ = 0;

    for my $meth (keys %{$result{$key}})
    {
	my $t = $result{$key}{$meth};
	my $val = $t->[$opt_c];
	if ((! defined $val) || $val =~ /^\s*$/ || $val == -1 ) {
	    $val = 3 * $max;
	}	
	$min_ = $val if ($val < $min_);
	$max_ = $val if ($val > $max_);	
    }

    for my $meth (keys %{$result{$key}})
    {
	my $t = $result{$key}{$meth};
	my $val = $t->[$opt_c];
	if ((! defined $val) || $val =~ /^\s*$/ || $val == -1 ) {
	    $val = 3 * $max;
	}
	$Max{$meth}++ if ($val == $max_);
	$Min{$meth}++ if ($val == $min_);	
	$total{$meth}++;
    }
}

print "Max:\n";
for my $meth (sort { $Max{$b} <=> $Max{$a} } (keys %Max))
{
    print "  $meth\t", $Max{$meth}, " / ", $total{$meth}, "\n";
}

print "\nMin:\n";
for my $meth (sort { $Min{$b} <=> $Min{$a} } (keys %Min))
{
    print "  $meth\t", $Min{$meth}, " / ", $total{$meth}, "\n";
}
